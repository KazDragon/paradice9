// ==========================================================================
// Paradice Connection
//
// Copyright (C) 2010 Matthew Chaplain, All Rights Reserved.
//
// Permission to reproduce, distribute, perform, display, and to prepare
// derivitive works from this file under the following conditions:
//
// 1. Any copy, reproduction or derivitive work of any part of this file
//    contains this copyright notice and licence in its entirety.
//
// 2. The rights granted to you under this license automatically terminate
//    should you attempt to assert any patent claims against the licensor
//    or contributors, which in any way restrict the ability of any party
//    from using this software or portions thereof in any form under the
//    terms of this license.
//
// Disclaimer: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
//             KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//             WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
//             PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//             OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
//             OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
//             OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//             SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ==========================================================================
#include "connection.hpp"
#include "odin/ansi/ansi_parser.hpp"
#include "odin/net/socket.hpp"
#include "odin/telnet/stream.hpp"
#include "odin/telnet/command_router.hpp"
#include "odin/telnet/negotiation_router.hpp"
#include "odin/telnet/subnegotiation_router.hpp"
#include "odin/telnet/input_visitor.hpp"
#include "odin/telnet/options/echo_server.hpp"
#include "odin/telnet/options/naws_client.hpp"
#include "odin/telnet/options/suppress_goahead_server.hpp"
#include "odin/telnet/options/terminal_type_client.hpp"
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/placeholders.hpp>
#include <deque>
#include <string>
#include <utility>

namespace paradice {

namespace {

// ==========================================================================
// ANSI_INPUT_VISITOR
// ==========================================================================
struct ansi_input_visitor
    : public boost::static_visitor<>
{
    ansi_input_visitor(
        std::function<void (odin::ansi::mouse_report const &)>     on_mouse_report,
        std::function<void (odin::ansi::control_sequence const &)> on_control_sequence,
        std::function<void (char)>                                 on_text)
      : on_mouse_report_(std::move(on_mouse_report)),
        on_control_sequence_(std::move(on_control_sequence)),
        on_text_(std::move(on_text))
    {
    }

    void operator()(odin::ansi::control_sequence const &control_sequence)
    {
        std::printf("Control Sequence(\n"
               "    META: %s\n"
               "    CSI:  0x%02X ('%c')\n"
               "    CMD:  0x%02X ('%c')\n"
          , (control_sequence.meta_ ? "Yes" : "No")
          , int(control_sequence.initiator_)
          , control_sequence.initiator_
          , int(control_sequence.command_)
          , control_sequence.command_
          );

        if (!control_sequence.arguments_.empty())
        {
            std::printf("    ARGUMENTS:\n");

            for (auto ch : control_sequence.arguments_)
            {
                std::printf("        [0x%02X]", int(ch));

                if (isprint(ch))
                {
                    std::printf(" (%c)", ch);
                }

                std::printf("\n");
            }
        }

        std::printf(")\n");

        if (on_control_sequence_)
        {
            on_control_sequence_(control_sequence);
        }
    }

    void operator()(odin::ansi::mouse_report report)
    {
        // Mouse reports' x- and y-positions are read as bytes.  Unfortunately,
        // the parser stores them as signed chars.  This means that positions
        // of 128 or over are negative.  Therefore, we do some casting magic
        // to make them unsigned again.
        report.x_position_ = odin::u8(odin::s8(report.x_position_));
        report.y_position_ = odin::u8(odin::s8(report.y_position_));

        // Mouse reports are all offset by 32 in the protocol, ostensibly to
        // avoid any other protocol errors.  It then has (1,1) as the origin
        // where we have (0,0), so we must compensate for that too.
        report.button_     -= 32;
        report.x_position_ -= 33;
        report.y_position_ -= 33;

        std::printf("[BUTTON=%d, X=%d, Y=%d]\n",
            int(report.button_),
            int(report.x_position_),
            int(report.y_position_));

        if (on_mouse_report_)
        {
            on_mouse_report_(report);
        }
    }

    void operator()(char text)
    {
        if (on_text_)
        {
            on_text_(text);
        }
    }

    std::function<void (odin::ansi::mouse_report const &)>     on_mouse_report_;
    std::function<void (odin::ansi::control_sequence const &)> on_control_sequence_;
    std::function<void (char)>                                 on_text_;
};

}

// ==========================================================================
// CONNECTION::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct connection::impl
    : public std::enable_shared_from_this<impl>
{
    // ======================================================================
    // START
    // ======================================================================
    void start()
    {
        schedule_next_read();
    }

    // ======================================================================
    // RECONNECT
    // ======================================================================
    void reconnect(std::shared_ptr<odin::net::socket> const &socket)
    {
        socket_ = socket;

        // Begin the keepalive process.  This sends regular heartbeats to the
        // client to help guard against his network settings timing him out
        // due to lack of activity.
        keepalive_timer_ =
            std::make_shared<boost::asio::deadline_timer>(
                std::ref(socket_->get_io_service()));
        schedule_keepalive();

        telnet_stream_ =
            std::make_shared<odin::telnet::stream>(
                socket_,
                std::ref(socket_->get_io_service()));

        telnet_command_router_ =
            std::make_shared<odin::telnet::command_router>();

        telnet_negotiation_router_ =
            std::make_shared<odin::telnet::negotiation_router>();

        telnet_subnegotiation_router_ =
            std::make_shared<odin::telnet::subnegotiation_router>();

        telnet_input_visitor_ = std::make_shared<odin::telnet::input_visitor>(
            telnet_command_router_,
            telnet_negotiation_router_,
            telnet_subnegotiation_router_,
            [this](auto const &text){on_text(text);});

        telnet_echo_server_ = std::make_shared<odin::telnet::options::echo_server>(
            telnet_stream_
          , telnet_negotiation_router_
          , telnet_subnegotiation_router_);
        telnet_echo_server_->set_activatable(true);
        telnet_echo_server_->activate();

        telnet_suppress_ga_server_ =
            std::make_shared<odin::telnet::options::suppress_goahead_server>(
            telnet_stream_
          , telnet_negotiation_router_
          , telnet_subnegotiation_router_);
        telnet_suppress_ga_server_->set_activatable(true);
        telnet_suppress_ga_server_->activate();

        telnet_naws_client_ =
            std::make_shared<odin::telnet::options::naws_client>(
            telnet_stream_
          , telnet_negotiation_router_
          , telnet_subnegotiation_router_);
        telnet_naws_client_->set_activatable(true);
        telnet_naws_client_->activate();
        telnet_naws_client_->on_size(
            [this](auto const &width, auto const &height)
            {
                on_window_size_changed(width, height);
            });

        telnet_terminal_type_client_ =
            std::make_shared<odin::telnet::options::terminal_type_client>(
            telnet_stream_
          , telnet_negotiation_router_
          , telnet_subnegotiation_router_);
        telnet_terminal_type_client_->set_activatable(true);
        telnet_terminal_type_client_->activate();
        telnet_terminal_type_client_->on_terminal_type_detected(
            [this](auto const &type)
            {
                on_terminal_type_detected(type);
            });
    }

    // ======================================================================
    // SCHEDULE_NEXT_READ
    // ======================================================================
    void schedule_next_read()
    {
        if (!telnet_stream_->is_alive())
        {
            return;
        }

        auto available = telnet_stream_->available();

        if (available.is_initialized())
        {
            telnet_stream_->async_read(
                odin::telnet::stream::input_size_type(available.get()),
                [pthis=shared_from_this()](auto const &data)
                {
                    pthis->data_read(data);
                });
        }
        else
        {
            telnet_stream_->async_read(
                odin::telnet::stream::input_size_type(1),
                [pthis=shared_from_this()](auto const &data)
                {
                    pthis->data_read(data);
                });
        }
    }

    // ======================================================================
    // DATA_READ
    // ======================================================================
    void data_read(odin::telnet::stream::input_storage_type const &data)
    {
        apply_input_range(*telnet_input_visitor_, data);
        schedule_next_read();
    }

    // ======================================================================
    // ON_KEEPALIVE
    // ======================================================================
    void on_keepalive(boost::system::error_code const &error)
    {
        if (!error && socket_->is_alive())
        {
            socket_->async_write(
                { odin::telnet::IAC, odin::telnet::NOP },
                nullptr);

            schedule_keepalive();
        }
    }

    // ======================================================================
    // SCHEDULE_KEEPALIVE
    // ======================================================================
    void schedule_keepalive()
    {
        keepalive_timer_->expires_from_now(boost::posix_time::seconds(30));
        keepalive_timer_->async_wait(
            [this](auto const &error_code)
            {
                on_keepalive(error_code);
            });
    }

    // ======================================================================
    // ON_TEXT
    // ======================================================================
    void on_text(std::string const &text)
    {
        ansi_input_visitor visitor(
            on_mouse_report_, on_control_sequence_, on_text_);

        for (auto const &ch : text)
        {
            parse_(ch);

            auto token = parse_.token();

            if (token.is_initialized())
            {
                apply_visitor(visitor, token.get());
            }
        }
    }

    // ======================================================================
    // ON_WINDOW_SIZE_CHANGED
    // ======================================================================
    void on_window_size_changed(odin::u16 width, odin::u16 height)
    {
        if (on_window_size_changed_)
        {
            on_window_size_changed_(width, height);
        }
    }

    // ======================================================================
    // ON_TERMINAL_TYPE_NEGOTIATED
    // ======================================================================
    void on_terminal_type_negotiated()
    {
        // No longer wait on the terminal type negotiation.
        telnet_terminal_type_client_->on_request_complete(NULL);

        // Get back to scheduling a terminal type get.
        schedule_get_terminal_type();
    }

    // ======================================================================
    // ON_TERMINAL_TYPE_DETECTED
    // ======================================================================
    void on_terminal_type_detected(std::string const &type)
    {
        terminal_type_ = type;
        announce_terminal_type();
    }

    // ======================================================================
    // ANNOUNCE_TERMINAL_TYPE
    // ======================================================================
    void announce_terminal_type()
    {
        for (auto const &callback : terminal_type_requests_)
        {
            callback(terminal_type_);
        }

        terminal_type_requests_.clear();
    }

    // ======================================================================
    // SCHEDULE_GET_TERMINAL_TYPE
    // ======================================================================
    void schedule_get_terminal_type()
    {
        if (telnet_terminal_type_client_->is_active())
        {
            // if we already know the terminal type, then simply schedule
            // an announcement.
            if (terminal_type_ != "")
            {
                socket_->get_io_service().post(bind(
                    &impl::announce_terminal_type
                  , this));
            }
            else
            {
                telnet_terminal_type_client_->send_request();
            }
        }
        else
        {
            // If the terminal type client is still negotiating, then set up
            // a callback for when it's done.
            if (telnet_terminal_type_client_->is_negotiating_activation())
            {
                telnet_terminal_type_client_->on_request_complete(bind(
                    &impl::on_terminal_type_negotiated
                  , this));
            }
            else
            {
                // It has completed the negotiation and the other end has
                // decided not to offer this functionality.  Therefore,
                // complete the terminal type lookup manually with "UNKNOWN".
                socket_->get_io_service().post(bind(
                    &impl::on_terminal_type_detected
                  , this
                  , "UNKNOWN"));
            }
        }
    }

    std::shared_ptr<odin::net::socket>                   socket_;
    std::shared_ptr<odin::telnet::stream>                telnet_stream_;
    std::shared_ptr<odin::telnet::command_router>        telnet_command_router_;
    std::shared_ptr<odin::telnet::negotiation_router>    telnet_negotiation_router_;
    std::shared_ptr<odin::telnet::subnegotiation_router> telnet_subnegotiation_router_;
    std::shared_ptr<odin::telnet::input_visitor>         telnet_input_visitor_;
    std::shared_ptr<odin::telnet::options::echo_server>  telnet_echo_server_;
    std::shared_ptr<odin::telnet::options::suppress_goahead_server> telnet_suppress_ga_server_;
    std::shared_ptr<odin::telnet::options::naws_client>  telnet_naws_client_;
    std::shared_ptr<odin::telnet::options::terminal_type_client> telnet_terminal_type_client_;

    std::function<void (odin::u16, odin::u16)>           on_window_size_changed_;
    std::function<void (char)>                           on_text_;
    std::function<void (odin::ansi::control_sequence const &)> on_control_sequence_;
    std::function<void (odin::ansi::mouse_report const &)> on_mouse_report_;

    odin::ansi::parser                                   parse_;
    std::shared_ptr<boost::asio::deadline_timer>         keepalive_timer_;

    std::string                                          terminal_type_;
    std::vector<std::function<void (std::string)>>       terminal_type_requests_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
connection::connection(std::shared_ptr<odin::net::socket> socket)
    : pimpl_(new impl)
{
    pimpl_->reconnect(socket);
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
connection::~connection()
{
    disconnect();
}

// ==========================================================================
// START
// ==========================================================================
void connection::start()
{
    pimpl_->start();
}

// ==========================================================================
// WRITE
// ==========================================================================
void connection::write(std::vector<odin::u8> const &data)
{
    pimpl_->socket_->async_write(data, NULL);
}

// ==========================================================================
// ON_WINDOW_SIZE_CHANGED
// ==========================================================================
void connection::on_window_size_changed(
    std::function<void (odin::u16, odin::u16)> const &callback)
{
    pimpl_->on_window_size_changed_ = callback;
}

// ==========================================================================
// ON_TEXT
// ==========================================================================
void connection::on_text(std::function<void (char)> const &callback)
{
    pimpl_->on_text_ = callback;
}

// ==========================================================================
// ON_MOUSE_REPORT
// ==========================================================================
void connection::on_mouse_report(
    std::function<void (odin::ansi::mouse_report const &)> const &callback)
{
    pimpl_->on_mouse_report_ = callback;
}

// ==========================================================================
// ON_CONTROL_SEQUENCE
// ==========================================================================
void connection::on_control_sequence(
    std::function<void (odin::ansi::control_sequence const &)> const &callback)
{
    pimpl_->on_control_sequence_ = callback;
}

// ==========================================================================
// ON_SOCKET_DEATH
// ==========================================================================
void connection::on_socket_death(std::function<void ()> const &callback)
{
    pimpl_->socket_->on_death(callback);
}

// ==========================================================================
// DISCONNECT
// ==========================================================================
void connection::disconnect()
{
    if (pimpl_->keepalive_timer_ != nullptr)
    {
        boost::system::error_code unused_error_code;
        pimpl_->keepalive_timer_->cancel(unused_error_code);
    }

    if (pimpl_->socket_ != nullptr)
    {
        pimpl_->socket_->close();
        pimpl_->socket_.reset();
    }
}

// ==========================================================================
// RECONNECT
// ==========================================================================
void connection::reconnect(std::shared_ptr<odin::net::socket> const &socket)
{
    pimpl_->reconnect(socket);
}

// ==========================================================================
// ASYNC_GET_TERMINAL_TYPE
// ==========================================================================
void connection::async_get_terminal_type(
    std::function<void (std::string const &)> const &callback)
{
    pimpl_->terminal_type_requests_.push_back(callback);
    pimpl_->schedule_get_terminal_type();
}

}

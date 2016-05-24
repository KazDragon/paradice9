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
#include "paradice/connection.hpp"
#include "odin/net/socket.hpp"
#include <telnetpp/telnetpp.hpp>
#include <telnetpp/options/echo/server.hpp>
#include <telnetpp/options/naws/client.hpp>
#include <telnetpp/options/suppress_ga/server.hpp>
#include <telnetpp/options/terminal_type/client.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/placeholders.hpp>
#include <deque>
#include <string>
#include <utility>

namespace paradice {

// ==========================================================================
// CONNECTION::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct connection::impl
    : public std::enable_shared_from_this<impl>
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(std::shared_ptr<odin::net::socket> const &socket)
      : socket_(socket),
        telnet_session_(
            [this](auto &&text) -> std::vector<telnetpp::token>
            {
                this->on_text(text);
                return {};
            })
    {
        telnet_echo_server_.set_activatable();
        telnet_session_.install(telnet_echo_server_);
        
        telnet_suppress_ga_server_.set_activatable();
        telnet_session_.install(telnet_suppress_ga_server_);
         
        telnet_naws_client_.set_activatable();
        telnet_naws_client_.on_window_size_changed.connect(
            [this](auto &&width, auto &&height) -> std::vector<telnetpp::token>
            {
                this->on_window_size_changed(width, height);
                return {};
            });
        telnet_session_.install(telnet_naws_client_);

        telnet_terminal_type_client_.set_activatable();
        telnet_terminal_type_client_.on_terminal_type.connect(
            [this](auto &&type) -> std::vector<telnetpp::token>
            {
                this->on_terminal_type_detected(type);
                return {};
            });
        telnet_terminal_type_client_.on_state_changed.connect(
            [this](auto &&state) -> std::vector<telnetpp::token>
            {
                if (telnet_terminal_type_client_.is_active())
                {
                    return telnet_terminal_type_client_.request_terminal_type();
                }
                
                return {};
            });
        telnet_session_.install(telnet_terminal_type_client_);

        // Begin the keepalive process.  This sends regular heartbeats to the
        // client to help guard against his network settings timing him out
        // due to lack of activity.
        keepalive_timer_ =
            std::make_shared<boost::asio::deadline_timer>(
                std::ref(socket_->get_io_service()));
        schedule_keepalive();
        
        // Send the required activations.
        write(telnet_session_.send(telnet_echo_server_.activate()));
        write(telnet_session_.send(telnet_suppress_ga_server_.activate()));
        write(telnet_session_.send(telnet_naws_client_.activate()));
        write(telnet_session_.send(telnet_terminal_type_client_.activate()));
    }

    // ======================================================================
    // START
    // ======================================================================
    void start()
    {
        schedule_next_read();
    }
    
    // ======================================================================
    // WRITE
    // ======================================================================
    void write(
        std::vector<boost::variant<telnetpp::u8stream, boost::any>> const &data)
    {
        // We currently talk directly to a socket type that expects a
        // straightforward stream of bytes -- no user-defined data.  This may
        // change in the future, in which case we will get a nice exception 
        // thrown here during testing of the new feature.
        for (auto &&datum : data)
        {
            auto &&stream = boost::get<telnetpp::u8stream>(datum);
            socket_->async_write({stream.begin(), stream.end()}, nullptr);
        }
        
    }

    // ======================================================================
    // SCHEDULE_NEXT_READ
    // ======================================================================
    void schedule_next_read()
    {
        if (!socket_->is_alive())
        {
            return;
        }

        auto available = socket_->available();
        auto amount = available 
                    ? *available 
                    : odin::net::socket::input_size_type{1};
                    
        socket_->async_read(
            amount,
            [this](auto &&data)
            {
                this->on_data(data);
            });
    }

    // ======================================================================
    // ON_DATA
    // ======================================================================
    void on_data(std::vector<odin::u8> const &data)
    {
        write(telnet_session_.send(
            telnet_session_.receive({data.begin(), data.end()})));
            
        schedule_next_read();
    }
    
    // ======================================================================
    // ON_KEEPALIVE
    // ======================================================================
    void on_keepalive(boost::system::error_code const &error)
    {
        if (!error && socket_->is_alive())
        {
            write(telnet_session_.send({
                    telnetpp::element(telnetpp::command(telnetpp::nop))
                }));

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
                this->on_keepalive(error_code);
            });
    }

    // ======================================================================
    // ON_TEXT
    // ======================================================================
    void on_text(std::string const &text)
    {
        if (on_data_read_)
        {
            on_data_read_(text);
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
    
    std::shared_ptr<odin::net::socket>                   socket_;
    std::vector<odin::u8>                                unparsed_bytes_;
    
    std::function<void (std::string const &)>            on_data_read_;
    telnetpp::session                                    telnet_session_;
    telnetpp::options::echo::server                      telnet_echo_server_;
    telnetpp::options::suppress_ga::server               telnet_suppress_ga_server_;
    telnetpp::options::naws::client                      telnet_naws_client_;
    telnetpp::options::terminal_type::client             telnet_terminal_type_client_;
    
    std::function<void (odin::u16, odin::u16)>           on_window_size_changed_;
    std::shared_ptr<boost::asio::deadline_timer>         keepalive_timer_;

    std::string                                          terminal_type_;
    std::vector<std::function<void (std::string)>>       terminal_type_requests_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
connection::connection(std::shared_ptr<odin::net::socket> const &socket)
    : pimpl_(std::make_shared<impl>(socket))
{
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
// ON_DATA_READ
// ==========================================================================
void connection::on_data_read(
    std::function<void (std::string const &)> const &callback)
{
    pimpl_->on_data_read_ = callback;
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
// ASYNC_GET_TERMINAL_TYPE
// ==========================================================================
void connection::async_get_terminal_type(
    std::function<void (std::string const &)> const &callback)
{
    pimpl_->terminal_type_requests_.push_back(callback);
}

}

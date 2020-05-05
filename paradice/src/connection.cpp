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

#include <serverpp/tcp_socket.hpp>
#include <telnetpp/telnetpp.hpp>
#include <telnetpp/options/echo/server.hpp>
#include <telnetpp/options/mccp/codec.hpp>
#include <telnetpp/options/mccp/server.hpp>
#include <telnetpp/options/mccp/zlib/compressor.hpp>
#include <telnetpp/options/naws/client.hpp>
#include <telnetpp/options/suppress_ga/server.hpp>
#include <telnetpp/options/terminal_type/client.hpp>
#include <boost/make_unique.hpp>

namespace paradice {

// ==========================================================================
// CONNECTION::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct connection::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(serverpp::tcp_socket &&socket)
      : socket_(std::move(socket))
    {
        telnet_naws_client_.on_window_size_changed.connect(
            [this](auto &&width, auto &&height, auto &&continuation)
            {
                this->on_window_size_changed(width, height);
            });

        telnet_terminal_type_client_.on_terminal_type.connect(
            [this](auto &&type, auto &&continuation)
            {
                std::string user_type(type.begin(), type.end());
                this->on_terminal_type_detected(user_type);
            });

        telnet_terminal_type_client_.on_state_changed.connect(
            [this](auto &&continuation)
            {
                if (telnet_terminal_type_client_.active())
                {
                    telnet_terminal_type_client_.request_terminal_type(continuation);
                }
            });

        telnet_mccp_server_.on_state_changed.connect(
            [this](auto &&continuation)
            {
                if (telnet_mccp_server_.active())
                {
                    telnet_mccp_server_.start_compression(continuation);
                }
            });

        telnet_session_.install(telnet_echo_server_);
        telnet_session_.install(telnet_suppress_ga_server_);
        telnet_session_.install(telnet_naws_client_);
        telnet_session_.install(telnet_terminal_type_client_);
        telnet_session_.install(telnet_mccp_server_);
        
        // Send the required activations.
        auto const &write_continuation = 
            [this](telnetpp::element const &elem)
            {
                this->write(elem);
            };

        telnet_echo_server_.activate(write_continuation);
        telnet_suppress_ga_server_.activate(write_continuation);
        telnet_naws_client_.activate(write_continuation);
        telnet_terminal_type_client_.activate(write_continuation);
        telnet_mccp_server_.activate(write_continuation);
    }

    // ======================================================================
    // CLOSE
    // ======================================================================
    void close()
    {
        socket_.close();
    }

    // ======================================================================
    // IS_ALIVE
    // ======================================================================
    bool is_alive() const
    {
        return socket_.is_alive();
    }

    // ======================================================================
    // RAW_WRITE
    // ======================================================================
    void raw_write(telnetpp::bytes data)
    {
        telnet_mccp_compressor_(
            data,
            [this](telnetpp::bytes compressed_data, bool)
            {
                this->socket_.write(compressed_data);
            });
    }
    
    // ======================================================================
    // WRITE
    // ======================================================================
    void write(telnetpp::element const &data)
    {
        telnet_session_.send(
            data, 
            [this](telnetpp::bytes data)
            {
                this->raw_write(data);
            });
    }

    // ======================================================================
    // ASYNC_READ
    // ======================================================================
    void async_read(
        std::function<void (serverpp::bytes)> const &data_continuation,
        std::function<void ()> const &read_complete_continuation)
    {
        socket_.async_read(
            [=](serverpp::bytes data)
            {
                telnet_session_.receive(
                    data, 
                    [=](telnetpp::bytes data, auto &&send)
                    {
                        data_continuation(data);
                    },
                    [=](telnetpp::bytes data)
                    {
                        this->raw_write(data);
                    });

                read_complete_continuation();
            });
    }

    // ======================================================================
    // ON_WINDOW_SIZE_CHANGED
    // ======================================================================
    void on_window_size_changed(std::uint16_t width, std::uint16_t height)
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
        // Announcing a terminal type request could plausibly disconnect and
        // destroy this connection object.  Therefore, we move the requests
        // structure out of the object proper before handling them.
        decltype(terminal_type_requests_) requests{
            std::move(terminal_type_requests_)};
        terminal_type_requests_.clear();

        for (auto const &callback : requests)
        {
            callback(terminal_type_);
        }
    }

    serverpp::tcp_socket socket_;

    telnetpp::session                                    telnet_session_;
    telnetpp::options::echo::server                      telnet_echo_server_;
    telnetpp::options::suppress_ga::server               telnet_suppress_ga_server_;
    telnetpp::options::mccp::zlib::compressor            telnet_mccp_compressor_;
    telnetpp::options::mccp::server                      telnet_mccp_server_{telnet_mccp_compressor_};
    telnetpp::options::naws::client                      telnet_naws_client_;
    telnetpp::options::terminal_type::client             telnet_terminal_type_client_;
    
    std::function<void (std::uint16_t, std::uint16_t)>   on_window_size_changed_;

    std::string                                          terminal_type_;
    std::vector<std::function<void (std::string)>>       terminal_type_requests_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
connection::connection(serverpp::tcp_socket &&new_socket)
    : pimpl_(boost::make_unique<impl>(std::move(new_socket)))
{
}

// ==========================================================================
// MOVE CONSTRUCTOR
// ==========================================================================
connection::connection(connection &&other) noexcept = default;

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
connection::~connection() = default;

// ==========================================================================
// MOVE ASSIGNMENT
// ==========================================================================
connection &connection::operator=(connection &&other) noexcept = default;

// ==========================================================================
// CLOSE
// ==========================================================================
void connection::close()
{
    pimpl_->close();
}

// ==========================================================================
// IS_ALIVE
// ==========================================================================
bool connection::is_alive() const
{
    return pimpl_->is_alive();
}

// ==========================================================================
// ASYNC_READ
// ==========================================================================
void connection::async_read(
    std::function<void (serverpp::bytes)> const &data_continuation,
    std::function<void ()> const &read_complete_continuation)
{
    pimpl_->async_read(data_continuation, read_complete_continuation);
}

// ==========================================================================
// WRITE
// ==========================================================================
void connection::write(serverpp::bytes data)
{
    pimpl_->write(data);
}

// ==========================================================================
// ASYNC_GET_TERMINAL_TYPE
// ==========================================================================
void connection::async_get_terminal_type(
    std::function<void (std::string const &)> const &continuation)
{
    pimpl_->terminal_type_requests_.push_back(continuation);
}

// ==========================================================================
// ON_WINDOW_SIZE_CHANGED
// ==========================================================================
void connection::on_window_size_changed(
    std::function<void (std::uint16_t, std::uint16_t)> const &continuation)
{
    pimpl_->on_window_size_changed_ = continuation;
}

}

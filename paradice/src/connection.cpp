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

#include <telnetpp/telnetpp.hpp>
#include <telnetpp/options/echo/server.hpp>
#include <telnetpp/options/mccp/codec.hpp>
#include <telnetpp/options/mccp/server.hpp>
#include <telnetpp/options/mccp/zlib/compressor.hpp>
#include <telnetpp/options/naws/client.hpp>
#include <telnetpp/options/suppress_ga/server.hpp>
#include <telnetpp/options/terminal_type/client.hpp>

#include <iostream>

namespace paradice {

// ==========================================================================
// CONNECTION::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct connection::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(std::unique_ptr<connection::channel_concept> &&channel)
      : channel_(std::move(channel))
    {
        telnet_naws_client_.on_window_size_changed.connect(
            [this](auto &&width, auto &&height)
            {
                this->on_window_size_changed(width, height);
            });

        telnet_terminal_type_client_.on_terminal_type.connect(
            [this](auto &&type)
            {
                std::string user_type(type.begin(), type.end());
                this->on_terminal_type_detected(user_type);
            });

        telnet_terminal_type_client_.on_state_changed.connect(
            [this]()
            {
                if (telnet_terminal_type_client_.active())
                {
                    telnet_terminal_type_client_.request_terminal_type();
                }
            });

        telnet_mccp_server_.on_state_changed.connect(
            [this]()
            {
                mccp_active_ = telnet_mccp_server_.active();

                if (mccp_active_)
                {
                    telnet_mccp_server_.start_compression();
                }
            });

        telnet_session_.install(telnet_echo_server_);
        telnet_session_.install(telnet_suppress_ga_server_);
        telnet_session_.install(telnet_naws_client_);
        telnet_session_.install(telnet_terminal_type_client_);
        telnet_session_.install(telnet_mccp_server_);
        
        // Send the required activations.
        telnet_echo_server_.activate();
        telnet_suppress_ga_server_.activate();
        telnet_naws_client_.activate();
        telnet_terminal_type_client_.activate();
        telnet_mccp_server_.activate();
    }

    // ======================================================================
    // CLOSE
    // ======================================================================
    void close()
    {
        channel_->close();
    }

    // ======================================================================
    // IS_ALIVE
    // ======================================================================
    bool is_alive() const
    {
        return channel_->is_alive();
    }

    // ======================================================================
    // RAW_WRITE
    // ======================================================================
    void raw_write(telnetpp::bytes data)
    {
        if (mccp_active_)
        {
            telnet_mccp_compressor_(
                data,
                [this](telnetpp::bytes compressed_data, bool)
                {
                    this->channel_->write(compressed_data);
                });
        }
        else
        {
            channel_->write(data);
        }
    }
    
    // ======================================================================
    // WRITE
    // ======================================================================
    void write(telnetpp::element const &data)
    {
        telnet_session_.write(data);
    }

    // ======================================================================
    // ASYNC_READ
    // ======================================================================
    void async_read(std::function<void (bytes)> const &callback)
    {
        telnet_session_.async_read([this, callback](bytes data) {
            if (data.empty())
            {
                telnetpp::byte_storage result;
                std::swap(telnet_read_cache_, result);
                callback(result);
            }
            else
            {
                telnet_read_cache_.append(data.begin(), data.end());
            }
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
        // Announcing a terminal type request ld plausibly disconnect and
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

    std::unique_ptr<connection::channel_concept>         channel_;

    telnetpp::session                                    telnet_session_{*channel_};
    telnetpp::options::echo::server                      telnet_echo_server_{telnet_session_};
    telnetpp::options::suppress_ga::server               telnet_suppress_ga_server_{telnet_session_};
    telnetpp::options::mccp::zlib::compressor            telnet_mccp_compressor_;
    telnetpp::options::mccp::server                      telnet_mccp_server_{telnet_session_, telnet_mccp_compressor_};
    telnetpp::options::naws::client                      telnet_naws_client_{telnet_session_};
    telnetpp::options::terminal_type::client             telnet_terminal_type_client_{telnet_session_};
    telnetpp::byte_storage                               telnet_read_cache_;
    
    std::function<void (std::uint16_t, std::uint16_t)>   on_window_size_changed_;

    std::string                                          terminal_type_;
    std::vector<std::function<void (std::string)>>       terminal_type_requests_;

    bool                                                 mccp_active_{false};
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
connection::connection(std::unique_ptr<channel_concept> ep)
    : pimpl_(std::make_unique<impl>(std::move(ep)))
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
void connection::async_read(std::function<void (bytes)> const &callback)
{
    pimpl_->async_read(callback);
}

// ==========================================================================
// WRITE
// ==========================================================================
void connection::write(bytes data)
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

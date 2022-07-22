// ==========================================================================
// Paradice9 Server
//
// Copyright (C) 2009 Matthew Chaplain, All Rights Reserved.
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
#include "paradice9/server.hpp"
#include "paradice9/context_impl.hpp"

#include <paradice/client.hpp>
#include <paradice/connection.hpp>

#include <serverpp/tcp_server.hpp>

#include <boost/make_unique.hpp>
#include <boost/range/algorithm/find.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

#include <map>
#include <utility>

namespace paradice9 {

namespace {

// ==========================================================================
// MAKE_BEHAVIOUR
// ==========================================================================
terminalpp::behaviour make_behaviour(std::string terminal_type)
{
    // TODO: In which we consult a database of terminal types and assign 
    // their behaviours.
    terminalpp::behaviour behaviour;
    behaviour.supports_basic_mouse_tracking = true;
    behaviour.supports_window_title_bel = true;
    return behaviour;
}

}

// ==========================================================================
// SERVER::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct server::impl
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(
        boost::asio::io_context &io_context,
        serverpp::port_identifier port,
        paradice::context &context)
      : io_context_(io_context),
        server_{
          io_context, 
          port,
          [this](serverpp::tcp_socket &&new_socket)
          {
              on_accept(std::move(new_socket));
          }},
        context_{context}
    {
    }

    // ======================================================================
    // SHUTDOWN
    // ======================================================================
    void shutdown()
    {
        server_.shutdown();
        pending_connections_.clear();
    }

private :
    // ======================================================================
    // SCHEDULE_NEXT_READ
    // ======================================================================
    void schedule_next_read(std::shared_ptr<paradice::connection> const &cnx)
    {
        // Set up callbacks to handle communication while the attributes of
        // the connection (window size, etc.) are being negotiated.  After
        // that point, this responsibility is handed off to the client object.
        cnx->async_read(
            [this, wcnx = std::weak_ptr<paradice::connection>(cnx)](serverpp::bytes)
            {
                // At this point in the negotiation process, we discard all
                // non-Telnet traffic.
                // TODO: later, cache it up and feed it into the client as the
                // first result of async_read, somehow.
                // Even if data received at the socket was all Telnet protocol,
                // there will still be a callback here of empty data.  That 
                // means that if this is called on the received packet with the
                // terminal type, this callback will occur afterwards.  In that
                // instance, the shared_ptr will have been destroyed.
                if (std::shared_ptr<paradice::connection> cnx = wcnx.lock();
                    cnx)
                {
                    if (cnx->is_alive())
                    {
                        schedule_next_read(cnx);
                    }
                    else
                    {
                        on_connection_death(cnx);
                    }
                }
            });
    }

    // ======================================================================
    // ON_ACCEPT
    // ======================================================================
    void on_accept(serverpp::tcp_socket &&new_socket)
    {
        // Create the connection and client structures for the socket.
        auto connection = std::make_shared<paradice::connection>(
            std::move(new_socket));

        {
            std::lock_guard<std::mutex> _(pending_connections_mutex_);
            pending_connections_.push_back(connection);
        }
        
        // Before creating a client object, we first negotiate some
        // knowledge about the connection.  Set up the callbacks for this.
        connection->on_window_size_changed(
            [this, wcnx=std::weak_ptr<paradice::connection>(connection)]
            (auto w, auto h) 
            {
                this->on_window_size_changed(wcnx, w, h);
            });

        connection->async_get_terminal_type(
            [this, 
             wcnx=std::weak_ptr<paradice::connection>(connection)]
            (auto const &type)
            {
                this->on_terminal_type(wcnx, type);
            });

        schedule_next_read(connection);
    }

    // ======================================================================
    // ON_TERMINAL_TYPE
    // ======================================================================
    void on_terminal_type(
        std::weak_ptr<paradice::connection>  weak_connection,
        std::string const                   &terminal_type)
    {
        std::cout << "Terminal type is: " << terminal_type << "\n";
        
        if (auto cnx = weak_connection.lock(); cnx)
        {
            std::unique_lock<std::mutex> lock(pending_connections_mutex_);
            auto pending_cnx = boost::find(pending_connections_, cnx);
            
            // There is a possibility that this is a stray terminal type.
            // If so, ignore it.
            if (pending_cnx == pending_connections_.end())
            {
                return;
            }
            
            pending_connections_.erase(pending_cnx);
            lock.unlock();

            // If the window's size has been set by the NAWS process,
            // then update it to that.  Otherwise, use the standard 80,24.
            std::unique_lock<std::mutex> pending_size_lock(pending_sizes_mutex_);
            std::pair<std::uint16_t, std::uint16_t> window_size{80, 24};

            if (auto psize = pending_sizes_.find(cnx);
                psize != pending_sizes_.end())
            {
                window_size = psize->second;
                pending_sizes_.erase(cnx);
            }

            auto client =
                std::make_shared<paradice::client>(
                    io_context_, 
                    context_,
                    std::move(*cnx),
                    make_behaviour(terminal_type));

            client->set_window_size(window_size.first, window_size.second);
            
            client->on_connection_death(
                [this, wclient = std::weak_ptr<paradice::client>(client)]
                {
                    if (auto client = wclient.lock(); client)
                    {
                        on_client_death(client);
                    }
                });

            context_.add_client(client);
            // context_.update_names();
        }
    }
    
    // ======================================================================
    // ON_CONNECTION_DEATH
    // ======================================================================
    void on_connection_death(std::shared_ptr<paradice::connection> const &cnx)
    {
        std::lock_guard<std::mutex> _(pending_connections_mutex_);
        boost::remove_erase(pending_connections_, cnx);
    }
    
    // ======================================================================
    // ON_CLIENT_DEATH
    // ======================================================================
    void on_client_death(std::shared_ptr<paradice::client> const &client)
    {
        context_.remove_client(client);
    }

    // ======================================================================
    // ON_WINDOW_SIZE_CHANGED
    // ======================================================================
    void on_window_size_changed(
        std::weak_ptr<paradice::connection> weak_connection,
        std::uint16_t width,
        std::uint16_t height)
    {
        // This is only called during the negotiation process.  We save
        // the size so that it can be given to the client once the process
        // has completed.
        auto connection = weak_connection.lock();
        
        if (connection)
        {
            std::lock_guard<std::mutex> _(pending_sizes_mutex_);
            pending_sizes_[connection] = std::make_pair(width, height);
        }
    }
    
    boost::asio::io_context &io_context_;
    serverpp::tcp_server server_;
    paradice::context &context_;

    // Clients whose connections are being negotiated.
    std::mutex pending_connections_mutex_;
    std::vector<std::shared_ptr<paradice::connection>> pending_connections_;

    std::mutex pending_sizes_mutex_;
    std::map<
        std::shared_ptr<paradice::connection>, 
        std::pair<std::uint16_t, std::uint16_t> > pending_sizes_; 
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server(
    boost::asio::io_context &io_context, 
    serverpp::port_identifier port,
    paradice::context &context)
  : pimpl_(boost::make_unique<impl>(io_context, port, context))
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
server::~server() = default;

// ==========================================================================
// SHUTDOWN
// ==========================================================================
void server::shutdown()
{
    pimpl_->shutdown();
}

}

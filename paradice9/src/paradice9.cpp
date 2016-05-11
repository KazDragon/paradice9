// ==========================================================================
// Paradice9 Application
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
#include "paradice9/paradice9.hpp"
#include "paradice9/context_impl.hpp"
#include "paradice/character.hpp"
#include "paradice/client.hpp"
#include "paradice/communication.hpp"
#include "paradice/connection.hpp"
#include "odin/net/server.hpp"
#include "odin/net/socket.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/asio/placeholders.hpp>
#include <map>
#include <utility>

// ==========================================================================
// PARADICE9::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct paradice9::impl
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(
        boost::asio::io_service                        &io_service
      , std::shared_ptr<boost::asio::io_service::work>  work
      , unsigned int                                    port)
        : io_service_(io_service) 
        , server_(new odin::net::server(
              io_service_
            , port
            , [this](auto socket){on_accept(socket);}))
        , context_(std::make_shared<context_impl>(
              std::ref(io_service), server_, std::ref(work)))
    {
    }

private :
    // ======================================================================
    // ON_ACCEPT
    // ======================================================================
    void on_accept(std::shared_ptr<odin::net::socket> const &socket)
    {
        // Create the connection and client structures for the socket.
        auto connection = std::make_shared<paradice::connection>(socket);
        pending_connections_.push_back(connection);
        
        // Before creating a client object, we first negotiate some
        // knowledge about the connection.  Set up the callbacks for this.
        connection->on_socket_death(
            [this, wp=std::weak_ptr<paradice::connection>(connection)] 
            {
                on_connection_death(wp);
            });
    
        connection->on_window_size_changed(
            [this, wp=std::weak_ptr<paradice::connection>(connection)]
            (auto w, auto h) 
            {
                on_window_size_changed(wp, w, h);
            });

        connection->async_get_terminal_type(
            [this, 
             ws=std::weak_ptr<odin::net::socket>(socket),
             wc=std::weak_ptr<paradice::connection>(connection)]
            (auto const &type)
            {
                on_terminal_type(ws, wc, type);
            });

        connection->start();
    }

    // ======================================================================
    // ON_TERMINAL_TYPE
    // ======================================================================
    void on_terminal_type(
        std::weak_ptr<odin::net::socket>     weak_socket
      , std::weak_ptr<paradice::connection>  weak_connection
      , std::string const                   &terminal_type)
    {
        printf("Terminal type is: \"%s\"\n", terminal_type.c_str());
        
        auto socket =     weak_socket.lock();
        auto connection = weak_connection.lock();
        
        if (socket != NULL && connection != NULL)
        {
            auto pending_connection =
                std::find(
                    pending_connections_.begin()
                  , pending_connections_.end()
                  , connection);
            
            // There is a possibility that this is a stray terminal type.
            // If so, ignore it.
            if (pending_connection == pending_connections_.end())
            {
                return;
            }
            
            pending_connections_.erase(pending_connection);

            auto client =
                std::make_shared<paradice::client>(std::ref(io_service_), context_);
            client->set_connection(connection);
            
            client->on_connection_death(bind(
                &impl::on_client_death
              , this
              , std::weak_ptr<paradice::client>(client)));

            context_->add_client(client);
            context_->update_names();
            
            // If the window's size has been set by the NAWS process,
            // then update it to that.  Otherwise, use the standard 80,24.
            auto psize = pending_sizes_.find(connection);
            
            if (psize != pending_sizes_.end())
            {
                client->set_window_size(
                    psize->second.first
                  , psize->second.second);
                pending_sizes_.erase(connection);
            }
            else
            {
                client->set_window_size(80, 24);
            }
        }
    }
    
    // ======================================================================
    // ON_CONNECTION_DEATH
    // ======================================================================
    void on_connection_death(std::weak_ptr<paradice::connection> const &weak_connection)
    {
        auto connection = weak_connection.lock();
    
        if (connection != NULL)
        {
            pending_connections_.erase(remove(
                    pending_connections_.begin()
                  , pending_connections_.end()
                  , connection)
              , pending_connections_.end());
            pending_sizes_.erase(connection);
        }
    }
    
    // ======================================================================
    // ON_CLIENT_DEATH
    // ======================================================================
    void on_client_death(std::weak_ptr<paradice::client> &weak_client)
    {
        auto client = weak_client.lock();
        
        if (client != NULL)
        {
            context_->remove_client(client);
            context_->update_names();
    
            auto character = client->get_character();
            
            if (character != NULL)
            {
                auto name = character->get_name();
            
                if (!name.empty())
                {
                    paradice::send_to_all(
                        context_
                      , "#SERVER: "
                      + context_->get_moniker(character)
                      + " has left Paradice.\n");
                }
            }
        }
    }
    
    // ======================================================================
    // ON_WINDOW_SIZE_CHANGED
    // ======================================================================
    void on_window_size_changed(
        std::weak_ptr<paradice::connection> weak_connection,
        odin::u16                           width,
        odin::u16                           height)
    {
        // This is only called during the negotiation process.  We save
        // the size so that it can be given to the client once the process
        // has completed.
        auto connection = weak_connection.lock();
        
        if (connection != NULL)
        {
            pending_sizes_[connection] = std::make_pair(width, height);
        }
    }
    
    boost::asio::io_service            &io_service_;
    std::shared_ptr<odin::net::server>  server_;
    std::shared_ptr<paradice::context>  context_;
    
    // A vector of clients whose connections are being negotiated.
    std::vector<std::shared_ptr<paradice::connection>> pending_connections_;
    std::map<
        std::shared_ptr<paradice::connection>, 
        std::pair<odin::u16, odin::u16> > pending_sizes_; 
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
paradice9::paradice9(
    boost::asio::io_service                        &io_service
  , std::shared_ptr<boost::asio::io_service::work>  work
  , unsigned int                        port)
    : pimpl_(new impl(io_service, work, port))  
{
}


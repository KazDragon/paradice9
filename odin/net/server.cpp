// ==========================================================================
// Paradice Server
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
#include "server.hpp"
#include "socket.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/foreach.hpp>

using namespace boost;

namespace odin { namespace net {
    
// ==========================================================================
// SERVER::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct server::impl
    : public enable_shared_from_this<server::impl>
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(asio::io_service             &io_service
       , uint16_t                      port
       , server::accept_handler const &on_accept)
        : io_service_(io_service)
        , acceptor_(
            io_service
          , asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
        , on_accept_(on_accept)
    {
    }
       
    // ======================================================================
    // HANDLE_ACCEPT
    // ======================================================================
    void handle_accept(
        shared_ptr<asio::ip::tcp::socket> const &new_socket
      , boost::system::error_code const &error)
    {
        if (!error)
        {
// Really need to get a logging library.
#if DEBUG_SERVER
            printf("Connection from: %s\n",
                new_socket->remote_endpoint().address().to_string().c_str());
#endif
            
            boost::shared_ptr<odin::net::socket> socket(
                new odin::net::socket(new_socket));

            on_accept_(socket);

            schedule_accept();
        }
    }

    // ======================================================================
    // SCHEDULE_ACCEPT
    // ======================================================================
    void schedule_accept()
    {
        shared_ptr<asio::ip::tcp::socket> new_socket(
            new asio::ip::tcp::socket(io_service_));

        acceptor_.async_accept(
            *new_socket.get()
          , bind(
              &impl::handle_accept
            , shared_from_this()
            , new_socket
            , asio::placeholders::error));
    }
    
    // ======================================================================
    // CANCEL
    // ======================================================================
    void cancel()
    {
        boost::system::error_code unused_error_code;
        acceptor_.close(unused_error_code);
    }
    
    asio::io_service        &io_service_;
    asio::ip::tcp::acceptor  acceptor_;
    server::accept_handler   on_accept_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server(
    asio::io_service     &io_service
  , uint16_t              port
  , accept_handler const &on_accept)
    : pimpl_(new impl(io_service, port, on_accept))
{
    pimpl_->schedule_accept();
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
server::~server()
{
    // Ensure that the server is stopped.  This allows pimpl_ to be destroyed
    // (eventually) in all cases.
    pimpl_->cancel();
}

// ==========================================================================
// SHUTDOWN
// ==========================================================================
void server::shutdown()
{
    pimpl_->cancel();
}

}}


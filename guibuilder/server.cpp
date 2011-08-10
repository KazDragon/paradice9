// ==========================================================================
// GuiBuilder Socket
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
#include "server.hpp"
#include "odin/net/socket.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace boost;

namespace guibuilder
{
    
struct server::impl
{
    impl(asio::io_service                                     &io_service
       , uint16_t                                              port
       , function<void (shared_ptr<odin::net::socket>)> const &on_accept)
        : io_service_(io_service)
        , acceptor_(
            io_service
          , asio::ip::tcp::endpoint(
                asio::ip::tcp::v4()
              , port))
        , on_accept_(on_accept)
    {
        schedule_accept();
    }
       
    void handle_accept(
        shared_ptr<asio::ip::tcp::socket> const &new_socket
      , boost::system::error_code const &error)
    {
        if (!error)
        {
            BOOST_AUTO(
                paradice_socket
              , make_shared<odin::net::socket>(new_socket));
            on_accept_(paradice_socket);

            schedule_accept();
        }
    }

    void schedule_accept()
    {
        BOOST_AUTO(
            new_socket, 
            make_shared<asio::ip::tcp::socket>(ref(io_service_)));

        acceptor_.async_accept(
            *new_socket.get()
          , bind(
              &impl::handle_accept
            , this
            , new_socket
            , asio::placeholders::error));
    }
    
    asio::io_service                               &io_service_;
    asio::ip::tcp::acceptor                         acceptor_;
    
    function<void (shared_ptr<odin::net::socket>)>  on_accept_;
};

server::server(
    asio::io_service                                     &io_service
  , uint16_t                                              port
  , function<void (shared_ptr<odin::net::socket>)> const &on_accept)
    : pimpl_(new impl(io_service, port, on_accept))
{
}

server::~server()
{
}

}

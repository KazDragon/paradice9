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
#ifndef PARADICE_SERVER_HPP_
#define PARADICE_SERVER_HPP_

#include <boost/cstdint.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace boost { namespace asio {
    class io_service;
}}

namespace odin { namespace net {
    class socket;
    
//* =========================================================================
/// \brief Implements a tcp/ip server.
/// \par Usage
/// Construct, passing a Boost.Asio io_service, a port number, and a function
/// to call whenever a new connection is made.  The handler for these 
/// connections will be called in the io_service's run() method.  To stop the
/// server and cancel any pending acceptance, call stop().
//* =========================================================================
class server
{
public :
    typedef boost::function<
        void (boost::shared_ptr<odin::net::socket>)
    > accept_handler;
    
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    server(boost::asio::io_service &io_service
         , boost::uint16_t          port
         , accept_handler const    &on_accept);
        
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
   ~server();
    
    //* =====================================================================
    /// \brief Shuts the server down by no longer accepting incoming
    /// sockets.
    //* =====================================================================
    void shutdown();
    
private :
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}}

#endif

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
#ifndef PARADICE9_HPP_
#define PARADICE9_HPP_

#include "paradice/context.hpp"
#include <serverpp/core.hpp>
#include <boost/asio/io_context.hpp>
#include <memory>

namespace paradice9 {

//* =========================================================================
/// \brief A class that implements the main engine for the Paradice9 server.
/// \brief port - The server will be set up on this port number.
//* =========================================================================
class server final
{
public :
    //* =====================================================================
    /// Constructor
    //* =====================================================================
    server(
        boost::asio::io_context &io_context, 
        serverpp::port_identifier port,
        paradice::context &context);
    
    //* =====================================================================
    /// Destructor
    //* =====================================================================
    ~server();

    //* =====================================================================
    /// Shuts Paradice9 down.  All running threads are released.
    //* =====================================================================
    void shutdown();

private :
    struct impl;
    std::unique_ptr<impl> pimpl_;
};

}

#endif

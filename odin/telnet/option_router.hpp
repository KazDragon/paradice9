// ==========================================================================
// Odin Telnet Option Router
//
// Copyright (C) 2003 Matthew Chaplain, All Rights Reserved.
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
#ifndef ODIN_TELNET_OPTION_ROUTER_HPP_
#define ODIN_TELNET_OPTION_ROUTER_HPP_

#include "odin/telnet/option.hpp"
#include <boost/shared_ptr.hpp>

namespace odin { namespace telnet {

class stream;
class client_option;
class server_option;
        
// OPTION_ROUTER ============================================================
//  FUNCTION : Causes telnet negotiations to be forwarded to registered
//             option components.
//  USAGE    : Create an instance of a component, passing a telnet stream.
//             This router will register with the callback methods of that
//             stream.  Register client and server option components with the
//             router.  When telnet negotiations occur, the router will
//             send the appropriate messages to the relevant option 
//             components.
//
//             Note - option_router registers with the following methods:
//             on_negotiation_initiated
//             on_negotiation_completed
//             on_subnegotiation
//
//             on_command is not used; this must still be handled elsewhere.
// ==========================================================================
class option_router
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    public :
        option_router(boost::shared_ptr<stream> stream);

    // ======================================================================
    // DESTRUCTOR
    // ======================================================================
    public :
        ~option_router();
        
    // ======================================================================
    // OPTION REGISTRATION
    // ======================================================================
    public :
        void register_client_option(boost::shared_ptr<client_option> option);
        void register_server_option(boost::shared_ptr<server_option> option);
        
        void unregister_client_option(boost::shared_ptr<client_option> option);
        void unregister_server_option(boost::shared_ptr<server_option> option);

    // ======================================================================
    // PRIVATE IMPLEMENTATION
    // ======================================================================
    private :
        struct impl;
        impl *pimpl_;
};
        
}}

#endif


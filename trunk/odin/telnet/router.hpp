// ==========================================================================
// Odin Telnet Router
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
#ifndef ODIN_TELNET_ROUTER_HPP_
#define ODIN_TELNET_ROUTER_HPP_

#include "odin/telnet/protocol.hpp"
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace odin { namespace telnet {

class stream;

//* =========================================================================
/// \brief A class that routes incoming Telnet negotiations and 
/// subnegotiations to registered callbacks.
//* =========================================================================
class router : private boost::noncopyable
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    router(boost::shared_ptr<odin::telnet::stream> const &underlying_stream);
    
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    ~router();
    
    //* =====================================================================
    /// \brief Register a callback to take place whenever a negotiation is
    /// received with the specified option id and request.
    //* =====================================================================
    void register_negotiation(
        odin::telnet::negotiation_request_type negotiation_request
      , odin::telnet::option_id_type           option_id
      , boost::function<void ()>               callback);
    
    //* =====================================================================
    /// \brief Unregisters a previously registered negotiation.
    //* =====================================================================
    void unregister_negotiation(
        odin::telnet::negotiation_request_type negotiation_request
      , odin::telnet::option_id_type           option_id);
    
    //* =====================================================================
    /// \brief Register a callback to take place whenever a subnegotiation
    /// is received with the specified option id.
    //* =====================================================================
    void register_subnegotiation(
        odin::telnet::option_id_type option_id
      , boost::function<
            void (odin::telnet::subnegotiation_type)
        > const &callback);
    
    //* =====================================================================
    /// \brief Unregisters a previously registered subnegotiation
    //* =====================================================================
    void unregister_subnegotiation(odin::telnet::option_id_type option_id);
    
private :
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}}

#endif

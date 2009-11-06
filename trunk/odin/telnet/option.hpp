// ==========================================================================
// Odin Telnet Option.
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
#ifndef ODIN_TELNET_OPTION_HPP_
#define ODIN_TELNET_OPTION_HPP_

#include "odin/telnet/option_router.hpp"
#include "odin/telnet/protocol.hpp"
#include <boost/function.hpp>

namespace odin { namespace telnet {
    
// OPTION ===================================================================
//  FUNCTION : Encapsulates one side of a telnet option.
//  USAGE    : Create a concrete option by inheriting from this interface,
//             and overriding the functions as described below.  To use,
//             register with an instance of option_router.
//             If the user requires notification of the option changing state
//             then register a function call-back with the on_state_change()
//             function.
// ==========================================================================
struct option
{
    virtual ~option() {}
    
    // ======================================================================
    // OPTION::ACTIVATE
    //  FUNCTION : If this is a server, then a WILL will be sent.  If is a
    //             client, a DO will be sent.
    // ======================================================================
    virtual void activate() = 0;
    
    // ======================================================================
    // OPTION::DEACTIVATE
    //  FUCNTION : If this is a server, then a WONT will be sent.  If it is
    //             a client, then a DONT will be sent.
    // ======================================================================
    virtual void deactivate() = 0;
    
    // ======================================================================
    // OPTION::IS_ACTIVE
    //  FUNCTION : Returns true if the option is currently active.  Options
    //             will only receive subnegotiations if this returns true.
    // ======================================================================
    virtual bool is_active() const = 0;
    
    // ======================================================================
    // OPTION::IS_NEGOTIATING_ACTIVATION
    //  FUNCTION : Returns true if the user has requested activation, and
    //             no response has yet been received.
    // ======================================================================
    virtual bool is_negotiating_activation() const = 0;
    
    // ======================================================================
    // OPTION::IS_NEGOTIATING_DEACTIVATION
    //  FUNCTION : Returns true if the user has requested deactivation, and
    //             no response has yet been received.
    // ======================================================================
    virtual bool is_negotiating_deactivation() const = 0;
    
    // ======================================================================
    // OPTION::ON_ACTIVATE_REQUESTED
    //  FUNCTION : If this is a server, then this will be called whenever
    //             a DO is received.  If it is a client, then it will be 
    //             called whenever a WILL is received.
    //
    //  RETURNS  : true if the response should be WILL or DO.  False 
    //             otherwise.
    // ======================================================================
    virtual void on_activate_requested() = 0;

    // ======================================================================
    // OPTION::ON_ACTIVATED
    //  FUNCTION : Called when, after a call to activate, and a DO/WILL was 
    //             sent, a WILL/DO was returned.
    // ======================================================================
    virtual void on_activated() = 0;

    // ======================================================================
    // OPTION::ON_ACTIVATE_DENIED
    //  FUNCTION : Called when, after a call to activate, and a DO/WILL was
    //             sent, a WONT/DONT was returned.
    // ======================================================================
    virtual void on_activate_denied() = 0;

    // ======================================================================
    // OPTION::ON_DEACTIVATED
    //  FUNCTION : Called when, after a call to deactivate, and a WONT/DONT
    //             was sent, DONT/WONT was returned.
    // ======================================================================
    virtual void on_deactivated() = 0;
    
    // ======================================================================
    // OPTION::ON_REQUEST_DEACTIVATE
    //  FUNCTION : If this is a server, then this will be called whenever
    //             a DONT is received.  If it is a client, then it will be 
    //             called whenever a WONT is received.
    //
    //  RETURNS  : true if the response should be WONT or DONT.  False 
    //             otherwise.
    // ======================================================================
    virtual void on_deactivate_requested() = 0;

    // ======================================================================
    // OPTION::ON_DEACTIVATE_DENIED
    //  FUNCTION : Called when, after a call to deactivate, and a WONT/DONT
    //             was sent, WILL/DO was returned.
    // ======================================================================
    virtual void on_deactivate_denied() = 0;
    
    // ======================================================================
    // OPTION::ON_SUBNEGOTIATION
    //  FUNCTION : Called when a subnegotiation for this option is received.
    // ======================================================================
    virtual void on_subnegotiation(
        subnegotiation_type const &subnegotiation) = 0;

    // ======================================================================
    // OPTION::GET_NEGOTIATION_TYPE
    //  FUNCTION : Returns the negotiation type of this option (for example,
    //             odin::telnet::NAWS, or odin::telnet::RTCE).
    // ======================================================================
    virtual negotiation_type get_negotiation_type() const = 0;

    // ======================================================================
    // OPTION::ON_STATE_CHANGE
    //  FUNCTION : Set a call-back that is called whenever the option changes
    //             state from activated to deactivated or vice-versa.
    // ======================================================================
    virtual void on_state_change(boost::function<void ()> fn) = 0;
};
    
} }

#endif


// ==========================================================================
// Odin Telnet Server-Option.
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
#ifndef ODIN_TELNET_SERVER_OPTION_HPP_
#define ODIN_TELNET_SERVER_OPTION_HPP_

#include "odin/telnet/option.hpp"
#include "odin/telnet/stream.hpp"
#include "odin/telnet/protocol.hpp"
#include <boost/shared_ptr.hpp>

namespace odin { namespace telnet {
    
// SERVER_OPTION ================================================================
//  FUNCTION : Encapsulates the server side of a telnet option.
//             The server side is defined as the half of a negotiation that
//             responds to "DO" or "DONT", or broadcasts "WILL" or "WONT".
//
//             Note that some RFCs have this the wrong way up.  For example,
//             RFC 1073, Telnet NAWS, has its "server" -receiving- the WILL/
//             WONT messages.  This is because, with this option, it is usually
//             a server that has been connected to by a client that listens
//             to these messages.  Note that "server" in that context is 
//             different to the "server option" in this context.
//
//  USAGE    : Create a concrete option by inheriting from this interface,
//             and overriding the functions as described below.  To use,
//             register with an instance of option_router.
//
//             The user can attempt to activate or deactivate this option by
//             the functions activate() and deactivate(), respectively.  This
//             begins the negotiation process.  The functions 
//             is_negotiating_activation() and is_negotiating_deactivation() can
//             be used to interrogate the option to see if its negotiation is
//             complete yet, and is_active() can be used to observe the final
//             outcome of the negotiation.
//
//             Note: While this implementation follows the Telnet specification
//             in that DONT is always responded to with WONT, it also accepts 
//             that non-compliant clients may respond with WILL, and caters for
//             that.  
// ==============================================================================
class server_option : public option
{
    // ==========================================================================
    // CONSTRUCTOR
    // ==========================================================================
    public :
        server_option(boost::shared_ptr<odin::telnet::stream> stream);

    // ==========================================================================
    // DESTRUCTOR
    // ==========================================================================
    public :
        virtual ~server_option();
    
    // ==========================================================================
    // OPTION PUBLIC INTERFACE
    // ==========================================================================
    public :
        // ======================================================================
        // OPTION::ACTIVATE
        //  FUNCTION : If this is a server, then a WILL will be sent.  If is a
        //             client, a DO will be sent.
        // ======================================================================
        virtual void activate();
        
        // ======================================================================
        // OPTION::DEACTIVATE
        //  FUCNTION : If this is a server, then a WONT will be sent.  If it is
        //             a client, then a DONT will be sent.
        // ======================================================================
        virtual void deactivate();
        
        // ======================================================================
        // OPTION::IS_ACTIVE
        //  FUNCTION : Returns true if the option is currently active.  Options
        //             will only receive subnegotiations if this returns true.
        // ======================================================================
        virtual bool is_active() const;
        
        // ======================================================================
        // OPTION::IS_NEGOTIATING_ACTIVATION
        //  FUNCTION : Returns true if the user has requested activation, and
        //             no response has yet been received.
        // ======================================================================
        virtual bool is_negotiating_activation() const;
        
        // ======================================================================
        // OPTION::IS_NEGOTIATING_DEACTIVATION
        //  FUNCTION : Returns true if the user has requested deactivation, and
        //             no response has yet been received.
        // ======================================================================
        virtual bool is_negotiating_deactivation() const;
        
        // ======================================================================
        // OPTION::ON_ACTIVATE_REQUESTED
        //  FUNCTION : If this is a server, then this will be called whenever
        //             a DO is received.  If it is a client, then it will be 
        //             called whenever a WILL is received.
        //
        //  RETURNS  : true if the response should be WILL or DO.  False 
        //             otherwise.
        // ======================================================================
        virtual void on_activate_requested();
    
        // ======================================================================
        // OPTION::ON_ACTIVATED
        //  FUNCTION : Called when, after a call to activate, and a DO/WILL was 
        //             sent, a WILL/DO was returned.
        // ======================================================================
        virtual void on_activated();
    
        // ======================================================================
        // OPTION::ON_ACTIVATE_DENIED
        //  FUNCTION : Called when, after a call to activate, and a DO/WILL was
        //             sent, a WONT/DONT was returned.
        // ======================================================================
        virtual void on_activate_denied();
    
        // ======================================================================
        // OPTION::ON_DEACTIVATED
        //  FUNCTION : Called when, after a call to deactivate, and a WONT/DONT
        //             was sent, DONT/WONT was returned.
        // ======================================================================
        virtual void on_deactivated();
        
        // ======================================================================
        // OPTION::ON_REQUEST_DEACTIVATE
        //  FUNCTION : If this is a server, then this will be called whenever
        //             a DONT is received.  If it is a client, then it will be 
        //             called whenever a WONT is received.
        //
        //  RETURNS  : true if the response should be WONT or DONT.  False 
        //             otherwise.
        // ======================================================================
        virtual void on_deactivate_requested();
    
        // ======================================================================
        // OPTION::ON_DEACTIVATE_DENIED
        //  FUNCTION : Called when, after a call to deactivate, and a WONT/DONT
        //             was sent, WILL/DO was returned.
        // ======================================================================
        virtual void on_deactivate_denied();
        
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
        virtual void on_state_change(boost::function<void ()> fn);
        
    // ==========================================================================
    // PUBLIC MEMBER FUNCTIONS
    // ==========================================================================
    public :
        // ======================================================================
        // SERVER_OPTION::SET_ACTIVATABLE
        //  FUNCTION : When true is passed to this method, this will put the
        //             option into a state when a remote request to activate
        //             the option will cause the option to be activated.  When
        //             false is passed, the converse occurs.  The default is
        //             false.
        // ======================================================================
        void set_activatable(bool activatable);
        
    // ==========================================================================
    // PRIVATE IMPLEMENTATION
    // ==========================================================================
    private :
        struct impl;
        impl *pimpl_;
};
    
} }

#endif


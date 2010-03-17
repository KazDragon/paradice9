// ==========================================================================
// Odin Telnet Client-Option.
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
#ifndef ODIN_TELNET_CLIENT_OPTION_HPP_
#define ODIN_TELNET_CLIENT_OPTION_HPP_

#include "odin/telnet/protocol.hpp"
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace odin { namespace telnet {

class negotiation_router;
class subnegotiation_router;
class stream;

//* =========================================================================
/// \brief Encapsulates the client side of a telnet option (the side of the
/// option that responds to "WILL" or "WONT", or broadcasts "DO" or "DONT".
//* =========================================================================

// CLIENT_OPTION ================================================================
//  FUNCTION : Encapsulates the client side of a telnet option.
//             The client side is defined as the half of a negotiation that
//             responds to "WILL" or "WONT", or broadcasts "DO" or "DONT".
//
//             Note that some RFCs have this the wrong way up.  For example,
//             RFC 1073, Telnet NAWS, has its "client" -sending- the WILL/
//             WONT messages.  This is because, with this option, it is usually
//             a server that has been connected to by a client that listens
//             to these messages.  Note that "client" in that context is 
//             different to the "client option" in this context.
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
//             in that WONT is always responded to with DONT, it also accepts 
//             that non-compliant servers may respond with DO, and caters for
//             that.  
// ==============================================================================
class client_option
{
public :    
    //* =====================================================================
    /// \brief Constructor
    /// \param option_id The option id that this option will exhibit.  For
    /// example, odin::telnet::NAWS.
    //* =====================================================================
    client_option(
        boost::shared_ptr<stream> const                &stream
      , boost::shared_ptr<negotiation_router> const    &negotiation_router
      , boost::shared_ptr<subnegotiation_router> const &subnegotiation_router
      , option_id_type                                  option_id);

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~client_option();

    //* =====================================================================
    /// \brief Returns the option ID of this client option.
    //* =====================================================================
    option_id_type get_option_id() const;

    //* =====================================================================
    /// \brief Attempts to activate the option.
    ///
    /// If the option is not already active, sends a DO request.  
    /// Upon a WILL response, the option is activated. Upon a WONT response, 
    /// the option remains inactive.  As soon as the negotiation is complete,
    /// any registered on_request_complete handler is fired.
    //* =====================================================================
    void activate();

    //* =====================================================================
    /// \brief Attempts to deactivate the option.
    ///
    /// If the option is not already deactivated, sends a DONT request.  
    /// Upon a WONT response, the option is deactivated.  Although it is 
    /// non-standard for the server to then respond with WILL, this is 
    /// handled and the option remains active.  As soon as the negotiation is 
    /// complete, any registered on_request_complete handler is fired.
    //* =====================================================================
    void deactivate();
    
    //* =====================================================================
    /// \brief Returns whether the option is active.
    /// 
    /// A client option is considered active if it has broadcast a DO and 
    /// been responded to with WILL, or has been sent a WILL and reacted with 
    /// DO.
    //* =====================================================================
    bool is_active() const;
    
    //* =====================================================================
    /// \brief Set whether the option can be activated by the server.
    //* =====================================================================
    void set_activatable(bool activatable);
    
    //* =====================================================================
    /// \brief Returns whether the option is activatable by the server.
    //* =====================================================================
    bool is_activatable() const;
    
    //* =====================================================================
    /// \brief Returns whether the option is currently negotiating
    /// activation.
    ///
    /// A client option is negotiating activation if it has sent a DO and  
    /// is awaiting a response.
    //* =====================================================================
    bool is_negotiating_activation() const;
    
    //* =====================================================================
    /// \brief Returns whether the option is currently negotiating
    /// deactivation.
    ///
    /// A client option is negotiating deactivation if it has sent a DONT
    /// and is awaiting a response.
    //* =====================================================================
    bool is_negotiating_deactivation() const;
    
    //* =====================================================================
    /// \brief Sets a callback to be called whenever a request is completed.
    //* =====================================================================
    void on_request_complete(boost::function<void ()> const &callback);
    
private :
    //* =====================================================================
    /// \brief Called when a subnegotiation for this option is received
    /// while the option is active.
    //* =====================================================================
    virtual void on_subnegotiation(
        subnegotiation_type const &subnegotiation) = 0;
    
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

} }

#endif


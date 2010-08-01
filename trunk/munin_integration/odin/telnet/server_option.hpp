// ==========================================================================
// Odin Telnet Server-Option.
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
#ifndef ODIN_TELNET_SERVER_OPTION_HPP_
#define ODIN_TELNET_SERVER_OPTION_HPP_

#include "odin/telnet/stream.hpp"
#include "odin/telnet/protocol.hpp"
#include <boost/shared_ptr.hpp>

namespace odin { namespace telnet {
    
class negotiation_router;
class subnegotiation_router;
class stream;
    
//* =========================================================================
/// \brief Encapsulates the server side of a telnet option (the side of the
/// option that responds to "DO" or "DONT", or broadcasts "WILL" or "WONT".
/// 
//* =========================================================================
    
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
class server_option
{
public :
    //* =====================================================================
    /// \brief Constructor
    /// \param option_id The option id that this option will exhibit.  For
    /// example, odin::telnet::NAWS.
    //* =====================================================================
    server_option(
        boost::shared_ptr<stream> const                &stream
      , boost::shared_ptr<negotiation_router> const    &negotiation_router
      , boost::shared_ptr<subnegotiation_router> const &subnegotiation_router
      , option_id_type                                  option_id);

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~server_option();

    //* =====================================================================
    /// \brief Returns the option ID of this client option.
    //* =====================================================================
    option_id_type get_option_id() const;

    //* =====================================================================
    /// \brief Attempts to activate the option.
    ///
    /// If the option is not already active, sends a WILL request.  
    /// Upon a DO response, the option is activated. Upon a DONT response, 
    /// the option remains inactive.  As soon as the negotiation is complete,
    /// any registered on_request_complete handler is fired.
    //* =====================================================================
    void activate();

    //* =====================================================================
    /// \brief Attempts to deactivate the option.
    ///
    /// If the option is not already deactivated, sends a WONT request.  
    /// Upon a DONT response, the option is deactivated.  Although it is 
    /// non-standard for the server to then respond with DO, this is handled
    /// and the option remains active.  As soon as the negotiation is 
    /// complete, any registered on_request_complete handler is fired.
    //* =====================================================================
    void deactivate();
    
    //* =====================================================================
    /// \brief Returns whether the option is active.
    /// 
    /// A client option is considered active if it has broadcast a WILL and 
    /// been responded to with DO, or has been sent a DO and reacted with 
    /// WILL.
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
    /// A client option is negotiating activation if it has sent a WILL and  
    /// is awaiting a response.
    //* =====================================================================
    bool is_negotiating_activation() const;
    
    //* =====================================================================
    /// \brief Returns whether the option is currently negotiating
    /// deactivation.
    ///
    /// A client option is negotiating deactivation if it has sent a WONT
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
    
    class impl;
    boost::shared_ptr<impl> pimpl_;
};
    
} }

#endif


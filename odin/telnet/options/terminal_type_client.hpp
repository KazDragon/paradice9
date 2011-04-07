// ==========================================================================
// Odin Telnet Terminal Type Client Option
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
#ifndef ODIN_TELNET_OPTIONS_TERMINAL_TYPE_CLIENT_HPP_
#define ODIN_TELNET_OPTIONS_TERMINAL_TYPE_CLIENT_HPP_

#include "odin/telnet/client_option.hpp"
#include <boost/function.hpp>
#include <string>

namespace odin { namespace telnet { 
    
class stream;
class negotiation_router;
class subnegotiation_router;
    
namespace options {

//* ==========================================================================
/// \brief A class that implements the Telnet Client side of the Terminal 
/// Type option (RFC 1091).
//* ==========================================================================
class terminal_type_client : public odin::telnet::client_option
{
public :
    //* ======================================================================
    /// \brief Constructor
    //* ======================================================================
    terminal_type_client(
        boost::shared_ptr<odin::telnet::stream> const                &stream
      , boost::shared_ptr<odin::telnet::negotiation_router> const    &negotiation_router
      , boost::shared_ptr<odin::telnet::subnegotiation_router> const &subnegotiation_router);

    //* ======================================================================
    /// \brief Destructor
    //* ======================================================================
    virtual ~terminal_type_client();
    
    //* =====================================================================
    /// \brief Sends a request to the server to enumerate a terminal type.
    //* =====================================================================
    void send_request();
    
    //* =====================================================================
    /// \brief Sets up a callback to be notified when a new terminal type
    /// has been detected.
    //* =====================================================================
    void on_terminal_type_detected(
        boost::function<void (std::string)> callback);
    
private :
    //* =====================================================================
    /// \brief Called when a subnegotiation for this option is received
    /// while the option is active.
    //* =====================================================================
    virtual void on_subnegotiation(
        subnegotiation_type const &subnegotiation);
    
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}}}

#endif

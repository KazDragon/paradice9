// ==========================================================================
// Odin Telnet Negotiate About Window Size (NAWS) Client Option
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
#ifndef ODIN_TELNET_OPTIONS_NAWS_CLIENT_HPP_
#define ODIN_TELNET_OPTIONS_NAWS_CLIENT_HPP_

#include "odin/telnet/client_option.hpp"
#include <boost/function.hpp>

namespace odin { namespace telnet { namespace options {

//* ==========================================================================
/// \brief A class that implements the Telnet Client side of the Negotiate
/// About Window Size option (RFC 1073).
//* ==========================================================================
class naws_client : public odin::telnet::client_option
{
public :
    //* =====================================================================
    /// \brief A type that describes the callback called whenever the server
    /// announces that its window size has changed.
    //* =====================================================================
    typedef boost::function<
        void (odin::u16 width, odin::u16 height)> callback_type;
    
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    naws_client(
        boost::shared_ptr<odin::telnet::stream> const                &stream
      , boost::shared_ptr<odin::telnet::negotiation_router> const    &negotiation_router
      , boost::shared_ptr<odin::telnet::subnegotiation_router> const &subnegotiation_router);

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~naws_client();
    
    //* =====================================================================
    /// \brief Register a callback to be called whenever the client receives
    /// a window size update from the server.
    //* =====================================================================
    void on_size(callback_type const &callback);
    
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


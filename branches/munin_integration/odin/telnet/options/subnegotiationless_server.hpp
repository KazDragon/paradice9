// ==========================================================================
// Odin Subnegotiationless Server Telnet Option
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
#ifndef ODIN_TELNET_OPTIONS_SUBNEGOTIATIONLESS_SERVER_HPP_
#define ODIN_TELNET_OPTIONS_SUBNEGOTIATIONLESS_SERVER_HPP_

#include "odin/telnet/server_option.hpp"

namespace odin { namespace telnet {
    class negotiation_router;
    class subnegotiation_router;
    class stream;
}}

namespace odin { namespace telnet { namespace options {

//* =========================================================================
/// \class odin::telnet::options::subnegotiationless_server<>
/// \brief A template that implements a pattern to be followed by any
/// Telnet Server side of the an option that has no subnegotiations.
//* =========================================================================
template <option_id_type OptionId>    
class subnegotiationless_server : public odin::telnet::server_option
{
//* =========================================================================
/// \brief Constructor
// ==========================================================================
public :
    subnegotiationless_server(
        boost::shared_ptr<odin::telnet::stream> const                &stream
      , boost::shared_ptr<odin::telnet::negotiation_router> const    &negotiation_router
      , boost::shared_ptr<odin::telnet::subnegotiation_router> const &subnegotiation_router)
        : odin::telnet::server_option(
            stream, negotiation_router, subnegotiation_router, OptionId)
    {
    }
    
private :    
    //* =====================================================================
    /// \brief Called when a subnegotiation for this option is received
    /// while the option is active.
    //* =====================================================================
    virtual void on_subnegotiation(subnegotiation_type const &subnegotiation)
    {
    }
};
             
}}}

#endif


// ==========================================================================
// Odin Subnegotiationless Client Telnet Option
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
#ifndef ODIN_TELNET_OPTIONS_SUBNEGOTIATIONLESS_CLIENT_HPP_
#define ODIN_TELNET_OPTIONS_SUBNEGOTIATIONLESS_CLIENT_HPP_

#include "odin/telnet/client_option.hpp"

namespace odin { namespace telnet { namespace options {

template <odin::telnet::negotiation_type NegotiationType>            
class subnegotiationless_client : public odin::telnet::client_option
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    public :
        subnegotiationless_client(boost::shared_ptr<odin::telnet::stream> stream)
            : client_option(stream)
        {
        }
        
    // ======================================================================
    // CLIENT_OPTION PUBLIC INTERFACE
    // ======================================================================
    public :
        // ==================================================================
        // OPTION::ON_SUBNEGOTIATION
        //  FUNCTION : Called when a subnegotiation for this option is 
        //             received.
        // ==================================================================
        virtual void on_subnegotiation(ksubnegotiation data, unsigned int len)
        {
        }

        // ==================================================================
        // OPTION::GET_NEGOTIATION_TYPE
        //  FUNCTION : Returns the negotiation type of this option (for 
        //             example, odin::telnet::NAWS, or odin::telnet::RTCE).
        // ==================================================================
        virtual negotiation_type get_negotiation_type() const
        {
            return NegotiationType;
        }
};
             
}}}

#endif


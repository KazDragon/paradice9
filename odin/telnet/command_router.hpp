// ==========================================================================
// Odin Telnet Command Router
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
#ifndef ODIN_TELNET_COMMAND_ROUTER_HPP_
#define ODIN_TELNET_COMMAND_ROUTER_HPP_

#include "odin/telnet/protocol.hpp"
#include "odin/router.hpp"

namespace odin { namespace telnet {

namespace detail {
    
    struct command_router_key_from_message_policy
    {
        static odin::telnet::command_type key_from_message(
            odin::telnet::command_type message);
    };
}

//* =========================================================================
/// \brief A class that routes incoming Telnet negotiations and 
/// subnegotiations to registered callbacks.
//* =========================================================================
class command_router 
    : public odin::router
      <
          odin::telnet::command_type
        , odin::telnet::command_type
        , odin::telnet::detail::command_router_key_from_message_policy
      >
{
};

}}

#endif

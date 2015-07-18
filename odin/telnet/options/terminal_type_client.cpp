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
#include "odin/telnet/options/terminal_type_client.hpp"

namespace odin { namespace telnet { namespace options {

// ==========================================================================
// TERMINAL_TYPE_CLIENT::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct terminal_type_client::impl
{
    std::function<void (std::string const &)> on_terminal_type_detected_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
terminal_type_client::terminal_type_client(
    std::shared_ptr<odin::telnet::stream>                stream,
    std::shared_ptr<odin::telnet::negotiation_router>    negotiation_router,
    std::shared_ptr<odin::telnet::subnegotiation_router> subnegotiation_router)
  : client_option(
        stream,
        negotiation_router,
        subnegotiation_router,
        odin::telnet::TERMINAL_TYPE),
    pimpl_(new impl)
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
terminal_type_client::~terminal_type_client()
{
}

// ==========================================================================
// SEND_REQUEST
// ==========================================================================
void terminal_type_client::send_request()
{
    odin::telnet::subnegotiation_content_type content;
    content.push_back(odin::telnet::TERMINAL_TYPE_SEND);
    send_subnegotiation(content);
}

// ==========================================================================
// ON_SUBNEGOTIATION
// ==========================================================================
void terminal_type_client::on_subnegotiation(
    subnegotiation_type const &subnegotiation)
{
    if (pimpl_->on_terminal_type_detected_
     && subnegotiation.content_.size() > 1
     && subnegotiation.content_[0] == odin::telnet::TERMINAL_TYPE_IS)
    {
        std::string response;

        response.insert(
            response.end()
          , subnegotiation.content_.begin() + 1
          , subnegotiation.content_.end());

        pimpl_->on_terminal_type_detected_(response);
    }
}

// ==========================================================================
// ON_TERMINAL_TYPE_DETECTED
// ==========================================================================
void terminal_type_client::on_terminal_type_detected(
    std::function<void (std::string const &)> const &callback)
{
    pimpl_->on_terminal_type_detected_ = callback;
}

}}}


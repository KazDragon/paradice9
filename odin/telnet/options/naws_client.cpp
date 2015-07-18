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
#include "odin/telnet/options/naws_client.hpp"
#include "odin/telnet/protocol.hpp"

namespace odin { namespace telnet { namespace options {

// ==========================================================================
// NAWS_CLIENT::IMPL
// ==========================================================================
struct naws_client::impl
{
    naws_client::callback_type on_size_;
};
    
// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
naws_client::naws_client(
    std::shared_ptr<odin::telnet::stream>                stream,
    std::shared_ptr<odin::telnet::negotiation_router>    negotiation_router,
    std::shared_ptr<odin::telnet::subnegotiation_router> subnegotiation_router)
  : odin::telnet::client_option(
        stream,
        negotiation_router,
        subnegotiation_router,
        odin::telnet::NAWS),
    pimpl_(std::make_shared<impl>())
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
naws_client::~naws_client()
{
}

// ==========================================================================
// ON_SIZE
// ==========================================================================
void naws_client::on_size(callback_type const &callback)
{
    pimpl_->on_size_ = callback;
}

// ==========================================================================
// ON_SUBNEGOTIATION
// ==========================================================================
void naws_client::on_subnegotiation(subnegotiation_type const &subnegotiation)
{
    if (pimpl_->on_size_ && subnegotiation.content_.size() == 4)
    {
        odin::u16 width = 
            odin::u16(subnegotiation.content_[0] << 8)
          | odin::u16(subnegotiation.content_[1] << 0);
          
        odin::u16 height =
            odin::u16(subnegotiation.content_[2] << 8)
          | odin::u16(subnegotiation.content_[3] << 0);
          
        pimpl_->on_size_(width, height);
    }
}

}}}


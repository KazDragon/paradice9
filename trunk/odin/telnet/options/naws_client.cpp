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
// NAWS_CLIENT::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct naws_client::impl
{
    naws_client::fn fn_;
};

// ==========================================================================
// NAWS_CLIENT::CONSTRUCTOR
// ==========================================================================
naws_client::naws_client(boost::shared_ptr<odin::telnet::stream> stream)
    : client_option(stream)
    , pimpl_(new impl)
{
}
            
// ==========================================================================
// NAWS_CLIENT::DESTRUCTOR
// ==========================================================================
naws_client::~naws_client()
{
    delete pimpl_;
}

// ==========================================================================
// NAWS_CLIENT::GET_NEGOTIATION_TYPE
// ==========================================================================
odin::telnet::negotiation_type naws_client::get_negotiation_type() const
{
    return odin::telnet::NAWS;
}

// ==========================================================================
// NAWS_CLIENT::ON_SUBNEGOTIATION
// ==========================================================================
void naws_client::on_subnegotiation(
    subnegotiation_type const &subnegotiation)
{
    if(subnegotiation.size() == 4)
    {
        if(pimpl_->fn_)
        {
            unsigned short width = 
                (unsigned short)(subnegotiation[0]) << 8
              | (unsigned short)(subnegotiation[1]);
            
            unsigned short height =
                (unsigned short)(subnegotiation[2]) << 8
              | (unsigned short)(subnegotiation[3]);
              
            pimpl_->fn_(width, height);
        }
    }
}

// ==========================================================================
// NAWS_CLIENT::ON_SIZE_SET
// ==========================================================================
void naws_client::on_size_set(fn f)
{
    pimpl_->fn_ = f;
}
            
}}}


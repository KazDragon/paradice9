// ==========================================================================
// Odin Telnet Negotiate About Window Size (NAWS) Server Option
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
#include "odin/telnet/options/naws_server.hpp"
#include "odin/telnet/protocol.hpp"

namespace odin { namespace telnet { namespace options {


// ==========================================================================
// NAWS_SERVER::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct naws_server::impl
{
    boost::shared_ptr<odin::telnet::stream> stream_;
};

// ==========================================================================
// NAWS_SERVER::CONSTRUCTOR
// ==========================================================================
naws_server::naws_server(boost::shared_ptr<odin::telnet::stream> stream)
    : server_option(stream)
    , pimpl_(new impl)
{
    pimpl_->stream_ = stream;
}

// ==========================================================================
// NAWS_SERVER::DESTRUCTOR
// ==========================================================================
naws_server::~naws_server()
{
    delete pimpl_;
}

// ==========================================================================
// NAWS_SERVER::GET_NEGOTIATION_TYPE
// ==========================================================================
odin::telnet::negotiation_type naws_server::get_negotiation_type() const
{
    return odin::telnet::NAWS;
}

// ==========================================================================
// NAWS_SERVER::SET_SIZE
// ==========================================================================
void naws_server::set_size(unsigned short width, unsigned short height)
{
    if(is_active())
    {
        odin::u8 const subnegotiation[] =
        {
            odin::u8(width >> 8),
            odin::u8(width),
            odin::u8(height >> 8),
            odin::u8(height)
        };

        pimpl_->stream_->send_subnegotiation(
            odin::telnet::NAWS, subnegotiation);
    }
}

}}}


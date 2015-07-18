// ==========================================================================
// Munin Element.
//
// Copyright (C) 2014 Matthew Chaplain, All Rights Reserved.
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
#include "yggdrasil/munin/element.hpp"
#include <boost/format.hpp>
#include <ostream>
#include <utility>
#include <cctype>

namespace yggdrasil { namespace munin {

// ==========================================================================
// ELEMENT DEFAULT CONSTRUCTOR
// ==========================================================================
element::element()
{
}

// ==========================================================================
// ELEMENT CONSTRUCTOR
// ==========================================================================
element::element(glyph gly, attribute attr)
  : glyph_(std::move(gly)),
    attribute_(std::move(attr))
{
}

// ==========================================================================
// ELEMENT ASSIGNMENT OPERATOR (Glyph)
// ==========================================================================
element &element::operator=(glyph const &gly)
{
    glyph_ = gly;
    attribute_ = {};
    return *this;
}

// ==========================================================================
// ELEMENT OPERATOR==
// ==========================================================================
bool operator==(element const &lhs, element const &rhs)
{
    return lhs.glyph_     == rhs.glyph_
        && lhs.attribute_ == rhs.attribute_;
}

// ==========================================================================
// ELEMENT OPERATOR!=
// ==========================================================================
bool operator!=(element const &lhs, element const &rhs)
{
    return !(lhs == rhs);
}

// ==========================================================================
// OSTREAM << ELEMENT
// ==========================================================================
std::ostream &operator<<(std::ostream &out, element const &element)
{
    out << "element['";
    
    if (isprint(element.glyph_.character_))
    {
        out << element.glyph_.character_;
    }
    else
    {
        out << boost::format("0x%02X")
            % int((unsigned char)(element.glyph_.character_));
    }
    
    out << "', ";
    
    out << element.attribute_;
    
    out << "]";
    
    return out;
}

}}

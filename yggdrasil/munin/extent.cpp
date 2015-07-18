// ==========================================================================
// Munin Extent.
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
#include "yggdrasil/munin/extent.hpp"
#include <ostream>

namespace yggdrasil { namespace munin {

// ==========================================================================
// EXTENT CONSTRUCTOR
// ==========================================================================
extent::extent()
  : width(0),
    height(0)
{
}

// ==========================================================================
// EXTENT CONSTRUCTOR
// ==========================================================================
extent::extent(yggdrasil::s32 w, yggdrasil::s32 h)
  : width(w),
    height(h)
{
}

// ==========================================================================
// EXTENT OPERATOR+=
// ==========================================================================
extent &extent::operator+=(extent const &rhs)
{
    width  += rhs.width;
    height += rhs.height;
    return *this;
}

// ==========================================================================
// EXTENT OPERATOR-=
// ==========================================================================
extent &extent::operator-=(extent const &rhs)
{
    width  -= rhs.width;
    height -= rhs.height;
    return *this;
}

// ==========================================================================
// OPERATOR==(EXTENT,EXTENT)
// ==========================================================================
bool operator==(extent const &lhs, extent const &rhs)
{
    return lhs.width == rhs.width && lhs.height == rhs.height;
}

// ==========================================================================
// OPERATOR!=(EXTENT,EXTENT)
// ==========================================================================
bool operator!=(extent const &lhs, extent const &rhs)
{
    return !(lhs == rhs);
}

// ==========================================================================
// OPERATOR+(EXTENT,EXTENT)
// ==========================================================================
extent operator+(extent lhs, extent const &rhs)
{
    return lhs += rhs;
}

// ==========================================================================
// OPERATOR-(EXTENT,EXTENT)
// ==========================================================================
extent operator-(extent lhs, extent const &rhs)
{
    return lhs -= rhs;
}

// ==========================================================================
// OSTREAM << EXTENT
// ==========================================================================
std::ostream &operator<<(std::ostream &out, extent const &ext)
{
    out << "extent(" << ext.width << ", " << ext.height << ")";
    return out;
}

}}

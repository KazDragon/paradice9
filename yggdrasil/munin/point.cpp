// ==========================================================================
// Munin Point.
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
#include "yggdrasil/munin/point.hpp"
#include <ostream>

namespace yggdrasil { namespace munin {

// ==========================================================================
// POINT CONSTRUCTOR
// ==========================================================================
point::point()
  : x(0),
    y(0)
{
}

// ==========================================================================
// POINT CONSTRUCTOR
// ==========================================================================
point::point(yggdrasil::s32 x_coordinate, yggdrasil::s32 y_coordinate)
  : x(x_coordinate),
    y(y_coordinate)
{
}

// ==========================================================================
// POINT OPERATOR+=
// ==========================================================================
point &point::operator+=(point const &rhs)
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

// ==========================================================================
// POINT OPERATOR-=
// ==========================================================================
point &point::operator-=(point const &rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

// ==========================================================================
// OPERATOR==(POINT,POINT)
// ==========================================================================
bool operator==(point const &lhs, point const &rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

// ==========================================================================
// OPERATOR!=(POINT,POINT)
// ==========================================================================
bool operator!=(point const &lhs, point const &rhs)
{
    return !(lhs == rhs);
}

// ==========================================================================
// OPERATOR+(POINT,POINT)
// ==========================================================================
point operator+(point lhs, point const &rhs)
{
    return lhs += rhs;
}

// ==========================================================================
// OPERATOR-(POINT,POINT)
// ==========================================================================
point operator-(point lhs, point const &rhs)
{
    return lhs -= rhs;
}

// ==========================================================================
// OSTREAM << POINT
// ==========================================================================
std::ostream &operator<<(std::ostream &out, point const &pt)
{
    out << "point(" << pt.x << ", " << pt.y << ")";
    return out;
}

}}

// ==========================================================================
// Munin Types.
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
#include "munin/types.hpp"
#include <boost/format.hpp>
#include <iostream>

namespace munin {

// ==========================================================================
// ELEMENT_TYPE DEFAULT CONSTRUCTOR
// ==========================================================================
element_type::element_type()
{
}

// ==========================================================================
// ELEMENT_TYPE CONSTRUCTOR
// ==========================================================================
element_type::element_type(glyph gly, attribute attr)
    : glyph_(gly)
    , attribute_(attr)
{
}

// ==========================================================================
// ELEMENT_TYPE OPERATOR==
// ==========================================================================
bool operator==(element_type const &lhs, element_type const &rhs)
{
    return lhs.glyph_     == rhs.glyph_
        && lhs.attribute_ == rhs.attribute_;
}

// ==========================================================================
// OSTREAM << ELEMENT_TYPE
// ==========================================================================
std::ostream &operator<<(std::ostream &out, element_type const &element)
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

// ==========================================================================
// POINT CONSTRUCTOR
// ==========================================================================
point::point()
    : x(0)
    , y(0)
{
}

// ==========================================================================
// POINT CONSTRUCTOR
// ==========================================================================
point::point(odin::s32 x_coordinate, odin::s32 y_coordinate)
    : x(x_coordinate)
    , y(y_coordinate)
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

// ==========================================================================
// EXTENT CONSTRUCTOR
// ==========================================================================
extent::extent()
    : width(0)
    , height(0)
{
}

// ==========================================================================
// EXTENT CONSTRUCTOR
// ==========================================================================
extent::extent(odin::s32 w, odin::s32 h)
    : width(w)
    , height(h)
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

// ==========================================================================
// RECTANGLE CONSTRUCTOR
// ==========================================================================
rectangle::rectangle()
{
}

// ==========================================================================
// RECTANGLE CONSTRUCTOR
// ==========================================================================
rectangle::rectangle(point org, extent sz)
    : origin(org)
    , size(sz)
{
}

// ==========================================================================
// OPERATOR+(POINT,EXTENT)
// ==========================================================================
rectangle operator+(point const &pt, extent const &ext)
{
    return rectangle(pt, ext);
}

// ==========================================================================
// OPERATOR==(RECTANGLE,RECTANGLE)
// ==========================================================================
bool operator==(rectangle const &lhs, rectangle const &rhs)
{
    return lhs.origin == rhs.origin && lhs.size == rhs.size;
}

// ==========================================================================
// OPERATOR!=(RECTANGLE,RECTANGLE)
// ==========================================================================
bool operator!=(rectangle const &lhs, rectangle const &rhs)
{
    return !(lhs == rhs);
}

// ==========================================================================
// OSTREAM << RECTANGLE
// ==========================================================================
std::ostream &operator<<(std::ostream &out, rectangle const &rect)
{
    out << "rectangle[" << rect.origin << ", " << rect.size << "]";
    return out;
}

}


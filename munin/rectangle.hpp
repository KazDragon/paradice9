// ==========================================================================
// Munin Rectangle.
//
// Copyright (C) 2015 Matthew Chaplain, All Rights Reserved.
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
#ifndef MUNIN_RECTANGLE_HPP_
#define MUNIN_RECTANGLE_HPP_

#include "munin/export.hpp"
#include <terminalpp/point.hpp>
#include <terminalpp/extent.hpp>
#include <iosfwd>

namespace munin {

//* =========================================================================
/// \brief A class that represents a rectangle in space.
//* =========================================================================
struct rectangle
{
    //* =====================================================================
    /// \brief Default Constructor
    /// \par
    /// Constructs the rectangle, leaving the origin and size uninitialised.
    //* =====================================================================
    constexpr rectangle() = default;

    //* =====================================================================
    /// \brief Constructor
    /// \par
    /// Constructs the rectangle, using the specified origin and size.
    //* =====================================================================
    constexpr rectangle(terminalpp::point org, terminalpp::extent sz)
      : origin(org),
        size(sz)
    {
    }

    terminalpp::point  origin;
    terminalpp::extent size;
};

// ==========================================================================
// OPERATOR+(POINT,EXTENT)
// ==========================================================================
constexpr rectangle operator+(
    terminalpp::point const &pt, terminalpp::extent const &ext)
{
    return rectangle(pt, ext);
}

// ==========================================================================
// OPERATOR==(RECTANGLE,RECTANGLE)
// ==========================================================================
constexpr bool operator==(rectangle const &lhs, rectangle const &rhs)
{
    return lhs.origin == rhs.origin && lhs.size == rhs.size;
}

// ==========================================================================
// OPERATOR!=(RECTANGLE,RECTANGLE)
// ==========================================================================
constexpr bool operator!=(rectangle const &lhs, rectangle const &rhs)
{
    return !(lhs == rhs);
}

// ==========================================================================
// OPERATOR<<(RECTANGLE)
// ==========================================================================
MUNIN_EXPORT
std::ostream& operator<<(std::ostream &out, rectangle const &rect);


}

#endif
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
#ifndef YGGDRASIL_MUNIN_EXTENT_HPP_
#define YGGDRASIL_MUNIN_EXTENT_HPP_

#include "yggdrasil/types.hpp"
#include <iosfwd>

namespace yggdrasil { namespace munin {

//* =========================================================================
/// \brief A class that represents a direction with distance in space
/// (a vector).
/// \par
/// A class that represents a direction in space, with width being the
/// extent in the horizontal axis, and height being the extent in the
/// vertical axis.
//* =========================================================================
struct extent
{
    //* =====================================================================
    /// \brief Default Constructor
    /// \par
    /// Constructs an extent, leaving the width and height zeroed.
    //* =====================================================================
    constexpr extent()
      : width(0),
        height(0)
    {
    }
    
    //* =====================================================================
    /// \brief Constructor
    /// \par
    /// Constructs an extent with width and height being set to the passed-in
    /// arguments.
    //* =====================================================================
    constexpr extent(yggdrasil::s32 w, yggdrasil::s32 h)
      : width(w),
        height(h)
    {
    }
    
    //* =====================================================================
    /// \brief Addition
    //* =====================================================================
    constexpr extent &operator+=(extent const &rhs)
    {
        width  += rhs.width;
        height += rhs.height;
        return *this;
    }
    
    //* =====================================================================
    /// \brief Subtraction
    //* =====================================================================
    constexpr extent &operator-=(extent const &rhs)
    {
        width  -= rhs.width;
        height -= rhs.height;
        return *this;
    }
    
    yggdrasil::s32 width;
    yggdrasil::s32 height;
};

// ==========================================================================
// OPERATOR==(EXTENT,EXTENT)
// ==========================================================================
constexpr bool operator==(extent const &lhs, extent const &rhs)
{
    return lhs.width == rhs.width && lhs.height == rhs.height;
}

// ==========================================================================
// OPERATOR!=(EXTENT,EXTENT)
// ==========================================================================
constexpr bool operator!=(extent const &lhs, extent const &rhs)
{
    return !(lhs == rhs);
}

// ==========================================================================
// OPERATOR+(EXTENT,EXTENT)
// ==========================================================================
constexpr extent operator+(extent lhs, extent const &rhs)
{
    return lhs += rhs;
}

// ==========================================================================
// OPERATOR-(EXTENT,EXTENT)
// ==========================================================================
constexpr extent operator-(extent lhs, extent const &rhs)
{
    return lhs -= rhs;
}

std::ostream& operator<<(std::ostream &out, extent const &ext);
    
}}

#endif

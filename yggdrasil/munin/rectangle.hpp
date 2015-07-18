// ==========================================================================
// Munin Rectangle.
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
#ifndef YGGDRASIL_MUNIN_RECTANGLE_HPP_
#define YGGDRASIL_MUNIN_RECTANGLE_HPP_

#include "yggdrasil/munin/point.hpp"
#include "yggdrasil/munin/extent.hpp"
#include <iosfwd>

namespace yggdrasil { namespace munin {

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
    rectangle();

    //* =====================================================================
    /// \brief Constructor
    /// \par
    /// Constructs the rectangle, using the specified origin and size.
    //* =====================================================================
    rectangle(point origin, extent size);
    
    point  origin;
    extent size;
};

bool operator==(rectangle const &lhs, rectangle const &rhs);
bool operator!=(rectangle const &lhs, rectangle const &rhs);
rectangle operator+(point const &pt, extent const &ext);
std::ostream& operator<<(std::ostream &out, rectangle const &rect);


}}

#endif

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
#ifndef YGGDRASIL_MUNIN_POINT_HPP_
#define YGGDRASIL_MUNIN_POINT_HPP_

#include "yggdrasil/types.hpp"
#include <iosfwd>

namespace yggdrasil { namespace munin {

//* =========================================================================
/// \brief A class that represents a position in space.
/// \par
/// A class that represents a position in space, where x is the co-ordinate 
/// along the horizontal axis and y being the co-ordinate along the vertical 
/// axis.
//* =========================================================================
struct point
{
    //* =====================================================================
    /// \brief Default Constructor
    /// \par
    /// Constructs a point, leaving the values uninitialised.
    //* =====================================================================
    point();
    
    //* =====================================================================
    /// \brief Constructor
    /// \par
    /// Constructs a point from a passed in x co-ordinate and a passed in
    /// y co-ordinate.
    //* =====================================================================
    point(yggdrasil::s32 x_coordinate, yggdrasil::s32 y_coordinate);
    
    //* =====================================================================
    /// \brief Addition
    //* =====================================================================
    point &operator+=(point const &rhs);
    
    //* =====================================================================
    /// \brief Subtraction
    //* =====================================================================
    point &operator-=(point const &rhs);
    
    yggdrasil::s32 x;
    yggdrasil::s32 y;
};

bool operator==(point const &lhs, point const &rhs);
bool operator!=(point const &lhs, point const &rhs);
point operator+(point lhs, point const &rhs);
point operator-(point lhs, point const &rhs);
std::ostream& operator<<(std::ostream &out, point const &pt);
    
}}

#endif

// ==========================================================================
// Munin Algorithm.
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
#ifndef MUNIN_ALGORITHM_HPP_
#define MUNIN_ALGORITHM_HPP_

#include "munin/export.hpp"
#include "munin/rectangle.hpp"
#include <boost/optional.hpp>
#include <string>
#include <vector>

namespace terminalpp {
    class canvas;
}

namespace munin {

//* =========================================================================
/// \brief Returns the intersection of two rectangles.
//* =========================================================================
MUNIN_EXPORT 
boost::optional<rectangle> intersection(
    rectangle const &lhs
  , rectangle const &rhs);

//* =========================================================================
/// \brief Returns an array of sliced rectangles.
/// \par
/// A rectangular slice takes an array of rectangles, and returns an array
/// of the fewest number of rectangles, each of height 1, that describes the
/// area covered by the original rectangles.  These are sorted from left to
/// right, top to bottom.
//* =========================================================================
MUNIN_EXPORT 
std::vector<rectangle> rectangular_slice(
    std::vector<rectangle> const &rectangles);

//* =========================================================================
/// \brief Returns an array of clipped regions.
/// \par
/// A clipped region is one whose bounds are trimmed to a particular size.
/// For example, if the size is (5,5), and a region is (3,2)->[3,3], then
/// the region is clipped to (3,2)->[2,3].
//* =========================================================================
std::vector<rectangle> clip_regions(
    std::vector<rectangle> regions
  , terminalpp::extent     size);

//* =========================================================================
/// \brief Returns the passed array, except that any regions that have a
/// height or width of 0 are omitted.
//* =========================================================================
std::vector<rectangle> prune_regions(std::vector<rectangle> regions);

//* =========================================================================
/// \brief Copies a region from one canvas to another.
//* =========================================================================
MUNIN_EXPORT
void copy_region(
    rectangle          const &region
  , terminalpp::canvas const &source
  , terminalpp::canvas       &destination);

}
    
#endif

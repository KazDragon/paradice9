// ==========================================================================
// Munin Compass Layout.
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
#ifndef MUNIN_COMPASS_LAYOUT_HPP_
#define MUNIN_COMPASS_LAYOUT_HPP_

#include "munin/layout.hpp"

namespace munin {

BOOST_STATIC_CONSTANT(odin::u32, COMPASS_LAYOUT_CENTRE = 0);
BOOST_STATIC_CONSTANT(odin::u32, COMPASS_LAYOUT_NORTH  = 1);
BOOST_STATIC_CONSTANT(odin::u32, COMPASS_LAYOUT_EAST   = 2);
BOOST_STATIC_CONSTANT(odin::u32, COMPASS_LAYOUT_SOUTH  = 3);
BOOST_STATIC_CONSTANT(odin::u32, COMPASS_LAYOUT_WEST   = 4);

//* =========================================================================
/// \brief A class that can lay out its components according to basic compass
/// directions.  Components can be laid out to the north, east, south, west,
/// and centre.  Components to the north and south are given their
/// preferred height while having the width of the containing component, and
/// components to the west and east are given their preferred width, while
/// having the height of the containing component.
//* =========================================================================
class MUNIN_EXPORT compass_layout : public layout
{
protected :
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to retrieve the preferred size of the layout
    /// in a custom manner.
    //* =====================================================================
    virtual terminalpp::extent do_get_preferred_size(
        std::vector<std::shared_ptr<component>> const &components
      , std::vector<boost::any>                 const &hints) const override;

    //* =====================================================================
    /// \brief Called by operator().  Derived classes must override this
    /// function in order to lay a container's components out in a custom
    /// manner.
    //* =====================================================================
    virtual void do_layout(
        std::vector<std::shared_ptr<component>> const &components
      , std::vector<boost::any>                 const &hints
      , terminalpp::extent                             size) override;
};

//* =========================================================================
/// \brief Returns a newly created compass layout
//* =========================================================================
MUNIN_EXPORT
std::unique_ptr<layout> make_compass_layout();

}

#endif

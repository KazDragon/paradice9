// ==========================================================================
// Munin Aligned Layout.
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
#ifndef MUNIN_ALIGNED_LAYOUT_HPP_
#define MUNIN_ALIGNED_LAYOUT_HPP_

#include "munin/layout.hpp"

namespace munin {

BOOST_STATIC_CONSTANT(odin::u32, HORIZONTAL_ALIGNMENT_LEFT   = 0);
BOOST_STATIC_CONSTANT(odin::u32, HORIZONTAL_ALIGNMENT_CENTRE = 1);
BOOST_STATIC_CONSTANT(odin::u32, HORIZONTAL_ALIGNMENT_RIGHT  = 2);

BOOST_STATIC_CONSTANT(odin::u32, VERTICAL_ALIGNMENT_TOP    = 0);
BOOST_STATIC_CONSTANT(odin::u32, VERTICAL_ALIGNMENT_CENTRE = 1);
BOOST_STATIC_CONSTANT(odin::u32, VERTICAL_ALIGNMENT_BOTTOM = 2);

struct alignment_data
{
    odin::u32 horizontal_alignment;
    odin::u32 vertical_alignment;
};

//* =========================================================================
/// \brief A class that knows how to lay components out in a container in
/// a aligned-like manner.  Each cell of the aligned has an identical size.
/// Components added to the aligned will be displayed left-to-right, top-to-
/// bottom.
//* =========================================================================
class MUNIN_EXPORT aligned_layout : public layout
{
protected :
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to retrieve the preferred size of the layout
    /// in a custom manner.
    //* =====================================================================
    virtual extent do_get_preferred_size(
        std::vector<std::shared_ptr<component>> const &components
      , std::vector<boost::any>                 const &hints) const;

    //* =====================================================================
    /// \brief Called by operator().  Derived classes must override this
    /// function in order to lay a container's components out in a custom
    /// manner.
    //* =====================================================================
    virtual void do_layout(
        std::vector<std::shared_ptr<component>> const &components
      , std::vector<boost::any>                 const &hints
      , extent                                         size);
};

}

#endif

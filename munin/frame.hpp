// ==========================================================================
// Munin Frame.
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
#ifndef MUNIN_FRAME_HPP_
#define MUNIN_FRAME_HPP_

#include "munin/export.hpp"
#include "munin/composite_component.hpp"
#include "odin/core.hpp"

namespace munin {

//* =========================================================================
/// \brief A base class that takes a series of components and uses them to
/// construct a frame.
/// \par ATTRIBUTE_PEN
/// Set the ATTRIBUTE_PEN attribute to an value of type munin::attribute.
/// This will be passed down to subcomponents.  You can use this, for
/// example, to show a border highlight when the interior component has
/// focus.
//* =========================================================================
class MUNIN_EXPORT frame : public munin::composite_component
{
public :
    //* =====================================================================
    /// \brief Returns the height of the top border
    //* =====================================================================
    odin::s32 get_top_border_height() const;

    //* =====================================================================
    /// \brief Returns the height of the bottom border
    //* =====================================================================
    odin::s32 get_bottom_border_height() const;

    //* =====================================================================
    /// \brief Returns the width of the left border
    //* =====================================================================
    odin::s32 get_left_border_width() const;

    //* =====================================================================
    /// \brief Returns the width of the right border
    //* =====================================================================
    odin::s32 get_right_border_width() const;

protected :
    //* =====================================================================
    /// \brief Called by get_top_border_height.  Derived classes must
    /// override this function in order to retrieve the top border height in
    /// a custom manner.
    //* =====================================================================
    virtual odin::s32 do_get_top_border_height() const = 0;

    //* =====================================================================
    /// \brief Called by get_bottom_border_height.  Derived classes must
    /// override this function in order to retrieve the bottomborder height
    /// int a custom manner.
    //* =====================================================================
    virtual odin::s32 do_get_bottom_border_height() const = 0;

    //* =====================================================================
    /// \brief Called by get_left_border_width.  Derived classes must
    /// override this function in order to retrieve the left border width in
    /// a custom manner.
    //* =====================================================================
    virtual odin::s32 do_get_left_border_width() const = 0;

    //* =====================================================================
    /// \brief Called by get_right_border_width.  Derived classes must
    /// override this function in order to retrieve the right border width in
    /// a custom manner.
    //* =====================================================================
    virtual odin::s32 do_get_right_border_width() const = 0;

private :
    struct impl;
    std::shared_ptr<impl> pimpl_;
};

}

#endif

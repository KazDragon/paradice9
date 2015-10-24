// ==========================================================================
// Munin Basic Frame.
//
// Copyright (C) 2011 Matthew Chaplain, All Rights Reserved.
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
#ifndef MUNIN_BASIC_FRAME_HPP_
#define MUNIN_BASIC_FRAME_HPP_

#include "munin/frame.hpp"

namespace munin {

//* =========================================================================
/// \brief A class that takes a series of components and uses them to
/// construct a frame.
//* =========================================================================
class MUNIN_EXPORT basic_frame : public munin::frame
{
public :
    //* =====================================================================
    /// \brief Constructor
    /// \par
    /// Takes 8 components.  The are arranged in the following manner:
    /// 1 2 3
    /// 4   5
    /// 6 7 8
    ///
    /// These form the frame outlines.
    ///
    /// Components 1, 2 and 3 are optional.  Leaving these NULL will create
    /// a frame without a top border.
    //* =====================================================================
    basic_frame(
        std::shared_ptr<component> top_left,
        std::shared_ptr<component> top,
        std::shared_ptr<component> top_right,
        std::shared_ptr<component> left,
        std::shared_ptr<component> right,
        std::shared_ptr<component> bottom_left,
        std::shared_ptr<component> bottom,
        std::shared_ptr<component> bottom_right);

protected :
    //* =====================================================================
    /// \brief Gets the top-left component.
    //* =====================================================================
    std::shared_ptr<component> get_top_left_component();

    //* =====================================================================
    /// \brief Gets the top component.
    //* =====================================================================
    std::shared_ptr<component> get_top_component();

    //* =====================================================================
    /// \brief Gets the top-right component.
    //* =====================================================================
    std::shared_ptr<component> get_top_right_component();

    //* =====================================================================
    /// \brief Gets the left component.
    //* =====================================================================
    std::shared_ptr<component> get_left_component();

    //* =====================================================================
    /// \brief Gets the right component.
    //* =====================================================================
    std::shared_ptr<component> get_right_component();

    //* =====================================================================
    /// \brief Gets the bottom-left component.
    //* =====================================================================
    std::shared_ptr<component> get_bottom_left_component();

    //* =====================================================================
    /// \brief Gets the bottom component.
    //* =====================================================================
    std::shared_ptr<component> get_bottom_component();

    //* =====================================================================
    /// \brief Gets the bottom-right component.
    //* =====================================================================
    std::shared_ptr<component> get_bottom_right_component();

    //* =====================================================================
    /// \brief Called by get_top_border_height.  Derived classes must
    /// override this function in order to retrieve the top border height in
    /// a custom manner.
    //* =====================================================================
    virtual odin::s32 do_get_top_border_height() const;

    //* =====================================================================
    /// \brief Called by get_bottom_border_height.  Derived classes must
    /// override this function in order to retrieve the bottomborder height
    /// int a custom manner.
    //* =====================================================================
    virtual odin::s32 do_get_bottom_border_height() const;

    //* =====================================================================
    /// \brief Called by get_left_border_width.  Derived classes must
    /// override this function in order to retrieve the left border width in
    /// a custom manner.
    //* =====================================================================
    virtual odin::s32 do_get_left_border_width() const;

    //* =====================================================================
    /// \brief Called by get_right_border_width.  Derived classes must
    /// override this function in order to retrieve the right border width in
    /// a custom manner.
    //* =====================================================================
    virtual odin::s32 do_get_right_border_width() const;

    //* =====================================================================
    /// \brief Called by set_attribute().  Derived classes must override this
    /// function in order to set an attribute in a custom manner.
    //* =====================================================================
    virtual void do_set_attribute(
        std::string const &name, boost::any const &attr);

private :
    struct impl;
    std::shared_ptr<impl> pimpl_;
};

}

#endif

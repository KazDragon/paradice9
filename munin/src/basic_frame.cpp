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
#include "munin/basic_frame.hpp"
#include "munin/basic_component.hpp"
#include "munin/compass_layout.hpp"
#include "munin/view.hpp"

namespace munin {

// ==========================================================================
// BASIC_FRAME::IMPLEMENTATION_STRUCTURE
// ==========================================================================
struct basic_frame::impl
{
    std::shared_ptr<component> top_left_;
    std::shared_ptr<component> top_;
    std::shared_ptr<component> top_right_;
    std::shared_ptr<component> left_;
    std::shared_ptr<component> right_;
    std::shared_ptr<component> bottom_left_;
    std::shared_ptr<component> bottom_;
    std::shared_ptr<component> bottom_right_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
basic_frame::basic_frame(
    std::shared_ptr<component> top_left,
    std::shared_ptr<component> top,
    std::shared_ptr<component> top_right,
    std::shared_ptr<component> left,
    std::shared_ptr<component> right,
    std::shared_ptr<component> bottom_left,
    std::shared_ptr<component> bottom,
    std::shared_ptr<component> bottom_right)
  : pimpl_(std::make_shared<impl>())
{
    pimpl_->top_left_     = top_left;
    pimpl_->top_          = top;
    pimpl_->top_right_    = top_right;
    pimpl_->left_         = left;
    pimpl_->right_        = right;
    pimpl_->bottom_left_  = bottom_left;
    pimpl_->bottom_       = bottom;
    pimpl_->bottom_right_ = bottom_right;

    std::shared_ptr<container> top_container;

    if (pimpl_->top_left_ && pimpl_->top_ && pimpl_->top_right_)
    {
        top_container = view(
            make_compass_layout(),
            top_left,  COMPASS_LAYOUT_WEST,
            top,       COMPASS_LAYOUT_CENTRE,
            top_right, COMPASS_LAYOUT_EAST);
    }

    auto bottom_container = view(
        make_compass_layout(),
        bottom_left,  COMPASS_LAYOUT_WEST,
        bottom,       COMPASS_LAYOUT_CENTRE,
        bottom_right, COMPASS_LAYOUT_EAST);
    
    auto content = get_container();
    content->set_layout(make_compass_layout());

    if (top_container)
    {
        content->add_component(top_container,     COMPASS_LAYOUT_NORTH);
    }
    content->add_component(bottom_container,      COMPASS_LAYOUT_SOUTH);
    content->add_component(left,                  COMPASS_LAYOUT_WEST);
    content->add_component(right,                 COMPASS_LAYOUT_EAST);
}

// ==========================================================================
// GET_TOP_LEFT_COMPONENT
// ==========================================================================
std::shared_ptr<component> basic_frame::get_top_left_component()
{
    return pimpl_->top_left_;
}

// ==========================================================================
// GET_TOP_COMPONENT
// ==========================================================================
std::shared_ptr<component> basic_frame::get_top_component()
{
    return pimpl_->top_;
}

// ==========================================================================
// GET_TOP_RIGHT_COMPONENT
// ==========================================================================
std::shared_ptr<component> basic_frame::get_top_right_component()
{
    return pimpl_->top_right_;
}

// ==========================================================================
// GET_LEFT_COMPONENT
// ==========================================================================
std::shared_ptr<component> basic_frame::get_left_component()
{
    return pimpl_->left_;
}

// ==========================================================================
// GET_RIGHT_COMPONENT
// ==========================================================================
std::shared_ptr<component> basic_frame::get_right_component()
{
    return pimpl_->right_;
}

// ==========================================================================
// GET_BOTTOM_LEFT_COMPONENT
// ==========================================================================
std::shared_ptr<component> basic_frame::get_bottom_left_component()
{
    return pimpl_->bottom_left_;
}

// ==========================================================================
// GET_BOTTOM_COMPONENT
// ==========================================================================
std::shared_ptr<component> basic_frame::get_bottom_component()
{
    return pimpl_->bottom_;
}

// ==========================================================================
// GET_BOTTOM_RIGHT_COMPONENT
// ==========================================================================
std::shared_ptr<component> basic_frame::get_bottom_right_component()
{
    return pimpl_->bottom_right_;
}

// ==========================================================================
// DO_GET_TOP_BORDER_HEIGHT
// ==========================================================================
odin::s32 basic_frame::do_get_top_border_height() const
{
    odin::s32 top_border_height = 0;

    if (pimpl_->top_left_ != NULL)
    {
        top_border_height = (std::max)(
            top_border_height
          , pimpl_->top_left_->get_preferred_size().height);
    }

    if (pimpl_->top_ != NULL)
    {
        top_border_height = (std::max)(
            top_border_height
          , pimpl_->top_->get_preferred_size().height);
    }

    if (pimpl_->top_right_ != NULL)
    {
        top_border_height = (std::max)(
            top_border_height
          , pimpl_->top_right_->get_preferred_size().height);
    }

    return top_border_height;
}

// ==========================================================================
// DO_GET_BOTTOM_BORDER_HEIGHT
// ==========================================================================
odin::s32 basic_frame::do_get_bottom_border_height() const
{
    return 1;
}

// ==========================================================================
// DO_GET_LEFT_BORDER_WIDTH
// ==========================================================================
odin::s32 basic_frame::do_get_left_border_width() const
{
    return 1;
}

// ==========================================================================
// DO_GET_RIGHT_BORDER_WIDTH
// ==========================================================================
odin::s32 basic_frame::do_get_right_border_width() const
{
    return 1;
}

/*
// ==========================================================================
// DO_GET_BORDER_HEIGHT
// ==========================================================================
s32 basic_frame::do_get_border_height() const
{
    // By default, we look at the preferred heights of the top, and the
    // preferred heights of the bottom edges, and add them up.
    s32 preferred_height_top    = 0;
    s32 preferred_height_bottom = 0;

    if (pimpl_->top_left_ && pimpl_->top_ && pimpl_->top_right_)
    {
        preferred_height_top =
            (max)(pimpl_->top_left_->get_preferred_size().height
          , (max)(pimpl_->top_->get_preferred_size().height
          ,       pimpl_->top_right_->get_preferred_size().height));
    }

    if (pimpl_->bottom_left_ && pimpl_->bottom_ && pimpl_->bottom_right_)
    {
        preferred_height_bottom =
            (max)(pimpl_->bottom_left_->get_preferred_size().height
          , (max)(pimpl_->bottom_->get_preferred_size().height
          ,       pimpl_->bottom_right_->get_preferred_size().height));
    }

    return preferred_height_top + preferred_height_bottom;
}

// ==========================================================================
// DO_GET_BORDER_WIDTH
// ==========================================================================
s32 basic_frame::do_get_border_width() const
{
    return 2;
}
*/

// ==========================================================================
// DO_SET_ATTRIBUTE
// ==========================================================================
void basic_frame::do_set_attribute(std::string const &name, boost::any const &attr)
{
    if (pimpl_->top_left_ && pimpl_->top_ && pimpl_->top_right_)
    {
        pimpl_->top_left_->set_attribute(name, attr);
        pimpl_->top_->set_attribute(name, attr);
        pimpl_->top_right_->set_attribute(name, attr);
    }

    pimpl_->left_->set_attribute(name, attr);
    pimpl_->right_->set_attribute(name, attr);
    pimpl_->bottom_left_->set_attribute(name, attr);
    pimpl_->bottom_->set_attribute(name, attr);
    pimpl_->bottom_right_->set_attribute(name, attr);
}

}

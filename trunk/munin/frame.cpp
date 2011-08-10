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
#include "munin/frame.hpp"
#include "munin/basic_component.hpp"
#include "munin/basic_container.hpp"
#include "munin/compass_layout.hpp"
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace odin;
using namespace boost;
using namespace std;

namespace munin {

// ==========================================================================
// FRAME::IMPLEMENTATION_STRUCTURE
// ==========================================================================
struct frame::impl
{
    shared_ptr<component> top_left_;
    shared_ptr<component> top_;
    shared_ptr<component> top_right_;
    shared_ptr<component> left_;
    shared_ptr<component> right_;
    shared_ptr<component> bottom_left_;
    shared_ptr<component> bottom_;
    shared_ptr<component> bottom_right_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
frame::frame(
    shared_ptr<component> top_left
  , shared_ptr<component> top
  , shared_ptr<component> top_right
  , shared_ptr<component> left
  , shared_ptr<component> right
  , shared_ptr<component> bottom_left
  , shared_ptr<component> bottom
  , shared_ptr<component> bottom_right)
  : pimpl_(make_shared<impl>())
{
    pimpl_->top_left_     = top_left;
    pimpl_->top_          = top;
    pimpl_->top_right_    = top_right;
    pimpl_->left_         = left;
    pimpl_->right_        = right;
    pimpl_->bottom_left_  = bottom_left;
    pimpl_->bottom_       = bottom;
    pimpl_->bottom_right_ = bottom_right;

    shared_ptr<basic_container> top_container;

    if (pimpl_->top_left_ && pimpl_->top_ && pimpl_->top_right_)
    {
        top_container = make_shared<basic_container>();
        top_container->set_layout(make_shared<compass_layout>());
        top_container->add_component(top_left,  COMPASS_LAYOUT_WEST);
        top_container->add_component(top,       COMPASS_LAYOUT_CENTRE);
        top_container->add_component(top_right, COMPASS_LAYOUT_EAST);
    }

    BOOST_AUTO(bottom_container, make_shared<basic_container>());
    bottom_container->set_layout(make_shared<compass_layout>());
    bottom_container->add_component(bottom_left,  COMPASS_LAYOUT_WEST);
    bottom_container->add_component(bottom,       COMPASS_LAYOUT_CENTRE);
    bottom_container->add_component(bottom_right, COMPASS_LAYOUT_EAST);

    BOOST_AUTO(content, get_container());
    content->set_layout(make_shared<compass_layout>());

    if (top_container)
    {
        content->add_component(top_container,     COMPASS_LAYOUT_NORTH);
    }
    content->add_component(bottom_container,      COMPASS_LAYOUT_SOUTH);
    content->add_component(left,                  COMPASS_LAYOUT_WEST);
    content->add_component(right,                 COMPASS_LAYOUT_EAST);
}

// ==========================================================================
// GET_BORDER_HEIGHT
// ==========================================================================
s32 frame::get_border_height() const
{
    return do_get_border_height();
}

// ==========================================================================
// GET_BORDER_WIDTH
// ==========================================================================
s32 frame::get_border_width() const
{
    return do_get_border_width();
}

// ==========================================================================
// DO_GET_BORDER_HEIGHT
// ==========================================================================
s32 frame::do_get_border_height() const
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
s32 frame::do_get_border_width() const
{
    return 2;
}

// ==========================================================================
// DO_SET_ATTRIBUTE
// ==========================================================================
void frame::do_set_attribute(string const &name, any const &attr)
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

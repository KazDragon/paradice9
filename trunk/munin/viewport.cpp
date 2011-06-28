// ==========================================================================
// Munin Viewport.
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
#include "munin/viewport.hpp"
#include "munin/canvas.hpp"
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/scope_exit.hpp>
#include <boost/typeof/typeof.hpp>

using namespace boost;
using namespace std;

namespace munin {
    
// ==========================================================================
// VIEWPORT::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct viewport::impl
{
    viewport              &self_;
    point                  origin_;
    shared_ptr<component>  component_;
    
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(viewport &self)
        : self_(self)
    {
    }
        
    // ======================================================================
    // ON_REDRAW
    // ======================================================================
    void on_redraw(vector<rectangle> regions)
    {
        // Adjust for offset.  If we are currently looking at the component
        // from (5,10), and it wants to update the region (7,12)->[2,2],
        // then we really want to update our own region of (2,2)->[2,2].
        BOOST_FOREACH(rectangle &region, regions)
        {
            region.origin.x -= origin_.x;
            region.origin.y -= origin_.y;
        }
        
        self_.on_redraw(regions);
    }

    // ======================================================================
    // ON_CURSOR_POSITION_CHANGED
    // ======================================================================
    void on_cursor_position_changed(point position)
    {
        // If the viewport has an origin of (1, 2) and the component reports
        // its cursor at (3, 4), then we must take the origin into account
        // and report that our cursor is at (2, 2).
        position.x -= origin_.x;
        position.y -= origin_.y;

        self_.on_cursor_position_changed(position);
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
viewport::viewport(shared_ptr<component> underlying_component)
{
    pimpl_             = make_shared<impl>(ref(*this));
    pimpl_->component_ = underlying_component;
    
    get_component()->on_redraw.connect(
        bind(&impl::on_redraw, pimpl_, _1));
    
    get_component()->on_focus_set.connect(
        bind(ref(on_focus_set)));

    get_component()->on_focus_lost.connect(
        bind(ref(on_focus_lost)));
    
    get_component()->on_cursor_state_changed.connect(
        bind(ref(on_cursor_state_changed), _1));

    get_component()->on_cursor_position_changed.connect(
        bind(&impl::on_cursor_position_changed, pimpl_, _1));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
viewport::~viewport()
{
}

// ==========================================================================
// GET_ORIGIN
// ==========================================================================
point viewport::get_origin() const
{
    return pimpl_->origin_;
}

// ==========================================================================
// SET_ORIGIN
// ==========================================================================
void viewport::set_origin(point origin)
{
    pimpl_->origin_ = origin;

    // It is highly likely that this will require redrawing the entire
    // contained object.
    vector<rectangle> regions;
    regions.push_back(rectangle(point(0, 0), get_size()));
    on_redraw(regions);
}

// ==========================================================================
// GET_COMPONENT
// ==========================================================================
shared_ptr<component> viewport::get_component()
{
    return pimpl_->component_;
}

// ==========================================================================
// DO_SET_SIZE
// ==========================================================================
void viewport::do_set_size(extent const &size) 
{
    // TODO: check if the cursor for the underlying component is within
    // our bounds.  If not, scroll the component.
    basic_component::do_set_size(size);
}

// ==========================================================================
// DO_SET_PARENT
// ==========================================================================
void viewport::do_set_parent(shared_ptr<component> parent)
{
    pimpl_->component_->set_parent(parent);
}

// ==========================================================================
// DO_GET_PARENT
// ==========================================================================
shared_ptr<component> viewport::do_get_parent() const
{
    return pimpl_->component_->get_parent();
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
extent viewport::do_get_preferred_size() const 
{
    return pimpl_->component_->get_preferred_size();
}

// ==========================================================================
// DO_HAS_FOCUS
// ==========================================================================
bool viewport::do_has_focus() const 
{
    return pimpl_->component_->has_focus();
}

// ==========================================================================
// DO_SET_CAN_FOCUS
// ==========================================================================
void viewport::do_set_can_focus(bool focus)
{
    pimpl_->component_->set_can_focus(focus);
}

// ==========================================================================
// DO_CAN_FOCUS
// ==========================================================================
bool viewport::do_can_focus() const 
{
    return pimpl_->component_->can_focus();
}

// ==========================================================================
// DO_SET_FOCUS
// ==========================================================================
void viewport::do_set_focus() 
{
    pimpl_->component_->set_focus();
}

// ==========================================================================
// DO_LOSE_FOCUS
// ==========================================================================
void viewport::do_lose_focus() 
{
    pimpl_->component_->lose_focus();
}

// ==========================================================================
// DO_FOCUS_NEXT
// ==========================================================================
void viewport::do_focus_next() 
{
    pimpl_->component_->focus_next();
}

// ==========================================================================
// DO_FOCUS_PREVIOUS
// ==========================================================================
void viewport::do_focus_previous() 
{
    pimpl_->component_->focus_previous();
}

// ==========================================================================
// DO_GET_FOCUSSED_COMPONENT
// ==========================================================================
shared_ptr<component> viewport::do_get_focussed_component() 
{
    return pimpl_->component_->get_focussed_component();
}

// ==========================================================================
// DO_ENABLE
// ==========================================================================
void viewport::do_enable()
{
    pimpl_->component_->enable();
}

// ==========================================================================
// DO_DISABLE
// ==========================================================================
void viewport::do_disable()
{
    pimpl_->component_->disable();
}

// ==========================================================================
// DO_IS_ENABLED
// ==========================================================================
bool viewport::do_is_enabled() const
{
    return pimpl_->component_->is_enabled();
}

// ==========================================================================
// DO_GET_CURSOR_STATE
// ==========================================================================
bool viewport::do_get_cursor_state() const 
{
    return pimpl_->component_->get_cursor_state();
}

// ==========================================================================
// DO_GET_CURSOR_POSITION
// ==========================================================================
point viewport::do_get_cursor_position() const 
{
    BOOST_AUTO(position, pimpl_->component_->get_cursor_position());
    
    // If we are looking at the component at (5,2) and the component reports 
    // its cursor at (10,10), then the cursor in our window is really at (5,8).
    // Adjust for this offset.
    position.x -= pimpl_->origin_.x;
    position.y -= pimpl_->origin_.y;
    
    return position;
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void viewport::do_draw(
    canvas          &cvs
  , rectangle const &region) 
{
    // If we are looking at the component from (3,5) and the region we want
    // to redraw is (0,0)->[2,2], then we need to offset the canvas by
    // (-3,-5) in order for it to render in our screen coordinates.
    BOOST_AUTO(offset, pimpl_->origin_);
    cvs.apply_offset(-offset.x, -offset.y);
    
    // Ensure that the offset is unapplied before any exit of this function.
    BOOST_SCOPE_EXIT( (&cvs)(&offset) )
    {
        cvs.apply_offset(offset.x, offset.y);
    } BOOST_SCOPE_EXIT_END
    
    pimpl_->component_->draw(cvs, region);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void viewport::do_event(any const &event) 
{
    pimpl_->component_->event(event);
}

// ==========================================================================
// DO_SET_ATTRIBUTE
// ==========================================================================
void viewport::do_set_attribute(string const &name, any const &attr)
{
    pimpl_->component_->set_attribute(name, attr);
}

}


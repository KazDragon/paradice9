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
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/scope_exit.hpp>
#include <boost/typeof/typeof.hpp>

using namespace odin;
using namespace boost;
using namespace boost::assign;
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
        // Also, clip the region to the viewport's extends.
        BOOST_AUTO(size, self_.get_size());
        BOOST_AUTO(origin, self_.get_origin());

        BOOST_FOREACH(rectangle &region, regions)
        {
            region.origin -= origin;

            if (region.origin.x < 0)
            {
                region.size.width += region.origin.x;
                region.origin.x = 0;
            }

            region.size.width = (min)(region.size.width, size.width);

            if (region.origin.y < 0)
            {
                region.size.height += region.origin.y;
                region.origin.y = 0;
            }

            region.size.height = (min)(region.size.height, size.height);
        }
        
        self_.on_redraw(regions);
    }

    // ======================================================================
    // ON_CURSOR_POSITION_CHANGED
    // ======================================================================
    void on_cursor_position_changed(point position)
    {
        // Check to see if the cursor has escaped below our view.
        BOOST_AUTO(size, self_.get_size());
        BOOST_AUTO(origin, self_.get_origin());

        // If there is a dimension with no size, then we can't do anything 
        // with the cursor.
        if (size.width == 0 || size.height == 0)
        {
            return;
        }

        if (position.y >= origin.y + size.height)
        {
            // It has.  Scroll the viewport so that the cursor is in view.
            origin.y = (position.y - size.height) + 1;
            self_.set_origin(origin);
        }
        // Check to see if the cursor has escaped above our view.
        else if (position.y < origin.y)
        {
            // It has.  Scroll the viewport so that the cursor is in view.
            origin.y = position.y;
            self_.set_origin(origin);
        }

        // If the viewport has an origin of (1, 2) and the component reports
        // its cursor at (3, 4), then we must take the origin into account
        // and report that our cursor is at (2, 2).
        position.x -= origin.x;
        position.y -= origin.y;

        self_.on_cursor_position_changed(position);
    }

    // ======================================================================
    // ON_SUBCOMPONENT_PREFERRED_SIZE_CHANGED
    // ======================================================================
    void on_subcomponent_preferred_size_changed()
    {
        // If the new preferred_size is shrinking, and is smaller than our 
        // component, then we will need to redraw the non-overlapping 
        // portions to ensure they get blanked correctly.
        BOOST_AUTO(component_size, component_->get_size());
        BOOST_AUTO(preferred_size, component_->get_preferred_size());
        BOOST_AUTO(size, self_.get_size());

        component_->set_size(component_->get_preferred_size());

        if (preferred_size.width < component_size.width
         && preferred_size.width < size.width)
        {
            on_redraw(list_of(rectangle(
                point(preferred_size.width, 0)
              , extent(size.width - preferred_size.width, size.height))));
        }

        if (preferred_size.height < component_size.height
         && preferred_size.height < size.height)
        {
            on_redraw(list_of(rectangle(
                point(0, preferred_size.height)
              , extent(size.width, size.height - preferred_size.height))));
        }
        
        self_.on_subcomponent_size_changed();
    }

    // ======================================================================
    // DO_PGUP_KEY_EVENT
    // ======================================================================
    void do_pgup_key_event()
    {
        // PgUp - shift the cursor and the origin one page up.
        BOOST_AUTO(size, self_.get_size());
        BOOST_AUTO(origin, self_.get_origin());
        BOOST_AUTO(component_size, component_->get_size());
        BOOST_AUTO(cursor_position, component_->get_cursor_position());
        
        if (component_size.height == 0)
        {
            return;
        }

        // If the component is smaller than this view, just move to the top.
        if (component_size.height < size.height)
        {
            cursor_position.y = 0;
        }
        // Otherwise, if we would page off the top of the component, then go
        // to the top.
        else if (origin.y + cursor_position.y < size.height)
        {
            cursor_position.y = 0;
        }
        // Otherwise, if we would go to the first page, set the origin and
        // then the cursor.
        else if (origin.y < size.height)
        {
            origin.y = 0;
            self_.set_origin(origin);
            cursor_position.y -= size.height;
        }
        // Otherwise, move the origin, then move the cursor.  Also redraw, 
        // since this will likely require that.
        else
        {
            origin.y -= size.height;
            self_.set_origin(origin);
            cursor_position.y -= size.height;
        }

        component_->set_cursor_position(cursor_position);
    }
    
    // ======================================================================
    // DO_PGDN_KEY_EVENT
    // ======================================================================
    void do_pgdn_key_event()
    {
        // PgDn - shift the cursor and the origin one page down.
        BOOST_AUTO(size, self_.get_size());
        BOOST_AUTO(origin, self_.get_origin());
        BOOST_AUTO(component_size, component_->get_size());
        BOOST_AUTO(cursor_position, component_->get_cursor_position());

        if (component_size.height == 0)
        {
            return;
        }

        // If the component is smaller than this view, just set the cursor
        // position to the bottom.
        if (component_size.height <= size.height)
        {
            cursor_position.y = component_size.height - 1;
        }
        // Otherwise, if we would hit the end, just set the cursor to the
        // end.
        else if (cursor_position.y + size.height >= component_size.height)
        {
            cursor_position.y = component_size.height - 1;
            self_.on_redraw(list_of(rectangle(point(), size)));
        }
        // Otherwise, if we would hit the last page, scroll until the last
        // page, then set the cursor.  This will cause a half-scroll so that
        // the page is still full, and the cursor is in the right place.
        else if (cursor_position.y + size.height >= component_size.height - size.height)
        {
            origin.y = component_size.height - size.height;
            self_.set_origin(origin);
            cursor_position.y += size.height;
        }
        // Otherwise, move the origin by a page, then move the cursor.
        // This causes the page to scroll by one, but leaves the cursor
        // in the same place on the screen.
        else
        {
            origin.y += size.height;
            self_.set_origin(origin);
            cursor_position.y += size.height;
        }

        component_->set_cursor_position(cursor_position);
    }

    // ======================================================================
    // DO_ANSI_CONTROL_SEQUENCE_EVENT
    // ======================================================================
    bool do_ansi_control_sequence_event(
        odin::ansi::control_sequence const &sequence)
    {
        if (sequence.initiator_ == odin::ansi::CONTROL_SEQUENCE_INTRODUCER)
        {
            if (sequence.command_ == odin::ansi::KEYPAD_FUNCTION)
            {
                // Check for the PGUP key
                if (sequence.arguments_.size() == 1
                 && sequence.arguments_[0] == '5')
                {
                    do_pgup_key_event();
                    return true;
                }
                // Check for the PGDN key
                if (sequence.arguments_.size() == 1
                 && sequence.arguments_[0] == '6')
                {
                    do_pgdn_key_event();
                    return true;
                }
            }
        }

        return false;
    }

    // ======================================================================
    // DO_EVENT
    // ======================================================================
    bool do_event(any const &event)
    {
        odin::ansi::control_sequence const *sequence = 
            any_cast<odin::ansi::control_sequence>(&event);
            
        if (sequence != NULL)
        {
            return do_ansi_control_sequence_event(*sequence);
        }

        return false;
    }

    // ======================================================================
    // INITIALISE_REGION
    // ======================================================================
    void initialise_region(canvas &cvs, rectangle const &region) 
    {
        // Apply the region's origin as an offset to our canvas.  This means that
        // the calculations below are done in terms of the region's origin.
        cvs.apply_offset(region.origin.x, region.origin.y);

        // Ensure that the offset is unapplied before any exit of this function.
        BOOST_SCOPE_EXIT( (&cvs)(&region) )
        {
            cvs.apply_offset(-region.origin.x, -region.origin.y);
        } BOOST_SCOPE_EXIT_END
        
        // Now, blank out the region.
        for (s32 row = 0; row < region.size.height; ++row)
        {
            for (s32 column = 0; column < region.size.width; ++column)
            {
                cvs[column][row] = element_type(' ');
            }
        }
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
    
    get_component()->on_preferred_size_changed.connect(
        bind(&impl::on_subcomponent_preferred_size_changed, pimpl_));

    get_component()->on_size_changed.connect(
        bind(ref(on_subcomponent_size_changed)));

    get_component()->on_cursor_state_changed.connect(
        bind(ref(on_cursor_state_changed), _1));

    get_component()->on_cursor_position_changed.connect(
        bind(&impl::on_cursor_position_changed, pimpl_, _1));

    // A component in a viewport is allowed to be whatever size it wants to be.
    // Therefore, the first thing we do is set it to that size.  It will then
    // notify us whenever that preferred size changes.
    get_component()->set_size(pimpl_->component_->get_preferred_size());
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

    // Check to see if this has moved the cursor off of our screen.  If so,
    // we will need to move the cursor.
    BOOST_AUTO(size, get_size());
    BOOST_AUTO(cursor_position, get_component()->get_cursor_position());

    // Only perform these checks if there's anywhere that a cursor could 
    // possibly be.
    if (size.height != 0)
    {
        // Check to see if it's off the top.
        if (cursor_position.y < origin.y)
        {
            cursor_position.y = origin.y;
            get_component()->set_cursor_position(cursor_position);
        }
        // Check to see if it's off the bottom.
        else if (cursor_position.y >= origin.y + size.height)
        {
            cursor_position.y = (origin.y + size.height) - 1;
            get_component()->set_cursor_position(cursor_position);
        }
    }

    // It is highly likely that this will require redrawing the entire
    // contained object.
    on_redraw(list_of(rectangle(point(), get_size())));

    // Also, announce that this has occurred.  This enables a container to
    // react to the fact that the origin has changed.  For example, by
    // moving some scrollbars.
    on_origin_changed();
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
    basic_component::do_set_size(size);
    get_component()->set_size(size);

    // Check if the cursor for the underlying component is within our bounds.
    // If not, scroll the component.
    BOOST_AUTO(origin, get_origin());
    BOOST_AUTO(cursor_position, get_component()->get_cursor_position());
    
    if (cursor_position.y < origin.y)
    {
        origin.y = cursor_position.y;
        set_origin(origin);
    }
    else if (cursor_position.y > origin.y + size.height)
    {
        origin.y = cursor_position.y - size.height;
        set_origin(origin);
    }
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
    // If we are looking at the component at (5,2) and the component reports 
    // its cursor at (10,10), then the cursor in our window is really at (5,8).
    // Adjust for this offset.
    return pimpl_->component_->get_cursor_position() - get_origin();
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void viewport::do_draw(
    canvas          &cvs
  , rectangle const &region) 
{
    // First, initialise the region on the canvas.
    pimpl_->initialise_region(cvs, region);

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
    
    // So with the canvas offset, we also need to recalculate the draw region
    // by that same offset.
    rectangle offset_region(
        point(region.origin.x + offset.x,
              region.origin.y + offset.y),
              region.size);

    pimpl_->component_->draw(cvs, offset_region);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void viewport::do_event(any const &event) 
{
    odin::ansi::mouse_report const *report =
        boost::any_cast<odin::ansi::mouse_report>(&event);
    
    if (report != NULL)
    {
        BOOST_AUTO(origin, get_origin());

        // Offset the mouse report by the origin.
        odin::ansi::mouse_report subreport = *report;
        subreport.x_position_ += origin.x;
        subreport.y_position_ += origin.y;

        pimpl_->component_->event(subreport);
    }
    else if (!pimpl_->do_event(event))
    {
        pimpl_->component_->event(event);
    }
}

// ==========================================================================
// DO_SET_ATTRIBUTE
// ==========================================================================
void viewport::do_set_attribute(string const &name, any const &attr)
{
    pimpl_->component_->set_attribute(name, attr);
}

}

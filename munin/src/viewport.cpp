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
#include "munin/context.hpp"
#include "munin/rectangle.hpp"
#include <terminalpp/canvas_view.hpp>
#include <terminalpp/virtual_key.hpp>
#include <boost/scope_exit.hpp>

namespace munin {

// ==========================================================================
// VIEWPORT::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct viewport::impl
{
    viewport                   &self_;
    terminalpp::point           origin_;
    std::shared_ptr<component>  component_;

    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(
        viewport &self, 
        std::shared_ptr<component> const &underlying_component)
      : self_(self),
        component_(std::move(underlying_component))
    {
    }

    // ======================================================================
    // ON_REDRAW
    // ======================================================================
    void on_redraw(std::vector<rectangle> regions)
    {
        // Adjust for offset.  If we are currently looking at the component
        // from (5,10), and it wants to update the region (7,12)->[2,2],
        // then we really want to update our own region of (2,2)->[2,2].
        // Also, clip the region to the viewport's extends.
        auto size = self_.get_size();
        auto origin = self_.get_origin();

        for (auto &region : regions)
        {
            region.origin -= origin;

            if (region.origin.x < 0)
            {
                region.size.width += region.origin.x;
                region.origin.x = 0;
            }

            region.size.width = (std::min)(region.size.width, size.width);

            if (region.origin.y < 0)
            {
                region.size.height += region.origin.y;
                region.origin.y = 0;
            }

            region.size.height = (std::min)(region.size.height, size.height);
        }

        self_.on_redraw(regions);
    }

    // ======================================================================
    // ON_CURSOR_POSITION_CHANGED
    // ======================================================================
    void on_cursor_position_changed(terminalpp::point position)
    {
        // Check to see if the cursor has escaped below our view.
        auto size = self_.get_size();
        auto origin = self_.get_origin();

        // If there is a dimension with no size, then we can't do anything
        // with the cursor.
        if (size.width <= 0 || size.height <= 0)
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
    // SET_SUBCOMPONENT_SIZE
    // ======================================================================
    void set_subcomponent_size()
    {
        auto size = self_.get_size();

        // First, we set the subcomponent's size to the viewport size.
        // This means that any boundary that the component shares with the
        // viewport (such as a text area's or list's width) is kept.
        component_->set_size(size);

        // We then ask it its preferred size.  Finally, we set its size again
        // so that it is at least as big as the viewport.
        auto preferred_size = component_->get_preferred_size();
        preferred_size.width  = (std::max)(size.width, preferred_size.width);
        preferred_size.height = (std::max)(size.height, preferred_size.height);

        component_->set_size(preferred_size);
    }

    // ======================================================================
    // ON_SUBCOMPONENT_PREFERRED_SIZE_CHANGED
    // ======================================================================
    void on_subcomponent_preferred_size_changed()
    {
        set_subcomponent_size();
        on_redraw({rectangle(terminalpp::point(0, 0), self_.get_size())});

        self_.on_subcomponent_size_changed();
    }

    // ======================================================================
    // DO_PGUP_KEY_EVENT
    // ======================================================================
    void do_pgup_key_event()
    {
        // PgUp - shift the cursor and the origin one page up.
        auto size = self_.get_size();
        auto origin = self_.get_origin();
        auto component_size = component_->get_size();
        auto cursor_position = component_->get_cursor_position();

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
        auto size = self_.get_size();
        auto origin = self_.get_origin();
        auto component_size = component_->get_size();
        auto cursor_position = component_->get_cursor_position();

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
            self_.on_redraw({rectangle({}, size)});
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
    // DO_VK_EVENT
    // ======================================================================
    bool do_vk_event(terminalpp::virtual_key const &vk)
    {
        switch (vk.key)
        {
            case terminalpp::vk::pgup :
                do_pgup_key_event();
                return true;
                
            case terminalpp::vk::pgdn :
                do_pgdn_key_event();
                return true;
                
            default :
                return false;
        }
    }

    // ======================================================================
    // DO_MOUSE_EVENT
    // ======================================================================
    bool do_mouse_event(terminalpp::ansi::mouse::report const &report)
    {
        switch (report.button_)
        {
            case terminalpp::ansi::mouse::report::SCROLLWHEEL_UP :
                do_pgup_key_event();
                return true;
                
            case terminalpp::ansi::mouse::report::SCROLLWHEEL_DOWN :
                do_pgdn_key_event();
                return true;
                
            default :
                return false;
        }
    }

    // ======================================================================
    // DO_EVENT
    // ======================================================================
    bool do_event(boost::any const &event)
    {
        auto vk = boost::any_cast<terminalpp::virtual_key>(&event);
        
        if (vk)
        {
            return do_vk_event(*vk);
        }
        
        auto report = boost::any_cast<terminalpp::ansi::mouse::report>(&event);
        
        if (report)
        {
            return do_mouse_event(*report);    
        }
        
        return false;
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
viewport::viewport(std::shared_ptr<component> const &underlying_component)
    : pimpl_(std::make_shared<impl>(std::ref(*this), underlying_component))
{
    get_component()->on_redraw.connect(
        [this](auto const &regions){pimpl_->on_redraw(regions);});

    get_component()->on_focus_set.connect([this]{on_focus_set();});

    get_component()->on_focus_lost.connect([this]{on_focus_lost();});

    get_component()->on_preferred_size_changed.connect(
        [this]{pimpl_->on_subcomponent_preferred_size_changed();});

    get_component()->on_size_changed.connect(
        [this]{on_subcomponent_size_changed();});

    get_component()->on_cursor_state_changed.connect(
        [this](auto const &state){on_cursor_state_changed(state);});

    get_component()->on_cursor_position_changed.connect(
        [this](auto const &pos){pimpl_->on_cursor_position_changed(pos);});

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
terminalpp::point viewport::get_origin() const
{
    return pimpl_->origin_;
}

// ==========================================================================
// SET_ORIGIN
// ==========================================================================
void viewport::set_origin(terminalpp::point const &origin)
{
    pimpl_->origin_ = origin;

    // Check to see if this has moved the cursor off of our screen.  If so,
    // we will need to move the cursor.
    auto size = get_size();
    auto cursor_position = get_component()->get_cursor_position();

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
    on_redraw({rectangle({}, get_size())});

    // Also, announce that this has occurred.  This enables a container to
    // react to the fact that the origin has changed.  For example, by
    // moving some scrollbars.
    on_origin_changed();
}

// ==========================================================================
// GET_COMPONENT
// ==========================================================================
std::shared_ptr<component> viewport::get_component()
{
    return pimpl_->component_;
}

// ==========================================================================
// DO_SET_SIZE
// ==========================================================================
void viewport::do_set_size(terminalpp::extent const &size)
{
    basic_component::do_set_size(size);
    pimpl_->set_subcomponent_size();

    // Check if the cursor for the underlying component is within our bounds.
    // If not, scroll the component.
    auto origin = get_origin();
    auto cursor_position = get_component()->get_cursor_position();

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

    on_redraw({rectangle({}, size)});
}

// ==========================================================================
// DO_SET_PARENT
// ==========================================================================
void viewport::do_set_parent(std::shared_ptr<component> const &parent)
{
    pimpl_->component_->set_parent(parent);
}

// ==========================================================================
// DO_GET_PARENT
// ==========================================================================
std::shared_ptr<component> viewport::do_get_parent() const
{
    return pimpl_->component_->get_parent();
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
terminalpp::extent viewport::do_get_preferred_size() const
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
std::shared_ptr<component> viewport::do_get_focussed_component()
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
terminalpp::point viewport::do_get_cursor_position() const
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
    context         &ctx
  , rectangle const &region)
{
    auto &cvs = ctx.get_canvas();

    // If we are looking at the component from (3,5) and the region we want
    // to redraw is (0,0)->[2,2], then we need to offset the canvas by
    // (-3,-5) in order for it to render in our screen coordinates.
    auto offset = pimpl_->origin_;
    cvs.offset_by({-offset.x, -offset.y});

    // Ensure that the offset is unapplied before any exit of this function.
    BOOST_SCOPE_EXIT( (&cvs)(&offset) )
    {
        cvs.offset_by({offset.x, offset.y});
    } BOOST_SCOPE_EXIT_END

    // So with the canvas offset, we also need to recalculate the draw region
    // by that same offset.
    munin::rectangle offset_region(
        { region.origin.x + offset.x,
          region.origin.y + offset.y },
        region.size);

    pimpl_->component_->draw(ctx, offset_region);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void viewport::do_event(boost::any const &event)
{
    auto report =
        boost::any_cast<terminalpp::ansi::mouse::report>(&event);
        
    if (report != nullptr)
    {
        auto origin = get_origin();

        // Offset the mouse report by the origin.
        terminalpp::ansi::mouse::report subreport = *report;
        subreport.x_position_ += origin.x;
        subreport.y_position_ += origin.y;

        if (!pimpl_->do_event(event))
        {
            pimpl_->component_->event(subreport);
        }
    }
    else if (!pimpl_->do_event(event))
    {
        pimpl_->component_->event(event);
    }
}

// ==========================================================================
// DO_SET_ATTRIBUTE
// ==========================================================================
void viewport::do_set_attribute(std::string const &name, boost::any const &attr)
{
    pimpl_->component_->set_attribute(name, attr);
}

// ==========================================================================
// MAKE_VIEWPORT
// ==========================================================================
std::shared_ptr<viewport> make_viewport(
    std::shared_ptr<component> const &underlying_component)
{
    return std::make_shared<viewport>(underlying_component);
}

}

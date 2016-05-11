// ==========================================================================
// Munin Composite Container.
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
#include "munin/composite_component.hpp"
#include "munin/basic_container.hpp"

namespace munin {

// ==========================================================================
// COMPOSITE_COMPONENT::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct composite_component::impl
{
    std::shared_ptr<container> container_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
composite_component::composite_component()
    : pimpl_(new impl)
{
    pimpl_->container_ = std::make_shared<basic_container>();

    // Connect the underlying container's default signals to the signals
    // of this component.
    get_container()->on_redraw.connect(
        [this](auto const &regions){on_redraw(regions);});

    get_container()->on_layout_change.connect([this]{on_layout_change();});

    get_container()->on_position_changed.connect(
        [this](auto x, auto y){on_position_changed(x, y);});

    get_container()->on_size_changed.connect([this]{on_size_changed();});

    get_container()->on_preferred_size_changed.connect(
        [this]{on_preferred_size_changed();});

    get_container()->on_focus_set.connect([this]{on_focus_set();});

    get_container()->on_focus_lost.connect([this]{on_focus_lost();});

    get_container()->on_cursor_state_changed.connect(
        [this](auto const &state){on_cursor_state_changed(state);});

    get_container()->on_cursor_position_changed.connect(
        [this](auto const &pos){on_cursor_position_changed(pos);});
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
composite_component::~composite_component()
{
}

// ==========================================================================
// GET_CONTAINER
// ==========================================================================
std::shared_ptr<container> composite_component::get_container()
{
    return pimpl_->container_;
}

// ==========================================================================
// DO_SET_POSITION
// ==========================================================================
void composite_component::do_set_position(terminalpp::point const &position)
{
    pimpl_->container_->set_position(position);
}

// ==========================================================================
// DO_GET_POSITION
// ==========================================================================
terminalpp::point composite_component::do_get_position() const
{
    return pimpl_->container_->get_position();
}

// ==========================================================================
// DO_SET_SIZE
// ==========================================================================
void composite_component::do_set_size(terminalpp::extent const &size)
{
    pimpl_->container_->set_size(size);
}

// ==========================================================================
// DO_GET_SIZE
// ==========================================================================
terminalpp::extent composite_component::do_get_size() const
{
    return pimpl_->container_->get_size();
}

// ==========================================================================
// DO_SET_PARENT
// ==========================================================================
void composite_component::do_set_parent(std::shared_ptr<component> const &parent)
{
    pimpl_->container_->set_parent(parent);
}

// ==========================================================================
// DO_GET_PARENT
// ==========================================================================
std::shared_ptr<component> composite_component::do_get_parent() const
{
    return pimpl_->container_->get_parent();
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
terminalpp::extent composite_component::do_get_preferred_size() const
{
    return pimpl_->container_->get_preferred_size();
}

// ==========================================================================
// DO_HAS_FOCUS
// ==========================================================================
bool composite_component::do_has_focus() const
{
    return pimpl_->container_->has_focus();
}

// ==========================================================================
// DO_SET_CAN_FOCUS
// ==========================================================================
void composite_component::do_set_can_focus(bool focus)
{
    pimpl_->container_->set_can_focus(focus);
}

// ==========================================================================
// DO_CAN_FOCUS
// ==========================================================================
bool composite_component::do_can_focus() const
{
    return pimpl_->container_->can_focus();
}

// ==========================================================================
// DO_SET_FOCUS
// ==========================================================================
void composite_component::do_set_focus()
{
    pimpl_->container_->set_focus();
}

// ==========================================================================
// DO_LOSE_FOCUS
// ==========================================================================
void composite_component::do_lose_focus()
{
    pimpl_->container_->lose_focus();
}

// ==========================================================================
// DO_FOCUS_NEXT
// ==========================================================================
void composite_component::do_focus_next()
{
    pimpl_->container_->focus_next();
}

// ==========================================================================
// DO_FOCUS_PREVIOUS
// ==========================================================================
void composite_component::do_focus_previous()
{
    pimpl_->container_->focus_previous();
}

// ==========================================================================
// DO_GET_FOCUSSED_COMPONENT
// ==========================================================================
std::shared_ptr<component> composite_component::do_get_focussed_component()
{
    return pimpl_->container_->get_focussed_component();
}

// ==========================================================================
// DO_ENABLE
// ==========================================================================
void composite_component::do_enable()
{
    pimpl_->container_->enable();
}

// ==========================================================================
// DO_DISABLE
// ==========================================================================
void composite_component::do_disable()
{
    pimpl_->container_->disable();
}

// ==========================================================================
// DO_IS_ENABLED
// ==========================================================================
bool composite_component::do_is_enabled() const
{
    return pimpl_->container_->is_enabled();
}

// ==========================================================================
// DO_GET_CURSOR_STATE
// ==========================================================================
bool composite_component::do_get_cursor_state() const
{
    return pimpl_->container_->get_cursor_state();
}

// ==========================================================================
// DO_GET_CURSOR_POSITION
// ==========================================================================
terminalpp::point composite_component::do_get_cursor_position() const
{
    return pimpl_->container_->get_cursor_position();
}

// ==========================================================================
// DO_SET_CURSOR_POSITION
// ==========================================================================
void composite_component::do_set_cursor_position(terminalpp::point const &position)
{
    pimpl_->container_->set_cursor_position(position);
}

// ==========================================================================
// DO_LAYOUT
// ==========================================================================
void composite_component::do_layout()
{
    pimpl_->container_->layout();
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void composite_component::do_draw(
    context         &ctx
  , rectangle const &region)
{
    pimpl_->container_->draw(ctx, region);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void composite_component::do_event(boost::any const &event)
{
    pimpl_->container_->event(event);
}

// ==========================================================================
// DO_SET_ATTRIBUTE
// ==========================================================================
void composite_component::do_set_attribute(
    std::string const &name,
    boost::any const  &attr)
{
    pimpl_->container_->set_attribute(name, attr);
}

}


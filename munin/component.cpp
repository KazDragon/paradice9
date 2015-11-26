// ==========================================================================
// Munin Component.
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
#include "munin/component.hpp"
#include <cassert>

namespace munin {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
component::component()
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
component::~component()
{
}

// ==========================================================================
// SET_POSITION
// ==========================================================================
void component::set_position(terminalpp::point const &position)
{
    do_set_position(position);
}

// ==========================================================================
// GET_POSITION
// ==========================================================================
terminalpp::point component::get_position() const
{
    return do_get_position();
}

// ==========================================================================
// SET_SIZE
// ==========================================================================
void component::set_size(terminalpp::extent const &size)
{
    assert(size.width >= 0);
    assert(size.height >= 0);
    do_set_size(size);
}

// ==========================================================================
// GET_SIZE
// ==========================================================================
terminalpp::extent component::get_size() const
{
    return do_get_size();
}

// ==========================================================================
// SET_PARENT
// ==========================================================================
void component::set_parent(std::shared_ptr<component> const &parent)
{
    do_set_parent(parent);
}

// ==========================================================================
// GET_PARENT
// ==========================================================================
std::shared_ptr<component> component::get_parent() const
{
    return do_get_parent();
}

// ==========================================================================
// GET_PREFERRED_SIZE
// ==========================================================================
terminalpp::extent component::get_preferred_size() const
{
    return do_get_preferred_size();
}

// ==========================================================================
// HAS_FOCUS
// ==========================================================================
bool component::has_focus() const
{
    return do_has_focus();
}

// ==========================================================================
// SET_CAN_FOCUS
// ==========================================================================
void component::set_can_focus(bool focus)
{
    do_set_can_focus(focus);
}

// ==========================================================================
// CAN_FOCUS
// ==========================================================================
bool component::can_focus() const
{
    return do_can_focus();
}

// ==========================================================================
// SET_FOCUS
// ==========================================================================
void component::set_focus()
{
    do_set_focus();
}

// ==========================================================================
// LOSE_FOCUS
// ==========================================================================
void component::lose_focus()
{
    do_lose_focus();
}

// ==========================================================================
// FOCUS_NEXT
// ==========================================================================
void component::focus_next()
{
    do_focus_next();
}

// ==========================================================================
// FOCUS_PREVIOUS
// ==========================================================================
void component::focus_previous()
{
    do_focus_previous();
}

// ==========================================================================
// GET_FOCUSSED_COMPONENT
// ==========================================================================
std::shared_ptr<component> component::get_focussed_component()
{
    return do_get_focussed_component();
}

// ==========================================================================
// ENABLE
// ==========================================================================
void component::enable()
{
    do_enable();
}

// ==========================================================================
// DISABLE
// ==========================================================================
void component::disable()
{
    do_disable();
}

// ==========================================================================
// IS_ENABLED
// ==========================================================================
bool component::is_enabled() const
{
    return do_is_enabled();
}

// ==========================================================================
// GET_CURSOR_STATE
// ==========================================================================
bool component::get_cursor_state() const
{
    return do_get_cursor_state();
}

// ==========================================================================
// GET_CURSOR_POSITION
// ==========================================================================
terminalpp::point component::get_cursor_position() const
{
    return do_get_cursor_position();
}

// ==========================================================================
// SET_CURSOR_POSITION
// ==========================================================================
void component::set_cursor_position(terminalpp::point const &position)
{
    do_set_cursor_position(position);
}

// ==========================================================================
// SET_ATTRIBUTE
// ==========================================================================
void component::set_attribute(std::string const &name, boost::any const &attr)
{
    do_set_attribute(name, attr);
}

// ==========================================================================
// LAYOUT
// ==========================================================================
void component::layout()
{
    do_layout();
}

// ==========================================================================
// DRAW
// ==========================================================================
void component::draw(
    context         &ctx
  , rectangle const &region)
{
    do_draw(ctx, region);
}

// ==========================================================================
// EVENT
// ==========================================================================
void component::event(boost::any const &ev)
{
    do_event(ev);
}

}


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
#include "munin/basic_component.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/weak_ptr.hpp>
#include <map>

using namespace odin;
using namespace boost;
using namespace std;

namespace munin {

// ==========================================================================
// BASIC_COMPONENT::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct basic_component::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(basic_component &self)
        : self_(self)
        , can_focus_(true)
        , has_focus_(false)
        , enabled_(true)
    {
    }
        
    // ======================================================================
    // TOGGLE_FOCUS
    // ======================================================================
    void toggle_focus()
    {
        if (has_focus_)
        {
            has_focus_ = false;
            self_.on_focus_lost();
        }
        else
        {
            has_focus_ = true;
            self_.on_focus_set();
        }
    }
    
    basic_component     &self_;
    weak_ptr<component>  parent_;
    map<string, any>     attributes_;
    rectangle            bounds_;
    bool                 can_focus_;
    bool                 has_focus_;
    bool                 enabled_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
basic_component::basic_component()
{
    pimpl_ = make_shared<impl>(ref(*this));
}

// ==========================================================================
// DESTRUCTOR 
// ==========================================================================
basic_component::~basic_component()
{
}

// ==========================================================================
// DO_SET_POSITION 
// ==========================================================================
void basic_component::do_set_position(point const &position)
{
    BOOST_AUTO(old_position, pimpl_->bounds_.origin);
    pimpl_->bounds_.origin = position;
    
    on_position_changed(old_position, position);
}

// ==========================================================================
// DO_GET_POSITION
// ==========================================================================
point basic_component::do_get_position() const
{
    return pimpl_->bounds_.origin;
}

// ==========================================================================
// DO_SET_SIZE
// ==========================================================================
void basic_component::do_set_size(extent const &size)
{
    BOOST_AUTO(old_size, pimpl_->bounds_.size);
    pimpl_->bounds_.size = size;

    on_size_changed(old_size, size);
}

// ==========================================================================
// DO_GET_SIZE
// ==========================================================================
extent basic_component::do_get_size() const
{
    return pimpl_->bounds_.size;
}

// ==========================================================================
// DO_SET_PARENT
// ==========================================================================
void basic_component::do_set_parent(shared_ptr<component> parent)
{
    pimpl_->parent_ = parent;
}

// ==========================================================================
// DO_GET_PARENT
// ==========================================================================
shared_ptr<component> basic_component::do_get_parent() const
{
    return pimpl_->parent_.lock();
}

// ==========================================================================
// DO_SET_CAN_FOCUS
// ==========================================================================
void basic_component::do_set_can_focus(bool focus)
{
    pimpl_->can_focus_ = focus;
}

// ==========================================================================
// DO_CAN_FOCUS
// ==========================================================================
bool basic_component::do_can_focus() const
{
    return pimpl_->can_focus_;
}

// ==========================================================================
// DO_HAS_FOCUS
// ==========================================================================
bool basic_component::do_has_focus() const
{
    return pimpl_->has_focus_;
}

// ==========================================================================
// DO_SET_FOCUS
// ==========================================================================
void basic_component::do_set_focus()
{
    if (can_focus())
    {
        pimpl_->has_focus_ = true;
        on_focus_set();
    }
}

// ==========================================================================
// DO_LOSE_FOCUS
// ==========================================================================
void basic_component::do_lose_focus()
{
    pimpl_->has_focus_ = false;
    on_focus_lost();
}

// ==========================================================================
// DO_FOCUS_NEXT
// ==========================================================================
void basic_component::do_focus_next()
{
    pimpl_->toggle_focus();
}

// ==========================================================================
// DO_FOCUS_PREVIOUS
// ==========================================================================
void basic_component::do_focus_previous()
{
    pimpl_->toggle_focus();
}

// ==========================================================================
// DO_GET_FOCUSSED_COMPONENT
// ==========================================================================
shared_ptr<component> basic_component::do_get_focussed_component()
{
    return pimpl_->has_focus_
         ? shared_ptr<component>(shared_from_this())
         : shared_ptr<component>();
}

// ==========================================================================
// DO_ENABLE
// ==========================================================================
void basic_component::do_enable()
{
    pimpl_->enabled_ = true;
}

// ==========================================================================
// DO_DISABLE
// ==========================================================================
void basic_component::do_disable()
{
    pimpl_->enabled_ = false;
}

// ==========================================================================
// DO_IS_ENABLED
// ==========================================================================
bool basic_component::do_is_enabled() const
{
    return pimpl_->enabled_;
}

// ==========================================================================
// DO_GET_CURSOR_STATE
// ==========================================================================
bool basic_component::do_get_cursor_state() const
{
    // By default, a component has no cursor.
    return false;
}

// ==========================================================================
// DO_GET_CURSOR_POSITION
// ==========================================================================
point basic_component::do_get_cursor_position() const
{
    // By default, a component has no cursor, so we choose a sentry
    // value of (0,0) for its non-existent location.
    return point();
}

// ==========================================================================
// GET_ATTRIBUTE
// ==========================================================================
any basic_component::get_attribute(string const &name) const
{
    BOOST_AUTO(attr_iterator, pimpl_->attributes_.find(name));
    
    return attr_iterator == pimpl_->attributes_.end()
         ? any()
         : *attr_iterator;
}

// ==========================================================================
// DO_SET_ATTRIBUTE
// ==========================================================================
void basic_component::do_set_attribute(
    string const &name, any const &attr)
{
    pimpl_->attributes_[name] = attr;
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void basic_component::do_event(any const &event)
{
    BOOST_AUTO(mouse, any_cast<odin::ansi::mouse_report>(&event));
    
    if (mouse != NULL)
    {
        if (!has_focus())
        {
            set_focus();
        }
    }
}

}


// ==========================================================================
// Munin Card.
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
#include "munin/card.hpp"
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>
#include <map>

using namespace odin;
using namespace boost;
using namespace boost::assign;
using namespace std;

namespace munin {

// ==========================================================================
// CARD::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct card::impl
{
    // ======================================================================
    //  GET_CURRENT_FACE
    // ======================================================================
    optional< shared_ptr<component> > get_current_face() const
    {
        if (current_face_.is_initialized())
        {
            BOOST_AUTO(face_iter, faces_.find(current_face_.get()));
    
            if (face_iter != faces_.end())
            {
                return face_iter->second;
            }
        }
    
        return NULL;
    }
    
    typedef map< string, shared_ptr<component> > face_map_type;
    face_map_type                                faces_;
    optional<string>                             current_face_;
};

// ==========================================================================
// CONSTRUCTOR 
// ==========================================================================
card::card()
    : pimpl_(make_shared<impl>())
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
card::~card()
{
}

// ==========================================================================
// ADD_FACE 
// ==========================================================================
void card::add_face(
    shared_ptr<component> const &comp,
    string                const &name)
{
    pimpl_->faces_[name] = comp;

    // Connect the underlying container's default signals to the signals
    // of this component with.
    comp->on_redraw.connect(
        boost::bind(boost::ref(on_redraw), _1));

    comp->on_layout_change.connect(
        boost::bind(boost::ref(on_layout_change)));

    comp->on_position_changed.connect(
        boost::bind(boost::ref(on_position_changed), _1, _2));

    comp->on_focus_set.connect(
        boost::bind(boost::ref(on_focus_set)));

    comp->on_focus_lost.connect(
        boost::bind(boost::ref(on_focus_lost)));
    
    comp->on_cursor_state_changed.connect(
        boost::bind(boost::ref(on_cursor_state_changed), _1));

    comp->on_cursor_position_changed.connect(
        boost::bind(boost::ref(on_cursor_position_changed), _1));
}

// ==========================================================================
// GET_NUMBER_OF_FACES
// ==========================================================================
u32 card::get_number_of_faces() const
{
    return pimpl_->faces_.size();
}

// ==========================================================================
// SELECT_FACE
// ==========================================================================
void card::select_face(string const &name)
{
    BOOST_AUTO(old_face,      pimpl_->get_current_face());
    BOOST_AUTO(old_face_name, pimpl_->current_face_);

    pimpl_->current_face_ = name;

    BOOST_AUTO(new_face, pimpl_->get_current_face());

    if (!old_face_name.is_initialized() 
     || old_face_name.get() != pimpl_->current_face_)
    {
        // The name was initialised, so there must be a face to go with 
        // it.

        // Determine any focus change between the faces.
        bool old_focussed = old_face.is_initialized()
                          ? old_face.get()->has_focus()
                          : false;

        bool new_focussed = new_face.is_initialized()
                          ? new_face.get()->has_focus()
                          : false;

        // Determine any cursor state change
        bool old_cursor_state = old_face.is_initialized()
                              ? old_face.get()->get_cursor_state()
                              : false;

        bool new_cursor_state = new_face.is_initialized()
                              ? new_face.get()->get_cursor_state()
                              : false;

        point cursor_position = new_face.is_initialized()
                              ? new_face.get()->get_cursor_position()
                              : point();

        // Fire any necessary events.
        if (old_focussed != new_focussed)
        {
            if (new_focussed)
            {
                on_focus_set();
            }
            else
            {
                on_focus_lost();
            }
        }

        if (old_cursor_state != new_cursor_state)
        {
            on_cursor_state_changed(new_cursor_state);
        }

        on_cursor_position_changed(cursor_position);

        // The face of the card has changed.  Fire an on_redraw event for
        // the entire component.
        on_redraw(list_of(rectangle(point(), get_size())));
    }
}

// ==========================================================================
// DO_HASE_FOCUS
// ==========================================================================
bool card::do_has_focus() const
{
    BOOST_AUTO(current_face, pimpl_->get_current_face());

    if (current_face.is_initialized())
    {
        return current_face.get()->has_focus();
    }

    return false;
}

// ==========================================================================
// DO_CAN_FOCUS
// ==========================================================================
bool card::do_can_focus() const
{
    BOOST_AUTO(current_face, pimpl_->get_current_face());

    if (current_face.is_initialized())
    {
        return current_face.get()->can_focus();
    }

    return false;
}

// ==========================================================================
// DO_SET_FOCUS
// ==========================================================================
void card::do_set_focus()
{
    BOOST_AUTO(current_face, pimpl_->get_current_face());

    if (current_face.is_initialized())
    {
        current_face.get()->set_focus();
    }
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
extent card::do_get_preferred_size() const
{
    return extent();
}

// ==========================================================================
// DO_LAYOUT
// ==========================================================================
void card::do_layout()
{
    BOOST_FOREACH(impl::face_map_type::value_type face_pair, pimpl_->faces_)
    {
        face_pair.second->layout();
    }
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void card::do_draw(
    canvas          &cvs
  , rectangle const &region)
{
    if (pimpl_->current_face_.is_initialized())
    {
        BOOST_AUTO(current_face, pimpl_->current_face_.get());
        BOOST_AUTO(face_iter, pimpl_->faces_.find(current_face));

        if (face_iter != pimpl_->faces_.end())
        {
            face_iter->second->draw(cvs, region);
        }
    }
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void card::do_event(any const &event)
{
    if (pimpl_->current_face_.is_initialized())
    {
        BOOST_AUTO(current_face, pimpl_->current_face_.get());
        BOOST_AUTO(face_iter, pimpl_->faces_.find(current_face));

        if (face_iter != pimpl_->faces_.end())
        {
            face_iter->second->event(event);
        }
    }
}

// ==========================================================================
// DO_SET_SIZE
// ==========================================================================
void card::do_set_size(extent const &size) 
{
    basic_component::do_set_size(size);

    BOOST_FOREACH(impl::face_map_type::value_type entry, pimpl_->faces_)
    {
        entry.second->set_size(size);
    }
}

// ==========================================================================
// DO_FOCUS_NEXT
// ==========================================================================
void card::do_focus_next() 
{
    BOOST_AUTO(current_face, pimpl_->get_current_face());

    if (current_face.is_initialized())
    {
        current_face.get()->focus_next();
    }
}

// ==========================================================================
// DO_FOCUS_PREVIOUS
// ==========================================================================
void card::do_focus_previous() 
{
    BOOST_AUTO(current_face, pimpl_->get_current_face());

    if (current_face.is_initialized())
    {
        current_face.get()->focus_previous();
    }
}

// ==========================================================================
// DO_GET_FOCUSSED_COMPONENT
// ==========================================================================
shared_ptr<component> card::do_get_focussed_component() 
{
    BOOST_AUTO(current_face, pimpl_->get_current_face());

    if (current_face.is_initialized())
    {
        return current_face.get()->get_focussed_component();
    }

    return shared_ptr<component>();
}

// ==========================================================================
// DO_ENABLE
// ==========================================================================
void card::do_enable()
{
    BOOST_AUTO(current_face, pimpl_->get_current_face());

    if (current_face.is_initialized())
    {
        current_face.get()->enable();
    }
}

// ==========================================================================
// DO_DISABLE
// ==========================================================================
void card::do_disable()
{
    BOOST_AUTO(current_face, pimpl_->get_current_face());

    if (current_face.is_initialized())
    {
        current_face.get()->disable();
    }
}

// ==========================================================================
// DO_IS_ENABLED
// ==========================================================================
bool card::do_is_enabled() const
{
    BOOST_AUTO(current_face, pimpl_->get_current_face());

    if (current_face.is_initialized())
    {
        return current_face.get()->is_enabled();
    }

    return false;
}

// ==========================================================================
// DO_GET_CURSOR_STATE
// ==========================================================================
bool card::do_get_cursor_state() const 
{
    BOOST_AUTO(current_face, pimpl_->get_current_face());

    if (current_face.is_initialized())
    {
        return current_face.get()->get_cursor_state();
    }
    
    return false;
}

// ==========================================================================
// DO_GET_CURSOR_POSITION
// ==========================================================================
point card::do_get_cursor_position() const 
{
    BOOST_AUTO(current_face, pimpl_->get_current_face());

    if (current_face.is_initialized())
    {
        return current_face.get()->get_cursor_position();
    }

    return point();
}

// ==========================================================================
// DO_SET_CURSOR_POSITION
// ==========================================================================
void card::do_set_cursor_position(point const &position)
{
    BOOST_AUTO(current_face, pimpl_->get_current_face());

    if (current_face.is_initialized())
    {
        return current_face.get()->set_cursor_position(position);
    }
}

// ==========================================================================
// DO_SET_ATTRIBUTE
// ==========================================================================
void card::do_set_attribute(
    string const &name, any const &attr)
{
    BOOST_AUTO(current_face, pimpl_->get_current_face());

    if (current_face.is_initialized())
    {
        current_face.get()->set_attribute(name, attr);
    }
}

}


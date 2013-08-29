// ==========================================================================
// Munin Basic Container.
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
#include "munin/basic_container.hpp"
#include "munin/canvas.hpp"
#include "munin/layout.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/scope_exit.hpp>
#include <boost/typeof/typeof.hpp>

using namespace odin;
using namespace boost;
using namespace std;

namespace munin {

namespace {
    typedef map< u32, shared_ptr<layout> > layered_layout_map;
}

// ==========================================================================
// BASIC_CONTAINER::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct basic_container::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(basic_container &self)
        : self_(self)
        , has_focus_(false)
        , cursor_state_(false)
        , enabled_(true)
    {
    }
    
    // ======================================================================
    // SUBCOMPONENT_FOCUS_SET_HANDLER
    // ======================================================================
    void subcomponent_focus_set_handler(
        weak_ptr<component> const &subcomponent)
    {
        BOOST_AUTO(focussed_component, subcomponent.lock());

        if (focussed_component != NULL)
        {
            // Iterate through all our subcomponents and ensure that this is
            // the only subcomponent to have focus.  Then, if we do not have
            // focus, set our focus.
            BOOST_FOREACH(
                boost::shared_ptr<component> current_component
              , components_)
            {
                if (current_component != focussed_component
                 && current_component->has_focus())
                {
                    current_component->lose_focus();
                }
            }        
    
            if (!has_focus_)
            {
                has_focus_ = true;
                self_.on_focus_set();
            }
            
            focus_changed();
        }
    }
    
    // ======================================================================
    // SUBCOMPONENT_FOCUS_LOST_HANDLER
    // ======================================================================
    void subcomponent_focus_lost_handler(
        weak_ptr<component> const &subcomponent)
    {
        BOOST_AUTO(unfocussed_component, subcomponent.lock());
        
        if (unfocussed_component != NULL)
        {
            // Iterate through all our subcomponents to see if we have any
            // subcomponents focused (for example, setting the focus of a 
            // new subcomponent may cause the focus for the reporting 
            // component to be lost).  If we do not, then unfocus this 
            // component.
            bool component_is_focused = false;
    
            BOOST_FOREACH(
                boost::shared_ptr<component> current_component
              , components_)
            {
                if (current_component->has_focus())
                {
                    component_is_focused = true;
                    break;
                }
            }
            
            if (!component_is_focused)
            {
                has_focus_ = false;
                self_.on_focus_lost();
            }
            
            focus_changed();
        }
    }
    
    // ======================================================================
    // FOCUS_CHANGED
    // ======================================================================
    void focus_changed()
    {
        if (has_focus_)
        {
            // Find the currently focussed subcomponent.
            BOOST_FOREACH(
                boost::shared_ptr<component> current_component
              , components_)
            {
                if (current_component->has_focus())
                {
                    // Update for the new cursor state if necessary.
                    bool new_cursor_state = 
                        current_component->get_cursor_state();

                    if (new_cursor_state != cursor_state_)
                    {
                        cursor_state_ = new_cursor_state;
                        self_.on_cursor_state_changed(cursor_state_);
                    }
                    
                    // If the cursor state is enabled, then update for a new
                    // cursor position.
                    if (cursor_state_)
                    {
                        point cursor_position =
                            current_component->get_position()
                          + current_component->get_cursor_position();
                          
                        self_.on_cursor_position_changed(cursor_position);
                    }
                }
            }
        }
    }

    // ======================================================================
    // SUBCOMPONENT_CURSOR_STATE_CHANGE_HANDLER
    // ======================================================================
    void subcomponent_cursor_state_change_handler(
        weak_ptr<component> const &weak_subcomponent
      , bool                       state)
    {
        BOOST_AUTO(subcomponent, weak_subcomponent.lock());
        
        if (subcomponent
         && state != cursor_state_ 
         && subcomponent->has_focus())
        {
            cursor_state_ = state;
            self_.on_cursor_state_changed(cursor_state_);
        }
    }
    
    // ======================================================================
    // SUBCOMPONENT_CURSOR_POSITION_CHANGE_HANDLER
    // ======================================================================
    void subcomponent_cursor_position_change_handler(
        weak_ptr<component> const &weak_subcomponent
      , point                      position)
    {
        BOOST_AUTO(subcomponent, weak_subcomponent.lock());
        
        if (subcomponent && subcomponent->has_focus())
        {
            self_.on_cursor_position_changed(
                subcomponent->get_position() + position);
        }
    }
    
    // ======================================================================
    // SUBCOMPONENT_POSITION_CHANGE_HANDLER
    // ======================================================================
    void subcomponent_position_change_handler(
        weak_ptr<component> const &weak_subcomponent
      , point                      changed_from
      , point                      changed_to)
    {
        BOOST_AUTO(subcomponent, weak_subcomponent.lock());
        
        if (subcomponent)
        {
            extent const subcomponent_size = subcomponent->get_size();
            
            vector<rectangle> regions;
            regions.push_back(rectangle(changed_from, subcomponent_size));
            regions.push_back(rectangle(changed_to, subcomponent_size));
        }
    }

    // ======================================================================
    // SUBCOMPONENT_PREFERRED_SIZE_CHANGE_HANDLER
    // ======================================================================
    void subcomponent_preferred_size_changed_handler(
        weak_ptr<component> const &weak_subcomponent)
    {
        BOOST_AUTO(subcomponent, weak_subcomponent.lock());

        if (subcomponent)
        {
            self_.on_preferred_size_changed();
        }
    }

    // ======================================================================
    // FOCUS_NEXT_HAS_FOCUS
    // ======================================================================
    void focus_next_has_focus()
    {
        // We have focus.  Therefore, find the subcomponent that also has 
        // focus and tell it to move to the next focus.  After that, if it
        // has no focus, we move the focus to the next subcomponent that
        // can be focused.  If there is no such object, then we lose our
        // focus.
        u32 number_of_components = u32(components_.size());
        
        for (u32 index = 0; index < number_of_components; ++index)
        {
            BOOST_AUTO(current_component, components_[index]);
                
            if (current_component->has_focus())
            {
                current_component->focus_next();
                
                if (!current_component->has_focus())
                {
                    bool focused = false;
                    
                    // Find the index of the next component that can have focus.
                    for (u32 next_index = index + 1; 
                         next_index < number_of_components && !focused;
                         ++next_index)
                    {
                        BOOST_AUTO(next_component, components_[next_index]);
                            
                        if (next_component->can_focus())
                        {
                            next_component->focus_next();
                            focused = true;
                        }
                    }
                    
                    if (!focused)
                    {
                        has_focus_ = false;
                        self_.on_focus_lost();
                    }
                }
                
                break;
            }
        }
    }

    // ======================================================================
    // FOCUS_NEXT_NO_FOCUS
    // ======================================================================
    void focus_next_no_focus()
    {
        // We do not currently have any focus, therefore we command the first
        // subcomponent to focus its next component.
        BOOST_FOREACH(shared_ptr<component> current_component, components_)
        {
            if (current_component->can_focus())
            {
                current_component->focus_next();
                break;
            }
        }
    }
    
    // ======================================================================
    // FOCUS_PREVIOUS_HAS_FOCUS
    // ======================================================================
    void focus_previous_has_focus()
    {
        // We do have focus.  Find the currently focused component and tell
        // it to move its focus to its previous subcomponent.  If, after 
        // that, it no longer has focus, then we find the component prior
        // to that that can have focus and tell it to set its previous focus.
        // If there is no component to focus, then just lose our focus.
        u32 number_of_components = u32(components_.size());

        for (u32 index = 0; index < number_of_components; ++index)
        {
            BOOST_AUTO(current_component, components_[index]);
                
            if (current_component->has_focus())
            {
                current_component->focus_previous();
                
                if (!current_component->has_focus())
                {
                    bool focused = false;
                    
                    // Seek for the first subcomponent prior to the unfocused 
                    // one that can have a focus, and focus it.
                    for (u32 last_index = index;
                         last_index > 0 && !focused;
                         --last_index)
                    {
                        BOOST_AUTO(last_component, components_[last_index - 1]);
                            
                        if (last_component->can_focus())
                        {
                            // If the component is a container, this will focus
                            // the last component in that container.
                            last_component->focus_previous();
                            focused = true;
                        }
                    }
                    
                    if (!focused)
                    {
                        // There are no subcomponents to focus.
                        has_focus_ = false;
                        self_.on_focus_lost();
                    }
                }
                
                break;
            }
        }
    }

    // ======================================================================
    // FOCUS_PREVIOUS_NO_FOCUS
    // ======================================================================
    void focus_previous_no_focus()
    {
        // Find the last component that could have focus and focus its
        // last element.
        u32 number_of_components = u32(components_.size());

        for (u32 index = number_of_components; index > 0; --index)
        {
            BOOST_AUTO(last_component, components_[index - 1]);
                
            if (last_component->can_focus())
            {
                last_component->focus_previous();
                break;
            }
        }
    }
    
    basic_container                               &self_;
    weak_ptr<component>                            parent_;
    vector< shared_ptr<component> >                components_;
    vector< any >                                  component_hints_;
    vector< u32 >                                  component_layers_;
    vector< vector<boost::signals::connection > >  component_connections_;
    layered_layout_map                             layouts_;
    rectangle                                      bounds_;
    bool                                           has_focus_;
    bool                                           cursor_state_;
    bool                                           enabled_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
basic_container::basic_container()
{
    pimpl_ = make_shared<impl>(ref(*this));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
basic_container::~basic_container()
{
    BOOST_FOREACH(
        vector<boost::signals::connection> &vec
      , pimpl_->component_connections_)
    {
        BOOST_FOREACH(boost::signals::connection &cnx, vec)
        {
            cnx.disconnect();
        }
    }
}

// ==========================================================================
// DO_SET_POSITION
// ==========================================================================
void basic_container::do_set_position(point const &position)
{
    pimpl_->bounds_.origin = position;
}

// ==========================================================================
// DO_GET_POSITION
// ==========================================================================
point basic_container::do_get_position() const
{
    return pimpl_->bounds_.origin;
}

// ==========================================================================
// DO_SET_SIZE
// ==========================================================================
void basic_container::do_set_size(extent const &size)
{
    pimpl_->bounds_.size = size;
    on_layout_change();
}

// ==========================================================================
// DO_GET_SIZE
// ==========================================================================
extent basic_container::do_get_size() const
{
    return pimpl_->bounds_.size;
}

// ==========================================================================
// DO_SET_PARENT
// ==========================================================================
void basic_container::do_set_parent(shared_ptr<component> parent)
{
    pimpl_->parent_ = parent;
}

// ==========================================================================
// DO_GET_PARENT
// ==========================================================================
shared_ptr<component> basic_container::do_get_parent() const
{
    return pimpl_->parent_.lock();
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
extent basic_container::do_get_preferred_size() const
{
    // If there are any layouts, then find the union of their preferred
    // sizes.  Otherwise, our current size is just fine.
    if (pimpl_->layouts_.empty())
    {
        return get_size();
    }
    
    extent preferred_size(0, 0);

    // Sort the components/hints into layers
    typedef map< u32, vector< shared_ptr<component> > > clmap;
    typedef map< u32, vector< any > >                   chmap;
    
    clmap component_layers_map;
    chmap component_hints_map;
    
    // Iterate through the components, sorting them by layer
    for (u32 index = 0; index < pimpl_->components_.size(); ++index)
    {
        BOOST_AUTO(comp,  pimpl_->components_[index]);
        BOOST_AUTO(hint,  pimpl_->component_hints_[index]);
        BOOST_AUTO(layer, pimpl_->component_layers_[index]);
        
        component_layers_map[layer].push_back(comp);
        component_hints_map[layer].push_back(hint);
    }
    
    // Iterate through the layers, layout out each.
    BOOST_FOREACH(clmap::value_type component_layer_pair, component_layers_map)
    {
        BOOST_AUTO(layer,      component_layer_pair.first);
        BOOST_AUTO(components, component_layer_pair.second);
        BOOST_AUTO(hints,      component_hints_map[layer]);

        BOOST_AUTO(lyt, get_layout(layer));
        
        if (lyt == NULL || components.size() == 0) 
        {
            // Either there is no layout for this layer, or there are no
            // components in this layer.  Hence no point in laying it out.
            // Continue with the next layer.
            continue;
        }
        
        BOOST_AUTO(
            lp_preferred_size
          , lyt->get_preferred_size(components, hints));
        
        preferred_size.width = 
            (max)(preferred_size.width, lp_preferred_size.width);
        preferred_size.height =
            (max)(preferred_size.height, lp_preferred_size.height);
    }

    return preferred_size;
}

// ==========================================================================
// DO_GET_NUMBER_OF_COMPONENTS
// ==========================================================================
u32 basic_container::do_get_number_of_components() const
{
    return u32(pimpl_->components_.size());
}

// ==========================================================================
// DO_ADD_COMPONENT
// ==========================================================================
void basic_container::do_add_component(
    shared_ptr<component> const &comp
  , any                   const &hint
  , u32                          layer)
{
    // Store the component and the layer in which it is to be drawn.
    pimpl_->components_.push_back(comp);
    pimpl_->component_hints_.push_back(hint);
    pimpl_->component_layers_.push_back(layer);
    
    vector<boost::signals::connection> component_connections;
    
    // Register for callbacks for when the new subcomponent either gains
    // or loses focus.  We can make sure our own focus is correct based
    // on this information.
    component_connections.push_back(comp->on_focus_set.connect(
        boost::bind(
            &basic_container::impl::subcomponent_focus_set_handler
          , pimpl_
          , boost::weak_ptr<component>(comp))));

    component_connections.push_back(comp->on_focus_lost.connect(
        boost::bind(
            &basic_container::impl::subcomponent_focus_lost_handler
          , pimpl_
          , boost::weak_ptr<component>(comp))));
    
    // Register for callbacks for when the subcomponent's cursor state
    // or position changes.
    component_connections.push_back(comp->on_cursor_state_changed.connect(
        boost::bind(
            &basic_container::impl::subcomponent_cursor_state_change_handler
          , pimpl_
          , boost::weak_ptr<component>(comp)
          , _1)));
    
    component_connections.push_back(comp->on_cursor_position_changed.connect(
        boost::bind(
            &basic_container::impl::subcomponent_cursor_position_change_handler
          , pimpl_
          , boost::weak_ptr<component>(comp)
          , _1)));
    
    // Register for callbacks for when the subcomponent's position
    // changes.
    component_connections.push_back(comp->on_position_changed.connect(
        boost::bind(
            &basic_container::impl::subcomponent_position_change_handler
          , pimpl_
          , boost::weak_ptr<component>(comp)
          , _1
          , _2)));
    
    // Register for callbacks for when the subcomponent's preferred size
    // changes.
    component_connections.push_back(comp->on_preferred_size_changed.connect(
        boost::bind(
            &basic_container::impl::subcomponent_preferred_size_changed_handler
           , pimpl_
           , boost::weak_ptr<component>(comp))));

    pimpl_->component_connections_.push_back(component_connections);
    
    comp->set_parent(shared_from_this());
}

// ==========================================================================
// DO_REMOVE_COMPONENT
// ==========================================================================
void basic_container::do_remove_component(
    shared_ptr<component> const &comp)
{
    // When we remove a component, we must also remove the associated hint,
    // layer, and signal connections (which must also be disconnected).
    BOOST_AUTO(current_component, pimpl_->components_.begin());
    BOOST_AUTO(current_hint,      pimpl_->component_hints_.begin());
    BOOST_AUTO(current_layer,     pimpl_->component_layers_.begin());
    BOOST_AUTO(current_signals,   pimpl_->component_connections_.begin());
    
    while (current_component != pimpl_->components_.end()
        && current_hint      != pimpl_->component_hints_.end()
        && current_layer     != pimpl_->component_layers_.end()
        && current_signals   != pimpl_->component_connections_.end())
    {
        if (*current_component == comp)
        {
            current_component  = pimpl_->components_.erase(current_component);
            current_hint       = pimpl_->component_hints_.erase(current_hint);
            current_layer      = pimpl_->component_layers_.erase(current_layer);
            
            BOOST_FOREACH(boost::signals::connection &sig, *current_signals)
            {
                sig.disconnect();
            }
            
            current_signals = pimpl_->component_connections_.erase(current_signals);
        }
        else
        {
            ++current_component;
            ++current_hint;
            ++current_layer;
            ++current_signals;
        }
    }
    
    comp->set_parent(shared_ptr<component>());
}

// ==========================================================================
// DO_GET_COMPONENT
// ==========================================================================
shared_ptr<component> basic_container::do_get_component(u32 index) const
{
    return pimpl_->components_[index];
}

// ==========================================================================
// DO_GET_COMPONENT_HINT
// ==========================================================================
any basic_container::do_get_component_hint(u32 index) const
{
    return pimpl_->component_hints_[index];
}

// ==========================================================================
// DO_GET_COMPONENT_LAYER
// ==========================================================================
u32 basic_container::do_get_component_layer(u32 index) const
{
    return pimpl_->component_layers_[index];
}

// ==========================================================================
// DO_SET_LAYOUT
// ==========================================================================
void basic_container::do_set_layout(
    shared_ptr<munin::layout> const &lyt
  , u32                              layer)
{
    pimpl_->layouts_[layer] = lyt;
}

// ==========================================================================
// DO_GET_LAYOUT
// ==========================================================================
shared_ptr<layout> basic_container::do_get_layout(u32 layer) const
{
    BOOST_AUTO(result, pimpl_->layouts_.find(layer));
    
    return result == pimpl_->layouts_.end()
                   ? shared_ptr<munin::layout>()
                   : result->second;
}

// ==========================================================================
// DO_GET_LAYOUT_LAYERS
// ==========================================================================
vector<u32> basic_container::do_get_layout_layers() const
{
    vector<u32> layers(pimpl_->layouts_.size());
    
    u32 index = 0;
    BOOST_FOREACH(layered_layout_map::value_type lp, pimpl_->layouts_)
    {
        layers[index++] = lp.first;
    }
    
    return layers;
}

// ==========================================================================
// DO_HAS_FOCUS
// ==========================================================================
bool basic_container::do_has_focus() const
{
    return pimpl_->has_focus_;
}

// ==========================================================================
// DO_SET_CAN_FOCUS
// ==========================================================================
void basic_container::do_set_can_focus(bool focus)
{
    // As this is controlled by contained components, this is
    // completely ignored.
}

// ==========================================================================
// DO_CAN_FOCUS
// ==========================================================================
bool basic_container::do_can_focus() const
{
    BOOST_FOREACH(shared_ptr<component> comp, pimpl_->components_)
    {
        if (comp->can_focus())
        {
            return true;
        }
    }
    
    return false;
}

// ==========================================================================
// DO_SET_FOCUS
// ==========================================================================
void basic_container::do_set_focus()
{
    if (can_focus())
    {
        // Find the first component that can be focussed and focus it.
        BOOST_FOREACH(
            shared_ptr<component> current_component
          , pimpl_->components_)
        {
            if (current_component->can_focus())
            {
                current_component->set_focus();
                break;
            }
        }
        
        pimpl_->has_focus_ = true;
        on_focus_set();
    }
}

// ==========================================================================
// DO_LOSE_FOCUS
// ==========================================================================
void basic_container::do_lose_focus()
{
    BOOST_FOREACH(
        shared_ptr<component> current_component
      , pimpl_->components_)
    {
        if (current_component->has_focus())
        {
            current_component->lose_focus();
        }
    }
    
    pimpl_->has_focus_ = false;
}

// ==========================================================================
// DO_FOCUS_NEXT
// ==========================================================================
void basic_container::do_focus_next()
{
    if (pimpl_->has_focus_)
    {
        pimpl_->focus_next_has_focus();
    }
    else
    {
        pimpl_->focus_next_no_focus();
    }
}

// ==========================================================================
// DO_FOCUS_PREVIOUS
// ==========================================================================
void basic_container::do_focus_previous()
{
    if (pimpl_->has_focus_)
    {
        pimpl_->focus_previous_has_focus();
    }
    else
    {
        pimpl_->focus_previous_no_focus();
    }
}

// ==========================================================================
// DO_GET_FOCUSSED_COMPONENT
// ==========================================================================
shared_ptr<component> basic_container::do_get_focussed_component()
{
    if (has_focus())
    {
        BOOST_FOREACH(
            shared_ptr<component> current_component
          , pimpl_->components_)
        {
            if (current_component->has_focus())
            {
                return current_component->get_focussed_component();
            }
        }
    }
    
    return shared_ptr<component>();
}

// ==========================================================================
// DO_ENABLE
// ==========================================================================
void basic_container::do_enable()
{
    pimpl_->enabled_ = true;
}

// ==========================================================================
// DO_DISABLE
// ==========================================================================
void basic_container::do_disable()
{
    pimpl_->enabled_ = false;
}

// ==========================================================================
// DO_IS_ENABLED
// ==========================================================================
bool basic_container::do_is_enabled() const
{
    return pimpl_->enabled_;
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void basic_container::do_event(any const &event)
{
    // We split the events into two types.  Mouse events are passed to
    // whichever component is under the mouse click.  All other events are
    // passed to the focussed component.
    BOOST_AUTO(mouse, any_cast<odin::ansi::mouse_report>(&event));
    
    if (mouse != NULL)
    {
        BOOST_FOREACH(
            shared_ptr<component> current_component
          , pimpl_->components_)
        {
            BOOST_AUTO(position, current_component->get_position());
            BOOST_AUTO(size,     current_component->get_size());
            
            // Check to see if the reported position is within the component's
            // bounds.
            if (mouse->x_position_ >= position.x
             && mouse->x_position_  < position.x + size.width
             && mouse->y_position_ >= position.y
             && mouse->y_position_  < position.y + size.height)
            {
                // Copy the mouse's report and adjust it so that the
                // subcomponent's position is taken into account.
                odin::ansi::mouse_report subreport;
                subreport.button_     = mouse->button_;
                subreport.x_position_ = u8(mouse->x_position_ - position.x);
                subreport.y_position_ = u8(mouse->y_position_ - position.y);
                
                // Forward the event onto the component, then look no further.
                current_component->event(subreport);
                break;
            }
        }
    }
    else
    {
        BOOST_FOREACH(
            shared_ptr<component> current_component
          , pimpl_->components_)
        {
            if (current_component->has_focus())
            {
                current_component->event(event);
                break;
            }
        }
    }
}

// ==========================================================================
// DO_GET_CURSOR_STATE
// ==========================================================================
bool basic_container::do_get_cursor_state() const
{
    return pimpl_->cursor_state_;
}

// ==========================================================================
// DO_GET_CURSOR_POSITION
// ==========================================================================
point basic_container::do_get_cursor_position() const
{
    // If we have no focus, then return the default position.
    if (pimpl_->has_focus_ && pimpl_->cursor_state_)
    {
        // Find the subcomponent that has focus and get its cursor 
        // position.  This must then be offset by the subcomponent's 
        // position within our container.
        BOOST_FOREACH(
            boost::shared_ptr<component> current_component
          , pimpl_->components_)
        {
            if (current_component->has_focus())
            {
                return current_component->get_position()
                     + current_component->get_cursor_position();
            }
        }
    }

    // Either we do not have focus, or the currently focussed subcomponent
    // does not have a cursor.  Return the default position.
    return point();
}

// ==========================================================================
// DO_SET_CURSOR_POSITIONG
// ==========================================================================
void basic_container::do_set_cursor_position(point const &position)
{
    // If we have no focus, then ignore this.
    if (pimpl_->has_focus_ && pimpl_->cursor_state_)
    {
        // Find the subcomponent that has focus and set its cursor 
        // position.  This must then be offset by the subcomponent's 
        // position within our container.
        BOOST_FOREACH(
            boost::shared_ptr<component> current_component
          , pimpl_->components_)
        {
            if (current_component->has_focus())
            {
                current_component->set_cursor_position(
                    position - current_component->get_position());
            }
        }
    }
}

// ==========================================================================
// DO_SET_ATTRIBUTE
// ==========================================================================
void basic_container::do_set_attribute(
    string const &name
  , any    const &attr)
{
    // There's no reason that a container should have any attributes set,
    // so instead this is passed on to all contained components.
    BOOST_FOREACH(shared_ptr<component> comp, pimpl_->components_)
    {
        comp->set_attribute(name, attr);
    }
}

// ==========================================================================
// DO_LAYOUT
// ==========================================================================
void basic_container::do_layout()
{
    // Sort the components/hints into layers
    typedef map< u32, vector< shared_ptr<component> > > clmap;
    typedef map< u32, vector< any > >                   chmap;
    
    clmap component_layers_map;
    chmap component_hints_map;
    
    // Iterate through the components, sorting them by layer
    for (u32 index = 0; index < pimpl_->components_.size(); ++index)
    {
        BOOST_AUTO(comp,  pimpl_->components_[index]);
        BOOST_AUTO(hint,  pimpl_->component_hints_[index]);
        BOOST_AUTO(layer, pimpl_->component_layers_[index]);
        
        component_layers_map[layer].push_back(comp);
        component_hints_map[layer].push_back(hint);
    }
    
    BOOST_AUTO(size, get_size());
    
    // Iterate through the layers, layout out each.
    BOOST_FOREACH(clmap::value_type component_layer_pair, component_layers_map)
    {
        BOOST_AUTO(layer,      component_layer_pair.first);
        BOOST_AUTO(components, component_layer_pair.second);
        BOOST_AUTO(hints,      component_hints_map[layer]);

        BOOST_AUTO(lyt, get_layout(layer));
        
        if (lyt == NULL || components.size() == 0) 
        {
            // Either there is no layout for this layer, or there are no
            // components in this layer.  Hence no point in laying it out.
            // Continue with the next layer.
            continue;
        }
        
        (*lyt)(components, hints, size);
    }
    
    // Now that all the sizes are correct for this container, iterate through 
    // each subcomponent, and lay them out in turn.
    BOOST_FOREACH(shared_ptr<component> comp, pimpl_->components_)
    {
        comp->layout();
    }
}

}


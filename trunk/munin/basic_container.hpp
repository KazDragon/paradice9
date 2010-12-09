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
#ifndef MUNIN_BASIC_CONTAINER_HPP_
#define MUNIN_BASIC_CONTAINER_HPP_

#include "munin/container.hpp"
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>
#include <algorithm>
#include <vector>

namespace munin {
    
//* =========================================================================
/// \brief A default implementation of a container.  Only 
/// do_initialise_region need be overridden to make some kind of default
/// background for the element type.
//* =========================================================================
template <class ElementType>
class basic_container 
    : public container<ElementType>
{
public :
    typedef ElementType            element_type;
    typedef component<ElementType> component_type;
    typedef container<ElementType> container_type;
    typedef layout<ElementType>    layout_type;
    
    basic_container()
        : bounds_(point(0, 0), extent(0, 0))
        , has_focus_(false)
        , cursor_state_(false)
        , enabled_(true)
    {
    }
    
protected :    
    //* =====================================================================
    /// \brief Called when a subcomponent's focus is gained.
    //* =====================================================================
    void subcomponent_focus_set_handler(
        boost::weak_ptr<component_type> const &subcomponent)
    {
        boost::shared_ptr<component_type> focused_component = 
            subcomponent.lock();
            
        if (!focused_component)
        {
            return;
        }
        
        // Iterate through all our subcomponents and ensure that this
        // is the only subcomponent to have focus.  Then, if we do not have
        // focus, set our focus.
        BOOST_FOREACH(
            boost::shared_ptr<component_type> current_component
          , components_)
        {
            if (current_component != focused_component
             && current_component->has_focus())
            {
                current_component->lose_focus();
            }
        }        

        if (!has_focus_)
        {
            has_focus_ = true;
            this->on_focus_set();
        }
        
        focus_changed();
    }
    
    //* =====================================================================
    /// \brief Called when a subcomponent's focus is lost.
    //* =====================================================================
    void subcomponent_focus_lost_handler(
        boost::weak_ptr<component_type> const &subcomponent)
    {
        boost::shared_ptr<component_type> unfocused_component =
            subcomponent.lock();
            
        if (!unfocused_component)
        {
            return;
        }
        
        // Iterate through all our subcomponents to see if we have any
        // subcomponents focused (for example, setting the focus of a new
        // subcomponent may cause the focus for the reporting component to
        // be lost).  If we do not, then unfocus this component.
        bool component_is_focused = false;

        BOOST_FOREACH(
            boost::shared_ptr<component_type> current_component
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
            this->on_focus_lost();
        }
        
        focus_changed();
    }
    
    //* =====================================================================
    /// \brief Called when this or a subcomponent's focus changes.  This is
    /// to ensure that properties that change when moving from one component
    /// to the next (such as cursor state and position) are updated properly.
    //* =====================================================================
    void focus_changed()
    {
        if (has_focus_)
        {
            // Find the currently focussed subcomponent.
            BOOST_FOREACH(
                boost::shared_ptr<component_type> current_component
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
                        this->on_cursor_state_changed(cursor_state_);
                    }
                    
                    // If the cursor state is enabled, then update for a new
                    // cursor position.
                    if (cursor_state_)
                    {
                        point cursor_position =
                            current_component->get_position()
                          + current_component->get_cursor_position();
                          
                        this->on_cursor_position_changed(cursor_position);
                    }
                }
            }
        }
    }
    
    //* =====================================================================
    /// \brief Called when a subcomponent's cursor state changes.  Propagates
    /// the change if necessary.
    //* =====================================================================
    void subcomponent_cursor_state_change_handler(
        boost::weak_ptr<component_type> const &weak_subcomponent
      , bool                                   state)
    {
        boost::shared_ptr<component_type> subcomponent(
            weak_subcomponent.lock());
        
        if (subcomponent
         && state != cursor_state_ 
         && subcomponent->has_focus())
        {
            cursor_state_ = state;
            this->on_cursor_state_changed(cursor_state_);
        }
    }
    
    //* =====================================================================
    /// \brief Called when a subcomponent's cursor position changes.
    /// Propagates the change if necessary.
    //* =====================================================================
    void subcomponent_cursor_position_change_handler(
        boost::weak_ptr<component_type> const &weak_subcomponent
      , point                                  position)
    {
        boost::shared_ptr<component_type> subcomponent(
            weak_subcomponent.lock());
        
        if (subcomponent && subcomponent->has_focus())
        {
            this->on_cursor_position_changed(
                subcomponent->get_position() + position);
        }
    }
    
    //* =====================================================================
    /// \brief Called when a subcomponent's position changes.  Propagates
    /// any necessary redraw events.
    //* =====================================================================
    void subcomponent_position_change_handler(
        boost::weak_ptr<component_type> const &weak_subcomponent
      , point                                  changed_from
      , point                                  changed_to)
    {
        boost::shared_ptr<component_type> subcomponent(
            weak_subcomponent.lock());
        
        if (subcomponent)
        {
            extent const subcomponent_size = subcomponent->get_size();
            
            std::vector<rectangle> redraw_regions;
            redraw_regions.push_back(
                rectangle(changed_from, subcomponent_size));
            redraw_regions.push_back(
                rectangle(changed_to, subcomponent_size));
            
            this->on_redraw(redraw_regions);
        }
    }
    
    //* =====================================================================
    /// \brief Called by set_position().  Derived classes must override this
    /// function in order to set the position of the component in a custom
    /// manner.
    //* =====================================================================
    virtual void do_set_position(point const &position)
    {
        bounds_.origin = position;
    }
    
    //* =====================================================================
    /// \brief Called by get_position().  Derived classes must override this
    /// function in order to get the position of the component in a custom
    /// manner.
    //* =====================================================================
    virtual point do_get_position() const
    {
        return bounds_.origin;
    }
    
    //* =====================================================================
    /// \brief Called by set_size().  Derived classes must override this 
    /// function in order to set the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual void do_set_size(extent const &size)
    {
        // After changing the size, the larger region of the two will need
        // redrawing.  For now, we can just put both regions in and let the
        // pruner take care of it.
        std::vector<rectangle> redraw_regions;
        redraw_regions.push_back(bounds_);

        bounds_.size = size;
        redraw_regions.push_back(bounds_);

        boost::shared_ptr<layout_type> layout = this->get_layout();

        if (layout)
        {
            (*layout)(this->shared_from_this());
        }

        this->on_redraw(redraw_regions);
    }

    //* =====================================================================
    /// \brief Called by get_size().  Derived classes must override this
    /// function in order to get the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual extent do_get_size() const
    {
        return bounds_.size;
    }
    
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to get the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual extent do_get_preferred_size() const
    {
        // If there is a layout, then ask it what the preferred size of this
        // container should be.  Otherwise, we are happy with the size that
        // we currently have.
        return layout_ != NULL
             ? layout_->get_preferred_size(this->shared_from_this())
             : this->get_size();
    }
    
    //* =====================================================================
    /// \brief Called by get_number_of_components().  Derived classes must
    /// override this function in order to retrieve the number of components
    /// in this basic_container in a custom manner.
    //* =====================================================================
    virtual odin::u32 do_get_number_of_components() const
    {
        return components_.size();
    }
    
    //* =====================================================================
    /// \brief Called by add_component().  Derived classes must override
    /// this function in order to add a component to the basic_container in a
    /// custom manner.
    //* =====================================================================
    virtual void do_add_component(
        boost::shared_ptr<component_type> const &component
      , boost::any                        const &hint
      , odin::u32                                layer)
    {
        // Store the component and the layer in which it is to be drawn.
        components_.push_back(component);
        component_hints_.push_back(hint);
        component_layers_.push_back(layer);
        
        // Register for callbacks for when the new subcomponent either gains
        // or loses focus.  We can make sure our own focus is correct based
        // on this information.
        component->on_focus_set.connect(
            boost::bind(
                &basic_container::subcomponent_focus_set_handler
              , this
              , boost::weak_ptr<component_type>(component)));
        
        component->on_focus_lost.connect(
            boost::bind(
                &basic_container::subcomponent_focus_lost_handler
              , this
              , boost::weak_ptr<component_type>(component)));
        
        // Register for callbacks for when the subcomponent's cursor state
        // or position changes.
        component->on_cursor_state_changed.connect(
            boost::bind(
                &basic_container::subcomponent_cursor_state_change_handler
              , this
              , boost::weak_ptr<component_type>(component)
              , _1));
        
        component->on_cursor_position_changed.connect(
            boost::bind(
                &basic_container::subcomponent_cursor_position_change_handler
              , this
              , boost::weak_ptr<component_type>(component)
              , _1));
        
        // Register for callbacks for when the subcomponent's position
        // changes.
        component->on_position_changed.connect(
            boost::bind(
                &basic_container::subcomponent_position_change_handler
              , this
              , boost::weak_ptr<component_type>(component)
              , _1
              , _2));
    }
    
    //* =====================================================================
    /// \brief Called by remove_component().  Derived classes must override
    /// this function in order to add a component to the basic_container in a
    /// custom manner.
    //* =====================================================================
    virtual void do_remove_component(
        boost::shared_ptr<component_type> const &component)
    {
        BOOST_AUTO(current_component, components_.begin());
        BOOST_AUTO(current_hint, component_hints_.begin());
        BOOST_AUTO(current_layer, component_layers_.begin());
        
        while (current_component != components_.end()
            && current_hint != component_hints_.end()
            && current_layer != component_layers_.end())
        {
            if (*current_component == component)
            {
                current_component  = components_.erase(current_component);
                current_hint       = component_hints_.erase(current_hint);
                current_layer      = component_layers_.erase(current_layer);
            }
            else
            {
                ++current_component;
                ++current_hint;
                ++current_layer;
            }
        }
    }
    
    //* =====================================================================
    /// \brief Called by get_component().  Derived classes must override this
    /// function in order to retrieve a component in a custom manner.
    //* =====================================================================
    virtual boost::shared_ptr<component_type> do_get_component(
        odin::u32 index) const
    {
        return components_[index];
    }
    
    //* =====================================================================
    /// \brief Called by get_component_hint().  Derived classes must
    /// override this function in order to retrieve a component hint in a
    /// custom manner.
    //* =====================================================================
    virtual boost::any do_get_component_hint(odin::u32 index) const
    {
        return component_hints_[index];
    }

    //* =====================================================================
    /// \brief Called by get_component_layer().  Derived classes must
    /// override this function in order to retrieve a component layer in a
    /// custom manner.
    //* =====================================================================
    virtual odin::u32 do_get_component_layer(odin::u32 index) const
    {
        return component_layers_[index];
    }
    
    //* =====================================================================
    /// \brief Called by set_layout.  Derived classes must override this
    /// function in order to set a layout in a custom manner.
    //* =====================================================================
    virtual void do_set_layout(
        boost::shared_ptr<layout_type> const &layout)
    {
        layout_ = layout;
    }
    
    //* =====================================================================
    /// \brief Called by get_layout.  Derived classes must override this
    /// function in order to get the basic_container's layout in a custom manner.
    //* =====================================================================
    virtual boost::shared_ptr<layout_type> do_get_layout() const
    {
        return layout_;
    }

    //* =====================================================================
    /// \brief Called by has_focus().  Derived classes must override this
    /// function in order to return whether this component has focus in a
    /// custom manner.
    //* =====================================================================
    virtual bool do_has_focus() const
    {
        return has_focus_;
    }
    
    //* =====================================================================
    /// \brief Called by set_can_focus().  Derived classes must override this
    /// function in order to set whether this component can be focussed in
    /// a custom manner.
    //* =====================================================================
    virtual void do_set_can_focus(bool focus)
    {
        // As this is controlled by contained components, this is
        // completely ignored.
    }

    //* =====================================================================
    /// \brief Called by can_focus().  Derived classes must override this
    /// function in order to return whether this component can be focused in
    /// a custom manner.
    //* =====================================================================
    virtual bool do_can_focus() const
    {
        // A container may only be focused if one of its subcomponents can
        // also be focused.
        odin::u32 number_of_components = this->get_number_of_components();
        
        for (odin::u32 index = 0; index < number_of_components; ++index)
        {
            if (this->get_component(index)->can_focus())
            {
                return true;
            }
        }
        
        return false;
    }
    
    //* =====================================================================
    /// \brief Called by set_focus().  Derived classes must override this
    /// function in order to set the focus to this component in a custom
    /// manner.
    //* =====================================================================
    virtual void do_set_focus()
    {
        if (this->can_focus())
        {
            // Find the first component that can be focussed and focus it.
            BOOST_FOREACH(
                boost::shared_ptr<component_type> current_component
              , components_)
            {
                if (current_component->can_focus())
                {
                    current_component->set_focus();
                    break;
                }
            }
            
            has_focus_ = true;
            this->on_focus_set();
        }
    }
    
    //* =====================================================================
    /// \brief Called by lose_focus().  Derived classes must override this
    /// function in order to lose the focus from this component in a
    /// custom manner.
    //* =====================================================================
    virtual void do_lose_focus()
    {
        BOOST_FOREACH(
            boost::shared_ptr<component_type> current_component
          , components_)
        {
            if (current_component->has_focus())
            {
                current_component->lose_focus();
            }
        }
        
        has_focus_ = false;
    }

    // ======================================================================
    // DO_FOCUS_NEXT_HAS_FOCUS
    // ======================================================================
    void do_focus_next_has_focus()
    {
        // We have focus.  Therefore, find the subcomponent that also has 
        // focus and tell it to move to the next focus.  After that, if it
        // has no focus, we move the focus to the next subcomponent that
        // can be focused.  If there is no such object, then we lose our
        // focus.
        odin::u32 number_of_components = this->get_number_of_components();
        
        for (odin::u32 index = 0; index < number_of_components; ++index)
        {
            boost::shared_ptr<component_type> current_component =
                this->get_component(index);
                
            if (current_component->has_focus())
            {
                current_component->focus_next();
                
                if (!current_component->has_focus())
                {
                    bool focused = false;
                    
                    // Find the index of the next component that can have focus.
                    for (odin::u32 next_index = index + 1; 
                         next_index < number_of_components && !focused;
                         ++next_index)
                    {
                        boost::shared_ptr<component_type> next_component =
                            this->get_component(next_index);
                            
                        if (next_component->can_focus())
                        {
                            next_component->focus_next();
                            focused = true;
                        }
                    }
                    
                    if (!focused)
                    {
                        has_focus_ = false;
                        this->on_focus_lost();
                    }
                }
                
                break;
            }
        }
    }

    // ======================================================================
    // DO_FOCUS_NEXT_NO_FOCUS
    // ======================================================================
    void do_focus_next_no_focus()
    {
        // We do not currently have any focus, therefore we command the first
        // subcomponent to focus its next component.
        odin::u32 number_of_components = this->get_number_of_components();

        if (number_of_components != 0)
        {
            bool focused = false;
            
            for (odin::u32 current_component = 0; 
                 current_component < number_of_components && !focused;
                 ++current_component)
            {
                boost::shared_ptr<component_type> comp =
                    this->get_component(current_component);

                if (comp->can_focus())
                {
                    comp->focus_next();
                    focused = true;
                }
            }
        }
    }
    
    //* =====================================================================
    /// \brief Called by focus_next().  Derived classes must override this
    /// function in order to move the focus in a custom manner.
    //* =====================================================================
    virtual void do_focus_next()
    {
        if (has_focus_)
        {
            do_focus_next_has_focus();
        }
        else
        {
            do_focus_next_no_focus();
        }
    }
    
    // ======================================================================
    // DO_FOCUS_PREVIOUS_HAS_FOCUS
    // ======================================================================
    void do_focus_previous_has_focus()
    {
        // We do have focus.  Find the currently focused component and tell
        // it to move its focus to its previous subcomponent.  If, after 
        // that, it no longer has focus, then we find the component prior
        // to that that can have focus and tell it to set its previous focus.
        // If there is no component to focus, then just lose our focus.
        odin::u32 number_of_components = this->get_number_of_components();

        for (odin::u32 index = 0; index < number_of_components; ++index)
        {
            boost::shared_ptr<component_type> current_component =
                this->get_component(index);
                
            if (current_component->has_focus())
            {
                current_component->focus_previous();
                
                if (!current_component->has_focus())
                {
                    bool focused = false;
                    
                    // Seek for the first subcomponent prior to the unfocused 
                    // one that can have a focus, and focus it.
                    for (odin::u32 last_index = index;
                         last_index > 0 && !focused;
                         --last_index)
                    {
                        boost::shared_ptr<component_type> last_component =
                            this->get_component(last_index - 1);
                            
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
                        this->on_focus_lost();
                    }
                }
                
                break;
            }
        }
    }

    // ======================================================================
    // DO_FOCUS_PREVIOUS_NO_FOCUS
    // ======================================================================
    void do_focus_previous_no_focus()
    {
        // Find the last component that could have focus and focus its
        // last element.
        odin::u32 number_of_components = this->get_number_of_components();

        for (odin::u32 index = number_of_components; index > 0; --index)
        {
            boost::shared_ptr<component_type> last_component =
                this->get_component(index - 1);
                
            if (last_component->can_focus())
            {
                last_component->focus_previous();
                break;
            }
        }
    }

    //* =====================================================================
    /// \brief Called by focus_previous().  Derived classes must override 
    /// this function in order to move the focus in a custom manner.
    //* =====================================================================
    virtual void do_focus_previous()
    {
        if (has_focus_)
        {
            do_focus_previous_has_focus();
        }
        else
        {
            do_focus_previous_no_focus();
        }
    }
    
    //* =====================================================================
    /// \brief Called by get_focussed_component().  Derived classes must
    /// override this function in order to return the focussed component
    /// in a custom manner.
    //* =====================================================================
    virtual boost::shared_ptr<component_type> do_get_focussed_component()
    {
        if (has_focus_)
        {
            odin::u32 number_of_components = this->get_number_of_components();
            
            for (odin::u32 index = 0; index < number_of_components; ++index)
            {
                boost::shared_ptr<component_type> current_component =
                    this->get_component(index);
                    
                if (current_component->has_focus())
                {
                    return current_component->get_focussed_component();
                }
            }
        }
        
        return boost::shared_ptr<component_type>();
    }

    //* =====================================================================
    /// \brief Called by enable().  Derived classes must override this
    /// function in order to disable the component in a custom manner.
    //* =====================================================================
    virtual void do_enable()
    {
        enabled_ = true;
    }
    
    //* =====================================================================
    /// \brief Called by disable().  Derived classes must override this
    /// function in order to disable the component in a custom manner.
    //* =====================================================================
    virtual void do_disable()
    {
        enabled_ = false;
    }
    
    //* =====================================================================
    /// \brief Called by is_enabled().  Derived classes must override this
    /// function in order to return whether the component is disabled or not
    /// in a custom manner.
    //* =====================================================================
    virtual bool do_is_enabled() const
    {
        return enabled_;
    }
    
    //* =====================================================================
    /// \brief Called by event().  Derived classes must override this 
    /// function in order to handle events in a custom manner.
    //* =====================================================================
    virtual void do_event(boost::any const &event)
    {
        // Filter the event down to the focused subcomponent.
        odin::u32 const number_of_components = this->get_number_of_components();
        
        for (odin::u32 index = 0; index < number_of_components; ++index)
        {
            boost::shared_ptr<component_type> current_component =
                this->get_component(index);
                
            if (current_component->has_focus())
            {
                current_component->event(event);
                break;
            }
        }
    }

    //* =====================================================================
    /// \brief Called by get_cursor_state().  Derived classes must override
    /// this function in order to return the cursor state in a custom manner.
    //* =====================================================================
    virtual bool do_get_cursor_state() const
    {
        return cursor_state_;
    }

    //* =====================================================================
    /// \brief Called by get_cursor_position().  Derived classes must
    /// override this function in order to return the cursor position in
    /// a custom manner.
    //* =====================================================================
    virtual point do_get_cursor_position() const
    {
        // If we have no focus, then return the default position.
        if (has_focus_ && cursor_state_)
        {
            // Find the subcomponent that has focus and get its cursor 
            // position.  This must then be offset by the subcomponent's 
            // position within our container.
            BOOST_FOREACH(
                boost::shared_ptr<component_type> current_component
              , components_)
            {
                if (current_component->has_focus())
                {
                    point component_position = 
                        current_component->get_position();
                        
                    point cursor_position =
                        current_component->get_cursor_position();
                        
                    return component_position + cursor_position;
                }
            }
        }

        // Either we do not have focus, or the currently focussed subcomponent
        // does not have a cursor.  Return the default position.
        return point(0, 0);
    }

    //* =====================================================================
    /// \brief Returns an attribute with a specified name.
    //* =====================================================================
    boost::any get_attribute(std::string const &name) const
    {
        BOOST_AUTO(attr_iterator, attributes_.find(name));
        
        return attr_iterator == attributes_.end()
             ? boost::any()
             : *attr_iterator;
    }
    
    //* =====================================================================
    /// \brief Called by set_attribute().  Derived classes must override this
    /// function in order to set an attribute in a custom manner.
    //* =====================================================================
    virtual void do_set_attribute(
        std::string const &name, boost::any const &attr)
    {
        attributes_[name] = attr;
    }
    
private :
    std::map<std::string, boost::any>                attributes_;
    std::vector< boost::shared_ptr<component_type> > components_;
    std::vector< boost::any >                        component_hints_;
    std::vector< odin::u32 >                         component_layers_;
    boost::shared_ptr<layout_type>                   layout_;
    rectangle                                        bounds_;
    bool                                             has_focus_;
    bool                                             cursor_state_;
    bool                                             enabled_;
    
};
    
}

#endif

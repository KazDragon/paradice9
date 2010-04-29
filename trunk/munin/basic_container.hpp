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
#include <boost/typeof/typeof.hpp>
#include <algorithm>
#include <vector>

namespace munin {
    
//* =========================================================================
/// \brief A default implementation of a container.
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
        : has_focus_(false)
    {
        bounds_.origin.x  = 0;
        bounds_.origin.y  = 0;
        bounds_.size.width  = 0;
        bounds_.size.height = 0;
    }
    
private :
    std::vector< boost::shared_ptr<component_type> > components_;
    std::vector< odin::u32 >                         component_layers_;
    boost::weak_ptr<container_type>                  parent_;
    boost::shared_ptr<layout_type>                   layout_;
    rectangle                                        bounds_;
    bool                                             has_focus_;
    
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
        odin::u32 number_of_components = this->get_number_of_components();
        
        for (odin::u32 index = 0; index < number_of_components; ++index)
        {
            boost::shared_ptr<component_type> current_component =
                this->get_component(index);
                
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
        odin::u32 number_of_components = this->get_number_of_components();
        bool      component_is_focused = false;

        for (odin::u32 index = 0;
             index < number_of_components && !component_is_focused;
             ++index)
        {
            if (this->get_component(index)->has_focus())
            {
                component_is_focused = true;
            }
        }
        
        if (!component_is_focused)
        {
            has_focus_ = false;
            this->on_focus_lost();
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
        bounds_.size = size;
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
        if (layout_ != NULL)
        {
            // If there is a layout, then ask it what the preferred size of
            // this container should be.
            return layout_->get_preferred_size();
        }
        else
        {
            // Otherwise, we are happy with the size that we currently have.
            return this->get_size();
        }
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
      , odin::u32                                layer)
    {
        components_.push_back(component);
        component_layers_.push_back(layer);
        
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
        BOOST_AUTO(current_layer, component_layers_.begin());
        
        while (current_component != components_.end()
            && current_layer != component_layers_.end())
        {
            if (*current_component == component)
            {
                current_component  = components_.erase(current_component);
                current_layer      = component_layers_.erase(current_layer);
            }
            else
            {
                ++current_component;
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
    /// \brief Called by set_parent().  Derived classes must override this
    /// function in order to set the parent of the component in a custom
    /// manner.
    //* =====================================================================
    virtual void do_set_parent(
        boost::shared_ptr< munin::container<ElementType> > const &parent)
    {
        parent_ = parent;
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
            if (this->get_number_of_components() != 0)
            {
                this->get_component(0)->set_focus();
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
        for (odin::u32 index = 0; 
             index < this->get_number_of_components(); 
             ++index)
        {
            boost::shared_ptr<component_type> current_component =
                this->get_component(index);
                
            if (current_component->has_focus())
            {
                current_component->lose_focus();
            }
        }
        
        has_focus_ = false;
    }
    
    //* =====================================================================
    /// \brief Called by focus_next().  Derived classes must override this
    /// function in order to move the focus in a custom manner.
    //* =====================================================================
    virtual void do_focus_next()
    {
        // If we do not have focus, then set the focus to the first
        // subcomponent.  Nothing else should be necessary.
        if (!has_focus_)
        {
            this->set_focus();
            return;
        }
        
        // First, find a component that has the focus and tell it to move
        // its focus on.  Then, if it no longer has focus, move the focus
        // to the next object.  If there is no next object, then lose the
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
                            next_component->set_focus();
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
    
    //* =====================================================================
    /// \brief Called by focus_previous().  Derived classes must override 
    /// this function in order to move the focus in a custom manner.
    //* =====================================================================
    virtual void do_focus_previous()
    {
        // First, if we have no subcomponents, then we just toggle our focus.
        odin::u32 number_of_components = this->get_number_of_components();
        
        // Next, if we have no focus, then call focus_previous on the last
        // component we have.  This will cater for any containers.  If we
        // still have no focus after that, then set its focus.  This will
        // cater for any bottom level components.
            
        if (!has_focus_)
        {
            // Find the last component that could have focus.
            for (odin::u32 index = number_of_components; index > 0; --index)
            {
                boost::shared_ptr<component_type> last_component =
                    this->get_component(index - 1);
                    
                if (last_component->can_focus())
                {
                    // If the component is a container, this will focus the
                    // last component in that container.
                    last_component->focus_previous();
                    
                    // It will have been ignored if the component is not a
                    // container.  In this case, we set its focus directly.
                    if (!last_component->has_focus())
                    {
                        last_component->set_focus();
                    }
                }
            }
            
            return;
        }
        
        // Finally, since we do have focus, find the currently focused
        // subcomponent and tell it to move its focus.  If it no longer has
        // focus after that, we tell the component prior to that to set its
        // previous focus (container) or focus (component).  If there is no
        // prior component, then just lose our focus.
        for (odin::u32 index = 0; index < number_of_components; ++index)
        {
            boost::shared_ptr<component_type> current_component =
                this->get_component(index);
                
            if (current_component->has_focus())
            {
                // If the current component is a container, this will command
                // it to focus its previous component.  After that, the 
                // component not having focus will mean that either the 
                // container has run out of components to focus, or it was not
                // a container.
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
                            
                            // It will have been ignored if the component is 
                            // not a container.  In this case, we set its focus
                            // directly.
                            if (!last_component->has_focus())
                            {
                                last_component->set_focus();
                            }
                            
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
    
    //* =====================================================================
    /// \brief Called by get_parent().  Derived classes must override this
    /// function in order to get the parent of the component in a custom
    /// manner.
    //* =====================================================================
    boost::shared_ptr< munin::container<ElementType> > do_get_parent() const
    {
        return parent_.lock();
    }
};
    
}

#endif

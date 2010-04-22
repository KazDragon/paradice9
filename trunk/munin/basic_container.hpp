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
#include <algorithm>
#include <vector>

namespace munin {
    
// BASIC_CONTAINER ================================================================
// ==========================================================================
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
    {
        position_.x  = 0;
        position_.y  = 0;
        size_.width  = 0;
        size_.height = 0;
    }
    
private :
    std::vector< boost::shared_ptr<component_type> > components_;
    boost::weak_ptr<container_type>                  parent_;
    boost::shared_ptr<layout_type>                   layout_;
    point                                            position_;
    extent                                           size_;
    
    //* =====================================================================
    /// \brief Called by set_position().  Derived classes must override this
    /// function in order to set the position of the component in a custom
    /// manner.
    //* =====================================================================
    virtual void do_set_position(point const &position)
    {
        position_ = position;
    }
    
    //* =====================================================================
    /// \brief Called by get_position().  Derived classes must override this
    /// function in order to get the position of the component in a custom
    /// manner.
    //* =====================================================================
    virtual point do_get_position() const
    {
        return position_;
    }
    
    //* =====================================================================
    /// \brief Called by set_size().  Derived classes must override this 
    /// function in order to set the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual void do_set_size(extent const &size)
    {
        size_ = size;
    }

    //* =====================================================================
    /// \brief Called by get_size().  Derived classes must override this
    /// function in order to get the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual extent do_get_size() const
    {
        return size_;
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
        boost::shared_ptr<component_type> const &component)
    {
        components_.push_back(component);
    }
    
    //* =====================================================================
    /// \brief Called by remove_component().  Derived classes must override
    /// this function in order to add a component to the basic_container in a
    /// custom manner.
    //* =====================================================================
    virtual void do_remove_component(
        boost::shared_ptr<component_type> const &component)
    {
        components_.erase(
            std::remove(components_.begin(), components_.end(), component)
          , components_.end());
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

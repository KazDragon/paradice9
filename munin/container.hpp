// ==========================================================================
// Munin Container.
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
#ifndef MUNIN_CONTAINER_HPP_
#define MUNIN_CONTAINER_HPP_

#include "munin/types.hpp"
#include "munin/component.hpp"
#include "munin/algorithm.hpp"
#include <boost/enable_shared_from_this.hpp>

namespace munin {
    
// CONTAINER ================================================================
// ==========================================================================
template <class ElementType>
class container 
    : public component<ElementType>
    , public boost::enable_shared_from_this< container<ElementType> >
{
public :
    typedef ElementType            element_type;
    typedef component<ElementType> component_type;
    
    //* =====================================================================
    /// \brief Retrieves the number of components that this container
    /// contains.
    //* =====================================================================
    odin::u32 get_number_of_components() const
    {
        return do_get_number_of_components();
    }
    
    //* =====================================================================
    /// \brief Adds a component to the container.
    //* =====================================================================
    void add_component(boost::shared_ptr<component_type> const &component)
    {
        do_add_component(component);
        component->set_parent(this->shared_from_this());
    }
    
    //* =====================================================================
    /// \brief Retrieves a component from the container.
    //* =====================================================================
    boost::shared_ptr<component_type> get_component(odin::u32 index)
    {
        return do_get_component(index);
    }
    
private :
    //* =====================================================================
    /// \brief Called by get_number_of_components().  Derived classes must
    /// override this function in order to retrieve the number of components
    /// in this container in a custom manner.
    //* =====================================================================
    virtual odin::u32 do_get_number_of_components() const = 0;
    
    //* =====================================================================
    /// \brief Called by add_component().  Derived classes must override
    /// this function in order to add a component to the container in a
    /// custom manner.
    //* =====================================================================
    virtual void do_add_component(
        boost::shared_ptr<component_type> const &component) = 0;
    
    //* =====================================================================
    /// \brief Called by get_component().  Derived classes must override this
    /// function in order to retrieve a component in a custom manner.
    //* =====================================================================
    virtual boost::shared_ptr<component_type> do_get_component(
        odin::u32 index) const = 0;
    
    //* =====================================================================
    /// \brief Called by draw().  Derived classes must override this function
    /// in order to draw onto the passed graphics context.  A component must
    /// only draw the part of itself specified by the region.
    ///
    /// \param context the context in which the component should draw itself.
    /// \param offset the position of the parent component (if there is one)
    ///        relative to the context.
    /// \param region the region relative to this component's origin that
    /// should be drawn.
    //* =====================================================================
    virtual void do_draw(
        graphics_context<element_type> &context
      , point const                    &offset
      , rectangle const                &region)
    {
        odin::u32 number_of_components = get_number_of_components();
        
        for (odin::u32 index = 0; index < number_of_components; ++index)
        {
            boost::shared_ptr<component_type> current_component =
                get_component(index);
                
            // The area we want to draw is the intersection of the region
            // passed in above and the region of space that the component
            // occupies.
            rectangle component_region;
            component_region.origin = current_component->get_position();
            component_region.size   = current_component->get_size();
            
            boost::optional<rectangle> draw_region =
                intersection(region, component_region);
                
            if (draw_region)
            {
                // The draw region is currently relative to this container's
                // origin.  It should be relative to the child's origin.
                draw_region->origin.x -= component_region.origin.x;
                draw_region->origin.y -= component_region.origin.y;
                
                // The offset to the component is this container's position
                // within its context plus the offset passed in.
                point position = this->get_position();
                
                point component_offset;
                component_offset.x = position.x + offset.x;
                component_offset.y = position.y + offset.y;
                
                current_component->draw(
                    context, component_offset, draw_region.get());
            }
        }
    }
};
    
}

#endif

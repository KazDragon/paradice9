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
#include "munin/layout.hpp"
#include "munin/algorithm.hpp"
#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/foreach.hpp>
#include <boost/weak_ptr.hpp>
#include <numeric>

//#define DEBUG_CONTAINER

#ifdef DEBUG_CONTAINER
#include <iostream>
#endif

namespace munin {
    
BOOST_STATIC_CONSTANT(
    odin::u32, HIGHEST_LAYER = std::numeric_limits<odin::u32>::max());
BOOST_STATIC_CONSTANT(
    odin::u32, LOWEST_LAYER = std::numeric_limits<odin::u32>::min());
BOOST_STATIC_CONSTANT(
    odin::u32, DEFAULT_LAYER = std::numeric_limits<odin::u32>::max() / 2);

//* =========================================================================
/// \brief A graphical element capable of containing and arranging other
/// subcomponents.
//* =========================================================================
template <class ElementType>
class container 
    : public component<ElementType>
    , public boost::enable_shared_from_this< container<ElementType> >
{
public :
    typedef ElementType            element_type;
    typedef component<ElementType> component_type;
    typedef layout<ElementType>    layout_type;
    
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
    /// \param component The component to add to the container
    /// \param layout_hint A hint to be passed to the container's current
    ///        layout.
    //* =====================================================================
    void add_component(
        boost::shared_ptr<component_type> const &component
      , boost::any                        const &layout_hint = boost::any()
      , odin::u32                                layer = DEFAULT_LAYER)
    {
        do_add_component(component, layer);
        component->set_parent(this->shared_from_this());

        // Subscribe to the component's redraw event.
        component->on_redraw.connect(
            boost::bind(
                &container::subcomponent_redraw_handler
              , this
              , boost::weak_ptr<component_type>(component)
              , _1));
        
        boost::shared_ptr<layout_type> current_layout = get_layout();
        
        if (current_layout != NULL)
        {
            current_layout->add_component(component, layout_hint);
        }
        
        // A redraw of the region which the component occupies is required.
        rectangle region;
        region.origin = component->get_position();
        region.size   = component->get_size();
        
        std::vector<rectangle> regions;
        regions.push_back(region);
        this->on_redraw(regions);
    }
    
    //* =====================================================================
    /// \brief Removes a component from the container.
    //* =====================================================================
    void remove_component(boost::shared_ptr<component_type> const &component)
    {
        do_remove_component(component);
        component->set_parent(boost::shared_ptr< container<ElementType> >());
    }
    
    //* =====================================================================
    /// \brief Retrieves a component from the container.
    //* =====================================================================
    boost::shared_ptr<component_type> get_component(odin::u32 index) const
    {
        return do_get_component(index);
    }
    
    //* =====================================================================
    /// \brief Retrieves a component's layer from the container.
    //* =====================================================================
    odin::u32 get_component_layer(odin::u32 index) const
    {
        return do_get_component_layer(index);
    }
    
    //* =====================================================================
    /// \brief Sets the container's current layout.
    //* =====================================================================
    void set_layout(boost::shared_ptr<layout_type> const &layout)
    {
        do_set_layout(layout);
    }
    
    //* =====================================================================
    /// \brief Retrieves the current layout from the container.
    //* =====================================================================
    boost::shared_ptr<layout_type> get_layout() const
    {
        return do_get_layout();
    }
    
private :
    //* =====================================================================
    /// \brief Called when a component within the container requests a
    /// redraw.
    //* =====================================================================
    void subcomponent_redraw_handler(
        boost::weak_ptr<component_type> weak_subcomponent
      , std::vector<rectangle>          regions)
    {
        boost::shared_ptr<component_type> subcomponent = 
            weak_subcomponent.lock();
            
        if (subcomponent != NULL)
        {
            // Each region is bound to the origin of the component in question.
            // It must be rebound to the origin of the container.  We do this
            // by offsetting the regions' origins by the origin of the
            // subcomponent within this container.
            point origin = subcomponent->get_position();
            
            BOOST_FOREACH(rectangle rect, regions)
            {
                rect.origin.x += origin.x;
                rect.origin.y += origin.y;
            }
        
            // This new information must be passed up the component heirarchy.
            this->on_redraw(regions);
        }
    }

    //* =====================================================================
    /// \brief Initialises a region prior to drawing.
    //* =====================================================================
    virtual void do_initialise_region(
        canvas<element_type> &cvs
      , point const          &offset
      , rectangle const      &region) = 0;

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
        boost::shared_ptr<component_type> const &component
      , odin::u32                                layer) = 0;
    
    //* =====================================================================
    /// \brief Called by remove_component().  Derived classes must override
    /// this function in order to add a component to the container in a
    /// custom manner.
    //* =====================================================================
    virtual void do_remove_component(
        boost::shared_ptr<component_type> const &component) = 0;
    
    //* =====================================================================
    /// \brief Called by get_component().  Derived classes must override this
    /// function in order to retrieve a component in a custom manner.
    //* =====================================================================
    virtual boost::shared_ptr<component_type> do_get_component(
        odin::u32 index) const = 0;
    
    //* =====================================================================
    /// \brief Called by get_component_layer().  Derived classes must
    /// override this function in order to retrieve a component layer in a
    /// custom manner.
    //* =====================================================================
    virtual odin::u32 do_get_component_layer(odin::u32 index) const = 0;
    
    //* =====================================================================
    /// \brief Called by set_layout.  Derived classes must override this
    /// function in order to set a layout in a custom manner.
    //* =====================================================================
    virtual void do_set_layout(
        boost::shared_ptr<layout_type> const &layout) = 0;
    
    //* =====================================================================
    /// \brief Called by get_layout.  Derived classes must override this
    /// function in order to get the container's layout in a custom manner.
    //* =====================================================================
    virtual boost::shared_ptr<layout_type> do_get_layout() const = 0;
    
    //* =====================================================================
    /// \brief Called by draw().  Derived classes must override this function
    /// in order to draw onto the passed canvas.  A component must only draw
    /// the part of itself specified by the region.
    ///
    /// \param cvs the canvas in which the component should draw itself.
    /// \param offset the position of the parent component (if there is one)
    ///        relative to the canvas.
    /// \param region the region relative to this component's origin that
    /// should be drawn.
    //* =====================================================================
    virtual void do_draw(
        canvas<element_type> &cvs
      , point const          &offset
      , rectangle const      &region)
    {
        // First, initialise that region to an undrawn state.
        do_initialise_region(cvs, offset, region);
        
#ifdef DEBUG_CONTAINER
        std::cout << "munin::container::do_draw\n"
                  << "  region = (" << region.origin.x << ", "
                                    << region.origin.y << ") -> ["
                                    << region.size.width << ", "
                                    << region.size.height << "]\n"
                  << "  offset = [" << offset.x << ", "
                                    << offset.y << "]"
                  << std::endl;
#endif

        odin::u32 number_of_components = get_number_of_components();
        
#ifdef DEBUG_CONTAINER
        std::cout << "#components = " << number_of_components << std::endl;
#endif

        // First, we obtain a list of components sorted by layer from lowest
        // to highest.
        std::vector< boost::shared_ptr<component_type> > components =
            do_get_components_sorted();
            
        for (odin::u32 index = 0; index < number_of_components; ++index)
        {
            boost::shared_ptr<component_type> &current_component =
                components[index];
                
            // The area we want to draw is the intersection of the region
            // passed in above and the region of space that the component
            // occupies.
            rectangle component_region;
            component_region.origin = current_component->get_position();
            component_region.size   = current_component->get_size();
            
#ifdef DEBUG_CONTAINER

            std::cout << "=== component # " << index << " ===\n"
                      << "  region = (" << component_region.origin.x << ", "
                                        << component_region.origin.y << ") -> ["
                                        << component_region.size.width << ", "
                                        << component_region.size.height << "]\n";
#endif            
            boost::optional<rectangle> draw_region =
                intersection(region, component_region);
                
            if (draw_region)
            {
#ifdef DEBUG_CONTAINER
                std::cout << "  intersection = ("
                          << draw_region->origin.x << ", "
                          << draw_region->origin.y << ") -> ["
                          << draw_region->size.width << ", "
                          << draw_region->size.height << "]\n";
#endif

                // The draw region is currently relative to this container's
                // origin.  It should be relative to the child's origin.
                draw_region->origin -= component_region.origin;
                
                // The offset to the component is this container's position
                // within its canvas plus the offset passed in.
                point component_offset = this->get_position() + offset;
                
#ifdef DEBUG_CONTAINER
                std::cout << "   offset = ("
                          << component_offset.x << ", "
                          << component_offset.y << ")\n" << std::endl;
#endif

                current_component->draw(
                    cvs, component_offset, draw_region.get());
            }
        }
    }
    
    //* =====================================================================
    /// \brief Returns a vector of components, sorted by layer.  A derived
    /// class may override this, for example if it has a more efficient
    /// implementation.
    //* =====================================================================
    virtual std::vector< boost::shared_ptr<component_type> > 
        do_get_components_sorted() const
    {
        std::vector< boost::shared_ptr<component_type> > components;
        std::vector< odin::u32 >                         layers;
        
#ifdef DEBUG_CONTAINER
        std::cout << "do_get_components_sorted\n"; 
#endif

        odin::u32 number_of_components = get_number_of_components();
        
#ifdef DEBUG_CONTAINER
        std::cout << "number of components = " << number_of_components << "\n";
#endif
        
        for (odin::u32 index = 0; index < number_of_components; ++index)
        {
#ifdef DEBUG_CONTAINER
            std::cout << "==== component #" << index << "\n";
#endif
            
            boost::shared_ptr<component_type> comp = get_component(index);
            odin::u32 layer = get_component_layer(index);
            
#ifdef DEBUG_CONTAINER
            std::cout << " o layer = " << layer << "\n";
#endif
            
            typename std::vector< boost::shared_ptr<component_type> >::iterator
                component_insert_position = components.begin();
            std::vector< odin::u32 >::iterator 
                layer_insert_position = layers.begin();
                
            int pos = 0;
            
            while (component_insert_position != components.end()
                && layer_insert_position != layers.end()
                && layer >= *layer_insert_position)
            {
                ++component_insert_position;
                ++layer_insert_position;
                ++pos;
            }
            
#ifdef DEBUG_CONTAINER
            std::cout << " o inserted at position " << pos << std::endl;
#endif
            
            components.insert(component_insert_position, comp);
            layers.insert(layer_insert_position, layer);
        }
        
#ifdef DEBUG_CONTAINER
        std::cout << "do_get_components_sorted done." << std::endl;
#endif
        
        return components;
    }
    
};
    
}

#endif

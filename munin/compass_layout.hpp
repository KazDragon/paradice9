// ==========================================================================
// Munin Compass Layout.
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
#ifndef MUNIN_COMPASS_LAYOUT_HPP_
#define MUNIN_COMPASS_LAYOUT_HPP_

#include "munin/basic_layout.hpp"
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>
#include <algorithm>
#include <utility>
#include <vector>

namespace munin {

    
BOOST_STATIC_CONSTANT(odin::u32, COMPASS_LAYOUT_CENTRE = 0);
BOOST_STATIC_CONSTANT(odin::u32, COMPASS_LAYOUT_NORTH  = 1);
BOOST_STATIC_CONSTANT(odin::u32, COMPASS_LAYOUT_EAST   = 2);
BOOST_STATIC_CONSTANT(odin::u32, COMPASS_LAYOUT_SOUTH  = 3);
BOOST_STATIC_CONSTANT(odin::u32, COMPASS_LAYOUT_WEST   = 4);
    
//* =========================================================================
/// \brief A class that can lay out its components according to basic compass
/// directions.  Components can be laid out to the north, east, south, west,
/// and centre.  Components to the north and south are given their
/// preferred height while having the width of the containing component, and
/// components to the west and east are given their preferred width, while
/// having the height of the containing component.
//* =========================================================================
template <class ElementType>
class compass_layout : public basic_layout<ElementType>
{
public :
    typedef ElementType             element_type;
    typedef component<element_type> component_type;
    typedef container<element_type> container_type;

protected :
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to retrieve the preferred size of the layout
    /// in a custom manner.
    //* =====================================================================
    virtual extent do_get_preferred_size() const
    {
        // This isn't quite right as it doesn't take into account the order
        // of insertion like do_draw does, but it will do for now.
        odin::u32 north_widths   = 0;
        odin::u32 south_widths   = 0;
        odin::u32 east_widths    = 0;
        odin::u32 centre_widths  = 0;
        odin::u32 west_widths    = 0;
        odin::u32 north_heights  = 0;
        odin::u32 south_heights  = 0;
        odin::u32 east_heights   = 0;
        odin::u32 centre_heights = 0;
        odin::u32 west_heights   = 0;
        
        for (odin::u32 index = 0; 
             index < this->get_number_of_components();
             ++index)
        {
            boost::shared_ptr<component_type> comp = 
                this->get_component(index);
                
            boost::any hint = this->get_hint(index);
            
            odin::u32 *direction_hint = boost::any_cast<odin::u32>(&hint);
            odin::u32 direction = COMPASS_LAYOUT_CENTRE;
            
            if (direction_hint != NULL)
            {
                direction = *direction_hint;
            }
            
            BOOST_AUTO(comp_size, comp->get_preferred_size());
            
            switch (direction)
            {
            case COMPASS_LAYOUT_NORTH :
                north_widths  += comp_size.width;
                north_heights += comp_size.height; 
                break;

            case COMPASS_LAYOUT_EAST :
                east_widths  += comp_size.width;
                east_heights += comp_size.height; 
                break;

            case COMPASS_LAYOUT_SOUTH :
                south_widths  += comp_size.width;
                south_heights += comp_size.height; 
                break;

            case COMPASS_LAYOUT_WEST :
                west_widths  += comp_size.width;
                west_heights += comp_size.height; 
                break;

            case COMPASS_LAYOUT_CENTRE :
            default :
                centre_widths  += comp_size.width;
                centre_heights += comp_size.height; 
                break;
            }
        }
        
        munin::extent preferred_size;
        
        preferred_size.width = north_widths;
        preferred_size.width = (std::max)(odin::u32(preferred_size.width), centre_widths);
        preferred_size.width = (std::max)(odin::u32(preferred_size.width), south_widths);
        preferred_size.width += (west_widths + east_widths);

        preferred_size.height = west_heights;
        preferred_size.height = (std::max)(odin::u32(preferred_size.height), centre_heights);
        preferred_size.height = (std::max)(odin::u32(preferred_size.height), east_heights);
        preferred_size.height += (north_heights + south_heights);
        
        return preferred_size;
    }
    
    //* =====================================================================
    /// \brief Called by operator().  Derived classes must override this
    /// function in order to lay a container's components out in a custom
    /// manner.
    //* =====================================================================
    virtual void do_layout(
        boost::shared_ptr<container_type> const &cont)
    {
        BOOST_AUTO(size, cont->get_size());
        
        std::vector< boost::shared_ptr<component_type> > centre_components;
        odin::u32 west_used = 0;
        odin::u32 north_used = 0;
        odin::u32 south_used = 0;
        odin::u32 east_used = 0;
        
        for (odin::u32 index = 0;
             index < this->get_number_of_components(); 
             ++index)
        {
            boost::shared_ptr<component_type> comp = 
                this->get_component(index);
            boost::any hint = this->get_hint(index);

            odin::u32 *direction = boost::any_cast<odin::u32>(&hint);
            
            if (direction == NULL
             || *direction == COMPASS_LAYOUT_CENTRE)
            {
                centre_components.push_back(comp);
            }
            else if (*direction == COMPASS_LAYOUT_WEST)
            {
                BOOST_AUTO(comp_size, comp->get_preferred_size());
                west_used = (std::max)(
                    west_used, odin::u32(comp_size.width));
                west_used = (std::min)(
                    west_used, odin::u32(size.width - east_used));
                
                BOOST_AUTO(
                    comp_height
                  , (size.height - north_used) - south_used);
                
                comp->set_position(munin::point(0, north_used));
                comp->set_size(munin::extent(west_used, comp_height));
            }
            else if (*direction == COMPASS_LAYOUT_EAST)
            {
                BOOST_AUTO(comp_size, comp->get_preferred_size());
                east_used = (std::max)(
                    east_used, odin::u32(comp_size.width));
                east_used = (std::min)(
                    east_used, odin::u32(size.width - west_used));
                
                BOOST_AUTO(
                    comp_height
                  , (size.height - north_used) - south_used);
                
                comp->set_position(munin::point(0, north_used));
                comp->set_size(munin::extent(east_used, comp_height));
            }
            else if (*direction == COMPASS_LAYOUT_NORTH)
            {
                BOOST_AUTO(comp_size, comp->get_preferred_size());
                north_used = (std::max)(
                    north_used, odin::u32(comp_size.height));
                north_used = (std::min)(
                    north_used, odin::u32(size.height - south_used));
                
                BOOST_AUTO(
                    comp_width
                  , (size.width - west_used) - east_used);
                
                comp->set_position(munin::point(west_used, 0));
                comp->set_size(munin::extent(comp_width, north_used));
            }
            else if (*direction == COMPASS_LAYOUT_SOUTH)
            {
                BOOST_AUTO(comp_size, comp->get_preferred_size());
                south_used = (std::max)(
                    south_used, odin::u32(comp_size.height));
                south_used = (std::min)(
                    south_used, odin::u32(size.height - north_used));
                
                BOOST_AUTO(
                    comp_width
                  , (size.width - west_used) - east_used);
                
                comp->set_position(munin::point(
                    west_used
                  , size.height - south_used));
                comp->set_size(munin::extent(comp_width, south_used));
            }
            else
            {
                centre_components.push_back(comp);
            }
            
            BOOST_FOREACH(
                boost::shared_ptr<component_type> comp
              , centre_components)
            {
                comp->set_position(munin::point(west_used, north_used));
                comp->set_size(munin::extent(
                    (size.width - west_used) - east_used
                  , (size.height - north_used) - south_used));
            }
        }
    }
};
    
}

#endif

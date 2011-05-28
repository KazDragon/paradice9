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
#include "munin/compass_layout.hpp"
#include "munin/container.hpp"
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>
#include <algorithm>
#include <vector>

using namespace odin;
using namespace boost;
using namespace std;

namespace munin {

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
extent compass_layout::do_get_preferred_size(
    runtime_array< shared_ptr<component> > const &components
  , runtime_array< any >                   const &hints) const
{
    // This isn't quite right as it doesn't take into account the order
    // of insertion like do_draw does, but it will do for now.
    u32 north_widths   = 0;
    u32 south_widths   = 0;
    u32 east_widths    = 0;
    u32 centre_widths  = 0;
    u32 west_widths    = 0;
    u32 north_heights  = 0;
    u32 south_heights  = 0;
    u32 east_heights   = 0;
    u32 centre_heights = 0;
    u32 west_heights   = 0;
    
    for (u32 index = 0; index < components.size(); ++index)
    {
        BOOST_AUTO(comp, components[index]);
        BOOST_AUTO(hint, hints[index]);
        
        u32 *direction_hint = any_cast<u32>(&hint);
        u32 direction = COMPASS_LAYOUT_SOUTH;
        
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
    
    extent preferred_size;
    
    preferred_size.width = north_widths;
    preferred_size.width = (max)(u32(preferred_size.width), centre_widths);
    preferred_size.width = (max)(u32(preferred_size.width), south_widths);
    preferred_size.width += (west_widths + east_widths);

    preferred_size.height = west_heights;
    preferred_size.height = (max)(u32(preferred_size.height), centre_heights);
    preferred_size.height = (max)(u32(preferred_size.height), east_heights);
    preferred_size.height += (north_heights + south_heights);
    
    return preferred_size;
}

// ==========================================================================
// DO_LAYOUT
// ==========================================================================
void compass_layout::do_layout(
    runtime_array< shared_ptr<component> > const &components
  , runtime_array< any >                   const &hints
  , extent                                        size)
{
    vector< shared_ptr<component> > centre_components;
    u32 west_used = 0;
    u32 north_used = 0;
    u32 south_used = 0;
    u32 east_used = 0;
    
    for (u32 index = 0; index < components.size(); ++index)
    {
        BOOST_AUTO(comp, components[index]);
        BOOST_AUTO(hint, hints[index]);

        u32 *direction = any_cast<u32>(&hint);
        
        if (direction == NULL
         || *direction == COMPASS_LAYOUT_CENTRE)
        {
            centre_components.push_back(comp);
        }
        else if (*direction == COMPASS_LAYOUT_WEST)
        {
            BOOST_AUTO(comp_size, comp->get_preferred_size());
            west_used = (max)(
                west_used, u32(comp_size.width));
            west_used = (min)(
                west_used, u32(size.width - east_used));
            
            BOOST_AUTO(
                comp_height
              , (size.height - north_used) - south_used);
            
            comp->set_position(point(0, north_used));
            comp->set_size(extent(west_used, comp_height));
        }
        else if (*direction == COMPASS_LAYOUT_EAST)
        {
            BOOST_AUTO(comp_size, comp->get_preferred_size());
            east_used = (max)(
                east_used, u32(comp_size.width));
            east_used = (min)(
                east_used, u32(size.width - west_used));
            
            BOOST_AUTO(
                comp_height
              , (size.height - north_used) - south_used);
  
            comp->set_position(point(
                size.width - east_used
              , north_used));
            comp->set_size(extent(east_used, comp_height));
        }
        else if (*direction == COMPASS_LAYOUT_NORTH)
        {
            BOOST_AUTO(comp_size, comp->get_preferred_size());
            north_used = (max)(
                north_used, u32(comp_size.height));
            north_used = (min)(
                north_used, u32(size.height - south_used));
            
            BOOST_AUTO(
                comp_width
              , (size.width - west_used) - east_used);
            
            comp->set_position(point(west_used, 0));
            comp->set_size(extent(comp_width, north_used));
        }
        else if (*direction == COMPASS_LAYOUT_SOUTH)
        {
            BOOST_AUTO(comp_size, comp->get_preferred_size());
            south_used = (max)(
                south_used, u32(comp_size.height));
            south_used = (min)(
                south_used, u32(size.height - north_used));
            
            BOOST_AUTO(
                comp_width
              , (size.width - west_used) - east_used);
            
            comp->set_position(point(
                west_used
              , size.height - south_used));
            comp->set_size(extent(comp_width, south_used));
        }
        else
        {
            centre_components.push_back(comp);
        }
        
        BOOST_FOREACH(
            shared_ptr<component> comp
          , centre_components)
        {
            comp->set_position(point(west_used, north_used));
            comp->set_size(extent(
                (size.width - west_used) - east_used
              , (size.height - north_used) - south_used));
        }
    }
}

}

// ==========================================================================
// Munin Horizontal Squeeze Layout.
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
#include "munin/horizontal_squeeze_layout.hpp"
#include "munin/container.hpp"
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>

using namespace odin;
using namespace boost;

namespace munin {
    
// ==========================================================================
// DO_GET_PREFERRED_SIZE 
// ==========================================================================
extent horizontal_squeeze_layout::do_get_preferred_size(
    runtime_array< shared_ptr<component> > const &components
  , runtime_array< any >                   const &hints) const
{
    // The preferred size of the whole component is the maximum preferred
    // height, and the maximum preferred height multiplied by the number
    // of components.
    extent maximum_preferred_size(0, 0);

    BOOST_FOREACH(shared_ptr<component> comp, components)
    {
        BOOST_AUTO(preferred_size, comp->get_preferred_size());

        maximum_preferred_size.width = (std::max)(
            maximum_preferred_size.width
          , preferred_size.width);

        maximum_preferred_size.height = (std::max)(
            maximum_preferred_size.height
          , preferred_size.height);
    }
    
    maximum_preferred_size.height *= components.size();

    return maximum_preferred_size;
}

// ==========================================================================
// DO_LAYOUT
// ==========================================================================
void horizontal_squeeze_layout::do_layout(
    runtime_array< shared_ptr<component> > const &components
  , runtime_array< any >                   const &hints
  , extent                                        size)
{
    BOOST_AUTO(amount, components.size());

    if (amount != 0)
    {
        BOOST_AUTO(height_per_component,  size.height / amount);
        BOOST_AUTO(height_remainder,      size.height % amount);
        BOOST_AUTO(height_remainder_used, u32(0));
        
        for (u32 index = 0; index < components.size(); ++index)
        {
            BOOST_AUTO(comp, components[index]);
            
            comp->set_position(point(
                u32(0)
              , (height_per_component * index) + height_remainder_used));
            
            BOOST_AUTO(height, height_per_component);
            
            if (height_remainder != height_remainder_used)
            {
                ++height;
                ++height_remainder_used;
            }
            
            comp->set_size(extent(size.width, height));
        }
    }
}

}


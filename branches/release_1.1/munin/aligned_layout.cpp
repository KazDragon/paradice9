// ==========================================================================
// Munin Aligned Layout.
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
#include "munin/aligned_layout.hpp"
#include "munin/container.hpp"
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/typeof/typeof.hpp>
#include <algorithm>

using namespace odin;
using namespace boost;
using namespace std;

namespace munin {

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
extent aligned_layout::do_get_preferred_size(
    vector< shared_ptr<component> > const &components
  , vector< any >                   const &hints) const
{
    // The preferred size of this component is the largest preferred
    // extents of all components.
    extent maximum_preferred_size(0, 0);

    BOOST_FOREACH(shared_ptr<component> comp, components)
    {
        BOOST_AUTO(preferred_size, comp->get_preferred_size());

        maximum_preferred_size.width = (max)(
            maximum_preferred_size.width
          , preferred_size.width);

        maximum_preferred_size.height = (max)(
            maximum_preferred_size.height
          , preferred_size.height);
    }

    return maximum_preferred_size;
}

// ==========================================================================
// DO_LAYOUT
// ==========================================================================
void aligned_layout::do_layout(
    vector< shared_ptr<component> > const &components
  , vector< any >                   const &hints
  , extent                                 size)
{
    for (u32 index = 0; index < components.size(); ++index)
    {
        BOOST_AUTO(comp, components[index]);
        BOOST_AUTO(hint, hints[index]);

        alignment_data const *alignment_hint = 
            any_cast<alignment_data>(&hint);
            
        // By default, components are centre-aligned.
        alignment_data alignment = {
            HORIZONTAL_ALIGNMENT_CENTRE
          , VERTICAL_ALIGNMENT_CENTRE
        };
        
        if (alignment_hint != NULL)
        {
            alignment = *alignment_hint;
        }
        
        BOOST_AUTO(comp_size, comp->get_preferred_size());
        
        point position;
        
        switch (alignment.horizontal_alignment)
        {
        case HORIZONTAL_ALIGNMENT_LEFT :
            position.x = 0;
            break;
            
        case HORIZONTAL_ALIGNMENT_RIGHT :
            position.x = comp_size.width > size.width
                       ? 0
                       : size.width - comp_size.width;
            break;
        
        case HORIZONTAL_ALIGNMENT_CENTRE :
        default :
            position.x = comp_size.width > size.width
                       ? 0
                       : ((size.width - comp_size.width) / 2);
            break;
        };
        
        switch (alignment.vertical_alignment)
        {
        case VERTICAL_ALIGNMENT_TOP :
            position.y = 0;
            break;
            
        case VERTICAL_ALIGNMENT_BOTTOM :
            position.y = comp_size.height > size.height
                       ? 0
                       : size.height - comp_size.height;
            break;
        
        case VERTICAL_ALIGNMENT_CENTRE :
        default :
            position.y = comp_size.height > size.height
                       ? 0
                       : ((size.height - comp_size.height) / 2);
            break;
        };
                
        comp->set_position(position);
        comp->set_size(comp_size);
    }
}

}

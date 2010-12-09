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
#ifndef MUNIN_ALIGNED_LAYOUT_HPP_
#define MUNIN_ALIGNED_LAYOUT_HPP_

#include "munin/layout.hpp"
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/typeof/typeof.hpp>
#include <algorithm>
#include <vector>

namespace munin {

BOOST_STATIC_CONSTANT(odin::u32, HORIZONTAL_ALIGNMENT_LEFT   = 0);    
BOOST_STATIC_CONSTANT(odin::u32, HORIZONTAL_ALIGNMENT_CENTRE = 1);    
BOOST_STATIC_CONSTANT(odin::u32, HORIZONTAL_ALIGNMENT_RIGHT  = 2);    

BOOST_STATIC_CONSTANT(odin::u32, VERTICAL_ALIGNMENT_TOP    = 0);    
BOOST_STATIC_CONSTANT(odin::u32, VERTICAL_ALIGNMENT_CENTRE = 1);    
BOOST_STATIC_CONSTANT(odin::u32, VERTICAL_ALIGNMENT_BOTTOM = 2);

struct alignment_data
{
    odin::u32 horizontal_alignment;
    odin::u32 vertical_alignment;
};

//* =========================================================================
/// \brief A class that knows how to lay components out in a container in
/// a aligned-like manner.  Each cell of the aligned has an identical size.
/// Components added to the aligned will be displayed left-to-right, top-to-
/// bottom.
//* =========================================================================
template <class ElementType>
class aligned_layout : public layout<ElementType>
{
public :
    
    typedef layout<ElementType> parent_type;
    typedef typename parent_type::component_type component_type;
    typedef typename parent_type::container_type container_type;

protected :
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to retrieve the preferred size of the layout
    /// in a custom manner.
    //* =====================================================================
    virtual extent do_get_preferred_size(
        boost::shared_ptr< munin::container<ElementType> const > const &cont) const
    {
        // The preferred size of this component is the largest preferred
        // extents of all components.
        extent maximum_preferred_size(0, 0);

        for (odin::u32 index = 0;
             index < cont->get_number_of_components();
             ++index)
        {
            BOOST_AUTO(comp,           cont->get_component(index));
            BOOST_AUTO(preferred_size, comp->get_preferred_size());

            maximum_preferred_size.width = (std::max)(
                maximum_preferred_size.width
              , preferred_size.width);

            maximum_preferred_size.height = (std::max)(
                maximum_preferred_size.height
              , preferred_size.height);
        }

        return maximum_preferred_size;
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
        
        for (odin::u32 index = 0;
             index < cont->get_number_of_components();
             ++index)
        {
            BOOST_AUTO(comp, cont->get_component(index));
            BOOST_AUTO(hint, cont->get_component_hint(index));

            alignment_data const *alignment_hint = 
                boost::any_cast<alignment_data>(&hint);
                
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
            
            munin::point position;
            
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
};
    
}

#endif

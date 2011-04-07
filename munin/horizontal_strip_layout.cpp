// ==========================================================================
// Munin Horizontal Strip Layout.
//
// Copyright (C) 2011 Matthew Chaplain, All Rights Reserved.
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
#include "munin/horizontal_strip_layout.hpp"
#include "munin/container.hpp"
#include <boost/typeof/typeof.hpp>

using namespace odin;
using namespace boost;

namespace munin {
    
// ==========================================================================
// DO_GET_PREFERRED_SIZE 
// ==========================================================================
extent horizontal_strip_layout::do_get_preferred_size(
    shared_ptr<container const> const &cont) const
{
    // The preferred size of the whole component is the maximum width of
    // the components and the sum of the preferred heights of the components.
    BOOST_AUTO(amount, cont->get_number_of_components());
    
    extent maximum_preferred_size(0, 0);

    for (u32 index = 0; index < amount; ++index) 
    {
        BOOST_AUTO(comp, cont->get_component(index));
        extent preferred_size = comp->get_preferred_size();

        maximum_preferred_size.width = (std::max)(
            maximum_preferred_size.width
          , preferred_size.width);

        maximum_preferred_size.height += preferred_size.height;
    }
    
    return maximum_preferred_size;
}

// ==========================================================================
// DO_LAYOUT
// ==========================================================================
void horizontal_strip_layout::do_layout(shared_ptr<container> const &cont)
{
    BOOST_AUTO(size,    cont->get_size());
    BOOST_AUTO(amount,  cont->get_number_of_components());
    BOOST_AUTO(y_coord, u32(0));
    
    for (u32 index = 0; index < amount; ++index)
    {
        BOOST_AUTO(comp,           cont->get_component(index));
        BOOST_AUTO(preferred_size, comp->get_preferred_size());
        BOOST_AUTO(width,          (std::min)(preferred_size.width, size.width));
        
        comp->set_position(point(0, y_coord));
        comp->set_size(extent(width, preferred_size.height));
        
        y_coord += preferred_size.height;
    }
}

}


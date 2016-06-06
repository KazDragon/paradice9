// ==========================================================================
// Munin Vertical Strip Layout.
//
// Copyright (C) 2013 Matthew Chaplain, All Rights Reserved.
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
#include "munin/vertical_strip_layout.hpp"
#include "munin/container.hpp"

namespace munin {
    
// ==========================================================================
// DO_GET_PREFERRED_SIZE 
// ==========================================================================
terminalpp::extent vertical_strip_layout::do_get_preferred_size(
    std::vector<std::shared_ptr<component>> const &components
  , std::vector<boost::any>                 const &hints) const
{
    // The preferred size of the whole component is the maximum height of
    // the components and the sum of the preferred widths of the components.
    terminalpp::extent maximum_preferred_size(0, 0);

    for (auto &comp : components)
    {
        terminalpp::extent preferred_size = comp->get_preferred_size();

        maximum_preferred_size.height = (std::max)(
            maximum_preferred_size.height
          , preferred_size.height);

        maximum_preferred_size.width += preferred_size.width;
    }
    
    return maximum_preferred_size;
}

// ==========================================================================
// DO_LAYOUT
// ==========================================================================
void vertical_strip_layout::do_layout(
    std::vector<std::shared_ptr<component>> const &components
  , std::vector<boost::any>                 const &hints
  , terminalpp::extent                             size)
{
    auto x_coord = odin::u32(0);
    
    for (auto &comp : components)
    {    
        auto preferred_size = comp->get_preferred_size();
        
        comp->set_position(terminalpp::point(x_coord, 0));
        comp->set_size(terminalpp::extent(preferred_size.width, size.height));
        
        x_coord += preferred_size.width;
    }
}

// ==========================================================================
// MAKE_VERTICAL_STRIP_LAYOUT
// ==========================================================================
std::unique_ptr<layout> make_vertical_strip_layout()
{
    return std::unique_ptr<vertical_strip_layout>(
        new vertical_strip_layout);
}


}


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

namespace munin {

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
terminalpp::extent horizontal_squeeze_layout::do_get_preferred_size(
    std::vector<std::shared_ptr<component>> const &components,
    std::vector<boost::any>                 const &hints) const
{
    // The preferred size of the whole component is the maximum preferred
    // height, and the maximum preferred height multiplied by the number
    // of components.
    terminalpp::extent maximum_preferred_size(0, 0);

    for (auto comp : components)
    {
        auto preferred_size = comp->get_preferred_size();

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
    std::vector<std::shared_ptr<component>> const &components,
    std::vector<boost::any>                 const &hints,
    terminalpp::extent                             size)
{
    auto amount = components.size();

    if (amount != 0)
    {
        auto height_per_component =  size.height / amount;
        auto height_remainder =      size.height % amount;
        auto height_remainder_used = odin::u32(0);

        for (odin::u32 index = 0; index < components.size(); ++index)
        {
            auto comp = components[index];

            comp->set_position(terminalpp::point(
                odin::u32(0)
              , (height_per_component * index) + height_remainder_used));

            auto height = height_per_component;

            if (height_remainder != height_remainder_used)
            {
                ++height;
                ++height_remainder_used;
            }

            comp->set_size(terminalpp::extent(size.width, height));
        }
    }
}

// ==========================================================================
// MAKE_HORIZONTAL_SQUEEZE_LAYOUT
// ==========================================================================
std::unique_ptr<layout> make_horizontal_squeeze_layout()
{
    return std::unique_ptr<horizontal_squeeze_layout>(
        new horizontal_squeeze_layout);
}

}


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
#include <algorithm>
#include <vector>

namespace munin {

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
terminalpp::extent compass_layout::do_get_preferred_size(
    std::vector<std::shared_ptr<component>> const &components,
    std::vector<boost::any >                const &hints) const
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

    for (odin::u32 index = 0; index < components.size(); ++index)
    {
        auto comp = components[index];
        auto hint = hints[index];

        odin::u32 *direction_hint = boost::any_cast<odin::u32>(&hint);
        odin::u32 direction = COMPASS_LAYOUT_SOUTH;

        if (direction_hint != NULL)
        {
            direction = *direction_hint;
        }

        auto comp_size = comp->get_preferred_size();

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

    terminalpp::extent preferred_size;

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

// ==========================================================================
// DO_LAYOUT
// ==========================================================================
void compass_layout::do_layout(
    std::vector<std::shared_ptr<component>> const &components,
    std::vector<boost::any>                 const &hints
  , terminalpp::extent                             size)
{
    std::vector<std::shared_ptr<component>> centre_components;
    odin::u32 west_used = 0;
    odin::u32 north_used = 0;
    odin::u32 south_used = 0;
    odin::u32 east_used = 0;

    for (odin::u32 index = 0; index < components.size(); ++index)
    {
        auto comp = components[index];
        auto hint = hints[index];

        odin::u32 *direction = boost::any_cast<odin::u32>(&hint);

        if (direction == NULL
         || *direction == COMPASS_LAYOUT_CENTRE)
        {
            centre_components.push_back(comp);
        }
        else if (*direction == COMPASS_LAYOUT_WEST)
        {
            auto comp_size = comp->get_preferred_size();
            west_used = (std::max)(
                west_used, odin::u32(comp_size.width));
            west_used = (std::min)(
                west_used, odin::u32(size.width - east_used));

            auto comp_height = (size.height - north_used) - south_used;

            comp->set_position(terminalpp::point(0, north_used));
            comp->set_size(terminalpp::extent(west_used, comp_height));
        }
        else if (*direction == COMPASS_LAYOUT_EAST)
        {
            auto comp_size = comp->get_preferred_size();
            east_used = (std::max)(
                east_used, odin::u32(comp_size.width));
            east_used = (std::min)(
                east_used, odin::u32(size.width - west_used));

            auto comp_height = (size.height - north_used) - south_used;

            comp->set_position(terminalpp::point(
                size.width - east_used
              , north_used));
            comp->set_size(terminalpp::extent(east_used, comp_height));
        }
        else if (*direction == COMPASS_LAYOUT_NORTH)
        {
            auto comp_size = comp->get_preferred_size();
            north_used = (std::max)(
                north_used, odin::u32(comp_size.height));
            north_used = (std::min)(
                north_used, odin::u32(size.height - south_used));

            auto comp_width = (size.width - west_used) - east_used;

            comp->set_position(terminalpp::point(west_used, 0));
            comp->set_size(terminalpp::extent(comp_width, north_used));
        }
        else if (*direction == COMPASS_LAYOUT_SOUTH)
        {
            auto comp_size = comp->get_preferred_size();
            south_used = (std::max)(
                south_used, odin::u32(comp_size.height));
            south_used = (std::min)(
                south_used, odin::u32(size.height - north_used));

            auto comp_width = (size.width - west_used) - east_used;

            comp->set_position(terminalpp::point(
                west_used
              , size.height - south_used));
            comp->set_size(terminalpp::extent(comp_width, south_used));
        }
        else
        {
            centre_components.push_back(comp);
        }
    }

    for (auto comp : centre_components)
    {
        comp->set_position(terminalpp::point(west_used, north_used));
        comp->set_size(terminalpp::extent(
            (size.width - west_used) - east_used
          , (size.height - north_used) - south_used));
    }
}

// ==========================================================================
// MAKE_COMPASS_LAYOUT
// ==========================================================================
std::unique_ptr<layout> make_compass_layout()
{
    return std::unique_ptr<layout>(new compass_layout);
}

}

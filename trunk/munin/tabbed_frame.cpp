// ==========================================================================
// Munin Tabbed Frame.
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
#include "munin/tabbed_frame.hpp"
#include "munin/basic_container.hpp"
#include "munin/compass_layout.hpp"
#include "munin/grid_layout.hpp"
#include <boost/make_shared.hpp>

#include "munin/filled_box.hpp"

using namespace odin;
using namespace boost;
using namespace std;

namespace munin {

// ==========================================================================
// TABBED_FRAME::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct tabbed_frame::impl
{
    string bar;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
tabbed_frame::tabbed_frame()
    : pimpl_(make_shared<impl>())
{
    // North - tabs
    BOOST_AUTO(north, make_shared<basic_container>());
    north->set_layout(make_shared<compass_layout>());
    north->add_component(
        make_shared<filled_box>(element_type('^'))
      , COMPASS_LAYOUT_NORTH);
    north->add_component(
        make_shared<filled_box>(element_type('+'))
      , COMPASS_LAYOUT_CENTRE);
    north->add_component(
        make_shared<filled_box>(element_type('v'))
      , COMPASS_LAYOUT_SOUTH);
        

    // West Container
    BOOST_AUTO(west, make_shared<basic_container>());
    west->set_layout(make_shared<grid_layout>(1, 1));
    west->add_component(
        make_shared<filled_box>(element_type(double_lined_vertical_beam)));

    // East Container
    BOOST_AUTO(east, make_shared<basic_container>());
    east->set_layout(make_shared<grid_layout>(1, 1));
    east->add_component(
        make_shared<filled_box>(element_type(double_lined_vertical_beam)));

    // South Container
    BOOST_AUTO(south, make_shared<basic_container>());
    south->set_layout(make_shared<compass_layout>());
    south->add_component(
        make_shared<filled_box>(element_type(double_lined_bottom_left_corner))
      , COMPASS_LAYOUT_WEST);
    south->add_component(
        make_shared<filled_box>(element_type(double_lined_horizontal_beam))
      , COMPASS_LAYOUT_CENTRE);
    south->add_component(
        make_shared<filled_box>(element_type(double_lined_bottom_right_corner))
      , COMPASS_LAYOUT_EAST);

    // All together.
    BOOST_AUTO(content, get_container());
    content->set_layout(make_shared<compass_layout>());
    content->add_component(north, COMPASS_LAYOUT_NORTH);
    content->add_component(south, COMPASS_LAYOUT_SOUTH);
    content->add_component(west, COMPASS_LAYOUT_WEST);
    content->add_component(east, COMPASS_LAYOUT_EAST);
}

// ==========================================================================
// DO_GET_TOP_BORDER_HEIGHT
// ==========================================================================
s32 tabbed_frame::do_get_top_border_height() const
{
    return 3;
}

// ==========================================================================
// DO_GET_BOTTOM_BORDER_HEIGHT
// ==========================================================================
s32 tabbed_frame::do_get_bottom_border_height() const
{
    return 1;
}

// ==========================================================================
// DO_GET_LEFT_BORDER_WIDTH
// ==========================================================================
s32 tabbed_frame::do_get_left_border_width() const
{
    return 1;
}

// ==========================================================================
// DO_GET_RIGHT_BORDER_WIDTH
// ==========================================================================
s32 tabbed_frame::do_get_right_border_width() const
{
    return 1;
}

}

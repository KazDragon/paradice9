// ==========================================================================
// Hugin Bestiary Page
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
#include "hugin/bestiary_page.hpp"
#include "munin/algorithm.hpp"
#include "munin/basic_container.hpp"
#include "munin/button.hpp"
#include "munin/compass_layout.hpp"
#include "munin/edit.hpp"
#include "munin/filled_box.hpp"
#include "munin/framed_component.hpp"
#include "munin/grid_layout.hpp"
#include "munin/list.hpp"
#include "munin/scroll_pane.hpp"
#include "munin/solid_frame.hpp"
#include "munin/text_area.hpp"
#include "munin/vertical_strip_layout.hpp"
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace munin;
using namespace boost;

namespace hugin {

// ==========================================================================
// BESTIARY_PAGE::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct bestiary_page::impl
{
    shared_ptr<list>      list_;
    shared_ptr<edit>      short_description_;
    shared_ptr<text_area> long_description_;
    shared_ptr<button>    new_button_;
    shared_ptr<button>    delete_button_;
    shared_ptr<button>    done_button_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
bestiary_page::bestiary_page()
    : pimpl_(make_shared<impl>())
{
    pimpl_->list_ = make_shared<list>();
    pimpl_->short_description_ = make_shared<edit>();
    pimpl_->long_description_  = make_shared<text_area>();
    pimpl_->new_button_        = make_shared<button>(
        string_to_elements("New"));
    pimpl_->delete_button_     = make_shared<button>(
        string_to_elements("Delete"));
    pimpl_->done_button_       = make_shared<button>(
        string_to_elements("Done"));

    // The left hand side of the upper half is dominated by the list.
    BOOST_AUTO(upper_left, make_shared<basic_container>());
    upper_left->set_layout(make_shared<grid_layout>(1, 1));
    upper_left->add_component(make_shared<scroll_pane>(pimpl_->list_));

    // The upper right has an edit field for the short description and a text
    // area for the long description.
    BOOST_AUTO(upper_right, make_shared<basic_container>());
    upper_right->set_layout(make_shared<compass_layout>());

    upper_right->add_component(
        make_shared<framed_component>(
            make_shared<solid_frame>()
          , pimpl_->short_description_)
      , COMPASS_LAYOUT_NORTH);

    upper_right->add_component(
        make_shared<scroll_pane>(pimpl_->long_description_)
      , COMPASS_LAYOUT_CENTRE);

    // The upper left and right are split 50/50 across the screen.
    BOOST_AUTO(upper, make_shared<basic_container>());
    upper->set_layout(make_shared<grid_layout>(1, 2));
    upper->add_component(upper_left);
    upper->add_component(upper_right);

    // The buttons line up in a row. They are then squeezed to the left along
    // the bottom.
    BOOST_AUTO(buttons, make_shared<basic_container>());
    buttons->set_layout(make_shared<vertical_strip_layout>());
    buttons->add_component(pimpl_->new_button_);
    buttons->add_component(pimpl_->delete_button_);
    buttons->add_component(pimpl_->done_button_);

    BOOST_AUTO(lower, make_shared<basic_container>());
    lower->set_layout(make_shared<compass_layout>());
    lower->add_component(buttons, COMPASS_LAYOUT_WEST);
    lower->add_component(
        make_shared<filled_box>(element_type(' '))
      , COMPASS_LAYOUT_CENTRE);

    // Arrange the upper and lower parts on the screen.
    BOOST_AUTO(content, get_container());
    content->set_layout(make_shared<compass_layout>());
    content->add_component(upper, COMPASS_LAYOUT_CENTRE);
    content->add_component(lower, COMPASS_LAYOUT_SOUTH);
}
    
// ==========================================================================
// DESTRUCTOR
// ==========================================================================
bestiary_page::~bestiary_page()
{
}
    
}

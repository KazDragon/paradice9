// ==========================================================================
// Delete Confirmation Dialog
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
#include "hugin/delete_confirmation_dialog.hpp"
#include "munin/algorithm.hpp"
#include "munin/aligned_layout.hpp"
#include "munin/basic_container.hpp"
#include "munin/button.hpp"
#include "munin/compass_layout.hpp"
#include "munin/filled_box.hpp"
#include "munin/grid_layout.hpp"
#include "munin/horizontal_squeeze_layout.hpp"
#include "munin/image.hpp"
#include "munin/vertical_squeeze_layout.hpp"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace munin;
using namespace boost;
using namespace std;

namespace hugin {

// ==========================================================================
// DELETE_CONFIRMATION_DIALOG::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct delete_confirmation_dialog::impl
{
    shared_ptr<image> delete_text_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
delete_confirmation_dialog::delete_confirmation_dialog()
    : pimpl_(make_shared<impl>())
{
    pimpl_->delete_text_ = make_shared<image>(string_to_elements("..."));

    alignment_data central_alignment;
    central_alignment.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTRE;
    central_alignment.vertical_alignment = VERTICAL_ALIGNMENT_CENTRE;

    // The upper half comprises the confirmation about the deletion
    BOOST_AUTO(
        confirmation_text
      , make_shared<image>(string_to_elements("Really delete")));

    BOOST_AUTO(confirmation_text_container, make_shared<basic_container>());
    confirmation_text_container->set_layout(make_shared<aligned_layout>());
    confirmation_text_container->add_component(
        confirmation_text
      , central_alignment);

    BOOST_AUTO(delete_text_container, make_shared<basic_container>());
    delete_text_container->set_layout(make_shared<aligned_layout>());
    delete_text_container->add_component(
        pimpl_->delete_text_
      , central_alignment);

    BOOST_AUTO(text_container, make_shared<basic_container>());
    text_container->set_layout(make_shared<horizontal_squeeze_layout>());
    text_container->add_component(confirmation_text_container);
    text_container->add_component(delete_text_container);

    BOOST_AUTO(upper_container, make_shared<basic_container>());
    upper_container->set_layout(make_shared<compass_layout>());
    upper_container->add_component(
        make_shared<filled_box>(element_type(' '))
      , COMPASS_LAYOUT_CENTRE);
    upper_container->add_component(text_container, COMPASS_LAYOUT_SOUTH);

    // The lower half comprises the yes and no buttons.
    BOOST_AUTO(yes_button, make_shared<button>(string_to_elements("Yes")));
    yes_button->on_click.connect(bind(ref(on_delete_confirmation)));

    BOOST_AUTO(yes_container, make_shared<basic_container>());
    yes_container->set_layout(make_shared<aligned_layout>());
    yes_container->add_component(yes_button, central_alignment);

    BOOST_AUTO(no_button, make_shared<button>(string_to_elements("No")));
    no_button->on_click.connect(bind(ref(on_delete_rejection)));

    BOOST_AUTO(no_container, make_shared<basic_container>());
    no_container->set_layout(make_shared<aligned_layout>());
    no_container->add_component(no_button, central_alignment);

    BOOST_AUTO(west_button_container, make_shared<basic_container>());
    west_button_container->set_layout(make_shared<compass_layout>());
    west_button_container->add_component(
        make_shared<filled_box>(element_type(' '))
      , COMPASS_LAYOUT_CENTRE);
    west_button_container->add_component(yes_container, COMPASS_LAYOUT_EAST);
        
    BOOST_AUTO(east_button_container, make_shared<basic_container>());
    east_button_container->set_layout(make_shared<compass_layout>());
    east_button_container->add_component(no_container, COMPASS_LAYOUT_WEST);
    east_button_container->add_component(
        make_shared<filled_box>(element_type(' '))
      , COMPASS_LAYOUT_CENTRE);

    BOOST_AUTO(button_container, make_shared<basic_container>());
    button_container->set_layout(make_shared<vertical_squeeze_layout>());
    button_container->add_component(west_button_container);
    button_container->add_component(east_button_container);

    BOOST_AUTO(lower_container, make_shared<basic_container>());
    lower_container->set_layout(make_shared<compass_layout>());
    lower_container->add_component(button_container, COMPASS_LAYOUT_NORTH);
    lower_container->add_component(
        make_shared<filled_box>(element_type(' '))
      , COMPASS_LAYOUT_CENTRE);

    BOOST_AUTO(content, get_container());
    content->set_layout(make_shared<horizontal_squeeze_layout>());
    content->add_component(upper_container);
    content->add_component(lower_container);

    content->set_layout(make_shared<grid_layout>(1, 1), LOWEST_LAYER);
    content->add_component(
        make_shared<filled_box>(element_type(' '))
      , any()
      , LOWEST_LAYER);
}
    
// ==========================================================================
// DESTRUCTOR
// ==========================================================================
delete_confirmation_dialog::~delete_confirmation_dialog()
{
}

// ==========================================================================
// SET_DELETION_TARGET_TEXT
// ==========================================================================
void delete_confirmation_dialog::set_deletion_target_text(string const &text)
{
    pimpl_->delete_text_->set_image(string_to_elements(text + "?"));
    get_container()->layout();
}

}

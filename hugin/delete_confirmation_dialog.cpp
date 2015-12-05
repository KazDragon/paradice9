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
#include <terminalpp/string.hpp>

namespace hugin {

// ==========================================================================
// DELETE_CONFIRMATION_DIALOG::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct delete_confirmation_dialog::impl
{
    std::shared_ptr<munin::image> delete_text_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
delete_confirmation_dialog::delete_confirmation_dialog()
    : pimpl_(std::make_shared<impl>())
{
    using namespace terminalpp::literals;
    
    pimpl_->delete_text_ = std::make_shared<munin::image>("..."_ts);

    munin::alignment_data central_alignment;
    central_alignment.horizontal_alignment = munin::HORIZONTAL_ALIGNMENT_CENTRE;
    central_alignment.vertical_alignment = munin::VERTICAL_ALIGNMENT_CENTRE;

    // The upper half comprises the confirmation about the deletion
    auto confirmation_text = std::make_shared<munin::image>("Really delete"_ts);

    auto confirmation_text_container = std::make_shared<munin::basic_container>();
    confirmation_text_container->set_layout(std::make_shared<munin::aligned_layout>());
    confirmation_text_container->add_component(
        confirmation_text
      , central_alignment);

    auto delete_text_container = std::make_shared<munin::basic_container>();
    delete_text_container->set_layout(std::make_shared<munin::aligned_layout>());
    delete_text_container->add_component(
        pimpl_->delete_text_
      , central_alignment);

    auto text_container = std::make_shared<munin::basic_container>();
    text_container->set_layout(std::make_shared<munin::horizontal_squeeze_layout>());
    text_container->add_component(confirmation_text_container);
    text_container->add_component(delete_text_container);

    auto upper_container = std::make_shared<munin::basic_container>();
    upper_container->set_layout(std::make_shared<munin::compass_layout>());
    upper_container->add_component(
        std::make_shared<munin::filled_box>(' ')
        , munin::COMPASS_LAYOUT_CENTRE);
    upper_container->add_component(text_container, munin::COMPASS_LAYOUT_SOUTH);

    // The lower half comprises the yes and no buttons.
    auto yes_button = std::make_shared<munin::button>("Yes"_ts);
    yes_button->on_click.connect([this]{on_delete_confirmation();});

    auto yes_container = std::make_shared<munin::basic_container>();
    yes_container->set_layout(std::make_shared<munin::aligned_layout>());
    yes_container->add_component(yes_button, central_alignment);

    auto no_button = std::make_shared<munin::button>("No"_ts);
    no_button->on_click.connect([this]{on_delete_rejection();});

    auto no_container = std::make_shared<munin::basic_container>();
    no_container->set_layout(std::make_shared<munin::aligned_layout>());
    no_container->add_component(no_button, central_alignment);

    auto west_button_container = std::make_shared<munin::basic_container>();
    west_button_container->set_layout(std::make_shared<munin::compass_layout>());
    west_button_container->add_component(
        std::make_shared<munin::filled_box>(' ')
      , munin::COMPASS_LAYOUT_CENTRE);
    west_button_container->add_component(yes_container, munin::COMPASS_LAYOUT_EAST);
        
    auto east_button_container = std::make_shared<munin::basic_container>();
    east_button_container->set_layout(std::make_shared<munin::compass_layout>());
    east_button_container->add_component(no_container, munin::COMPASS_LAYOUT_WEST);
    east_button_container->add_component(
        std::make_shared<munin::filled_box>(' ')
      , munin::COMPASS_LAYOUT_CENTRE);

    auto button_container = std::make_shared<munin::basic_container>();
    button_container->set_layout(std::make_shared<munin::vertical_squeeze_layout>());
    button_container->add_component(west_button_container);
    button_container->add_component(east_button_container);

    auto lower_container = std::make_shared<munin::basic_container>();
    lower_container->set_layout(std::make_shared<munin::compass_layout>());
    lower_container->add_component(button_container, munin::COMPASS_LAYOUT_NORTH);
    lower_container->add_component(
        std::make_shared<munin::filled_box>(' ')
      , munin::COMPASS_LAYOUT_CENTRE);

    auto content = get_container();
    content->set_layout(std::make_shared<munin::horizontal_squeeze_layout>());
    content->add_component(upper_container);
    content->add_component(lower_container);

    content->set_layout(std::make_shared<munin::grid_layout>(1, 1), munin::LOWEST_LAYER);
    content->add_component(
        std::make_shared<munin::filled_box>(' ')
      , {}
      , munin::LOWEST_LAYER);
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
void delete_confirmation_dialog::set_deletion_target_text(
    terminalpp::string const &text)
{
    pimpl_->delete_text_->set_image(text + "?");
    get_container()->layout();
}

}

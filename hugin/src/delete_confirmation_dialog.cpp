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
#include <munin/algorithm.hpp>
#include <munin/aligned_layout.hpp>
#include <munin/background_fill.hpp>
#include <munin/button.hpp>
#include <munin/compass_layout.hpp>
#include <munin/filled_box.hpp>
#include <munin/grid_layout.hpp>
#include <munin/horizontal_squeeze_layout.hpp>
#include <munin/image.hpp>
#include <munin/vertical_squeeze_layout.hpp>
#include <munin/view.hpp>
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
    pimpl_->delete_text_ = munin::make_image("...");

    // The upper half comprises the confirmation about the deletion
    auto confirmation_text = munin::make_image("Really delete");
    
    auto text_container = munin::view(
        std::make_shared<munin::horizontal_squeeze_layout>(),
        munin::view(
            std::make_shared<munin::aligned_layout>(),
            confirmation_text, munin::alignment_hcvc),
        munin::view(
            std::make_shared<munin::aligned_layout>(),
            pimpl_->delete_text_, munin::alignment_hcvc));

    auto upper_container = munin::view(
        munin::make_compass_layout(),
        munin::make_background_fill(), munin::COMPASS_LAYOUT_CENTRE,
        text_container, munin::COMPASS_LAYOUT_SOUTH);

    // The lower half comprises the yes and no buttons.
    auto yes_button = munin::make_button("Yes");
    yes_button->on_click.connect([this]{on_delete_confirmation();});

    auto no_button = munin::make_button("No");
    no_button->on_click.connect([this]{on_delete_rejection();});

    auto lower_container = munin::view(
        munin::make_compass_layout(),
        munin::view(
            munin::make_vertical_squeeze_layout(),
            munin::view(
                munin::make_compass_layout(),
                munin::make_background_fill(), munin::COMPASS_LAYOUT_CENTRE,
                munin::view(
                    std::make_shared<munin::aligned_layout>(),
                    yes_button, munin::alignment_hcvc
                ), munin::COMPASS_LAYOUT_EAST
            ), munin::COMPASS_LAYOUT_CENTRE,
            munin::view(
                munin::make_compass_layout(),
                munin::view(
                    std::make_shared<munin::aligned_layout>(),
                    no_button, munin::alignment_hcvc
                ), munin::COMPASS_LAYOUT_WEST)
            ), munin::COMPASS_LAYOUT_NORTH,
        munin::make_background_fill(), munin::COMPASS_LAYOUT_CENTRE);

    auto content = get_container();
    content->set_layout(munin::make_horizontal_squeeze_layout());
    content->add_component(upper_container);
    content->add_component(lower_container);

    content->set_layout(munin::make_grid_layout(1, 1), munin::LOWEST_LAYER);
    content->add_component(
        munin::make_background_fill(), {}, munin::LOWEST_LAYER);
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

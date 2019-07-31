// ==========================================================================
// Hugin Beast Editor
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
#include "hugin/beast_editor.hpp"
#include <munin/algorithm.hpp>
#include <munin/aligned_layout.hpp>
#include <munin/background_fill.hpp>
#include <munin/button.hpp>
#include <munin/compass_layout.hpp>
#include <munin/dropdown_list.hpp>
#include <munin/edit.hpp>
#include <munin/filled_box.hpp>
#include <munin/framed_component.hpp>
#include <munin/grid_layout.hpp>
#include <munin/image.hpp>
#include <munin/scroll_pane.hpp>
#include <munin/solid_frame.hpp>
#include <munin/text_area.hpp>
#include <munin/view.hpp>
#include <terminalpp/string.hpp>

namespace hugin {

// ==========================================================================
// BEAST_EDITOR::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct beast_editor::impl
{
    std::shared_ptr<munin::edit>          name_field_;
    std::shared_ptr<munin::text_area>     description_area_;
    std::shared_ptr<munin::dropdown_list> alignment_dropdown_;
    std::shared_ptr<munin::button>        save_button_;
    std::shared_ptr<munin::button>        revert_button_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
beast_editor::beast_editor()
    : pimpl_(std::make_shared<impl>())
{
    // Initialise all the viewable components.
    auto name_label        = munin::make_image("       Name: ");
    auto description_label = munin::make_image("Description: ");
    auto alignment_label   = munin::make_image("  Alignment: ");

    pimpl_->name_field_ = munin::make_edit();
    pimpl_->description_area_ = munin::make_text_area();
    pimpl_->alignment_dropdown_ = munin::make_dropdown_list();

    auto dropdown_items = std::vector<terminalpp::string> {
        "Hostile",
        "Neutral",
        "Friendly"
    };

    pimpl_->alignment_dropdown_->set_items(dropdown_items);
    pimpl_->alignment_dropdown_->set_item_index(1);
    
    pimpl_->save_button_   = munin::make_button(" Save ");
    pimpl_->revert_button_ = munin::make_button("Revert");

    // Set up callbacks
    pimpl_->save_button_->on_click.connect([this]{on_save();});
    pimpl_->revert_button_->on_click.connect([this]{on_revert();});

    // Lay out a container for the name label and field.
    auto name_container = munin::view(
        munin::make_compass_layout(),
        munin::view(
            munin::make_aligned_layout(),
            name_label, munin::alignment_hrvc
        ), munin::COMPASS_LAYOUT_WEST,
        munin::make_framed_component(
            munin::make_solid_frame(),
            pimpl_->name_field_
        ), munin::COMPASS_LAYOUT_CENTRE);

    // Lay out a container for the description label and field.
    auto description_container = munin::view(
        munin::make_compass_layout(),
        munin::view(
            munin::make_compass_layout(),
            munin::make_background_fill(), munin::COMPASS_LAYOUT_NORTH,
            description_label, munin::COMPASS_LAYOUT_CENTRE
        ), munin::COMPASS_LAYOUT_WEST,
        munin::make_scroll_pane(
            pimpl_->description_area_
        ), munin::COMPASS_LAYOUT_CENTRE);

    // Lay out a container for the alignment label and dropdown.
    auto alignment_container = munin::view(
        munin::make_compass_layout(),
        munin::view(
            munin::make_aligned_layout(),
            alignment_label, munin::alignment_hrvc
        ), munin::COMPASS_LAYOUT_WEST,
        pimpl_->alignment_dropdown_, munin::COMPASS_LAYOUT_CENTRE);

    // Lay out the current set of containers.
    auto editor_container = munin::view(
        munin::make_compass_layout(),
        name_container, munin::COMPASS_LAYOUT_NORTH,
        description_container, munin::COMPASS_LAYOUT_CENTRE,
        alignment_container, munin::COMPASS_LAYOUT_SOUTH);

    // Lay out a container for the buttons.
    auto button_container = munin::view(
        munin::make_compass_layout(),
        pimpl_->save_button_, munin::COMPASS_LAYOUT_WEST,
        munin::make_background_fill(), munin::COMPASS_LAYOUT_CENTRE,
        pimpl_->revert_button_, munin::COMPASS_LAYOUT_EAST);
    
    // Finally, add the contents to the component.
    auto content = get_container();
    content->set_layout(munin::make_compass_layout());
    content->add_component(editor_container, munin::COMPASS_LAYOUT_CENTRE);
    content->add_component(button_container, munin::COMPASS_LAYOUT_SOUTH);
    
    // Ensure that the background clears nicely - the layouts described above
    // have a couple of blank areas in them.
    content->set_layout(munin::make_grid_layout(1, 1), munin::LOWEST_LAYER);
    content->add_component(
        munin::make_background_fill(), {}, munin::LOWEST_LAYER);
}
    
// ==========================================================================
// DESTRUCTOR
// ==========================================================================
beast_editor::~beast_editor()
{
}

// ==========================================================================
// SET_BEAST_NAME
// ==========================================================================
void beast_editor::set_beast_name(terminalpp::string const &name)
{
    auto doc = pimpl_->name_field_->get_document();
    doc->delete_text({0, doc->get_text_size()});
    doc->insert_text(name);
}

// ==========================================================================
// GET_BEAST_NAME
// ==========================================================================
terminalpp::string beast_editor::get_beast_name() const
{
    auto doc = pimpl_->name_field_->get_document();
    auto name = doc->get_line(0);
    
    return name;
}

// ==========================================================================
// SET_BEAST_DESCRIPTION
// ==========================================================================
void beast_editor::set_beast_description(terminalpp::string const &description)
{
    auto doc = pimpl_->description_area_->get_document();
    doc->delete_text({0, doc->get_text_size()});
    doc->insert_text(description);
}

// ==========================================================================
// GET_BEAST_DESCRIPTION
// ==========================================================================
terminalpp::string beast_editor::get_beast_description() const
{
    terminalpp::string result;

    auto doc = pimpl_->description_area_->get_document();
    auto document_size = doc->get_size();

    for (std::int32_t index = 0; index < document_size.height; ++index)
    {
        result += doc->get_line(index);
        result += "\n";
    }

    return result;
}

}

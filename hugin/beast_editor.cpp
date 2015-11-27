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
#include <munin/basic_container.hpp>
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
    using namespace terminalpp::literals;
    
    // Initialise all the viewable components.
    auto name_label = 
        std::make_shared<munin::image>("       Name: "_ts);

    auto description_label = 
        std::make_shared<munin::image>("Description: "_ts);

    auto alignment_label = 
        std::make_shared<munin::image>("  Alignment: "_ts);

    pimpl_->name_field_ = std::make_shared<munin::edit>();
    pimpl_->description_area_ = std::make_shared<munin::text_area>();
    pimpl_->alignment_dropdown_ = std::make_shared<munin::dropdown_list>();

    std::vector<terminalpp::string> dropdown_items;
    dropdown_items.push_back("Hostile"_ts);
    dropdown_items.push_back("Neutral"_ts);
    dropdown_items.push_back("Friendly"_ts);

    pimpl_->alignment_dropdown_->set_items(dropdown_items);
    pimpl_->alignment_dropdown_->set_item_index(1);
    
    pimpl_->save_button_ = 
        std::make_shared<munin::button>(" Save "_ts);
    pimpl_->revert_button_ = 
        std::make_shared<munin::button>("Revert"_ts);

    // Set up callbacks
    pimpl_->save_button_->on_click.connect([this]{on_save();});
    pimpl_->revert_button_->on_click.connect([this]{on_revert();});

    // Lay out a container for the name label and field.
    auto name_label_container = std::make_shared<munin::basic_container>();
    name_label_container->set_layout(std::make_shared<munin::aligned_layout>());

    munin::alignment_data alignment;
    alignment.horizontal_alignment = munin::HORIZONTAL_ALIGNMENT_RIGHT;
    alignment.vertical_alignment = munin::VERTICAL_ALIGNMENT_CENTRE;
    name_label_container->add_component(name_label, alignment);

    auto name_container = std::make_shared<munin::basic_container>();
    name_container->set_layout(std::make_shared<munin::compass_layout>());
    name_container->add_component(name_label_container, munin::COMPASS_LAYOUT_WEST);
    name_container->add_component(std::make_shared<munin::framed_component>(
        std::make_shared<munin::solid_frame>()
      , pimpl_->name_field_)
      , munin::COMPASS_LAYOUT_CENTRE);
    // Ensure that the backdrop clears nicely.
    name_container->set_layout(
        std::make_shared<munin::grid_layout>(1, 1), munin::LOWEST_LAYER);
    name_container->add_component(
        std::make_shared<munin::filled_box>(terminalpp::glyph(' '))
      , {}
      , munin::LOWEST_LAYER);

    // Lay out a container for the description label and field.
    auto description_label_container = std::make_shared<munin::basic_container>();
    description_label_container->set_layout(std::make_shared<munin::compass_layout>());
    description_label_container->add_component(
        std::make_shared<munin::filled_box>(terminalpp::glyph(' ')),
        munin::COMPASS_LAYOUT_NORTH);
    description_label_container->add_component(
        description_label,
        munin::COMPASS_LAYOUT_CENTRE);

    auto description_container = std::make_shared<munin::basic_container>();
    description_container->set_layout(std::make_shared<munin::compass_layout>());
    description_container->add_component(
        description_label_container,
        munin::COMPASS_LAYOUT_WEST);
    description_container->add_component(
        std::make_shared<munin::scroll_pane>(pimpl_->description_area_),
        munin::COMPASS_LAYOUT_CENTRE);

    auto alignment_label_container = std::make_shared<munin::basic_container>();
    alignment_label_container->set_layout(std::make_shared<munin::aligned_layout>());

    alignment.horizontal_alignment = munin::HORIZONTAL_ALIGNMENT_RIGHT;
    alignment.vertical_alignment = munin::VERTICAL_ALIGNMENT_CENTRE;
    alignment_label_container->add_component(alignment_label, alignment);

    // Lay out a container for the alignment label and dropdown.
    auto alignment_container = std::make_shared<munin::basic_container>();
    alignment_container->set_layout(std::make_shared<munin::compass_layout>());
    alignment_container->add_component(
        alignment_label_container,
        munin::COMPASS_LAYOUT_WEST);
    alignment_container->add_component(
        pimpl_->alignment_dropdown_,
        munin::COMPASS_LAYOUT_CENTRE);
    alignment_container->set_layout(
        std::make_shared<munin::grid_layout>(1, 1), munin::LOWEST_LAYER);
    alignment_container->add_component(
        std::make_shared<munin::filled_box>(terminalpp::glyph(' ')),
        {},
        munin::LOWEST_LAYER);

    // Lay out the current set of containers.
    auto editor_container = std::make_shared<munin::basic_container>();
    editor_container->set_layout(std::make_shared<munin::compass_layout>());
    editor_container->add_component(name_container, munin::COMPASS_LAYOUT_NORTH);
    editor_container->add_component(description_container, munin::COMPASS_LAYOUT_CENTRE);
    editor_container->add_component(alignment_container, munin::COMPASS_LAYOUT_SOUTH);

    // Lay out a container for the buttons.
    auto button_container = std::make_shared<munin::basic_container>();
    button_container->set_layout(std::make_shared<munin::compass_layout>());
    button_container->add_component(pimpl_->save_button_, munin::COMPASS_LAYOUT_WEST);
    button_container->add_component(
        std::make_shared<munin::filled_box>(terminalpp::glyph(' ')),
        munin::COMPASS_LAYOUT_CENTRE);
    button_container->add_component(pimpl_->revert_button_, munin::COMPASS_LAYOUT_EAST);

    // Finally, add the contents to the component.
    auto content = get_container();
    content->set_layout(std::make_shared<munin::compass_layout>());
    content->add_component(editor_container, munin::COMPASS_LAYOUT_CENTRE);
    content->add_component(button_container, munin::COMPASS_LAYOUT_SOUTH);
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

    for (odin::s32 index = 0; index < document_size.height; ++index)
    {
        result += doc->get_line(index);
        result += "\n";
    }

    return result;
}

}

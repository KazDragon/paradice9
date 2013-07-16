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
#include <munin/ansi/protocol.hpp>
#include <munin/basic_container.hpp>
#include <munin/button.hpp>
#include <munin/compass_layout.hpp>
#include <munin/edit.hpp>
#include <munin/filled_box.hpp>
#include <munin/framed_component.hpp>
#include <munin/grid_layout.hpp>
#include <munin/image.hpp>
#include <munin/scroll_pane.hpp>
#include <munin/solid_frame.hpp>
#include <munin/text_area.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace odin;
using namespace munin;
using namespace munin::ansi;
using namespace boost;
using namespace std;

namespace hugin {

// ==========================================================================
// BEAST_EDITOR::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct beast_editor::impl
{
    shared_ptr<edit>      name_field_;
    shared_ptr<text_area> description_area_;
    shared_ptr<image>     alignment_widget_;
    shared_ptr<button>    save_button_;
    shared_ptr<button>    revert_button_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
beast_editor::beast_editor()
    : pimpl_(make_shared<impl>())
{
    // Initialise all the viewable components.
    BOOST_AUTO(
        name_label
      , make_shared<image>(string_to_elements("       Name: ")));

    BOOST_AUTO(
        description_label
      , make_shared<image>(string_to_elements("Description: ")));

    BOOST_AUTO(
        alignment_label
      , make_shared<image>(string_to_elements("  Alignment: ")));

    pimpl_->name_field_ = make_shared<edit>();
    pimpl_->description_area_ = make_shared<text_area>();
    pimpl_->alignment_widget_ = make_shared<image>(
        string_to_elements("<-----|----->"));
    
    pimpl_->save_button_ = 
        make_shared<button>(string_to_elements(" Save "));
    pimpl_->revert_button_ = 
        make_shared<button>(string_to_elements("Revert"));

    // Set up callbacks
    pimpl_->save_button_->on_click.connect(bind(ref(on_save)));
    pimpl_->revert_button_->on_click.connect(bind(ref(on_revert)));

    // Lay out a container for the name label and field.
    BOOST_AUTO(name_label_container, make_shared<basic_container>());
    name_label_container->set_layout(make_shared<aligned_layout>());

    alignment_data alignment;
    alignment.horizontal_alignment = HORIZONTAL_ALIGNMENT_RIGHT;
    alignment.vertical_alignment = VERTICAL_ALIGNMENT_CENTRE;
    name_label_container->add_component(name_label, alignment);

    BOOST_AUTO(name_container, make_shared<basic_container>());
    name_container->set_layout(make_shared<compass_layout>());
    name_container->add_component(name_label_container, COMPASS_LAYOUT_WEST);
    name_container->add_component(make_shared<framed_component>(
        make_shared<solid_frame>()
      , pimpl_->name_field_)
      , COMPASS_LAYOUT_CENTRE);
    // Ensure that the backdrop clears nicely.
    name_container->set_layout(make_shared<grid_layout>(1, 1), LOWEST_LAYER);
    name_container->add_component(
        make_shared<filled_box>(element_type(' '))
      , any()
      , LOWEST_LAYER);

    // Lay out a container for the description label and field.
    BOOST_AUTO(description_label_container, make_shared<basic_container>());
    description_label_container->set_layout(make_shared<compass_layout>());
    description_label_container->add_component(
        make_shared<filled_box>(element_type(' '))
      , COMPASS_LAYOUT_NORTH);
    description_label_container->add_component(
        description_label
      , COMPASS_LAYOUT_CENTRE);

    BOOST_AUTO(description_container, make_shared<basic_container>());
    description_container->set_layout(make_shared<compass_layout>());
    description_container->add_component(
        description_label_container
      , COMPASS_LAYOUT_WEST);
    description_container->add_component(
        make_shared<scroll_pane>(pimpl_->description_area_)
      , COMPASS_LAYOUT_CENTRE);

    // Lay out a container for the alignment label and widget.
    BOOST_AUTO(alignment_container, make_shared<basic_container>());
    alignment_container->set_layout(make_shared<compass_layout>());
    alignment_container->add_component(
        alignment_label
      , COMPASS_LAYOUT_WEST);
    alignment_container->add_component(
        pimpl_->alignment_widget_
      , COMPASS_LAYOUT_CENTRE);

    // Lay out the current set of containers.
    BOOST_AUTO(editor_container, make_shared<basic_container>());
    editor_container->set_layout(make_shared<compass_layout>());
    editor_container->add_component(name_container, COMPASS_LAYOUT_NORTH);
    editor_container->add_component(description_container, COMPASS_LAYOUT_CENTRE);
    editor_container->add_component(alignment_container, COMPASS_LAYOUT_SOUTH);

    // Lay out a container for the buttons.
    BOOST_AUTO(button_container, make_shared<basic_container>());
    button_container->set_layout(make_shared<compass_layout>());
    button_container->add_component(pimpl_->save_button_, COMPASS_LAYOUT_WEST);
    button_container->add_component(
        make_shared<filled_box>(element_type(' '))
      , COMPASS_LAYOUT_CENTRE);
    button_container->add_component(pimpl_->revert_button_, COMPASS_LAYOUT_EAST);

    // Finally, add the contents to the component.
    BOOST_AUTO(content, get_container());
    content->set_layout(make_shared<compass_layout>());
    content->add_component(editor_container, COMPASS_LAYOUT_CENTRE);
    content->add_component(button_container, COMPASS_LAYOUT_SOUTH);
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
void beast_editor::set_beast_name(string const &name)
{
    BOOST_AUTO(doc, pimpl_->name_field_->get_document());
    doc->delete_text(make_pair(0, doc->get_text_size()));
    doc->insert_text(string_to_elements(name));
}

// ==========================================================================
// GET_BEAST_NAME
// ==========================================================================
string beast_editor::get_beast_name() const
{
    BOOST_AUTO(doc, pimpl_->name_field_->get_document());
    BOOST_AUTO(name, doc->get_line(0));
    
    return string_from_elements(name);
}

// ==========================================================================
// SET_BEAST_DESCRIPTION
// ==========================================================================
void beast_editor::set_beast_description(string const &description)
{
    BOOST_AUTO(doc, pimpl_->description_area_->get_document());
    doc->delete_text(make_pair(0, doc->get_text_size()));
    doc->insert_text(string_to_elements(description));
}

// ==========================================================================
// GET_BEAST_DESCRIPTION
// ==========================================================================
string beast_editor::get_beast_description() const
{
    string result;

    BOOST_AUTO(doc, pimpl_->description_area_->get_document());
    BOOST_AUTO(document_size, doc->get_size());

    for (s32 index = 0; index < document_size.height; ++index)
    {
        result += string_from_elements(doc->get_line(index));
        result += "\n";
    }

    return result;
}

}

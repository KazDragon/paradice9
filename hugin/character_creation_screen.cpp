// ==========================================================================
// Hugin Character Creation Screen
//
// Copyright (C) 2011 Matthew Chaplain, All Rights Reserved.
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
#include "character_creation_screen.hpp"
#include "munin/aligned_layout.hpp"
#include "munin/basic_container.hpp"
#include "munin/button.hpp"
#include "munin/compass_layout.hpp"
#include "munin/edit.hpp"
#include "munin/filled_box.hpp"
#include "munin/framed_component.hpp"
#include "munin/grid_layout.hpp"
#include "munin/image.hpp"
#include "munin/named_frame.hpp"
#include "munin/solid_frame.hpp"
#include "munin/toggle_button.hpp"
#include <terminalpp/string.hpp>
#include <terminalpp/virtual_key.hpp>

namespace hugin {

// ==========================================================================
// CHARACTER_CREATION_SCREEN::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct character_creation_screen::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(character_creation_screen &self)
      : self_(self)
    {
    }
    
    // ======================================================================
    // ON_CHARACTER_CREATION_OK
    // ======================================================================
    void on_character_creation_ok()
    {
        auto document = name_field_->get_document();
        auto elements = document->get_line(0);
        auto name = to_string(elements);
        
        self_.on_character_created(name, gm_toggle_->get_toggle());
    }
    
    // ======================================================================
    // ON_CHARACTER_CREATION_CANCELLED
    // ======================================================================
    void on_character_creation_cancelled()
    {
        self_.on_character_creation_cancelled();
    }
    
    character_creation_screen              &self_;

    // Character Creation components
    std::shared_ptr<munin::edit>            name_field_;
    std::shared_ptr<munin::toggle_button>   gm_toggle_;
    std::shared_ptr<munin::button>          ok_button_;
    std::shared_ptr<munin::button>          cancel_button_;
    std::vector<boost::signals::connection> connections_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
character_creation_screen::character_creation_screen()
    : pimpl_(std::make_shared<impl>(std::ref(*this)))
{
    using namespace terminalpp::literals;
    
    auto content = get_container();
    content->set_layout(std::make_shared<munin::grid_layout>(1, 1));
    
    auto screen_frame = std::make_shared<munin::named_frame>();
    screen_frame->set_name("CHARACTER CREATION"_ts);
    
    // Create the Name, Password and Password (verify) labels.
    munin::alignment_data alignment;
    alignment.horizontal_alignment = munin::HORIZONTAL_ALIGNMENT_RIGHT;
    alignment.vertical_alignment   = munin::VERTICAL_ALIGNMENT_CENTRE;
    
    auto name_container = std::make_shared<munin::basic_container>();
    name_container->set_layout(std::make_shared<munin::aligned_layout>());
    name_container->add_component(
        std::make_shared<munin::image>("Name: "_ts)
      , alignment);

    auto gm_container = std::make_shared<munin::basic_container>();
    gm_container->set_layout(std::make_shared<munin::aligned_layout>());
    gm_container->add_component(
        std::make_shared<munin::image>("GM: "_ts)
      , alignment);
    
    auto labels_container = std::make_shared<munin::basic_container>();
    labels_container->set_layout(std::make_shared<munin::grid_layout>(2, 1));
    labels_container->add_component(name_container);
    labels_container->add_component(gm_container);
    
    // Create the Name field and GM toggle button.
    pimpl_->name_field_ = std::make_shared<munin::edit>();
    pimpl_->gm_toggle_  = std::make_shared<munin::toggle_button>(false);
    
    auto gm_toggle_container = std::make_shared<munin::basic_container>();
    gm_toggle_container->set_layout(std::make_shared<munin::compass_layout>());
    gm_toggle_container->add_component(pimpl_->gm_toggle_, munin::COMPASS_LAYOUT_WEST);

    auto fields_container = std::make_shared<munin::basic_container>();
    fields_container->set_layout(std::make_shared<munin::grid_layout>(2, 1));
    fields_container->add_component(
        std::make_shared<munin::framed_component>(
            std::make_shared<munin::solid_frame>()
          , pimpl_->name_field_));
    fields_container->add_component(gm_toggle_container);
    
    auto labels_fields_container = std::make_shared<munin::basic_container>();
    labels_fields_container->set_layout(std::make_shared<munin::compass_layout>());
    labels_fields_container->add_component(
        labels_container,
        munin::COMPASS_LAYOUT_WEST);
    labels_fields_container->add_component(
        fields_container,
        munin::COMPASS_LAYOUT_CENTRE);
    
    auto inner_container = std::make_shared<munin::basic_container>();
    inner_container->set_layout(std::make_shared<munin::compass_layout>());
    inner_container->add_component(
        labels_fields_container,
        munin::COMPASS_LAYOUT_NORTH);

    // Create the OK and Cancel buttons.
    pimpl_->ok_button_     = std::make_shared<munin::button>("  OK  "_ts);
    pimpl_->connections_.push_back(pimpl_->ok_button_->on_click.connect(
        [this]{pimpl_->on_character_creation_ok();}));
    
    pimpl_->cancel_button_ = std::make_shared<munin::button>("Cancel"_ts);
    pimpl_->connections_.push_back(pimpl_->cancel_button_->on_click.connect(
        [this]{pimpl_->on_character_creation_cancelled();}));
    
    auto buttons_inner_container = std::make_shared<munin::basic_container>();
    buttons_inner_container->set_layout(std::make_shared<munin::grid_layout>(1, 2));
    buttons_inner_container->add_component(pimpl_->ok_button_);
    buttons_inner_container->add_component(pimpl_->cancel_button_);
    
    auto buttons_outer_container = std::make_shared<munin::basic_container>();
    buttons_outer_container->set_layout(std::make_shared<munin::compass_layout>());
    buttons_outer_container->add_component(
        buttons_inner_container,
        munin::COMPASS_LAYOUT_EAST);
    
    auto inner_container2 = std::make_shared<munin::basic_container>();
    inner_container2->set_layout(std::make_shared<munin::compass_layout>());
    inner_container2->add_component(
        inner_container,
        munin::COMPASS_LAYOUT_NORTH);
    inner_container2->add_component(
        buttons_outer_container,
        munin::COMPASS_LAYOUT_CENTRE);
    
    auto inner_container3 = std::make_shared<munin::basic_container>();
    inner_container3->set_layout(std::make_shared<munin::compass_layout>());
    inner_container3->add_component(inner_container2, munin::COMPASS_LAYOUT_NORTH);
    // TODO: Coalesce?    
    content->add_component(std::make_shared<munin::framed_component>(
        screen_frame
      , inner_container3));

    // Add a filler to ensure that the background is opaque.
    content->set_layout(
        std::make_shared<munin::grid_layout>(1, 1)
      , munin::LOWEST_LAYER);
    content->add_component(
        std::make_shared<munin::filled_box>(' ')
      , {}
      , munin::LOWEST_LAYER);
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
character_creation_screen::~character_creation_screen()
{
    for (auto &cnx : pimpl_->connections_)
    {
        cnx.disconnect();
    }
}

// ==========================================================================
// CLEAR
// ==========================================================================
void character_creation_screen::clear()
{
    auto document = pimpl_->name_field_->get_document();
    document->delete_text({odin::u32(0), document->get_text_size()});
    
    pimpl_->gm_toggle_->set_toggle(false);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void character_creation_screen::do_event(boost::any const &ev)
{
    auto const *vk = boost::any_cast<terminalpp::virtual_key>(&ev);
    bool handled = false;
    
    if (vk)
    {
        if (vk->key == terminalpp::vk::ht)
        {
            focus_next();
            
            if (!has_focus())
            {
                focus_next();
            }
            
            handled = true;
        }
        else if (vk->key == terminalpp::vk::bt)
        {
            focus_previous();

            if (!has_focus())
            {
                focus_previous();
            }
            
            handled = true;
        }
    }
    
    if (!handled)
    {
        composite_component::do_event(ev);
    }
}

}


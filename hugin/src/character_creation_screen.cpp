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
#include "hugin/character_creation_screen.hpp"
#include <munin/aligned_layout.hpp>
#include <munin/background_fill.hpp>
#include <munin/button.hpp>
#include <munin/compass_layout.hpp>
#include <munin/edit.hpp>
#include <munin/filled_box.hpp>
#include <munin/framed_component.hpp>
#include <munin/grid_layout.hpp>
#include <munin/image.hpp>
#include <munin/named_frame.hpp>
#include <munin/solid_frame.hpp>
#include <munin/toggle_button.hpp>
#include <munin/view.hpp>
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
    pimpl_->name_field_ = std::make_shared<munin::edit>();
    pimpl_->gm_toggle_  = std::make_shared<munin::toggle_button>(false);
    
    pimpl_->ok_button_     = std::make_shared<munin::button>("  OK  ");
    pimpl_->connections_.push_back(pimpl_->ok_button_->on_click.connect(
        [this]{pimpl_->on_character_creation_ok();}));
    
    pimpl_->cancel_button_ = std::make_shared<munin::button>("Cancel");
    pimpl_->connections_.push_back(pimpl_->cancel_button_->on_click.connect(
        [this]{pimpl_->on_character_creation_cancelled();}));
    
    auto labels_fields_container = munin::view(
        munin::make_compass_layout(),
        munin::view(
            munin::make_compass_layout(),
            munin::view(
                munin::make_grid_layout(2, 1),
                munin::view(
                    munin::make_aligned_layout(),
                    munin::make_image("Name: "), munin::alignment_hrvc),
                munin::view(
                    munin::make_aligned_layout(),
                    munin::make_image("GM: "), munin::alignment_hrvc)
            ), munin::COMPASS_LAYOUT_WEST,
            munin::view(
                munin::make_grid_layout(2, 1),
                munin::make_framed_component(
                    munin::make_solid_frame(),
                    pimpl_->name_field_),
                munin::view(
                    munin::make_compass_layout(),
                    pimpl_->gm_toggle_, munin::COMPASS_LAYOUT_WEST)
            ), munin::COMPASS_LAYOUT_CENTRE
        ), munin::COMPASS_LAYOUT_NORTH);

    auto buttons_container = munin::view(
        munin::make_compass_layout(),
        munin::view(
            munin::make_grid_layout(1, 2),
            pimpl_->ok_button_,
            pimpl_->cancel_button_
        ), munin::COMPASS_LAYOUT_EAST);

    auto content = get_container();
    content->set_layout(munin::make_grid_layout(1, 1));
    content->add_component(munin::make_framed_component(
        munin::make_named_frame("CHARACTER CREATION"),
        munin::view(
            munin::make_compass_layout(),
            munin::view(
                munin::make_compass_layout(),
                labels_fields_container, munin::COMPASS_LAYOUT_NORTH,
                buttons_container, munin::COMPASS_LAYOUT_CENTRE
            ), munin::COMPASS_LAYOUT_NORTH)));
    
    // Add a filler to ensure that the background is opaque.
    content->set_layout(munin::make_grid_layout(1, 1), munin::LOWEST_LAYER);
    content->add_component(
        munin::make_background_fill(), {}, munin::LOWEST_LAYER);
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


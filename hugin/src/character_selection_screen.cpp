// ==========================================================================
// Hugin Character Selection Screen
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
#include "hugin/character_selection_screen.hpp"
#include "munin/basic_container.hpp"
#include "munin/grid_layout.hpp"
#include "munin/filled_box.hpp"
#include "munin/framed_component.hpp"
#include "munin/image.hpp"
#include "munin/named_frame.hpp"
#include <terminalpp/encoder.hpp>
#include <terminalpp/virtual_key.hpp>
#include <boost/format.hpp>

namespace hugin {

// ==========================================================================
// CHARACTER_SELECTION_SCREEN::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct character_selection_screen::impl
{
    std::vector<std::pair<std::string, std::string>> characters_;
    
    std::shared_ptr<munin::image>                    character_list_;
    std::shared_ptr<munin::container>                inner_content_;
    
    std::shared_ptr<munin::container> create_inner_content()
    {
        return std::make_shared<munin::basic_container>();
    }

    std::shared_ptr<munin::image> create_character_list()
    {
        using namespace terminalpp::literals;

        // Create an array of element 'strings' big enough to hold all the
        // character names, and also the "create new character" option.
        std::vector<terminalpp::string> text(characters_.size() + 1);
        
        // Fill the first element with the "create new character" text.
        text[0] = "+. <Create new character>"_ts;
        
        // Fill the remainder with the names of the characters.
        for (size_t index = 0; index < characters_.size(); ++index)
        {
            text[index + 1] = terminalpp::encode(
                boost::str(boost::format("%d. %s")
                % index
                % characters_[index].second));
        }
        
        auto img = std::make_shared<munin::image>(text);
        img->set_can_focus(true);
        return img;
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
character_selection_screen::character_selection_screen()
  : pimpl_(std::make_shared<impl>())
{
    using namespace terminalpp::literals;

    pimpl_->character_list_ = pimpl_->create_character_list();
    pimpl_->inner_content_  = pimpl_->create_inner_content();
    
    pimpl_->inner_content_->set_layout(std::make_shared<munin::grid_layout>(1, 1));
    pimpl_->inner_content_->add_component(pimpl_->character_list_);
    
    auto border = std::make_shared<munin::named_frame>();
    border->set_name("CHARACTER SELECTION"_ts);

    auto content = get_container();
    content->set_layout(std::make_shared<munin::grid_layout>(1, 1));
    content->add_component(std::make_shared<munin::framed_component>(
        border
      , pimpl_->inner_content_));

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
character_selection_screen::~character_selection_screen()
{
}

// ==========================================================================
// CLEAR
// ==========================================================================
void character_selection_screen::clear()
{
}

// ==========================================================================
// SET_CHARACTER_NAMES
// ==========================================================================
void character_selection_screen::set_character_names(
    std::vector<std::pair<std::string, std::string>> const &names)
{
    pimpl_->inner_content_->remove_component(pimpl_->character_list_);
    pimpl_->characters_ = names;
    pimpl_->character_list_ = pimpl_->create_character_list();
    pimpl_->inner_content_->add_component(pimpl_->character_list_);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void character_selection_screen::do_event(boost::any const &event)
{
    auto vk = boost::any_cast<terminalpp::virtual_key>(&event);
    
    if (vk)
    {
        if (vk->key == terminalpp::vk::plus)
        {
            on_new_character();
        }
        else
        {
            unsigned int selection = char(vk->key) - '0';
            
            if (selection < pimpl_->characters_.size())
            {
                on_character_selected(
                    pimpl_->characters_[selection].first);
            }
        }
    }
}

}


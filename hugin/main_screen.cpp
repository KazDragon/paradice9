// ==========================================================================
// Hugin Main Screen
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
#include "main_screen.hpp"
#include "hugin/active_encounter_view.hpp"
#include "hugin/command_prompt.hpp"
#include "hugin/wholist.hpp"
#include "munin/basic_container.hpp"
#include "munin/compass_layout.hpp"
#include "munin/edit.hpp"
#include "munin/framed_component.hpp"
#include "munin/horizontal_squeeze_layout.hpp"
#include "munin/named_frame.hpp"
#include "munin/solid_frame.hpp"
#include "munin/scroll_pane.hpp"
#include "munin/text_area.hpp"
#include "munin/vertical_squeeze_layout.hpp"
#include <terminalpp/string.hpp>
#include <terminalpp/virtual_key.hpp>

namespace hugin {

// ==========================================================================
// MAIN_SCREEN::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct main_screen::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(main_screen &self)
        : self_(self)
        , help_field_visible_(false)
        , active_encounter_view_visible_(false)
    {
    } 
    
    // ======================================================================
    // ON_INPUT_ENTERED
    // ======================================================================
    void on_input_entered()
    {
        auto document = input_field_->get_document();
        auto elements = document->get_line(0);
        auto input = to_string(elements); 

        document->delete_text({odin::u32(0), document->get_text_size()});

        if (!input.empty())
        {
            self_.on_input_entered(input);
        }
    }

    // ======================================================================
    // FIRE_HELP_CLOSED
    // ======================================================================
    void fire_help_closed()
    {
        self_.on_help_closed();
    }
    
    main_screen                           &self_;
    
    std::shared_ptr<wholist>               wholist_;
    std::shared_ptr<command_prompt>        input_field_;
    std::shared_ptr<munin::container>      output_encounter_container_;
    std::shared_ptr<munin::container>      output_container_;
    std::shared_ptr<munin::text_area>      output_field_;
    std::shared_ptr<munin::text_area>      help_field_;
    std::shared_ptr<munin::scroll_pane>    help_field_frame_;
    bool                                   help_field_visible_;
    std::shared_ptr<active_encounter_view> active_encounter_view_;
    bool                                   active_encounter_view_visible_;
    
    std::function<void (std::string const &)> on_input_entered_;
    std::function<void ()>                 on_help_closed_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
main_screen::main_screen()
  : pimpl_(std::make_shared<impl>(std::ref(*this)))
{
    using namespace terminalpp::literals;

    auto content = get_container();
    content->set_layout(std::make_shared<munin::compass_layout>());
    
    pimpl_->input_field_ = std::make_shared<command_prompt>();
    content->add_component(
        std::make_shared<munin::framed_component>(
            std::make_shared<munin::solid_frame>()
          , pimpl_->input_field_)
        , munin::COMPASS_LAYOUT_SOUTH);
    
    pimpl_->wholist_ = std::make_shared<wholist>();
    
    auto wholist_frame = std::make_shared<munin::named_frame>();
    wholist_frame->set_name("CURRENTLY PLAYING"_ts);
    
    content->add_component(
        std::make_shared<munin::framed_component>(wholist_frame, pimpl_->wholist_)
        , munin::COMPASS_LAYOUT_NORTH);
    
    pimpl_->output_container_ = std::make_shared<munin::basic_container>();
    pimpl_->output_container_->set_layout(
        std::make_shared<munin::vertical_squeeze_layout>());
    
    pimpl_->output_field_ = std::make_shared<munin::text_area>();
    pimpl_->output_field_->disable();
    pimpl_->output_container_->add_component(
        std::make_shared<munin::scroll_pane>(pimpl_->output_field_));
    
    pimpl_->help_field_ = std::make_shared<munin::text_area>();
    pimpl_->help_field_->disable();
    
    pimpl_->help_field_frame_ = std::make_shared<munin::scroll_pane>(pimpl_->help_field_);

    pimpl_->output_encounter_container_ = std::make_shared<munin::basic_container>();
    pimpl_->output_encounter_container_->set_layout(
        std::make_shared<munin::horizontal_squeeze_layout>());

    pimpl_->output_encounter_container_->add_component(
        pimpl_->output_container_);

    pimpl_->active_encounter_view_ = std::make_shared<active_encounter_view>();

    content->add_component(pimpl_->output_encounter_container_, munin::COMPASS_LAYOUT_CENTRE);
}

// ==========================================================================
// CLEAR
// ==========================================================================
void main_screen::clear()
{
    auto document = pimpl_->input_field_->get_document();
    document->delete_text({odin::u32(0), document->get_text_size()});
    
    document = pimpl_->output_field_->get_document();
    document->delete_text({odin::u32(0), document->get_text_size()});
    
    document = pimpl_->help_field_->get_document();
    document->delete_text({odin::u32(0), document->get_text_size()});
    
    update_wholist({});
    hide_help_window();
    pimpl_->input_field_->clear_history();
}

// ==========================================================================
// ADD_OUTPUT_TEXT
// ==========================================================================
void main_screen::add_output_text(terminalpp::string const &text)
{
    pimpl_->output_field_->get_document()->insert_text(
        text
      , pimpl_->output_field_->get_document()->get_text_size());
}

// ==========================================================================
// UPDATE_WHOLIST
// ==========================================================================
void main_screen::update_wholist(std::vector<std::string> const &names)
{
    pimpl_->wholist_->set_names(names);
}

// ==========================================================================
// ADD_COMMAND_HISTORY
// ==========================================================================
void main_screen::add_command_history(std::string const &history)
{
    pimpl_->input_field_->add_history(history);
}

// ==========================================================================
// SHOW_HELP_WINDOW
// ==========================================================================
void main_screen::show_help_window()
{
    if (!pimpl_->help_field_visible_)
    {
        pimpl_->output_container_->add_component(pimpl_->help_field_frame_);
        pimpl_->help_field_visible_ = true;
    }
}

// ==========================================================================
// HIDE_HELP_WINDOW
// ==========================================================================
void main_screen::hide_help_window()
{
    if (pimpl_->help_field_visible_)
    {
        pimpl_->output_container_->remove_component(pimpl_->help_field_frame_);
        pimpl_->help_field_visible_ = false;
        
        if (pimpl_->help_field_frame_->has_focus())
        {
            pimpl_->input_field_->set_focus();
        }
    }
}

// ==========================================================================
// SHOW_ACTIVE_ENCOUNTER_WINDOW
// ==========================================================================
void main_screen::show_active_encounter_window()
{
    if (!pimpl_->active_encounter_view_visible_)
    {
        pimpl_->output_encounter_container_->add_component(
            pimpl_->active_encounter_view_);
        pimpl_->active_encounter_view_visible_ = true;
    }
}

// ==========================================================================
// HIDE_ACTIVE_ENCOUNTER_WINDOW
// ==========================================================================
void main_screen::hide_active_encounter_window()
{
    if (pimpl_->active_encounter_view_visible_)
    {
        pimpl_->output_encounter_container_->remove_component(
            pimpl_->active_encounter_view_);
        pimpl_->active_encounter_view_visible_ = false;

        if (pimpl_->active_encounter_view_->has_focus())
        {
            pimpl_->input_field_->set_focus();
        }
    }
}

// ==========================================================================
// SET_ACTIVE_ENCOUNTER
// ==========================================================================
void main_screen::set_active_encounter(
    std::shared_ptr<paradice::active_encounter> const &active_encounter)
{
    pimpl_->active_encounter_view_->set_encounter(active_encounter);
}

// ==========================================================================
// SET_HELP_WINDOW_TEXT
// ==========================================================================
void main_screen::set_help_window_text(terminalpp::string const &text)
{
    auto document = pimpl_->help_field_->get_document();
    document->delete_text({odin::u32(0), document->get_text_size()});
    document->insert_text(text);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void main_screen::do_event(boost::any const &ev)
{
    auto const *vk = boost::any_cast<terminalpp::virtual_key>(&ev);
    bool handled = false;
    
    if (vk)
    {
        if (pimpl_->input_field_->has_focus() 
         && vk->key == terminalpp::vk::enter)
        {
            pimpl_->on_input_entered();
            handled = true;
        }
        else if (vk->key == terminalpp::vk::ht)
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


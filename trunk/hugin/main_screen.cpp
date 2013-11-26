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
#include "munin/ansi/protocol.hpp"
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
#include "odin/ansi/protocol.hpp"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace munin::ansi;
using namespace munin;
using namespace paradice;
using namespace odin;
using namespace boost;
using namespace std;

namespace hugin {

// ==========================================================================
// MAIN_SCREEN::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct main_screen::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl()
        : help_field_visible_(false)
        , active_encounter_view_visible_(false)
    {
    } 
    
    // ======================================================================
    // ON_INPUT_ENTERED
    // ======================================================================
    void on_input_entered()
    {
        if (on_input_entered_)
        {
            BOOST_AUTO(document, input_field_->get_document());
            BOOST_AUTO(elements, document->get_line(0));
            BOOST_AUTO(input,   string_from_elements(elements)); 

            document->delete_text(
                make_pair(u32(0), document->get_text_size()));

            if (!input.empty())
            {
                on_input_entered_(input);
            }
        }
    }

    // ======================================================================
    // FIRE_HELP_CLOSED
    // ======================================================================
    void fire_help_closed()
    {
        if (on_help_closed_)
        {
            on_help_closed_();
        }
    }
    
    shared_ptr<wholist>               wholist_;
    shared_ptr<command_prompt>        input_field_;
    shared_ptr<container>             output_encounter_container_;
    shared_ptr<container>             output_container_;
    shared_ptr<text_area>             output_field_;
    shared_ptr<text_area>             help_field_;
    shared_ptr<scroll_pane>           help_field_frame_;
    bool                              help_field_visible_;
    shared_ptr<active_encounter_view> active_encounter_view_;
    bool                              active_encounter_view_visible_;
    
    function<void (string)>           on_input_entered_;
    function<void ()>                 on_help_closed_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
main_screen::main_screen()
    : pimpl_(make_shared<impl>())
{
    BOOST_AUTO(content, get_container());
    content->set_layout(make_shared<compass_layout>());
    
    pimpl_->input_field_ = make_shared<command_prompt>();
    content->add_component(
        make_shared<framed_component>(
            make_shared<solid_frame>()
          , pimpl_->input_field_)
      , COMPASS_LAYOUT_SOUTH);
    
    pimpl_->wholist_ = make_shared<wholist>();
    
    BOOST_AUTO(wholist_frame, make_shared<named_frame>());
    wholist_frame->set_name("CURRENTLY PLAYING");
    
    content->add_component(
        make_shared<framed_component>(wholist_frame, pimpl_->wholist_)
      , COMPASS_LAYOUT_NORTH);
    
    pimpl_->output_container_ = make_shared<basic_container>();
    pimpl_->output_container_->set_layout(
        make_shared<vertical_squeeze_layout>());
    
    pimpl_->output_field_ = make_shared<text_area>();
    pimpl_->output_field_->disable();
    pimpl_->output_container_->add_component(
        make_shared<scroll_pane>(pimpl_->output_field_));
    
    pimpl_->help_field_ = make_shared<text_area>();
    pimpl_->help_field_->disable();
    
    pimpl_->help_field_frame_ = make_shared<scroll_pane>(pimpl_->help_field_);

    pimpl_->output_encounter_container_ = make_shared<basic_container>();
    pimpl_->output_encounter_container_->set_layout(
        make_shared<horizontal_squeeze_layout>());

    pimpl_->output_encounter_container_->add_component(
        pimpl_->output_container_);

    pimpl_->active_encounter_view_ = make_shared<active_encounter_view>();

    content->add_component(pimpl_->output_encounter_container_, COMPASS_LAYOUT_CENTRE);
}

// ==========================================================================
// CLEAR
// ==========================================================================
void main_screen::clear()
{
    BOOST_AUTO(document, pimpl_->input_field_->get_document());
    document->delete_text(make_pair(u32(0), document->get_text_size()));
    
    document = pimpl_->output_field_->get_document();
    document->delete_text(make_pair(u32(0), document->get_text_size()));
    
    document = pimpl_->help_field_->get_document();
    document->delete_text(make_pair(u32(0), document->get_text_size()));
    
    update_wholist(vector<string>());
    hide_help_window();
    pimpl_->input_field_->clear_history();
}

// ==========================================================================
// ON_INPUT_ENTERED
// ==========================================================================
void main_screen::on_input_entered(function<void (string)> callback)
{
    pimpl_->on_input_entered_ = callback;
}

// ==========================================================================
// ADD_OUTPUT_TEXT
// ==========================================================================
void main_screen::add_output_text(vector<element_type> const &text)
{
    pimpl_->output_field_->get_document()->insert_text(
        text
      , pimpl_->output_field_->get_document()->get_text_size());
}

// ==========================================================================
// UPDATE_WHOLIST
// ==========================================================================
void main_screen::update_wholist(vector<string> const &names)
{
    pimpl_->wholist_->set_names(names);
}

// ==========================================================================
// ADD_COMMAND_HISTORY
// ==========================================================================
void main_screen::add_command_history(string const &history)
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
    shared_ptr<active_encounter> active_encounter)
{
}

// ==========================================================================
// SET_HELP_WINDOW_TEXT
// ==========================================================================
void main_screen::set_help_window_text(vector<element_type> const &text)
{
    BOOST_AUTO(document, pimpl_->help_field_->get_document());
    document->delete_text(make_pair(u32(0), document->get_text_size()));
    document->insert_text(text);
}

// ==========================================================================
// ON_HELP_CLOSED
// ==========================================================================
void main_screen::on_help_closed(function<void ()> callback)
{
    pimpl_->on_help_closed_ = callback;
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void main_screen::do_event(any const &ev)
{
    bool handled = false;

    char const *ch = any_cast<char>(&ev);
    odin::ansi::control_sequence const *control_sequence = 
        any_cast<odin::ansi::control_sequence>(&ev);

    if (pimpl_->input_field_->has_focus() && ch != NULL)
    {
        if (*ch == '\n')
        {
            pimpl_->on_input_entered();
            handled = true;
        }
    }
    else if (ch != NULL && *ch == '\t')
    {
        focus_next();
        
        if (!has_focus())
        {
            focus_next();
        }
        
        handled = true;
    }
    else if (control_sequence != NULL
          && control_sequence->initiator_ == odin::ansi::CONTROL_SEQUENCE_INTRODUCER
          && control_sequence->command_   == odin::ansi::CURSOR_BACKWARD_TABULATION)
    {
        focus_previous();

        if (!has_focus())
        {
            focus_previous();
        }
        
        handled = true;
    }
    
    if (!handled)
    {
        composite_component::do_event(ev);
    }
}

}


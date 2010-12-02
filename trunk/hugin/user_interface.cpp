// ==========================================================================
// Hugin User Interface
//
// Copyright (C) 2010 Matthew Chaplain, All Rights Reserved.
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
#include "user_interface.hpp"
#include "hugin/wholist.hpp"
#include "munin/ansi/basic_container.hpp"
#include "munin/ansi/edit.hpp"
#include "munin/ansi/frame.hpp"
#include "munin/ansi/framed_component.hpp"
#include "munin/ansi/image.hpp"
#include "munin/ansi/protocol.hpp"
#include "munin/ansi/text_area.hpp"
#include "munin/aligned_layout.hpp"
#include "munin/grid_layout.hpp"
#include "munin/card.hpp"
#include "munin/compass_layout.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace odin;
using namespace boost;
using namespace std;

typedef munin::component<munin::ansi::element_type>       component;
typedef munin::container<munin::ansi::element_type>       container;
typedef munin::card<munin::ansi::element_type>            card;
typedef munin::aligned_layout<munin::ansi::element_type>  aligned_layout;
typedef munin::compass_layout<munin::ansi::element_type>  compass_layout;
typedef munin::grid_layout<munin::ansi::element_type>     grid_layout;
typedef munin::ansi::basic_container                      basic_container;
typedef munin::ansi::edit                                 edit;
typedef munin::ansi::image                                image;
typedef munin::ansi::text_area                            text_area;
typedef munin::ansi::frame                                frame;
typedef munin::ansi::framed_component                     framed_component;

namespace hugin {

static string main_image[] = {
    
    "             _"
  , "    |/ _._  | \\.__. _  _ ._/ _"
  , "    |\\(_|/_ |_/|(_|(_|(_)| |_>"
  , "                    _|"
  , " "
  , "                   ___                   ___           ___"
  , "                  / _ \\___ ________ ____/ (_)______   / _ \\"
  , "                 / ___/ _ `/ __/ _ `/ _  / / __/ -_)  \\_, /"
  , "                /_/   \\_,_/_/  \\_,_/\\_,_/_/\\__/\\__/  /___/"
  , "                                                     v1.0"
};

// ==========================================================================
// USER_INTERFACE::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct user_interface::impl
{
    // ======================================================================
    // CREATE_INTRO_SCREEN
    // ======================================================================
    shared_ptr<component> create_intro_screen()
    {
        BOOST_AUTO(inner_content, make_shared<basic_container>());
        inner_content->set_layout(make_shared<compass_layout>());

        BOOST_AUTO(image_container, make_shared<basic_container>());
        image_container->set_layout(make_shared<aligned_layout>());

        BOOST_AUTO(
            greetings_image
          , make_shared<image>(munin::ansi::image_from_text(main_image)));
        image_container->add_component(greetings_image);

        inner_content->add_component(
            image_container
          , munin::COMPASS_LAYOUT_CENTRE);

        string name_label_elements[] = { "Name: " };

        BOOST_AUTO(
            name_label
          , make_shared<image>(
                munin::ansi::image_from_text(name_label_elements)));

        BOOST_AUTO(name_container, make_shared<basic_container>());
        name_container->set_layout(make_shared<aligned_layout>());

        munin::alignment_data alignment;
        alignment.horizontal_alignment = munin::HORIZONTAL_ALIGNMENT_RIGHT;
        alignment.vertical_alignment   = munin::VERTICAL_ALIGNMENT_CENTRE;

        name_container->add_component(name_label, alignment);

        intro_name_field_ = make_shared<edit>();

        BOOST_AUTO(bottom_container, make_shared<basic_container>());
        bottom_container->set_layout(make_shared<compass_layout>());
        bottom_container->add_component(
            name_container
          , munin::COMPASS_LAYOUT_WEST);
        bottom_container->add_component(
            make_shared<framed_component>(
                make_shared<frame>()
              , intro_name_field_)
          , munin::COMPASS_LAYOUT_CENTRE);

        inner_content->add_component(
            bottom_container
          , munin::COMPASS_LAYOUT_SOUTH);

        BOOST_AUTO(outer_content, make_shared<basic_container>());
        outer_content->set_layout(make_shared<compass_layout>());
        
        statusbar_ = make_shared<edit>();
        statusbar_->set_can_focus(false);
        
        outer_content->add_component(
            inner_content
          , munin::COMPASS_LAYOUT_CENTRE);
        outer_content->add_component(
            statusbar_
          , munin::COMPASS_LAYOUT_SOUTH);
        
        return outer_content;
    }
    
    // ======================================================================
    // CREATE_MAIN_SCREEN
    // ======================================================================
    shared_ptr<component> create_main_screen()
    {
        BOOST_AUTO(content, make_shared<basic_container>());
        content->set_layout(make_shared<compass_layout>());
        
        input_field_ = make_shared<edit>();
        content->add_component(
            make_shared<framed_component>(
                make_shared<frame>()
              , input_field_)
          , munin::COMPASS_LAYOUT_SOUTH);
        
        wholist_ = make_shared<wholist>();
        content->add_component(
            wholist_
          , munin::COMPASS_LAYOUT_NORTH);
        
        output_field_ = make_shared<text_area>();
        output_field_->disable();
        content->add_component(
            make_shared<framed_component>(
                make_shared<frame>()
              , output_field_)
          , munin::COMPASS_LAYOUT_CENTRE);
        
        return content;
    }

    // ======================================================================
    // ON_USERNAME_ENTERED
    // ======================================================================
    void on_username_entered()
    {
        if (on_username_entered_)
        {
            BOOST_AUTO(document, intro_name_field_->get_document());
            BOOST_AUTO(elements, document->get_text_line(0));

            string username;

            BOOST_FOREACH(munin::ansi::element_type element, elements)
            {
                username += element.first;
            }

            on_username_entered_(username);
        }
    }

    // ======================================================================
    // ON_INPUT_ENTERED
    // ======================================================================
    void on_input_entered()
    {
        if (on_input_entered_)
        {
            BOOST_AUTO(document, input_field_->get_document());
            BOOST_AUTO(elements, document->get_text_line(0));
            BOOST_AUTO(input,   string_from_elements(elements)); 

            document->delete_text(
                make_pair(u32(0), document->get_text_size()));

            on_input_entered_(input);
        }
    }

    string                  face_name_;
    shared_ptr<card>        active_screen_;
    
    // Intro Screen components
    shared_ptr<edit>        intro_name_field_;
    shared_ptr<edit>        statusbar_;
    function<void (string)> on_username_entered_;
    
    // Main Screen components
    shared_ptr<wholist>     wholist_;
    shared_ptr<edit>        input_field_;
    shared_ptr<text_area>   output_field_;
    function<void (string)> on_input_entered_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
user_interface::user_interface()
    : munin::composite_component<munin::ansi::element_type>(
           make_shared<munin::ansi::basic_container>())
    , pimpl_(new impl)
{
    pimpl_->active_screen_ = make_shared<card>();
    pimpl_->face_name_ = hugin::FACE_INTRO;
    
    pimpl_->active_screen_->add_face(
        pimpl_->create_intro_screen(), hugin::FACE_INTRO);
    pimpl_->active_screen_->add_face(
        pimpl_->create_main_screen(), hugin::FACE_MAIN);
    
    pimpl_->active_screen_->select_face(hugin::FACE_INTRO);

    BOOST_AUTO(container, get_container());
    container->set_layout(make_shared<grid_layout>(1, 1));
    container->add_component(pimpl_->active_screen_);
}

// ==========================================================================
// ON_USERNAME_ENTERED
// ==========================================================================
void user_interface::on_username_entered(function<void (string)> callback)
{
    pimpl_->on_username_entered_ = callback;
}

// ==========================================================================
// ON_INPUT_ENTERED
// ==========================================================================
void user_interface::on_input_entered(function<void (string)> callback)
{
    pimpl_->on_input_entered_ = callback;
}

// ==========================================================================
// SELECT_FACE
// ==========================================================================
void user_interface::select_face(string const &face_name)
{
    pimpl_->active_screen_->select_face(face_name);
    pimpl_->face_name_ = face_name;
}

// ==========================================================================
// ADD_OUTPUT_TEXT
// ==========================================================================
void user_interface::add_output_text(
    runtime_array<munin::ansi::element_type> const &text)
{
    pimpl_->output_field_->get_document()->insert_text(
        text
      , pimpl_->output_field_->get_document()->get_text_size());
}

// ==========================================================================
// SET_STATUSBAR_TEXT
// ==========================================================================
void user_interface::set_statusbar_text(
    runtime_array<munin::ansi::element_type> const &text)
{
    BOOST_AUTO(document, pimpl_->statusbar_->get_document());
    document->delete_text(make_pair(u32(0), document->get_text_size()));
    document->insert_text(text);
}

// ==========================================================================
// UPDATE_WHOLIST
// ==========================================================================
void user_interface::update_wholist(runtime_array<string> const &names)
{
    pimpl_->wholist_->set_names(names);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void user_interface::do_event(any const &ev)
{
    char const *ch = any_cast<char>(&ev);
    odin::ansi::control_sequence const *control_sequence = 
        any_cast<odin::ansi::control_sequence>(&ev);

    if (pimpl_->face_name_ == hugin::FACE_INTRO 
     && pimpl_->intro_name_field_->has_focus()
     && ch != NULL)
    {
        if (*ch == '\n')
        {
            pimpl_->on_username_entered();
        }
        else if (*ch == ' ')
        {
            // Do nothing - we don't allow spaces in names.
        }
        else
        {
            munin::composite_component<munin::ansi::element_type>::do_event(ev);
        }
    }
    else if (pimpl_->face_name_ == hugin::FACE_MAIN
          && pimpl_->input_field_->has_focus()
          && ch != NULL
          && *ch == '\n')
    {
        pimpl_->on_input_entered();
    }
    else if (pimpl_->face_name_ == hugin::FACE_MAIN)
    {
        if (ch != NULL && *ch == '\t')
        {
            focus_next();
            
            if (!has_focus())
            {
                focus_next();
            }
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
        }
        else
        {
            munin::composite_component<munin::ansi::element_type>::do_event(ev);
        }
    }
    else
    {
        munin::composite_component<munin::ansi::element_type>::do_event(ev);
    }
}

}

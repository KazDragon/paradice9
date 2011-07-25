// ==========================================================================
// Hugin Intro Screen
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
#include "intro_screen.hpp"
#include "munin/ansi/protocol.hpp"
#include "munin/aligned_layout.hpp"
#include "munin/basic_container.hpp"
#include "munin/compass_layout.hpp"
#include "munin/edit.hpp"
#include "munin/framed_component.hpp"
#include "munin/grid_layout.hpp"
#include "munin/image.hpp"
#include "munin/solid_frame.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace munin::ansi;
using namespace munin;
using namespace odin;
using namespace boost;
using namespace std;

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
  , "                                                   v1.0.3"
};

// ==========================================================================
// INTRO_SCREEN::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct intro_screen::impl
{
    // ======================================================================
    // ON_ACCOUNT_DETAILS_ENTERED
    // ======================================================================
    void on_account_details_entered()
    {
        if (on_account_details_entered_)
        {
            BOOST_AUTO(document, intro_name_field_->get_document());
            BOOST_AUTO(elements, document->get_line(0));
            BOOST_AUTO(username, string_from_elements(elements));

            document = intro_password_field_->get_document();
            elements = document->get_line(0);
            
            BOOST_AUTO(password, string_from_elements(elements));
            
            on_account_details_entered_(username, password);
        }
    }

    shared_ptr<edit>                 intro_name_field_;
    shared_ptr<edit>                 intro_password_field_;
    shared_ptr<edit>                 statusbar_;
    function<void (string, string)>  on_account_details_entered_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
intro_screen::intro_screen()
    : pimpl_(make_shared<impl>())
{
    BOOST_AUTO(inner_content, make_shared<basic_container>());
    inner_content->set_layout(make_shared<compass_layout>());

    BOOST_AUTO(image_container, make_shared<basic_container>());
    image_container->set_layout(make_shared<aligned_layout>());

    BOOST_AUTO(
        greetings_image
      , make_shared<image>(elements_from_strings(main_image)));
    image_container->add_component(greetings_image);

    inner_content->add_component(image_container, COMPASS_LAYOUT_CENTRE);

    alignment_data alignment;
    alignment.horizontal_alignment = HORIZONTAL_ALIGNMENT_RIGHT;
    alignment.vertical_alignment   = VERTICAL_ALIGNMENT_CENTRE;

    BOOST_AUTO(name_container, make_shared<basic_container>());
    name_container->set_layout(make_shared<aligned_layout>());
    name_container->add_component(
        make_shared<image>(elements_from_string("Name: "))
      , alignment);

    BOOST_AUTO(password_container, make_shared<basic_container>());
    password_container->set_layout(make_shared<aligned_layout>());
    password_container->add_component(
        make_shared<image>(elements_from_string("Password: "))
      , alignment);
    
    BOOST_AUTO(labels_container, make_shared<basic_container>());
    labels_container->set_layout(make_shared<grid_layout>(2, 1));
    
    labels_container->add_component(name_container);
    labels_container->add_component(password_container);
    
    pimpl_->intro_name_field_     = make_shared<edit>();
    pimpl_->intro_password_field_ = make_shared<edit>();
    
    element_type password_element;
    password_element.glyph_ = '*';
    password_element.attribute_.foreground_colour_ =
        odin::ansi::graphics::COLOUR_RED;
    pimpl_->intro_password_field_->set_attribute(
        EDIT_PASSWORD_ELEMENT
      , password_element);
    
    BOOST_AUTO(fields_container, make_shared<basic_container>());
    fields_container->set_layout(make_shared<grid_layout>(2, 1));
    
    fields_container->add_component(make_shared<framed_component>(
        make_shared<solid_frame>()
      , pimpl_->intro_name_field_));
    fields_container->add_component(make_shared<framed_component>(
        make_shared<solid_frame>()
      , pimpl_->intro_password_field_));

    string instructions_label_elements[] = {
        "Enter details, or leave empty to create a new account. "
        "Then hit enter."
    };
    
    BOOST_AUTO(instructions_container, make_shared<basic_container>());
    instructions_container->set_layout(make_shared<aligned_layout>());
    instructions_container->add_component(
        make_shared<image>(elements_from_strings(instructions_label_elements))
      , alignment);
    
    BOOST_AUTO(bottom_container, make_shared<basic_container>());
    bottom_container->set_layout(make_shared<compass_layout>());
    bottom_container->add_component(instructions_container, COMPASS_LAYOUT_NORTH);
    bottom_container->add_component(labels_container, COMPASS_LAYOUT_WEST);
    bottom_container->add_component(fields_container, COMPASS_LAYOUT_CENTRE);

    inner_content->add_component(bottom_container, COMPASS_LAYOUT_SOUTH);

    BOOST_AUTO(outer_content, get_container());
    outer_content->set_layout(make_shared<compass_layout>());
    
    pimpl_->statusbar_ = make_shared<edit>();
    pimpl_->statusbar_->set_can_focus(false);
    
    outer_content->add_component(inner_content, COMPASS_LAYOUT_CENTRE);
    outer_content->add_component(pimpl_->statusbar_, COMPASS_LAYOUT_SOUTH);
}

// ==========================================================================
// CLEAR
// ==========================================================================
void intro_screen::clear()
{
    // Erase the text in the name field.    
    BOOST_AUTO(document, pimpl_->intro_name_field_->get_document());
    document->delete_text(make_pair(u32(0), document->get_text_size()));
    
    // Erase the text in the password field.
    document = pimpl_->intro_password_field_->get_document();
    document->delete_text(make_pair(u32(0), document->get_text_size()));

    // Erase the text on the status bar.
    document = pimpl_->statusbar_->get_document();
    document->delete_text(make_pair(u32(0), document->get_text_size()));
}

// ==========================================================================
// ON_ACCOUNT_DETAILS_ENTERED
// ==========================================================================
void intro_screen::on_account_details_entered(
    function<void (string, string)> callback)
{
    pimpl_->on_account_details_entered_ = callback;
}

// ==========================================================================
// SET_STATUSBAR_TEXT
// ==========================================================================
void intro_screen::set_statusbar_text(
    runtime_array<element_type> const &text)
{
    BOOST_AUTO(document, pimpl_->statusbar_->get_document());
    document->delete_text(make_pair(u32(0), document->get_text_size()));
    document->insert_text(text);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void intro_screen::do_event(any const &ev)
{
    bool handled = false;
    
    char const *ch = any_cast<char>(&ev);
    odin::ansi::control_sequence const *control_sequence = 
        any_cast<odin::ansi::control_sequence>(&ev);

    if (ch)
    {
        if (*ch == '\t')
        {
            focus_next();
            
            if (!has_focus())
            {
                focus_next();
            }
            
            handled = true;
        }
        else if (*ch == '\n')
        {
            pimpl_->on_account_details_entered();
            handled = true;
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
        
        handled = true;
    }
    
    if (!handled)
    {
        composite_component::do_event(ev);
    }
}

}


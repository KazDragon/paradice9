// ==========================================================================
// Hugin Account Creation Screen
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
#include "account_creation_screen.hpp"
#include "munin/ansi/protocol.hpp"
#include "munin/aligned_layout.hpp"
#include "munin/basic_container.hpp"
#include "munin/button.hpp"
#include "munin/compass_layout.hpp"
#include "munin/edit.hpp"
#include "munin/framed_component.hpp"
#include "munin/grid_layout.hpp"
#include "munin/image.hpp"
#include "munin/named_frame.hpp"
#include "munin/solid_frame.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace munin::ansi;
using namespace munin;
using namespace odin;
using namespace boost;
using namespace std;

namespace hugin {

// ==========================================================================
// ACCOUNT_CREATION_SCREEN::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct account_creation_screen::impl
{
    // ======================================================================
    // ON_ACCOUNT_CREATION_OK
    // ======================================================================
    void on_account_creation_ok()
    {
        if (on_account_created_)
        {
            BOOST_AUTO(document, name_field_->get_document());
            BOOST_AUTO(elements, document->get_line(0));
            BOOST_AUTO(name, string_from_elements(elements));
            
            document = password_field_->get_document();
            elements = document->get_line(0);
            
            BOOST_AUTO(password, string_from_elements(elements));
            
            document = password_verify_field_->get_document();
            elements = document->get_line(0);
            
            BOOST_AUTO(password_verify, string_from_elements(elements));
            
            on_account_created_(name, password, password_verify);
        }
    }
    
    // ======================================================================
    // ON_ACCOUNT_CREATION_CANCELLED
    // ======================================================================
    void on_account_creation_cancelled()
    {
        if (on_account_creation_cancelled_)
        {
            on_account_creation_cancelled_();
        }
    }
    
    // Account Creation components
    shared_ptr<edit>                        name_field_;
    shared_ptr<edit>                        password_field_;
    shared_ptr<edit>                        password_verify_field_;
    shared_ptr<edit>                        statusbar_;
    shared_ptr<button>                      ok_button_;
    shared_ptr<button>                      cancel_button_;
    function<void (string, string, string)> on_account_created_;
    function<void ()>                       on_account_creation_cancelled_;
    vector<boost::signals::connection>      connections_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
account_creation_screen::account_creation_screen()
    : pimpl_(make_shared<impl>())
{
    BOOST_AUTO(content, get_container());
    content->set_layout(make_shared<grid_layout>(1, 1));
    
    BOOST_AUTO(screen_frame, make_shared<named_frame>());
    screen_frame->set_name("ACCOUNT CREATION");
    
    // Create the Name, Password and Password (verify) labels.
    alignment_data alignment;
    alignment.horizontal_alignment = HORIZONTAL_ALIGNMENT_RIGHT;
    alignment.vertical_alignment   = VERTICAL_ALIGNMENT_CENTRE;
    
    BOOST_AUTO(name_container, make_shared<basic_container>());
    name_container->set_layout(make_shared<aligned_layout>());
    name_container->add_component(
        make_shared<image>(elements_from_string("Name: "))
      , alignment);
    
    BOOST_AUTO(password0_container, make_shared<basic_container>());
    password0_container->set_layout(make_shared<aligned_layout>());
    password0_container->add_component(
        make_shared<image>(elements_from_string("Password: "))
      , alignment);
    
    BOOST_AUTO(password1_container, make_shared<basic_container>());
    password1_container->set_layout(make_shared<aligned_layout>());
    password1_container->add_component(
        make_shared<image>(elements_from_string("Password (verify): "))
      , alignment);

    BOOST_AUTO(labels_container, make_shared<basic_container>());
    labels_container->set_layout(make_shared<grid_layout>(3, 1));
    labels_container->add_component(name_container);
    labels_container->add_component(password0_container);
    labels_container->add_component(password1_container);
    
    // Create the Name, Password, Password (verify) edit fields.
    pimpl_->name_field_            = make_shared<edit>();
    pimpl_->password_field_        = make_shared<edit>();
    pimpl_->password_verify_field_ = make_shared<edit>();
    
    element_type password_element;
    password_element.glyph_ = '*';
    password_element.attribute_.foreground_colour_ =
        odin::ansi::graphics::COLOUR_RED;
    pimpl_->password_field_->set_attribute(
        EDIT_PASSWORD_ELEMENT
      , password_element);
    pimpl_->password_verify_field_->set_attribute(
        EDIT_PASSWORD_ELEMENT
      , password_element);

    BOOST_AUTO(fields_container, make_shared<basic_container>());
    fields_container->set_layout(make_shared<grid_layout>(3, 1));
    fields_container->add_component(
        make_shared<framed_component>(
            make_shared<solid_frame>()
          , pimpl_->name_field_));
    fields_container->add_component(
        make_shared<framed_component>(
            make_shared<solid_frame>()
          , pimpl_->password_field_));
    fields_container->add_component(
        make_shared<framed_component>(
            make_shared<solid_frame>()
          , pimpl_->password_verify_field_));
    
    BOOST_AUTO(labels_fields_container, make_shared<basic_container>());
    labels_fields_container->set_layout(make_shared<compass_layout>());
    labels_fields_container->add_component(
        labels_container
      , COMPASS_LAYOUT_WEST);
    labels_fields_container->add_component(
        fields_container
      , COMPASS_LAYOUT_CENTRE);
    
    BOOST_AUTO(inner_container, make_shared<basic_container>());
    inner_container->set_layout(make_shared<compass_layout>());
    inner_container->add_component(
        labels_fields_container
      , COMPASS_LAYOUT_NORTH);

    // Create the OK and Cancel buttons.
    pimpl_->ok_button_ = make_shared<button>(
        elements_from_string("  OK  "));
    pimpl_->connections_.push_back(pimpl_->ok_button_->on_click.connect(
        bind(&impl::on_account_creation_ok, pimpl_)));
    
    pimpl_->cancel_button_ = make_shared<button>(
        elements_from_string("Cancel"));
    pimpl_->connections_.push_back(pimpl_->cancel_button_->on_click.connect(
        bind(&impl::on_account_creation_cancelled, pimpl_)));
    
    BOOST_AUTO(buttons_inner_container, make_shared<basic_container>());
    buttons_inner_container->set_layout(make_shared<grid_layout>(1, 2));
    buttons_inner_container->add_component(pimpl_->ok_button_);
    buttons_inner_container->add_component(pimpl_->cancel_button_);
    
    BOOST_AUTO(buttons_outer_container, make_shared<basic_container>());
    buttons_outer_container->set_layout(make_shared<compass_layout>());
    buttons_outer_container->add_component(
        buttons_inner_container
      , COMPASS_LAYOUT_EAST);
    
    BOOST_AUTO(inner_container2, make_shared<basic_container>());
    inner_container2->set_layout(make_shared<compass_layout>());
    inner_container2->add_component(
        inner_container
      , COMPASS_LAYOUT_NORTH);
    inner_container2->add_component(
        buttons_outer_container
      , COMPASS_LAYOUT_CENTRE);
    
    pimpl_->statusbar_ = make_shared<edit>();
    pimpl_->statusbar_->set_can_focus(false);
    
    BOOST_AUTO(inner_container3, make_shared<basic_container>());
    inner_container3->set_layout(make_shared<compass_layout>());
    inner_container3->add_component(inner_container2, COMPASS_LAYOUT_NORTH);
    inner_container3->add_component(pimpl_->statusbar_, COMPASS_LAYOUT_CENTRE);
    
    content->add_component(make_shared<framed_component>(
        screen_frame
      , inner_container3));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
account_creation_screen::~account_creation_screen()
{
    BOOST_FOREACH(boost::signals::connection &cnx, pimpl_->connections_)
    {
        cnx.disconnect();
    }
}

// ==========================================================================
// CLEAR
// ==========================================================================
void account_creation_screen::clear()
{
    BOOST_AUTO(document, pimpl_->name_field_->get_document());
    document->delete_text(make_pair(u32(0), document->get_text_size()));
    
    document = pimpl_->password_field_->get_document();
    document->delete_text(make_pair(u32(0), document->get_text_size()));
    
    document = pimpl_->password_verify_field_->get_document();
    document->delete_text(make_pair(u32(0), document->get_text_size()));
    
    document = pimpl_->statusbar_->get_document();
    document->delete_text(make_pair(u32(0), document->get_text_size()));
}

// ==========================================================================
// ON_ACCOUNT_CREATED
// ==========================================================================
void account_creation_screen::on_account_created(
    function<void (string, string, string)> callback)
{
    pimpl_->on_account_created_ = callback;
}

// ==========================================================================
// ON_ACCOUNT_CREATION_CANCELLED
// ==========================================================================
void account_creation_screen::on_account_creation_cancelled(function<void ()> callback)
{
    pimpl_->on_account_creation_cancelled_ = callback;
}

// ==========================================================================
// SET_STATUSBAR_TEXT
// ==========================================================================
void account_creation_screen::set_statusbar_text(
    runtime_array<element_type> const &text)
{
    BOOST_AUTO(document, pimpl_->statusbar_->get_document());
    document->delete_text(make_pair(u32(0), document->get_text_size()));
    document->insert_text(text);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void account_creation_screen::do_event(any const &ev)
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


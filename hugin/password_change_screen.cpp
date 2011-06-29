// ==========================================================================
// Hugin Password Change Screen
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
#include "password_change_screen.hpp"
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
// PASSWORD_CHANGE_SCREEN::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct password_change_screen::impl
{
    // ======================================================================
    // ON_PASSWORD_CHANGE_OK
    // ======================================================================
    void on_password_change_ok()
    {
        if (on_password_changed_)
        {
            BOOST_AUTO(document, old_password_field_->get_document());
            BOOST_AUTO(elements, document->get_line(0));
            BOOST_AUTO(old_password, string_from_elements(elements));
            
            document = new_password_field_->get_document();
            elements = document->get_line(0);
            
            BOOST_AUTO(new_password, string_from_elements(elements));
            
            document = new_password_verify_field_->get_document();
            elements = document->get_line(0);
            
            BOOST_AUTO(new_password_verify, string_from_elements(elements));
            
            on_password_changed_(
                old_password, new_password, new_password_verify);
        }
    }
    
    // ======================================================================
    // ON_PASSWORD_CHANGE_CANCELLED
    // ======================================================================
    void on_password_change_cancelled()
    {
        if (on_password_change_cancelled_)
        {
            on_password_change_cancelled_();
        }
    }
    
    // Password Change components
    shared_ptr<edit>                        old_password_field_;
    shared_ptr<edit>                        new_password_field_;
    shared_ptr<edit>                        new_password_verify_field_;
    shared_ptr<edit>                        statusbar_;
    shared_ptr<button>                      ok_button_;
    shared_ptr<button>                      cancel_button_;
    function<void (string, string, string)> on_password_changed_;
    function<void ()>                       on_password_change_cancelled_;
    vector<boost::signals::connection>      connections_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
password_change_screen::password_change_screen()
    : composite_component(make_shared<basic_container>())
    , pimpl_(make_shared<impl>())
{
    BOOST_AUTO(content, get_container());
    content->set_layout(make_shared<grid_layout>(1, 1));
    
    BOOST_AUTO(screen_frame, make_shared<named_frame>());
    screen_frame->set_name("PASSWORD CHANGE");
    
    // Create the Old Password, Password and Password (verify) labels.
    alignment_data alignment;
    alignment.horizontal_alignment = HORIZONTAL_ALIGNMENT_RIGHT;
    alignment.vertical_alignment   = VERTICAL_ALIGNMENT_CENTRE;
    
    BOOST_AUTO(old_password_container, make_shared<basic_container>());
    old_password_container->set_layout(make_shared<aligned_layout>());
    old_password_container->add_component(
        make_shared<image>(elements_from_string("Old Password: "))
      , alignment);
    
    BOOST_AUTO(password0_container, make_shared<basic_container>());
    password0_container->set_layout(make_shared<aligned_layout>());
    password0_container->add_component(
        make_shared<image>(elements_from_string("New Password: "))
      , alignment);
    
    BOOST_AUTO(password1_container, make_shared<basic_container>());
    password1_container->set_layout(make_shared<aligned_layout>());
    password1_container->add_component(
        make_shared<image>(elements_from_string("New Password (verify): "))
      , alignment);

    BOOST_AUTO(labels_container, make_shared<basic_container>());
    labels_container->set_layout(make_shared<grid_layout>(3, 1));
    labels_container->add_component(old_password_container);
    labels_container->add_component(password0_container);
    labels_container->add_component(password1_container);
    
    // Create the Old Password, Password, Password (verify) edit fields.
    pimpl_->old_password_field_        = make_shared<edit>();
    pimpl_->new_password_field_        = make_shared<edit>();
    pimpl_->new_password_verify_field_ = make_shared<edit>();
    
    element_type password_element;
    password_element.glyph_ = '*';
    password_element.attribute_.foreground_colour_ =
        odin::ansi::graphics::COLOUR_RED;
    pimpl_->old_password_field_->set_attribute(
        EDIT_PASSWORD_ELEMENT
      , password_element);
    pimpl_->new_password_field_->set_attribute(
        EDIT_PASSWORD_ELEMENT
      , password_element);
    pimpl_->new_password_verify_field_->set_attribute(
        EDIT_PASSWORD_ELEMENT
      , password_element);

    BOOST_AUTO(fields_container, make_shared<basic_container>());
    fields_container->set_layout(make_shared<grid_layout>(3, 1));
    fields_container->add_component(
        make_shared<framed_component>(
            make_shared<solid_frame>()
          , pimpl_->old_password_field_));
    fields_container->add_component(
        make_shared<framed_component>(
            make_shared<solid_frame>()
          , pimpl_->new_password_field_));
    fields_container->add_component(
        make_shared<framed_component>(
            make_shared<solid_frame>()
          , pimpl_->new_password_verify_field_));
    
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
        bind(&impl::on_password_change_ok, pimpl_)));
    
    pimpl_->cancel_button_ = make_shared<button>(
        elements_from_string("Cancel"));
    pimpl_->connections_.push_back(pimpl_->cancel_button_->on_click.connect(
        bind(&impl::on_password_change_cancelled, pimpl_)));
    
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
password_change_screen::~password_change_screen()
{
    BOOST_FOREACH(boost::signals::connection &cnx, pimpl_->connections_)
    {
        cnx.disconnect();
    }
}

// ==========================================================================
// CLEAR
// ==========================================================================
void password_change_screen::clear()
{
    BOOST_AUTO(document, pimpl_->old_password_field_->get_document());
    document->delete_text(make_pair(u32(0), document->get_text_size()));
    
    document = pimpl_->new_password_field_->get_document();
    document->delete_text(make_pair(u32(0), document->get_text_size()));
    
    document = pimpl_->new_password_verify_field_->get_document();
    document->delete_text(make_pair(u32(0), document->get_text_size()));
    
    document = pimpl_->statusbar_->get_document();
    document->delete_text(make_pair(u32(0), document->get_text_size()));
}

// ==========================================================================
// ON_PASSWORD_CHANGED
// ==========================================================================
void password_change_screen::on_password_changed(
    function<void (string, string, string)> callback)
{
    pimpl_->on_password_changed_ = callback;
}

// ==========================================================================
// ON_PASSWORD_CHANGE_CANCELLED
// ==========================================================================
void password_change_screen::on_password_change_cancelled(function<void ()> callback)
{
    pimpl_->on_password_change_cancelled_ = callback;
}

// ==========================================================================
// SET_STATUSBAR_TEXT
// ==========================================================================
void password_change_screen::set_statusbar_text(
    runtime_array<element_type> const &text)
{
    BOOST_AUTO(document, pimpl_->statusbar_->get_document());
    document->delete_text(make_pair(u32(0), document->get_text_size()));
    document->insert_text(text);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void password_change_screen::do_event(any const &ev)
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


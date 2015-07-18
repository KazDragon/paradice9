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
#include "munin/algorithm.hpp"
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
#include "odin/ansi/protocol.hpp"

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
            auto document = name_field_->get_document();
            auto elements = document->get_line(0);
            auto name = munin::ansi::string_from_elements(elements);
            
            document = password_field_->get_document();
            elements = document->get_line(0);
            
            auto password = munin::ansi::string_from_elements(elements);
            
            document = password_verify_field_->get_document();
            elements = document->get_line(0);
            
            auto password_verify = munin::ansi::string_from_elements(elements);
            
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
    std::shared_ptr<munin::edit>              name_field_;
    std::shared_ptr<munin::edit>              password_field_;
    std::shared_ptr<munin::edit>              password_verify_field_;
    std::shared_ptr<munin::button>            ok_button_;
    std::shared_ptr<munin::button>            cancel_button_;
    std::function<void (std::string const &, 
                        std::string const &, 
                        std::string const &)> on_account_created_;
    std::function<void ()>                    on_account_creation_cancelled_;
    std::vector<boost::signals::connection>   connections_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
account_creation_screen::account_creation_screen()
    : pimpl_(std::make_shared<impl>())
{
    auto content = get_container();
    content->set_layout(std::make_shared<munin::grid_layout>(1, 1));
    
    auto screen_frame = std::make_shared<munin::named_frame>();
    screen_frame->set_name("ACCOUNT CREATION");
    
    // Create the Name, Password and Password (verify) labels.
    munin::alignment_data alignment;
    alignment.horizontal_alignment = munin::HORIZONTAL_ALIGNMENT_RIGHT;
    alignment.vertical_alignment   = munin::VERTICAL_ALIGNMENT_CENTRE;
    
    auto name_container = std::make_shared<munin::basic_container>();
    name_container->set_layout(std::make_shared<munin::aligned_layout>());
    name_container->add_component(
        std::make_shared<munin::image>(munin::string_to_elements("Name: ")),
        alignment);
    
    auto password0_container = std::make_shared<munin::basic_container>();
    password0_container->set_layout(std::make_shared<munin::aligned_layout>());
    password0_container->add_component(
        std::make_shared<munin::image>(munin::string_to_elements("Password: "))
      , alignment);
    
    auto password1_container = std::make_shared<munin::basic_container>();
    password1_container->set_layout(std::make_shared<munin::aligned_layout>());
    password1_container->add_component(
        std::make_shared<munin::image>(
            munin::string_to_elements("Password (verify): ")),
        alignment);

    auto labels_container = std::make_shared<munin::basic_container>();
    labels_container->set_layout(std::make_shared<munin::grid_layout>(3, 1));
    labels_container->add_component(name_container);
    labels_container->add_component(password0_container);
    labels_container->add_component(password1_container);
    
    // Create the Name, Password, Password (verify) edit fields.
    pimpl_->name_field_            = std::make_shared<munin::edit>();
    pimpl_->password_field_        = std::make_shared<munin::edit>();
    pimpl_->password_verify_field_ = std::make_shared<munin::edit>();
    
    munin::element_type password_element;
    password_element.glyph_ = '*';
    password_element.attribute_.foreground_colour_ =
        odin::ansi::graphics::colour::red;
    pimpl_->password_field_->set_attribute(
        munin::EDIT_PASSWORD_ELEMENT
      , password_element);
    pimpl_->password_verify_field_->set_attribute(
        munin::EDIT_PASSWORD_ELEMENT
      , password_element);

    auto fields_container = std::make_shared<munin::basic_container>();
    fields_container->set_layout(std::make_shared<munin::grid_layout>(3, 1));
    fields_container->add_component(
        std::make_shared<munin::framed_component>(
            std::make_shared<munin::solid_frame>()
          , pimpl_->name_field_));
    fields_container->add_component(
        std::make_shared<munin::framed_component>(
            std::make_shared<munin::solid_frame>()
          , pimpl_->password_field_));
    fields_container->add_component(
        std::make_shared<munin::framed_component>(
            std::make_shared<munin::solid_frame>()
          , pimpl_->password_verify_field_));
    
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
    pimpl_->ok_button_ = std::make_shared<munin::button>(
        munin::string_to_elements("  OK  "));
    pimpl_->connections_.push_back(pimpl_->ok_button_->on_click.connect(
        [this]{pimpl_->on_account_creation_ok();}));
    
    pimpl_->cancel_button_ = std::make_shared<munin::button>(
        munin::string_to_elements("Cancel"));
    pimpl_->connections_.push_back(pimpl_->cancel_button_->on_click.connect(
        [this]{pimpl_->on_account_creation_cancelled();}));
    
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
    // TODO: I think this can be coalesced now the statusbar has gone.
    content->add_component(std::make_shared<munin::framed_component>(
        screen_frame
      , inner_container3));

    // Add a filler to ensure that the background is opaque.
    content->set_layout(
        std::make_shared<munin::grid_layout>(1, 1)
      , munin::LOWEST_LAYER);
    content->add_component(
        std::make_shared<munin::filled_box>(munin::element_type(' '))
      , {}
      , munin::LOWEST_LAYER);
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
account_creation_screen::~account_creation_screen()
{
    for (auto &cnx : pimpl_->connections_)
    {
        cnx.disconnect();
    }
}

// ==========================================================================
// CLEAR
// ==========================================================================
void account_creation_screen::clear()
{
    auto document = pimpl_->name_field_->get_document();
    document->delete_text(std::make_pair(odin::u32(0), document->get_text_size()));
    
    document = pimpl_->password_field_->get_document();
    document->delete_text(std::make_pair(odin::u32(0), document->get_text_size()));
    
    document = pimpl_->password_verify_field_->get_document();
    document->delete_text(std::make_pair(odin::u32(0), document->get_text_size()));
}

// ==========================================================================
// ON_ACCOUNT_CREATED
// ==========================================================================
void account_creation_screen::on_account_created(
    std::function<
        void (std::string const &, 
              std::string const &, 
              std::string const &)> const &callback)
{
    pimpl_->on_account_created_ = callback;
}

// ==========================================================================
// ON_ACCOUNT_CREATION_CANCELLED
// ==========================================================================
void account_creation_screen::on_account_creation_cancelled(
    std::function<void ()> const &callback)
{
    pimpl_->on_account_creation_cancelled_ = callback;
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void account_creation_screen::do_event(boost::any const &ev)
{
    bool handled = false;
    
    auto const *ch = boost::any_cast<char>(&ev);
    auto const *control_sequence = 
        boost::any_cast<odin::ansi::control_sequence>(&ev);

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


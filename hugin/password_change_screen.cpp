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
#include <terminalpp/string.hpp>
#include <terminalpp/virtual_key.hpp>

namespace hugin {

// ==========================================================================
// PASSWORD_CHANGE_SCREEN::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct password_change_screen::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(password_change_screen &self)
      : self_(self)
     {
     }

    // ======================================================================
    // ON_PASSWORD_CHANGE_OK
    // ======================================================================
    void on_password_change_ok()
    {
        auto document = old_password_field_->get_document();
        auto elements = document->get_line(0);
        auto old_password = to_string(elements);
        
        document = new_password_field_->get_document();
        elements = document->get_line(0);
        
        auto new_password = to_string(elements);
        
        document = new_password_verify_field_->get_document();
        elements = document->get_line(0);
        
        auto new_password_verify = to_string(elements);
        
        self_.on_password_changed(
            old_password, new_password, new_password_verify);
    }
    
    // ======================================================================
    // ON_PASSWORD_CHANGE_CANCELLED
    // ======================================================================
    void on_password_change_cancelled()
    {
        self_.on_password_change_cancelled();
    }

    password_change_screen                 &self_;
    
    // Password Change components
    std::shared_ptr<munin::edit>            old_password_field_;
    std::shared_ptr<munin::edit>            new_password_field_;
    std::shared_ptr<munin::edit>            new_password_verify_field_;
    std::shared_ptr<munin::button>          ok_button_;
    std::shared_ptr<munin::button>          cancel_button_;
    std::vector<boost::signals::connection> connections_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
password_change_screen::password_change_screen()
    : pimpl_(std::make_shared<impl>(std::ref(*this)))
{
    using namespace terminalpp::literals;

    auto content = get_container();
    content->set_layout(std::make_shared<munin::grid_layout>(1, 1));
    
    auto screen_frame = std::make_shared<munin::named_frame>();
    screen_frame->set_name("PASSWORD CHANGE"_ts);
    
    // Create the Old Password, Password and Password (verify) labels.
    munin::alignment_data alignment;
    alignment.horizontal_alignment = munin::HORIZONTAL_ALIGNMENT_RIGHT;
    alignment.vertical_alignment   = munin::VERTICAL_ALIGNMENT_CENTRE;
    
    auto old_password_container = std::make_shared<munin::basic_container>();
    old_password_container->set_layout(std::make_shared<munin::aligned_layout>());
    old_password_container->add_component(
        std::make_shared<munin::image>("Old Password: "_ts)
      , alignment);
    
    auto password0_container = std::make_shared<munin::basic_container>();
    password0_container->set_layout(std::make_shared<munin::aligned_layout>());
    password0_container->add_component(
        std::make_shared<munin::image>("New Password: "_ts)
      , alignment);
    
    auto password1_container = std::make_shared<munin::basic_container>();
    password1_container->set_layout(std::make_shared<munin::aligned_layout>());
    password1_container->add_component(
        std::make_shared<munin::image>("New Password (verify): "_ts)
      , alignment);

    auto labels_container = std::make_shared<munin::basic_container>();
    labels_container->set_layout(std::make_shared<munin::grid_layout>(3, 1));
    labels_container->add_component(old_password_container);
    labels_container->add_component(password0_container);
    labels_container->add_component(password1_container);
    
    // Create the Old Password, Password, Password (verify) edit fields.
    pimpl_->old_password_field_        = std::make_shared<munin::edit>();
    pimpl_->new_password_field_        = std::make_shared<munin::edit>();
    pimpl_->new_password_verify_field_ = std::make_shared<munin::edit>();
    
    terminalpp::element password_element;
    password_element.glyph_ = '*';
    password_element.attribute_.foreground_colour_ =
        terminalpp::ansi::graphics::colour::red;
    pimpl_->old_password_field_->set_attribute(
        munin::EDIT_PASSWORD_ELEMENT
      , password_element);
    pimpl_->new_password_field_->set_attribute(
        munin::EDIT_PASSWORD_ELEMENT
      , password_element);
    pimpl_->new_password_verify_field_->set_attribute(
        munin::EDIT_PASSWORD_ELEMENT
      , password_element);

    auto fields_container = std::make_shared<munin::basic_container>();
    fields_container->set_layout(std::make_shared<munin::grid_layout>(3, 1));
    fields_container->add_component(
        std::make_shared<munin::framed_component>(
            std::make_shared<munin::solid_frame>()
          , pimpl_->old_password_field_));
    fields_container->add_component(
        std::make_shared<munin::framed_component>(
            std::make_shared<munin::solid_frame>()
          , pimpl_->new_password_field_));
    fields_container->add_component(
        std::make_shared<munin::framed_component>(
            std::make_shared<munin::solid_frame>()
          , pimpl_->new_password_verify_field_));
    
    auto labels_fields_container = std::make_shared<munin::basic_container>();
    labels_fields_container->set_layout(std::make_shared<munin::compass_layout>());
    labels_fields_container->add_component(
        labels_container
        , munin::COMPASS_LAYOUT_WEST);
    labels_fields_container->add_component(
        fields_container
        , munin::COMPASS_LAYOUT_CENTRE);
    
    auto inner_container = std::make_shared<munin::basic_container>();
    inner_container->set_layout(std::make_shared<munin::compass_layout>());
    inner_container->add_component(
        labels_fields_container
        , munin::COMPASS_LAYOUT_NORTH);

    // Create the OK and Cancel buttons.
    pimpl_->ok_button_ = std::make_shared<munin::button>("  OK  "_ts);
    pimpl_->connections_.push_back(pimpl_->ok_button_->on_click.connect(
        [this]{pimpl_->on_password_change_ok();}));
    
    pimpl_->cancel_button_ = std::make_shared<munin::button>("Cancel"_ts);
    pimpl_->connections_.push_back(pimpl_->cancel_button_->on_click.connect(
        [this]{pimpl_->on_password_change_cancelled();}));
    
    auto buttons_inner_container = std::make_shared<munin::basic_container>();
    buttons_inner_container->set_layout(std::make_shared<munin::grid_layout>(1, 2));
    buttons_inner_container->add_component(pimpl_->ok_button_);
    buttons_inner_container->add_component(pimpl_->cancel_button_);
    
    auto buttons_outer_container = std::make_shared<munin::basic_container>();
    buttons_outer_container->set_layout(std::make_shared<munin::compass_layout>());
    buttons_outer_container->add_component(
        buttons_inner_container
        , munin::COMPASS_LAYOUT_EAST);
    
    auto inner_container2 = std::make_shared<munin::basic_container>();
    inner_container2->set_layout(std::make_shared<munin::compass_layout>());
    inner_container2->add_component(
        inner_container
        , munin::COMPASS_LAYOUT_NORTH);
    inner_container2->add_component(
        buttons_outer_container
        , munin::COMPASS_LAYOUT_CENTRE);
    
    auto inner_container3 = std::make_shared<munin::basic_container>();
    inner_container3->set_layout(std::make_shared<munin::compass_layout>());
    inner_container3->add_component(inner_container2, munin::COMPASS_LAYOUT_NORTH);
    // TODO: Coalesce
    
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
password_change_screen::~password_change_screen()
{
    for (auto &cnx : pimpl_->connections_)
    {
        cnx.disconnect();
    }
}

// ==========================================================================
// CLEAR
// ==========================================================================
void password_change_screen::clear()
{
    auto document = pimpl_->old_password_field_->get_document();
    document->delete_text({odin::u32(0), document->get_text_size()});
    
    document = pimpl_->new_password_field_->get_document();
    document->delete_text({odin::u32(0), document->get_text_size()});
    
    document = pimpl_->new_password_verify_field_->get_document();
    document->delete_text({odin::u32(0), document->get_text_size()});
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void password_change_screen::do_event(boost::any const &ev)
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


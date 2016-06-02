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
#include "hugin/password_change_screen.hpp"
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
#include <munin/vertical_squeeze_layout.hpp>
#include <munin/view.hpp>
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
    pimpl_->old_password_field_        = munin::make_edit();
    pimpl_->new_password_field_        = munin::make_edit();
    pimpl_->new_password_verify_field_ = munin::make_edit();
    
    auto password_element = terminalpp::element('*');
    password_element.attribute_.foreground_colour_ =
        terminalpp::ansi::graphics::colour::red;
        
    pimpl_->old_password_field_->set_attribute(
        munin::EDIT_PASSWORD_ELEMENT, password_element);
    pimpl_->new_password_field_->set_attribute(
        munin::EDIT_PASSWORD_ELEMENT, password_element);
    pimpl_->new_password_verify_field_->set_attribute(
        munin::EDIT_PASSWORD_ELEMENT, password_element);
    
    pimpl_->ok_button_ = munin::make_button("OK");
    pimpl_->connections_.push_back(pimpl_->ok_button_->on_click.connect(
        [this]{pimpl_->on_password_change_ok();}));
    
    pimpl_->cancel_button_ = munin::make_button("Cancel");
    pimpl_->connections_.push_back(pimpl_->cancel_button_->on_click.connect(
        [this]{pimpl_->on_password_change_cancelled();}));
    
    auto fields_container = munin::view(
        munin::make_compass_layout(),
        // On the west go the titles for the fields.
        munin::view(
            munin::make_grid_layout(3, 1),
            munin::view(
                munin::make_aligned_layout(),
                munin::make_image("Old Password: "), munin::alignment_hrvc),
            munin::view(
                munin::make_aligned_layout(),
                munin::make_image("New Password: "), munin::alignment_hrvc),
            munin::view(
                munin::make_aligned_layout(),
                munin::make_image("New Password (verify): "), munin::alignment_hrvc)
        ), munin::COMPASS_LAYOUT_WEST,
        // On the east go the fields themselves.
        munin::view(
            munin::make_grid_layout(3, 1),
            munin::make_framed_component(
                munin::make_solid_frame(),
                pimpl_->old_password_field_),
            munin::make_framed_component(
                munin::make_solid_frame(),
                pimpl_->new_password_field_),
            munin::make_framed_component(
                munin::make_solid_frame(),
                pimpl_->new_password_verify_field_)
        ), munin::COMPASS_LAYOUT_CENTRE);
    
    auto inner_container = munin::view(
        munin::make_compass_layout(),
        fields_container, munin::COMPASS_LAYOUT_NORTH,
        munin::view(
            munin::make_compass_layout(),
            munin::make_background_fill(), munin::COMPASS_LAYOUT_CENTRE,
            munin::view(
                munin::make_vertical_squeeze_layout(),
                pimpl_->ok_button_,
                pimpl_->cancel_button_
            ), munin::COMPASS_LAYOUT_EAST
        ), munin::COMPASS_LAYOUT_SOUTH);
            
    auto content = get_container();
    content->set_layout(munin::make_grid_layout(1, 1));
    content->add_component(
        munin::make_framed_component(
            munin::make_named_frame("PASSWORD CHANGE"),
            munin::view(
                munin::make_compass_layout(),
                inner_container, munin::COMPASS_LAYOUT_NORTH,
                munin::make_background_fill(), munin::COMPASS_LAYOUT_CENTRE
            )));

    // Add a filler to ensure that the background is opaque.
    content->set_layout(munin::make_grid_layout(1, 1), munin::LOWEST_LAYER);
    content->add_component(
        munin::make_background_fill(), {}, munin::LOWEST_LAYER);
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


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
#include "hugin/account_creation_screen.hpp"
#include <munin/algorithm.hpp>
#include <munin/aligned_layout.hpp>
#include <munin/background_fill.hpp>
#include <munin/button.hpp>
#include <munin/compass_layout.hpp>
#include <munin/edit.hpp>
#include <munin/filled_box.hpp>
#include <munin/framed_component.hpp>
#include <munin/grid_layout.hpp>
#include <munin/horizontal_strip_layout.hpp>
#include <munin/image.hpp>
#include <munin/named_frame.hpp>
#include <munin/solid_frame.hpp>
#include <munin/vertical_squeeze_layout.hpp>
#include <munin/view.hpp>
#include <terminalpp/string.hpp>
#include <terminalpp/virtual_key.hpp>

namespace hugin {

// ==========================================================================
// ACCOUNT_CREATION_SCREEN::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct account_creation_screen::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(account_creation_screen &self)
      : self_(self)
    {
    }
     
    // ======================================================================
    // ON_ACCOUNT_CREATION_OK
    // ======================================================================
    void on_account_creation_ok()
    {
        auto document = name_field_->get_document();
        auto elements = document->get_line(0);
        auto name = to_string(elements);
        
        document = password_field_->get_document();
        elements = document->get_line(0);
        
        auto password = to_string(elements);
        
        document = password_verify_field_->get_document();
        elements = document->get_line(0);
        
        auto password_verify = to_string(elements);
        
        self_.on_account_created(name, password, password_verify);
    }
    
    // ======================================================================
    // ON_ACCOUNT_CREATION_CANCELLED
    // ======================================================================
    void on_account_creation_cancelled()
    {
        self_.on_account_creation_cancelled();
    }
    
    account_creation_screen                  &self_;
    
    // Account Creation components
    std::shared_ptr<munin::edit>              name_field_;
    std::shared_ptr<munin::edit>              password_field_;
    std::shared_ptr<munin::edit>              password_verify_field_;
    std::shared_ptr<munin::button>            ok_button_;
    std::shared_ptr<munin::button>            cancel_button_;

    std::vector<boost::signals::connection>   connections_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
account_creation_screen::account_creation_screen()
    : pimpl_(std::make_shared<impl>(std::ref(*this)))
{
    auto content = get_container();
    content->set_layout(munin::make_grid_layout(1, 1));
    
    pimpl_->name_field_            = munin::make_edit();
    pimpl_->password_field_        = munin::make_edit();
    pimpl_->password_verify_field_ = munin::make_edit();
    
    auto password_element = terminalpp::element('*');
    password_element.attribute_.foreground_colour_ =
        terminalpp::ansi::graphics::colour::red;
    pimpl_->password_field_->set_attribute(
        munin::EDIT_PASSWORD_ELEMENT
      , password_element);
    pimpl_->password_verify_field_->set_attribute(
        munin::EDIT_PASSWORD_ELEMENT
      , password_element);

    pimpl_->ok_button_ = munin::make_button("OK");
    pimpl_->connections_.push_back(pimpl_->ok_button_->on_click.connect(
        [this]{pimpl_->on_account_creation_ok();}));
    
    pimpl_->cancel_button_ = munin::make_button("Cancel");
    pimpl_->connections_.push_back(pimpl_->cancel_button_->on_click.connect(
        [this]{pimpl_->on_account_creation_cancelled();}));
    
    auto fields_container = munin::view(
        munin::make_compass_layout(),
        munin::view(
            munin::make_compass_layout(),
            munin::view(
                munin::make_grid_layout(3, 1),
                munin::view(
                    munin::make_aligned_layout(),
                    munin::make_image("Name: "), 
                    munin::alignment_hrvc),
                munin::view(
                    munin::make_aligned_layout(),
                    munin::make_image("Password: "), 
                    munin::alignment_hrvc),
                munin::view(
                    munin::make_aligned_layout(),
                    munin::make_image("Password (verify): "), 
                    munin::alignment_hrvc)
            ), munin::COMPASS_LAYOUT_WEST,
            munin::view(
                munin::make_grid_layout(3, 1),
                munin::make_framed_component(
                    munin::make_solid_frame(), pimpl_->name_field_),
                munin::make_framed_component(
                    munin::make_solid_frame(), pimpl_->password_field_),
                munin::make_framed_component(
                    munin::make_solid_frame(), pimpl_->password_verify_field_)
            ), munin::COMPASS_LAYOUT_CENTRE));
    
    auto buttons_container = munin::view(
        munin::make_compass_layout(),
        munin::view(
            munin::make_vertical_squeeze_layout(),
            pimpl_->ok_button_,
            pimpl_->cancel_button_
        ), munin::COMPASS_LAYOUT_EAST);
    
    content->add_component(
        munin::make_framed_component(
            munin::make_named_frame("ACCOUNT CREATION"),
            munin::view(
                munin::make_compass_layout(),
                munin::view(
                    munin::make_horizontal_strip_layout(),
                    fields_container,
                    buttons_container
                ), munin::COMPASS_LAYOUT_NORTH)));

    // Add a filler to ensure that the background is opaque.
    content->set_layout(munin::make_grid_layout(1, 1), munin::LOWEST_LAYER);
    content->add_component(
        munin::make_background_fill(), {}, munin::LOWEST_LAYER);
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
// DO_EVENT
// ==========================================================================
void account_creation_screen::do_event(boost::any const &ev)
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


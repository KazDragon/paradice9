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
#include "hugin/intro_screen.hpp"
#include <munin/algorithm.hpp>
#include <munin/aligned_layout.hpp>
#include <munin/background_fill.hpp>
#include <munin/button.hpp>
#include <munin/compass_layout.hpp>
#include <munin/edit.hpp>
#include <munin/filled_box.hpp>
#include <munin/framed_component.hpp>
#include <munin/grid_layout.hpp>
#include <munin/image.hpp>
#include <munin/solid_frame.hpp>
#include <munin/vertical_strip_layout.hpp>
#include <munin/view.hpp>
#include <terminalpp/string.hpp>
#include <terminalpp/virtual_key.hpp>
#include <vector>

namespace hugin {

/*    
static vector<string> const main_image = list_of
  ( "             _"                                                            )
  ( "    |/ _._  | \\\\.__. _  _ ._/ _"                                         )
  ( "    |\\\\(_|/_ |_/|(_|(_|(_)| |_>"                                         )
  ( "                    _|"                                                    )
  ( " "                                                                         )
  ( "                   ___                   ___           ___"                )
  ( "                  / _ \\\\___ ________ ____/ (_)______   / _ \\\\"         )
  ( "                 / ___/ _ `/ __/ _ `/ _  / / __/ -_)  \\\\_, /"            )
  ( "                /_/   \\\\_,_/_/  \\\\_,_/\\\\_,_/_/\\\\__/\\\\__/  /___/" )
  ( "                                                   v1.1"                   )
  ;
*/  
// Attempt at a palm tree with the new colour codes:

namespace {
    
using namespace terminalpp::literals;

static std::vector<terminalpp::string> const main_image = {
 "       \\[2__ _.--..--._ _\\x                  _"_ets,
 "    \\[2.-' _/   _/\\\\_   \\\\_'-._\\x     |/ _._  | \\\\.__. _  _ ._ /_"_ets,
 "    \\[2|__ /   _/\\[3\\\\__/\\[2\\\\_   \\\\__|\\x    |\\\\(_|/_ |_/|(_|(_|(_)| |_>"_ets,
 "       \\[2|___/\\[3\\\\_\\\\__/\\[2  \\\\___|\\x                      _|"_ets,
 "              \\[3\\\\__/\\x         ___                   ___           \\i>___"_ets,
 "              \\[3\\\\__/\\x        / _ \\\\___ ________ ____/ (_)______   \\i>/ _ \\\\"_ets,
 "               \\[3\\\\__/\\x      / ___/ _ `/ __/ _ `/ _  / / __/ -_)  \\i>\\\\_, /"_ets,
 "                \\[3\\\\__/\\x    /_/   \\\\_,_/_/  \\\\_,_/\\\\_,_/_/\\\\__/\\\\__/  \\i>/___/"_ets,
 "             \\[3____\\\\__/___\\x                              v1.1"_ets,
 "       \\[3. - '             ' -."_ets,
 "      \\[3/                      \\\\"_ets,
 "\\[4~~~~~~~  ~~~~~ ~~~~~  ~~~ ~~~  ~~~~~"_ets,
 "\\[4  ~~~   ~~~~~   ~~~~   ~~ ~  ~ ~ ~~~"_ets,
};

}

// ==========================================================================
// INTRO_SCREEN::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct intro_screen::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(intro_screen &self)
        : self_(self)
    {
    }

    // ======================================================================
    // ON_LOGIN_CLICKED
    // ======================================================================
    void on_login_clicked()
    {
        auto document = intro_name_field_->get_document();
        auto elements = document->get_line(0);
        auto username = to_string(elements);

        document = intro_password_field_->get_document();
        elements = document->get_line(0);
        
        auto password = to_string(elements);
            
        self_.on_login(username, password);
    }

    intro_screen                   &self_;
    std::shared_ptr<munin::edit>    intro_name_field_;
    std::shared_ptr<munin::edit>    intro_password_field_;
    std::shared_ptr<munin::button>  login_button_;
    std::shared_ptr<munin::button>  new_account_button_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
intro_screen::intro_screen()
{
    pimpl_ = std::make_shared<impl>(boost::ref(*this));

    pimpl_->intro_name_field_     = munin::make_edit();
    pimpl_->intro_password_field_ = munin::make_edit();
    
    auto password_element = terminalpp::element('*');
    password_element.attribute_.foreground_colour_ =
        terminalpp::ansi::graphics::colour::red;

    pimpl_->intro_password_field_->set_attribute(
        munin::EDIT_PASSWORD_ELEMENT
      , password_element);
    
    pimpl_->login_button_ = munin::make_button(" Login ");
    pimpl_->login_button_->on_click.connect(
        [this]{pimpl_->on_login_clicked();});

    pimpl_->new_account_button_ = munin::make_button(" New ");
    pimpl_->new_account_button_->on_click.connect(on_new_account);
    
    auto labels_container = munin::view(
        munin::make_grid_layout(2, 1),
        munin::view(
            munin::make_aligned_layout(),
            munin::make_image("Name: "), munin::alignment_hrvc),
        munin::view(
            munin::make_aligned_layout(),
            munin::make_image("Password: "), munin::alignment_hrvc));

    auto buttons_container = munin::view(
        munin::make_compass_layout(),
        munin::make_background_fill(), munin::COMPASS_LAYOUT_CENTRE,
        munin::view(
            munin::make_vertical_strip_layout(),
            pimpl_->login_button_,
            pimpl_->new_account_button_
        ), munin::COMPASS_LAYOUT_EAST);

    auto fields_container = munin::view(
        munin::make_grid_layout(2, 1),
        munin::make_framed_component(
            munin::make_solid_frame(),
            pimpl_->intro_name_field_
        ),
        munin::make_framed_component(
            munin::make_solid_frame(),
            pimpl_->intro_password_field_
        ));

    auto bottom_container = munin::view(
        munin::make_compass_layout(),
        munin::view(
            munin::make_compass_layout(),
            labels_container, munin::COMPASS_LAYOUT_WEST,
            fields_container, munin::COMPASS_LAYOUT_CENTRE
        ), munin::COMPASS_LAYOUT_NORTH,
        buttons_container, munin::COMPASS_LAYOUT_SOUTH);

    auto content = get_container();
    content->set_layout(munin::make_compass_layout());
    content->add_component(
        munin::view(
            munin::make_aligned_layout(),
            munin::make_image(main_image), munin::alignment_hcvc), 
        munin::COMPASS_LAYOUT_CENTRE);
    content->add_component(bottom_container, munin::COMPASS_LAYOUT_SOUTH);

    // Add a filler to ensure that the background is opaque.
    content->set_layout(munin::make_grid_layout(1, 1), munin::LOWEST_LAYER);
    content->add_component(
        munin::make_background_fill(), {}, munin::LOWEST_LAYER);
}

// ==========================================================================
// CLEAR
// ==========================================================================
void intro_screen::clear()
{
    // Erase the text in the name field.    
    auto document = pimpl_->intro_name_field_->get_document();
    document->delete_text({odin::u32(0), document->get_text_size()});
    
    // Erase the text in the password field.
    document = pimpl_->intro_password_field_->get_document();
    document->delete_text({odin::u32(0), document->get_text_size()});
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void intro_screen::do_event(boost::any const &ev)
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
        else if (vk->key == terminalpp::vk::enter)
        {
            pimpl_->on_login_clicked();
        }
    }
    
    if (!handled)
    {
        composite_component::do_event(ev);
    }
}

}


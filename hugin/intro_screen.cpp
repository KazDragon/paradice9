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
#include "munin/solid_frame.hpp"
#include "odin/ansi/protocol.hpp"
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

static std::vector<std::string> const main_image = {
 "       \\[2__ _.--..--._ _\\x                  _",
 "    \\[2.-' _/   _/\\\\_   \\\\_'-._\\x     |/ _._  | \\\\.__. _  _ ._ /_",
 "    \\[2|__ /   _/\\[3\\\\__/\\[2\\\\_   \\\\__|\\x    |\\\\(_|/_ |_/|(_|(_|(_)| |_>",
 "       \\[2|___/\\[3\\\\_\\\\__/\\[2  \\\\___|\\x                      _|",
 "              \\[3\\\\__/\\x         ___                   ___           \\i>___",
 "              \\[3\\\\__/\\x        / _ \\\\___ ________ ____/ (_)______   \\i>/ _ \\\\",
 "               \\[3\\\\__/\\x      / ___/ _ `/ __/ _ `/ _  / / __/ -_)  \\i>\\\\_, /",
 "                \\[3\\\\__/\\x    /_/   \\\\_,_/_/  \\\\_,_/\\\\_,_/_/\\\\__/\\\\__/  \\i>/___/",
 "             \\[3____\\\\__/___\\x                              v1.1",
 "       \\[3. - '             ' -.",
 "      \\[3/                      \\\\",
 "\\[4~~~~~~~  ~~~~~ ~~~~~  ~~~ ~~~  ~~~~~",
 "\\[4  ~~~   ~~~~~   ~~~~   ~~ ~  ~ ~ ~~~",
};

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
        auto username = munin::ansi::string_from_elements(elements);

        document = intro_password_field_->get_document();
        elements = document->get_line(0);
        
        auto password = munin::ansi::string_from_elements(elements);
            
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

    auto content = get_container();
    content->set_layout(std::make_shared<munin::compass_layout>());

    auto image_container = std::make_shared<munin::basic_container>();
    image_container->set_layout(std::make_shared<munin::aligned_layout>());

    auto greetings_image = std::make_shared<munin::image>(
        munin::strings_to_elements(main_image));
    image_container->add_component(greetings_image);

    content->add_component(image_container, munin::COMPASS_LAYOUT_CENTRE);

    munin::alignment_data alignment;
    alignment.horizontal_alignment = munin::HORIZONTAL_ALIGNMENT_RIGHT;
    alignment.vertical_alignment   = munin::VERTICAL_ALIGNMENT_CENTRE;

    auto name_container = std::make_shared<munin::basic_container>();
    name_container->set_layout(std::make_shared<munin::aligned_layout>());
    name_container->add_component(
        std::make_shared<munin::image>(munin::string_to_elements("Name: "))
      , alignment);

    auto password_container = std::make_shared<munin::basic_container>();
    password_container->set_layout(std::make_shared<munin::aligned_layout>());
    password_container->add_component(
        std::make_shared<munin::image>(munin::string_to_elements("Password: "))
      , alignment);
    
    auto labels_container = std::make_shared<munin::basic_container>();
    labels_container->set_layout(std::make_shared<munin::grid_layout>(2, 1));
    
    labels_container->add_component(name_container);
    labels_container->add_component(password_container);
    
    pimpl_->intro_name_field_     = std::make_shared<munin::edit>();
    pimpl_->intro_password_field_ = std::make_shared<munin::edit>();
    
    munin::element_type password_element;
    password_element.glyph_ = '*';
    password_element.attribute_.foreground_colour_ =
        odin::ansi::graphics::colour::red;
    pimpl_->intro_password_field_->set_attribute(
        munin::EDIT_PASSWORD_ELEMENT
      , password_element);
    
    pimpl_->login_button_ = std::make_shared<munin::button>(
        munin::string_to_elements(" Login "));
    pimpl_->login_button_->on_click.connect(
        [this]{pimpl_->on_login_clicked();});

    pimpl_->new_account_button_ = std::make_shared<munin::button>(
        munin::string_to_elements(" New "));
    pimpl_->new_account_button_->on_click.connect(on_new_account);

    auto buttons_container = std::make_shared<munin::basic_container>();
    buttons_container->set_layout(std::make_shared<munin::compass_layout>());
    buttons_container->add_component(
        pimpl_->login_button_, munin::COMPASS_LAYOUT_WEST);
    buttons_container->add_component(
        pimpl_->new_account_button_, munin::COMPASS_LAYOUT_EAST);

    auto outer_buttons_container = std::make_shared<munin::basic_container>();
    outer_buttons_container->set_layout(std::make_shared<munin::compass_layout>());
    outer_buttons_container->add_component(
        std::make_shared<munin::filled_box>(munin::element_type(' ')), 
        munin::COMPASS_LAYOUT_CENTRE);
    outer_buttons_container->add_component(
        buttons_container, munin::COMPASS_LAYOUT_EAST);

    auto fields_container = std::make_shared<munin::basic_container>();
    fields_container->set_layout(std::make_shared<munin::grid_layout>(2, 1));
    
    fields_container->add_component(std::make_shared<munin::framed_component>(
        std::make_shared<munin::solid_frame>()
      , pimpl_->intro_name_field_));
    fields_container->add_component(std::make_shared<munin::framed_component>(
        std::make_shared<munin::solid_frame>()
      , pimpl_->intro_password_field_));

    auto bottom_container = std::make_shared<munin::basic_container>();
    bottom_container->set_layout(std::make_shared<munin::compass_layout>());
    bottom_container->add_component(labels_container, munin::COMPASS_LAYOUT_WEST);
    bottom_container->add_component(fields_container, munin::COMPASS_LAYOUT_CENTRE);

    auto outer_container = std::make_shared<munin::basic_container>();
    outer_container->set_layout(std::make_shared<munin::compass_layout>());
    outer_container->add_component(bottom_container, munin::COMPASS_LAYOUT_NORTH);
    outer_container->add_component(outer_buttons_container, munin::COMPASS_LAYOUT_SOUTH);

    content->add_component(outer_container, munin::COMPASS_LAYOUT_SOUTH);

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


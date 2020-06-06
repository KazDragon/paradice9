// ==========================================================================
// Paradice Title Page
//
// Copyright (C) 2020 Matthew Chaplain, All Rights Reserved.
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
#include "paradice/ui/title_page.hpp"
#include <munin/aligned_layout.hpp>
#include <munin/button.hpp>
#include <munin/compass_layout.hpp>
#include <munin/edit.hpp>
#include <munin/filled_box.hpp>
#include <munin/framed_component.hpp>
#include <munin/grid_layout.hpp>
#include <munin/image.hpp>
#include <munin/render_surface.hpp>
#include <munin/titled_frame.hpp>
#include <munin/vertical_strip_layout.hpp>
#include <munin/view.hpp>
#include <terminalpp/algorithm/for_each_in_region.hpp>

using namespace terminalpp::literals;

namespace paradice { namespace ui {

namespace {

class password_field : public munin::edit
{
private:
    void do_draw(
        munin::render_surface &surface,
        terminalpp::rectangle const &region) const
    {
        auto const content_size = get_text().size();
        
        terminalpp::for_each_in_region(
            surface,
            region,
            [=](terminalpp::element &elem,
                terminalpp::coordinate_type column,
                terminalpp::coordinate_type row)
            {
                static constexpr auto password_element = terminalpp::element {
                    '*',
                    terminalpp::attribute { 
                        terminalpp::ansi::graphics::colour::red 
                    }
                };

                static constexpr auto fill_element = terminalpp::element {
                    ' ' 
                };

                elem = column < content_size ? password_element : fill_element;
            });
    }
};

std::shared_ptr<password_field> make_password_field()
{
    return std::make_shared<password_field>();
}

std::vector<terminalpp::string> const main_image = {
 "       \\[2__ _.--..--._ _\\x                  _"_ets,
 "    \\[2.-' _/   _/\\\\_   \\\\_'-._\\x     |/ _._  | \\\\.__. _  _ ._ /_"_ets,
 "    \\[2|__ /   _/\\[3\\\\__/\\[2\\\\_   \\\\__|\\x    |\\\\(_|/_ |_/|(_|(_|(_)| |_>"_ets,
 "       \\[2|___/\\[3\\\\_\\\\__/\\[2  \\\\___|\\x                      _|"_ets,
 "              \\[3\\\\__/\\x         ___                   ___           \\i>___"_ets,
 "              \\[3\\\\__/\\x        / _ \\\\___ ________ ____/ (_)______   \\i>/ _ \\\\"_ets,
 "               \\[3\\\\__/\\x      / ___/ _ `/ __/ _ `/ _  / / __/ -_)  \\i>\\\\_, /"_ets,
 "                \\[3\\\\__/\\x    /_/   \\\\_,_/_/  \\\\_,_/\\\\_,_/_/\\\\__/\\\\__/  \\i>/___/"_ets,
 "             \\[3____\\\\__/___\\x                              v2.0"_ets,
 "       \\[3. - '             ' -."_ets,
 "      \\[3/                      \\\\"_ets,
 "\\[4~~~~~~~  ~~~~~ ~~~~~  ~~~ ~~~  ~~~~~"_ets,
 "\\[4  ~~~   ~~~~~   ~~~~   ~~ ~  ~ ~ ~~~"_ets,
};

const terminalpp::attribute highlight_attribute = []{
    terminalpp::attribute highlight_attribute;
    highlight_attribute.foreground_colour_ = terminalpp::high_colour{4, 5, 1};
    return highlight_attribute;
}();

}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
title_page::title_page()
  : test_(munin::make_image()),
    new_button_(munin::make_button(" New ")),
    login_button_(munin::make_button(" Login "))
{
    new_button_->on_click.connect([this]{ test_->set_content("New!"); });
    login_button_->on_click.connect([this]{ test_->set_content("Login!"); });
    
    auto const name_edit = munin::make_edit();
    auto const password_edit = make_password_field();

    auto const name_frame = munin::make_titled_frame("Name");
    auto const password_frame = munin::make_titled_frame("Password");

    auto const fields_container = munin::view(
        munin::make_grid_layout({1, 2}),
        munin::make_framed_component(name_frame, name_edit),
        munin::make_framed_component(password_frame, password_edit));

    auto const buttons_container = munin::view(
        munin::make_compass_layout(),
        //munin::make_fill(' '),
        test_,
        munin::compass_layout::heading::centre,
        munin::view(
            munin::make_vertical_strip_layout(),
            new_button_,
            login_button_),
        munin::compass_layout::heading::east);

    auto const lower_section = munin::view(
        munin::make_compass_layout(),
        fields_container,
        munin::compass_layout::heading::north,
        buttons_container,
        munin::compass_layout::heading::south);

    set_layout(munin::make_grid_layout({1, 1}));
    add_component(
        munin::view(
            munin::make_compass_layout(),
            munin::make_image(main_image),
            munin::compass_layout::heading::centre,
            lower_section, 
            munin::compass_layout::heading::south));

    name_edit->set_focus();
}

}}

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
#include "paradice/ui/detail/password_edit.hpp"
#include <munin/aligned_layout.hpp>
#include <munin/brush.hpp>
#include <munin/button.hpp>
#include <munin/compass_layout.hpp>
#include <munin/edit.hpp>
#include <munin/filled_box.hpp>
#include <munin/framed_component.hpp>
#include <munin/grid_layout.hpp>
#include <munin/image.hpp>
#include <munin/render_surface.hpp>
#include <munin/scroll_pane.hpp>
#include <munin/titled_frame.hpp>
#include <munin/vertical_strip_layout.hpp>
#include <munin/view.hpp>
#include <terminalpp/algorithm/for_each_in_region.hpp>

using namespace terminalpp::literals;

namespace paradice { namespace ui {

namespace {

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

}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
title_page::title_page()
  : new_button_(munin::make_button(" New ")),
    login_button_(munin::make_button(" Login "))
{
    auto const name_edit = munin::make_edit();
    auto const password_edit = detail::make_password_edit();

    new_button_->on_click.connect(on_new_account);
    login_button_->on_click.connect(
        [=]
        { 
            auto const &account_name = 
                terminalpp::to_string(name_edit->get_text());
            auto const &password =
                terminalpp::to_string(password_edit->get_text());
            
            on_account_login(account_name, encrypt(password));

        });

    auto const labels_container = munin::view(
        munin::make_grid_layout({1, 2}),
        munin::view(
            munin::make_aligned_layout(),
            munin::make_fill(' '),
            munin::alignment_fill,
            munin::make_image("Name"),
            munin::alignment_hrvc),
        munin::view(
            munin::make_aligned_layout(),
            munin::make_fill(' '),
            munin::alignment_fill,
            munin::make_image("Password"),
            munin::alignment_hrvc));

    auto const edits_container = munin::view(
        munin::make_grid_layout({1, 2}),
        munin::make_scroll_pane(name_edit),
        munin::make_scroll_pane(password_edit));

    auto const fields_container = munin::view(
        munin::make_compass_layout(),
        labels_container,
        munin::compass_layout::heading::west,
        edits_container,
        munin::compass_layout::heading::centre);

    auto const buttons_container = munin::view(
        munin::make_compass_layout(),
        munin::make_fill(' '),
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

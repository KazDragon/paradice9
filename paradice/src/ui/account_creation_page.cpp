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
#include "paradice/ui/account_creation_page.hpp"
#include "paradice/ui/detail/password_edit.hpp"
#include <munin/aligned_layout.hpp>
#include <munin/button.hpp>
#include <munin/compass_layout.hpp>
#include <munin/edit.hpp>
#include <munin/filled_box.hpp>
#include <munin/framed_component.hpp>
#include <munin/grid_layout.hpp>
#include <munin/image.hpp>
#include <munin/render_surface.hpp>
#include <munin/solid_frame.hpp>
#include <munin/titled_frame.hpp>
#include <munin/vertical_strip_layout.hpp>
#include <munin/view.hpp>
#include <munin/viewport.hpp>
#include <terminalpp/algorithm/for_each_in_region.hpp>

using namespace terminalpp::literals;

namespace paradice { namespace ui {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
account_creation_page::account_creation_page()
{
    set_layout(munin::make_grid_layout({1, 1}));

    auto name_edit = munin::make_edit();
    auto password_edit = detail::make_password_edit();
    auto repeat_password_edit = detail::make_password_edit();

    auto fields = munin::view(
        munin::make_grid_layout({1, 3}),
        munin::make_framed_component(
            munin::make_titled_frame("Name"),
            name_edit),
        munin::make_framed_component(
            munin::make_titled_frame("Password"),
            password_edit),
        munin::make_framed_component(
            munin::make_titled_frame("Password (Repeat)"),
            repeat_password_edit));

    auto return_button = munin::make_button("Return");
    auto next_button = munin::make_button("Next");

    return_button->on_click.connect(on_return);
    next_button->on_click.connect(
        [=]
        {
            on_next(
                terminalpp::to_string(name_edit->get_text()),
                terminalpp::to_string(password_edit->get_text()));
        });

    auto buttons = munin::view(
        munin::make_compass_layout(),
        return_button, munin::compass_layout::heading::west,
        munin::make_fill(' '), munin::compass_layout::heading::centre,
        next_button, munin::compass_layout::heading::east);

    add_component(munin::view(
        munin::make_grid_layout({1, 1}),
        munin::make_framed_component(
            munin::make_titled_frame("Create New Account"),
            munin::view(
                munin::make_compass_layout(),
                fields, munin::compass_layout::heading::north,
                munin::make_fill(' '), munin::compass_layout::heading::centre,
                buttons, munin::compass_layout::heading::south))));

    name_edit->set_focus();
}

}}

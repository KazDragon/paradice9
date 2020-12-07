// ==========================================================================
// Paradice Character Selection Page
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
#include "paradice/ui/character_selection_page.hpp"
#include "paradice/ui/detail/password_edit.hpp"
#include <munin/aligned_layout.hpp>
#include <munin/button.hpp>
#include <munin/compass_layout.hpp>
#include <munin/edit.hpp>
#include <munin/filled_box.hpp>
#include <munin/framed_component.hpp>
#include <munin/grid_layout.hpp>
#include <munin/image.hpp>
#include <munin/list.hpp>
#include <munin/render_surface.hpp>
#include <munin/solid_frame.hpp>
#include <munin/titled_frame.hpp>
#include <munin/vertical_strip_layout.hpp>
#include <munin/view.hpp>
#include <munin/viewport.hpp>
#include <terminalpp/algorithm/for_each_in_region.hpp>
#include <terminalpp/string.hpp>
#include <boost/make_unique.hpp>

using namespace terminalpp::literals;

namespace paradice { namespace ui {

// ==========================================================================
// CHARACTER_SELECTION_PAGE::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct character_selection_page::impl
{
    std::shared_ptr<munin::list> character_list_ { munin::make_list() };
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
character_selection_page::character_selection_page()
  : pimpl_(boost::make_unique<impl>())
{
    using namespace terminalpp::literals;

    pimpl_->character_list_->set_items({
        "Character 0"_ts,
        "Character 1"_ts,
        "Character 2"_ts,
        "Character 3"_ts,
        "Character 4"_ts,
        "Character 5"_ts,
        "Character 6"_ts,
        "Character 7"_ts,
        "Character 8"_ts,
        "Character 9"_ts,
        "Character 10"_ts,
        "Character 11"_ts,
        "Character 12"_ts,
        "Character 13"_ts,
        "Character 14"_ts,
        "Character 15"_ts,
        "Character 16"_ts,
        "Character 17"_ts,
        "Character 18"_ts,
        "Character 19"_ts,
        "Character 20"_ts,
        "Character 21"_ts,
        "Character 22"_ts,
        "Character 23"_ts,
        "Character 24"_ts,
        "Character 25"_ts,
        "Character 26"_ts,
        "Character 27"_ts,
        "Character 28"_ts,
        "Character 29"_ts,
        "Character 30"_ts,
        "Character 31"_ts,
        "Character 32"_ts
    });

    auto new_button = munin::make_button(" New ");
    auto select_button = munin::make_button(" Select ");

    auto const buttons_container = munin::view(
        munin::make_compass_layout(),
        munin::make_fill(' '),
        munin::compass_layout::heading::centre,
        munin::view(
            munin::make_vertical_strip_layout(),
            new_button,
            select_button),
        munin::compass_layout::heading::east);

    set_layout(munin::make_compass_layout());
    add_component(
        munin::make_framed_component(
            munin::make_titled_frame("Select Your Character"),
            munin::view(
                munin::make_compass_layout(),
                munin::make_viewport(pimpl_->character_list_))),
        munin::compass_layout::heading::centre);
    add_component(
        buttons_container,
        munin::compass_layout::heading::south);

    pimpl_->character_list_->set_focus();
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
character_selection_page::~character_selection_page() = default;

}}

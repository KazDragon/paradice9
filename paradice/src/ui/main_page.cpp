// ==========================================================================
// Paradice Main Page
//
// Copyright (C) 2021 Matthew Chaplain, All Rights Reserved.
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
#include "paradice/ui/main_page.hpp"
#include <munin/compass_layout.hpp>
#include <munin/edit.hpp>
#include <munin/framed_component.hpp>
#include <munin/image.hpp>
#include <munin/solid_frame.hpp>
#include <munin/titled_frame.hpp>
#include <munin/text_area.hpp>
#include <munin/view.hpp>
#include <munin/viewport.hpp>
#include <boost/make_unique.hpp>

using namespace terminalpp::literals;

namespace paradice { namespace ui { 

struct main_page::impl
{
};

main_page::main_page()
  : pimpl_(boost::make_unique<impl>())
{
    set_layout(munin::make_compass_layout());

    add_component(
        munin::make_framed_component(
            munin::make_titled_frame("Currently Playing"_ts),
            munin::make_image("You"_ts)),
        munin::compass_layout::heading::north);

    auto text_area = munin::make_text_area();
    text_area->insert_text("Welcome to Paradice!\n");

    add_component(
        munin::make_framed_component(
            munin::make_solid_frame(),
            munin::make_viewport(text_area)),
        munin::compass_layout::heading::centre);

    add_component(
        munin::make_framed_component(
            munin::make_solid_frame(),
            munin::make_viewport(munin::make_edit())),
        munin::compass_layout::heading::south);
}

main_page::~main_page() = default;

}}
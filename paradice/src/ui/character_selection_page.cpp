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
character_selection_page::character_selection_page()
{
    set_layout(munin::make_grid_layout({1, 1}));
    add_component(munin::make_framed_component(
        munin::make_titled_frame("Select Your Character"),
        munin::view(
            munin::make_compass_layout(),
            munin::make_fill(' '))));
}

}}

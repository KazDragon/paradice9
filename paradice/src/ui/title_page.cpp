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
#include <munin/grid_layout.hpp>
#include <munin/image.hpp>

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
{
    set_layout(munin::make_grid_layout({1, 1}));
    add_component(munin::make_image(main_image));
}

}}

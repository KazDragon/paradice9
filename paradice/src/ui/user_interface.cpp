// ==========================================================================
// Paradice User Interface
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
#include "paradice/ui/user_interface.hpp"
#include <munin/brush.hpp>
#include <munin/grid_layout.hpp>
#include <boost/make_unique.hpp>

namespace paradice { namespace ui {

// ==========================================================================
// USER_INTERFACE::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct user_interface::impl
{
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
user_interface::user_interface()
  : pimpl_(boost::make_unique<impl>())
{
    using namespace terminalpp::literals;

    set_layout(munin::make_grid_layout({1, 1}));
    add_component(munin::make_brush({
        { "\\[1!\\[2?"_ets },
        { "\\[3?\\[4?"_ets }
    }));
};

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
user_interface::~user_interface() = default;

}}

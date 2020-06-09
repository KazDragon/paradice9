// ==========================================================================
// Paradice Password Edit
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
#include "paradice/ui/detail/password_edit.hpp"
#include <munin/render_surface.hpp>
#include <terminalpp/algorithm/for_each_in_region.hpp>

namespace paradice { namespace ui { namespace detail { 

// ==========================================================================
// DO_DRAW
// ==========================================================================
void password_edit::do_draw(
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

// ==========================================================================
// MAKE_PASSWORD_EDIT
// ==========================================================================
std::shared_ptr<password_edit> make_password_edit()
{
    return std::make_shared<password_edit>();
}


}}}

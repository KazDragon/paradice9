// ==========================================================================
// Munin Button.
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
#include "munin/button.hpp"
#include "munin/aligned_layout.hpp"
#include "munin/background_fill.hpp"
#include "munin/container.hpp"
#include "munin/framed_component.hpp"
#include "munin/grid_layout.hpp"
#include "munin/image.hpp"
#include "munin/solid_frame.hpp"
#include "munin/view.hpp"
#include <terminalpp/ansi/mouse.hpp>
#include <terminalpp/virtual_key.hpp>

namespace munin {

// ==========================================================================
// BUTTON::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct button::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl()
    {
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
button::button(terminalpp::string const &caption)
    : pimpl_(std::make_shared<impl>())
{
    auto image = munin::make_image(caption);
    image->set_can_focus(true);
    
    auto content = get_container();
    content->set_layout(munin::make_grid_layout(1, 1));
    content->add_component(
        munin::make_framed_component(
            munin::make_solid_frame(),
            munin::view(
                munin::make_aligned_layout(),
                image, munin::alignment_hcvc)));
    content->set_layout(munin::make_grid_layout(1, 1), munin::LOWEST_LAYER);
    content->add_component(
        munin::make_background_fill(), {}, munin::LOWEST_LAYER);
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
button::~button()
{
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void button::do_event(boost::any const &event)
{
    auto vk = boost::any_cast<terminalpp::virtual_key>(&event);
    
    if (vk)
    {
        if (vk->key == terminalpp::vk::space
         || vk->key == terminalpp::vk::enter)
        {
            on_click();
        }
    }
    
    auto report = boost::any_cast<terminalpp::ansi::mouse::report>(&event);
    
    if (report
     && report->button_ == terminalpp::ansi::mouse::report::LEFT_BUTTON_DOWN)
    {
        on_click();
    }
}

// ==========================================================================
// MAKE_BUTTON
// ==========================================================================
std::shared_ptr<button> make_button(terminalpp::string const &caption)
{
    return std::make_shared<button>(caption);
}


}


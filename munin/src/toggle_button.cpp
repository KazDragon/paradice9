// ==========================================================================
// Munin Toggle Button.
//
// Copyright (C) 2012 Matthew Chaplain, All Rights Reserved.
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
#include "munin/toggle_button.hpp"
#include "munin/container.hpp"
#include "munin/framed_component.hpp"
#include "munin/grid_layout.hpp"
#include "munin/image.hpp"
#include "munin/solid_frame.hpp"
#include <terminalpp/ansi/mouse.hpp>
#include <terminalpp/string.hpp>
#include <terminalpp/virtual_key.hpp>

namespace munin {

// ==========================================================================
// TOGGLE_BUTTON::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct toggle_button::impl
{
    std::shared_ptr<image> image_;
    bool                   state_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
toggle_button::toggle_button(bool default_state)
  : pimpl_(std::make_shared<impl>())
{
    pimpl_->image_ = make_image(default_state ? "X" : " ");
    pimpl_->image_->set_can_focus(true);

    pimpl_->state_ = default_state;

    get_container()->set_layout(make_grid_layout(1, 1));

    get_container()->add_component(
        make_framed_component(make_solid_frame(), pimpl_->image_));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
toggle_button::~toggle_button()
{
}

// ==========================================================================
// SET_TOGGLE
// ==========================================================================
void toggle_button::set_toggle(bool toggle)
{
    using namespace terminalpp::literals;
    
    if (toggle != pimpl_->state_)
    {
        pimpl_->state_ = toggle;

        pimpl_->image_->set_image(toggle ? "X" : " ");
    }
}

// ==========================================================================
// GET_TOGGLE
// ==========================================================================
bool toggle_button::get_toggle() const
{
    return pimpl_->state_;
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void toggle_button::do_event(boost::any const &event)
{
    auto vk = boost::any_cast<terminalpp::virtual_key>(&event);
    
    if (vk)
    {
        if (vk->key == terminalpp::vk::space
         || vk->key == terminalpp::vk::enter)
        {
            set_toggle(!get_toggle());
            set_focus();
        }
    }
    
    auto report = boost::any_cast<terminalpp::ansi::mouse::report>(&event);
    
    if (report
     && report->button_ == terminalpp::ansi::mouse::report::LEFT_BUTTON_DOWN)
    {
        set_toggle(!get_toggle());
        set_focus();
    }
}

}


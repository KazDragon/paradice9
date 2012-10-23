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
#include "munin/framed_component.hpp"
#include "munin/grid_layout.hpp"
#include "munin/image.hpp"
#include "munin/solid_frame.hpp"
#include "munin/ansi/protocol.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace odin;
using namespace boost;
using namespace std;

namespace munin {

// ==========================================================================
// TOGGLE_BUTTON::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct toggle_button::impl
{
    shared_ptr<image> image_;
    bool              state_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
toggle_button::toggle_button(bool default_state)
    : pimpl_(make_shared<impl>())
{
    pimpl_->image_ = make_shared<image>(munin::ansi::elements_from_string(
        default_state ? "X" : " "));
    pimpl_->image_->set_can_focus(true);

    pimpl_->state_ = default_state;


    get_container()->set_layout(
        make_shared<grid_layout>(1, 1));
    
    get_container()->add_component(
        make_shared<framed_component>(
            make_shared<solid_frame>()
          , pimpl_->image_));
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
    if (toggle != pimpl_->state_)
    {
        pimpl_->state_ = toggle;

        pimpl_->image_->set_image(
            munin::ansi::elements_from_string(toggle ? "X" : " "));
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
void toggle_button::do_event(any const &event)
{
    BOOST_AUTO(ch, any_cast<char>(&event));
    
    if (ch != NULL)
    {
        if (*ch == ' ' || *ch == '\n')
        {
            set_toggle(!get_toggle());
            set_focus();
        }
    }

    BOOST_AUTO(report, any_cast<odin::ansi::mouse_report>(&event));
        
    if (report != NULL)
    {
        if (report->button_ == 0)
        {
            set_toggle(!get_toggle());
            set_focus();
        }
    }
}

}


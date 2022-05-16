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
#include "paradice/ui/command_prompt.hpp"
#include "paradice/ui/message.hpp"
#include <munin/compass_layout.hpp>
#include <munin/edit.hpp>
#include <munin/framed_component.hpp>
#include <munin/image.hpp>
#include <munin/scroll_pane.hpp>
#include <munin/titled_frame.hpp>
#include <munin/text_area.hpp>
#include <munin/view.hpp>
#include <boost/make_unique.hpp>

using namespace terminalpp::literals;

namespace paradice { namespace ui { 

struct main_page::impl
{
    std::shared_ptr<munin::image> who_list_ { munin::make_image("You"_ts) };
    std::shared_ptr<munin::text_area> text_area_ { munin::make_text_area() };
    std::shared_ptr<command_prompt> command_prompt_ { make_command_prompt() };
};

main_page::main_page()
  : pimpl_(boost::make_unique<impl>())
{
    set_layout(munin::make_compass_layout());

    add_component(
        munin::make_framed_component(
            munin::make_titled_frame("Currently Playing"_ts),
            pimpl_->who_list_),
        munin::compass_layout::heading::north);

    add_component(
        munin::make_scroll_pane(
            pimpl_->text_area_,
            munin::make_vertical_viewport_resize_strategy()),
        munin::compass_layout::heading::centre);

    add_component(
        munin::make_scroll_pane(pimpl_->command_prompt_),
        munin::compass_layout::heading::south);

    pimpl_->command_prompt_->on_command.connect(on_command);
    pimpl_->command_prompt_->set_focus();
}

main_page::~main_page() = default;

// ==========================================================================
// DO_EVENT
// ==========================================================================
void main_page::do_event(boost::any const &ev)
{
    auto *msg = boost::any_cast<message>(&ev);

    if (msg)
    {
        pimpl_->text_area_->insert_text(msg->content);
        pimpl_->text_area_->insert_text("\n\n");
    }
    else
    {
        composite_component::do_event(ev);
    }
}

}}
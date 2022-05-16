// ==========================================================================
// Paradice Command Prompt
//
// Copyright (C) 2022 Matthew Chaplain, All Rights Reserved.
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
#include "paradice/ui/command_prompt.hpp"
#include <munin/grid_layout.hpp>
#include <munin/edit.hpp>
#include <terminalpp/virtual_key.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/make_unique.hpp>

namespace paradice { namespace ui {

// ==========================================================================
// COMMAND_PROMPT::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct command_prompt::impl
{
    impl(command_prompt &self)
      : self_{self},
        edit_{munin::make_edit()}
    {
        buffer_.push_back("");
    }

    // ======================================================================
    // EVENT
    // ======================================================================
    bool event(boost::any const &ev)
    {
        bool handled = false;

        auto *vk_event = boost::any_cast<terminalpp::virtual_key>(&ev);

        if (vk_event)
        {
            if (vk_event->key == terminalpp::vk::enter)
            {
                auto command = edit_->get_text();
                buffer_.back() = command;
                buffer_.push_back("");
                current_index_ = buffer_.size() - 1;
                edit_->set_text("");

                self_.on_command(terminalpp::to_string(command));
                return true;
            }
            else if (vk_event->key == terminalpp::vk::cursor_up)
            {
                if (current_index_ > 0)
                {
                    edit_->set_text(buffer_[--current_index_]);
                }

                return true;
            }
            else if (vk_event->key == terminalpp::vk::cursor_down)
            {
                if (current_index_ < buffer_.size() - 1)
                {
                    edit_->set_text(buffer_[++current_index_]);
                }

                return true;
            }
        }

        return handled;
    }

    command_prompt &self_;
    std::shared_ptr<munin::edit> edit_;
    boost::circular_buffer<terminalpp::string> buffer_{11};
    int current_index_ = 0;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
command_prompt::command_prompt()
  : pimpl_{boost::make_unique<impl>(*this)}
{
    set_layout(munin::make_grid_layout({1, 1}));
    add_component(pimpl_->edit_);
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
command_prompt::~command_prompt() = default;

// ==========================================================================
// DO_EVENT
// ==========================================================================
void command_prompt::do_event(boost::any const &event)
{
    if (!pimpl_->event(event))
    {
        composite_component::do_event(event);
    }
}

// ==========================================================================
// MAKE_COMMAND_PROMPT
// ==========================================================================
std::shared_ptr<command_prompt> make_command_prompt()
{
    return std::make_shared<command_prompt>();
}

}}

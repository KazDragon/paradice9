// ==========================================================================
// Hugin Command Prompt
//
// Copyright (C) 2010 Matthew Chaplain, All Rights Reserved.
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
#include "hugin/command_prompt.hpp"
#include <munin/basic_container.hpp>
#include <munin/compass_layout.hpp>
#include <munin/edit.hpp>
#include <terminalpp/string.hpp> 
#include <terminalpp/virtual_key.hpp>
#include <boost/optional.hpp>
#include <algorithm>
#include <deque>

BOOST_STATIC_CONSTANT(std::uint32_t, MAX_HISTORY = 50);

namespace hugin {

// ==========================================================================
// COMMAND_PROMPT::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct command_prompt::impl
{
    std::shared_ptr<munin::edit>     edit_ = munin::make_edit();

    std::uint32_t                        current_history_;
    std::deque<std::string>          history_;

    // Text to remember while running through the history.
    terminalpp::string               current_text_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
command_prompt::command_prompt()
  : pimpl_(std::make_shared<impl>())
{
    get_container()->set_layout(munin::make_compass_layout());
    get_container()->add_component(
        pimpl_->edit_, munin::COMPASS_LAYOUT_SOUTH);
}

// ==========================================================================
// GET_DOCUMENT
// ==========================================================================
std::shared_ptr<munin::text::document> command_prompt::get_document()
{
    return pimpl_->edit_->get_document();
}

// ==========================================================================
// ADD_HISTORY
// ==========================================================================
void command_prompt::add_history(std::string const &history)
{
    pimpl_->history_.push_front(history);

    if (pimpl_->history_.size() > MAX_HISTORY)
    {
        pimpl_->history_.pop_back();
    }

    pimpl_->current_history_ = 0;
}

// ==========================================================================
// CLEAR_HISTORY
// ==========================================================================
void command_prompt::clear_history()
{
    pimpl_->history_.clear();
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void command_prompt::do_event(boost::any const &ev)
{
    auto const *vk = boost::any_cast<terminalpp::virtual_key>(&ev);
    bool handled = false;

    if (vk)
    {
        if (vk->key == terminalpp::vk::cursor_up)
        {
            if (!pimpl_->history_.empty())
            {
                std::uint32_t index = (std::min)(
                    pimpl_->current_history_ + vk->repeat_count
                  , std::uint32_t(pimpl_->history_.size()));

                if (index != pimpl_->current_history_)
                {
                    auto document = get_document();

                    if (pimpl_->current_history_ == 0)
                    {
                        // Remember our current text in case we come back.
                        pimpl_->current_text_ = document->get_line(0);
                    }

                    pimpl_->current_history_ = index;
                    
                    document->delete_text(
                        {0, document->get_text_size()});

                    document->insert_text(pimpl_->history_[index - 1]);
                }
            }
            
            handled = true;
        }
        else if (vk->key == terminalpp::vk::cursor_down)
        {
            if (!pimpl_->history_.empty())
            {
                std::uint32_t index = vk->repeat_count > pimpl_->current_history_
                          ? 0
                          : pimpl_->current_history_ - vk->repeat_count;

                if (index != pimpl_->current_history_)
                {
                    auto document = get_document();

                    document->delete_text(
                        {0, document->get_text_size()});

                    pimpl_->current_history_ = index;

                    if (pimpl_->current_history_ == 0)
                    {
                        document->insert_text(pimpl_->current_text_);
                    }
                    else
                    {
                        document->insert_text(pimpl_->history_[index - 1]);
                    }
                }
            }

            handled = true;
        }
    }
    
    if (!handled)
    {
        composite_component::do_event(ev);
    }
}

}


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
#include "command_prompt.hpp"
#include "munin/basic_container.hpp"
#include "munin/compass_layout.hpp"
#include "munin/edit.hpp"
#include "munin/ansi/protocol.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>
#include <boost/typeof/typeof.hpp>
#include <algorithm>
#include <deque>

using namespace munin;
using namespace odin;
using namespace boost;
using namespace std;

BOOST_STATIC_CONSTANT(u32, MAX_HISTORY = 50);

namespace hugin {

// ==========================================================================
// COMMAND_PROMPT::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct command_prompt::impl
{
    shared_ptr<edit>            edit_;

    u32                         current_history_;
    deque<string>               history_;

    // Text to remember while running through the history.
    runtime_array<element_type> current_text_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
command_prompt::command_prompt()
    : composite_component(make_shared<basic_container>())
    ,  pimpl_(new impl)
{
    pimpl_->edit_ = make_shared<edit>();

    get_container()->set_layout(make_shared<compass_layout>());
    get_container()->add_component(
        pimpl_->edit_, munin::COMPASS_LAYOUT_SOUTH);
}

// ==========================================================================
// GET_DOCUMENT
// ==========================================================================
shared_ptr<munin::text::document> command_prompt::get_document()
{
    return pimpl_->edit_->get_document();
}

// ==========================================================================
// ADD_HISTORY
// ==========================================================================
void command_prompt::add_history(string const &history)
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
void command_prompt::do_event(any const &ev)
{
    odin::ansi::control_sequence const *sequence = 
        any_cast<odin::ansi::control_sequence>(&ev);

    if (sequence != NULL)
    {
        if (sequence->initiator_ == odin::ansi::CONTROL_SEQUENCE_INTRODUCER
         && sequence->command_ == odin::ansi::CURSOR_UP)
        {
            u32 amount = sequence->arguments_.empty()
                       ? 1
                       : atoi(sequence->arguments_.c_str());

            u32 index = (min)(
                pimpl_->current_history_ + amount
              , u32(pimpl_->history_.size()));

            if (index != pimpl_->current_history_)
            {
                BOOST_AUTO(document, get_document());

                if (pimpl_->current_history_ == 0)
                {
                    // Remember our current text in case we come back.
                    pimpl_->current_text_ = document->get_line(0);
                }

                pimpl_->current_history_ = index;
                
                document->delete_text(
                    make_pair(0, document->get_text_size()));

                document->insert_text(munin::ansi::elements_from_string(
                    pimpl_->history_[index - 1]));
            }
        }
        else if (sequence->initiator_ == odin::ansi::CONTROL_SEQUENCE_INTRODUCER
            && sequence->command_ == odin::ansi::CURSOR_DOWN)
        {
            u32 amount = sequence->arguments_.empty()
                       ? 1
                       : atoi(sequence->arguments_.c_str());

            u32 index = amount > pimpl_->current_history_
                      ? 0
                      : pimpl_->current_history_ - amount;

            if (index != pimpl_->current_history_)
            {
                BOOST_AUTO(document, get_document());

                document->delete_text(
                    make_pair(0, document->get_text_size()));

                pimpl_->current_history_ = index;

                if (pimpl_->current_history_ == 0)
                {
                    document->insert_text(pimpl_->current_text_);
                }
                else
                {
                    document->insert_text(
                        munin::ansi::elements_from_string(
                            pimpl_->history_[index - 1]));
                }
            }
        }
        else
        {
            composite_component::do_event(ev);
        }
    }
    else
    {
        composite_component::do_event(ev);
    }
}

}


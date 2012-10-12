// ==========================================================================
// Odin ANSI Parser.
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
#include "odin/ansi/ansi_parser.hpp"
#include "odin/ansi/protocol.hpp"
#include "odin/state_machine.hpp"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

using namespace boost;
using namespace odin;
using namespace std;

namespace odin { namespace ansi {

enum state
{
    state_normal
  , state_escape
  , state_initiator
  , state_mouse_button
  , state_mouse_x
  , state_mouse_y
};

// ==========================================================================
// PARSER::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct parser::impl
    : public state_machine<state, char>
{
    impl()
        : state_machine(state_normal)
        , ready_(false)
    {
        state[state_normal].undefined_transition           = bind(&impl::normal_undefined, this, _1);
        state[state_normal].transition[odin::ansi::ESCAPE] = bind(&impl::normal_escape, this);

        state[state_escape].undefined_transition           = bind(&impl::escape_undefined, this, _1);
        state[state_escape].transition[odin::ansi::ESCAPE] = bind(&impl::escape_escape, this);

        state[state_initiator].undefined_transition        = bind(&impl::initiator_undefined, this, _1);
        state[state_initiator].transition['M']             = bind(&impl::initiator_m, this);

        state[state_mouse_button].undefined_transition     = bind(&impl::mouse_button, this, _1);
        state[state_mouse_x].undefined_transition          = bind(&impl::mouse_x, this, _1);
        state[state_mouse_y].undefined_transition          = bind(&impl::mouse_y, this, _1);
    }

    void normal_undefined(char ch)
    {
        token_ = ch;
        ready_ = true;
    }

    void normal_escape()
    {
        set_state(state_escape);
        sequence_.arguments_ = "";
        sequence_.meta_      = false;
    }

    void escape_undefined(char ch)
    {
        sequence_.initiator_ = ch;
        set_state(state_initiator);
    }

    void escape_escape()
    {
        sequence_.meta_ = true;
    }

    void initiator_undefined(char ch)
    {
        if ((ch >= '0' && ch <= '9') || ch == odin::ansi::PARAMETER_SEPARATOR)
        {
            sequence_.arguments_ += ch;
        }
        else
        {
            sequence_.command_ = ch;
            token_ = sequence_;
            ready_ = true;
            set_state(state_normal);
        }
    }

    void initiator_m()
    {
        set_state(state_mouse_button);
    }

    void mouse_button(char ch)
    {
        mouse_report_.button_ = ch;
        set_state(state_mouse_x);
    }

    void mouse_x(char ch)
    {
        mouse_report_.x_position_ = ch;
        set_state(state_mouse_y);
    }

    void mouse_y(char ch)
    {
        mouse_report_.y_position_ = ch;
        token_ = mouse_report_;
        ready_ = true;
        set_state(state_normal);
    }

    bool                         ready_;
    odin::ansi::control_sequence sequence_;
    odin::ansi::mouse_report     mouse_report_;
    parser::token_type           token_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
parser::parser()
    : pimpl_(make_shared<impl>())
{
}

// ==========================================================================
// READY
// ==========================================================================
bool parser::ready() const
{
    return pimpl_->ready_;
}

// ==========================================================================
// CLEAR_TOKEN
// ==========================================================================
void parser::clear_token()
{
    pimpl_->ready_ = false;
}
    
// ==========================================================================
// STORE_ELEMENT
// ==========================================================================
void parser::store_element(element_type const &element)
{
    (*pimpl_)(element);
}
    
// ==========================================================================
// STORED_TOKEN
// ==========================================================================
parser::token_type const &parser::stored_token() const
{
    return pimpl_->token_;
}

}}

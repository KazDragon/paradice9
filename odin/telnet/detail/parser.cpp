// ==========================================================================
// Odin Telnet Parser.
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
#include "odin/telnet/detail/parser.hpp"
#include "odin/state_machine.hpp"
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>

using namespace odin;
using namespace boost;
using namespace std;

namespace odin { namespace telnet { namespace detail {

enum state
{
    state_normal
  , state_iac
  , state_negotiation
  , state_subnegotiation_type
  , state_subnegotiation_data
  , state_subnegotiation_iac
};

// ==========================================================================
// PARSER::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct parser::impl
    : public state_machine<state, odin::u8>
{
    impl()
        : state_machine(state_normal)
        , ready_(false)
    {
        state[state_normal].undefined_transition          = bind(&impl::normal_undefined, this, _1);
        state[state_normal].transition[odin::telnet::IAC] = bind(&impl::normal_iac, this);

        state[state_iac].undefined_transition             = bind(&impl::iac_undefined, this, _1);
        state[state_iac].transition[odin::telnet::IAC]    = bind(&impl::iac_iac, this);
        state[state_iac].transition[odin::telnet::WILL]   = bind(&impl::iac_negotiation, this, odin::telnet::WILL);
        state[state_iac].transition[odin::telnet::WONT]   = bind(&impl::iac_negotiation, this, odin::telnet::WONT);
        state[state_iac].transition[odin::telnet::DO]     = bind(&impl::iac_negotiation, this, odin::telnet::DO);
        state[state_iac].transition[odin::telnet::DONT]   = bind(&impl::iac_negotiation, this, odin::telnet::DONT);
        state[state_iac].transition[odin::telnet::SB]     = bind(&impl::iac_sb, this);

        state[state_negotiation].undefined_transition     = bind(&impl::negotiation_undefined, this, _1);

        state[state_subnegotiation_type].undefined_transition = bind(&impl::subnegotiation_type_undefined, this, _1);

        state[state_subnegotiation_data].undefined_transition = bind(&impl::subnegotiation_data_undefined, this, _1);
        state[state_subnegotiation_data].transition[odin::telnet::IAC] = bind(&impl::subnegotiation_data_iac, this);

        state[state_subnegotiation_iac].undefined_transition           = bind(&impl::subnegotiation_iac_undefined, this, _1);
        state[state_subnegotiation_iac].transition[odin::telnet::SE]   = bind(&impl::subnegotiation_iac_se, this);
    }

    void normal_undefined(odin::u8 element)
    {
        result_ = char(element);
        ready_  = true;
    }

    void normal_iac()
    {
        set_state(state_iac);
    }

    void iac_undefined(odin::u8 element)
    {
        result_ = command(element);
        ready_  = true;
        set_state(state_normal);
    }

    void iac_iac()
    {
        result_ = char(odin::telnet::IAC);
        ready_  = true;
        set_state(state_normal);
    }

    void iac_negotiation(odin::u8 element)
    {
        negotiation_.request_ = element;
        set_state(state_negotiation);
    }

    void iac_sb()
    {
        set_state(state_subnegotiation_type);
    }

    void negotiation_undefined(odin::u8 element)
    {
        negotiation_.option_id_ = element;
        result_                 = negotiation_;
        ready_                  = true;
        set_state(state_normal);
    }

    void subnegotiation_type_undefined(odin::u8 element)
    {
        subnegotiation_.option_id_ = element;
        subnegotiation_.content_.clear();
        set_state(state_subnegotiation_data);
    }

    void subnegotiation_data_undefined(odin::u8 element)
    {
        subnegotiation_.content_.push_back(element);
    }

    void subnegotiation_data_iac()
    {
        set_state(state_subnegotiation_iac);
    }

    void subnegotiation_iac_undefined(odin::u8 element)
    {
        subnegotiation_.content_.push_back(element);
        set_state(state_subnegotiation_data);
    }

    void subnegotiation_iac_se()
    {
        result_ = subnegotiation_;
        ready_  = true;
        set_state(state_normal);
    }

    bool                              ready_;
    odin::telnet::negotiation_type    negotiation_;
    odin::telnet::subnegotiation_type subnegotiation_;
    optional<parser::token_type>      result_;
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
    pimpl_->result_.reset();
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
    return pimpl_->result_.get();
}

}}}

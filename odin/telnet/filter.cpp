// ==========================================================================
// Odin Telnet Filter.
//
// Copyright (C) 2003 Matthew Chaplain, All Rights Reserved.
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
#include "odin/telnet/filter.hpp"
#include "odin/state_machine.hpp"
#include <boost/bind.hpp>
#include <algorithm>
#include <vector>

using namespace std;
using namespace boost;

namespace odin { namespace telnet { 

enum states
{
    normal
  , iac
  , wwdd
  , sb
  , sb_main
  , sb_end
};

class filter::impl
    : public odin::state_machine<states, odin::u8>
{
    typedef odin::state_machine<states, odin::u8> parent_type;
    
public :
    impl()
        : parent_type(normal)
        , filtered_(true)
    {
        state[normal].transition[IAC]       = bind(&impl::normal_iac,   this);
        state[normal].undefined_transition  = bind(&impl::normal_undef, this, _1);

        state[iac].transition[IAC]          = bind(&impl::iac_iac, this);
        state[iac].transition[WILL]         = bind(&impl::iac_will, this);
        state[iac].transition[WONT]         = bind(&impl::iac_wont, this);        
        state[iac].transition[DO]           = bind(&impl::iac_do, this);        
        state[iac].transition[DONT]         = bind(&impl::iac_dont, this);
        state[iac].transition[SB]           = bind(&impl::iac_sb, this);        
        state[iac].undefined_transition     = bind(&impl::iac_undef, this, _1);
        
        state[wwdd].undefined_transition    = bind(&impl::wwdd_undef, this, _1);
        
        state[sb].undefined_transition      = bind(&impl::sb_undef, this, _1);

        state[sb_main].transition[IAC]      = bind(&impl::sb_main_iac, this);
        state[sb_main].undefined_transition = bind(&impl::sb_main_undef, this, _1);
        
        state[sb_end].transition[SE]        = bind(&impl::sb_end_se, this);
        state[sb_end].undefined_transition  = bind(&impl::sb_end_undef, this, _1);
    }
    
    bool was_filtered() const
    {
        return filtered_;
    }
    
    filter::command_callback        on_command_;
    filter::negotiation_callback    on_negotiation_;
    filter::subnegotiation_callback on_subnegotiation_;
    
private :
    void normal_iac()
    {
        filtered_ = true;
        set_state(iac);
    }
    
    void normal_undef(odin::u8 /*value*/)
    {
        filtered_ = false;
    }
    
    void iac_iac()
    {
        filtered_ = false;
        set_state(normal);
    }
    
    void iac_will()
    {
        filtered_ = true;
        request_  = WILL;
        set_state(wwdd);
    }
    
    void iac_wont()
    {
        filtered_ = true;
        request_  = WONT;
        set_state(wwdd);
    }

    void iac_do()
    {
        filtered_ = true;
        request_  = DO;
        set_state(wwdd);
    }
    
    void iac_dont()
    {
        filtered_ = true;
        request_  = DONT;
        set_state(wwdd);
    }
    
    void iac_sb()
    {
        filtered_ = true;
        set_state(sb);
    }

    void iac_undef(odin::u8 value)
    {
        filtered_ = true;
        set_state(normal);
        
        if (on_command_)
        {
            on_command_(odin::telnet::command(value));
        }
    }
    
    void wwdd_undef(odin::u8 value)
    {
        filtered_ = true;
        set_state(normal);
        
        if (on_negotiation_)
        {
            on_negotiation_(request_, value);
        }
    }
    
    void sb_undef(odin::u8 value)
    {
        filtered_ = true;
        subnegotiation_id_ = value;
        subnegotiation_.clear();
        set_state(sb_main);
    }

    void sb_main_iac()
    {
        filtered_ = true;
        set_state(sb_end);
    }
    
    void sb_main_undef(odin::u8 value)
    {
        filtered_ = true;
        subnegotiation_.push_back(value);
        set_state(sb_main);
    }

    void sb_end_se()
    {
        filtered_ = true;
        set_state(normal);
        
        if (on_subnegotiation_)
        {
            subnegotiation_type subnegotiation(subnegotiation_.size());
            copy(
                subnegotiation_.begin()
              , subnegotiation_.end()
              , subnegotiation.begin());
            
            on_subnegotiation_(subnegotiation_id_, subnegotiation);
        }
    }
    
    void sb_end_undef(odin::u8 value)
    {
        filtered_ = true;
        subnegotiation_.push_back(value);
        set_state(sb_main);
    }
    
    bool                   filtered_;
    negotiation_request    request_;
    subnegotiation_id_type subnegotiation_id_;
    vector<odin::u8>       subnegotiation_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
filter::filter()
    : pimpl_(new impl)
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
filter::~filter()
{
}

// ==========================================================================
// ON_COMMAND
// ==========================================================================
void filter::on_command(command_callback const &callback)
{
    pimpl_->on_command_ = callback;
}

// ==========================================================================
// ON_NEGOTIATION
// ==========================================================================
void filter::on_negotiation(negotiation_callback const &callback)
{
    pimpl_->on_negotiation_ = callback;
}

// ==========================================================================
// ON_SUBNEGOTIATION
// ==========================================================================
void filter::on_subnegotiation(subnegotiation_callback const &callback)
{
    pimpl_->on_subnegotiation_ = callback;
}

// ==========================================================================
// DO_FILTER
// ==========================================================================
bool filter::do_filter(argument_type ch)
{
    (*pimpl_)(ch);
    
    return !pimpl_->was_filtered();
}

}}

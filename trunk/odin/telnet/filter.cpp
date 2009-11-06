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
#include <vector>

namespace odin { namespace telnet { 

enum state_t
{
    normal
  , iac
  , option
  , sub_type
  , sub_main
  , sub_may_end
};

struct filter::impl : public odin::state_machine<impl, state_t, unsigned char>
{
    typedef odin::state_machine<impl, state_t, unsigned char> parent_t;

    enum { consumed = -1 };
    
    impl()
        : parent_t(normal)
        , last_parsed_(consumed)
    {
        state[normal].transition[IAC]           = &impl::normal_iac;
        state[normal].undefined_transition      = &impl::normal_char;
        
        state[iac].transition[IAC]              = &impl::iac_iac;
        state[iac].transition[WILL]             = &impl::iac_will;
        state[iac].transition[WONT]             = &impl::iac_wont;
        state[iac].transition[DO]               = &impl::iac_do;
        state[iac].transition[DONT]             = &impl::iac_dont;
        state[iac].transition[SB]               = &impl::iac_sb;
        state[iac].undefined_transition         = &impl::command_received;
        
        state[option].undefined_transition      = &impl::option_received;
        
        state[sub_type].undefined_transition    = &impl::sb_type;
        
        state[sub_main].transition[IAC]         = &impl::sb_iac;
        state[sub_main].undefined_transition    = &impl::sb;
        
        state[sub_may_end].transition[SE]       = &impl::sbme_se;
        state[sub_may_end].transition[IAC]      = &impl::sbme_iac;
        state[sub_may_end].undefined_transition = &impl::sbme_unk;
    }
    
    bool do_parse(char ch)
    {
        state_transition((unsigned char)(ch));
        return last_parsed_ != impl::consumed;
    }
    
    int                        last_parsed_;
    subnegotiation_type        subnegotiation_type_;
    std::vector<unsigned char> subnegotiation_;
    command                    option_type_;

    filter::command_callback        command_callback_;
    filter::negotiation_callback    negotiation_callback_;
    filter::subnegotiation_callback subnegotiation_callback_;

    void normal_iac()
    {
        last_parsed_ = consumed;
        set_state(iac);
    }
    
    void normal_char(unsigned char ch)
    {
        last_parsed_ = ch;
    }
    
    void iac_iac()
    {
        last_parsed_ = IAC;
        set_state(normal);
    }
    
    void iac_will()
    {
        option_type_ = WILL;
        set_state(option);
    }
    
    void iac_wont()
    {
        option_type_ = WONT;
        set_state(option);
    }
    
    void iac_do()
    {
        option_type_ = DO;
        set_state(option);
    }
    
    void iac_dont()
    {
        option_type_ = DONT;
        set_state(option);
    }
    
    void iac_sb()
    {
        subnegotiation_.clear();
        set_state(sub_type);
    }
    
    void command_received(unsigned char ch)
    {
        if(command_callback_ != NULL)
        {
            command_callback_(command(ch));
        }
        
        set_state(normal);
    }
    
    void option_received(unsigned char ch)
    {
        if(negotiation_callback_ != NULL)
        {
            negotiation_callback_(ch, option_type_);
        }
        
        set_state(normal);
    }
    
    void sb_type(unsigned char ch)
    {
        subnegotiation_type_ = ch;
        set_state(sub_main);
    }
    
    void sb_iac()
    {
        set_state(sub_may_end);
    }
    
    void sb(unsigned char ch)
    {
        subnegotiation_.push_back(ch);
    }
    
    void sbme_se()
    {
        if(subnegotiation_callback_ != NULL)
        {
            subnegotiation_callback_(subnegotiation_type_
                                   , &*subnegotiation_.begin()
                                   , subnegotiation_.size());
        }
        
        set_state(normal);
    }
    
    void sbme_iac()
    {
        subnegotiation_.push_back(IAC);
        set_state(option);
    }
    
    void sbme_unk(unsigned char ch)
    {
        // Erroneous input; ignore it.
        set_state(sub_main);
    }
};

// ==========================================================================
// FILTER::CONSTRUCTOR
// ==========================================================================
filter::filter()
    : pimpl_(new impl)
{
}

// ==========================================================================
// FILTER::DESTRUCTOR
// ==========================================================================
filter::~filter()
{
    delete pimpl_;
}

// ==========================================================================
// FILTER::DO_PARSE
// ==========================================================================
bool filter::do_parse(argument_type ch)
{
    return pimpl_->do_parse(ch);
}

// ==========================================================================
// FILTER::ON_COMMAND
// ==========================================================================
void filter::on_command(command_callback const &callback)
{
    pimpl_->command_callback_ = callback;
}

// ==========================================================================
// FILTER::ON_OPTION
// ==========================================================================
void filter::on_negotiation(negotiation_callback const &callback)
{
    pimpl_->negotiation_callback_ = callback;
}

// ==========================================================================
// FILTER::ON_SUBNEGOTIATION
// ==========================================================================
void filter::on_subnegotiation(subnegotiation_callback const &callback)
{
    pimpl_->subnegotiation_callback_ = callback;
}

}}




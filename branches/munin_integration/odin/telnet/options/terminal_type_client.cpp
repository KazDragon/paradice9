// ==========================================================================
// Odin Telnet Terminal Type Client Option
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
#include "odin/telnet/options/terminal_type_client.hpp"
#include <algorithm>
#include <string>
#include <vector>

namespace odin { namespace telnet { namespace options {

// ==========================================================================
// TERMINAL_TYPE_CLIENT::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct terminal_type_client::impl
{
    boost::shared_ptr<odin::telnet::stream> stream_;
    
    boost::function<void ()> on_terminal_type_added_;
    boost::function<void ()> on_terminal_types_complete_;
    boost::function<void ()> on_terminal_type_set_;
    
    bool                     detecting_types_;
    std::vector<std::string> detected_types_;

    unsigned int             current_type_;

    bool                     setting_type_;
    bool                     setting_type_pending_;
    std::string              setting_type_to_;

    void request_send()
    {
        odin::u8 const subnegotiation[] =
        {
            odin::telnet::TERMINAL_TYPE_SEND
        };
        
        stream_->send_subnegotiation(
            odin::telnet::TERMINAL_TYPE, subnegotiation);
    }
};

// ==========================================================================
// TERMINAL_TYPE_CLIENT::CONSTRUCTOR
// ==========================================================================
terminal_type_client::terminal_type_client(
    boost::shared_ptr<odin::telnet::stream> stream)
    : client_option(stream)
    , pimpl_(new impl)
{
    pimpl_->stream_          = stream;
    
    pimpl_->detecting_types_      = false;
    pimpl_->current_type_         = 0;
    pimpl_->setting_type_         = false;
    pimpl_->setting_type_pending_ = false;
}

// ==========================================================================
// TERMINAL_TYPE_CLIENT::DESTRUCTOR
// ==========================================================================
terminal_type_client::~terminal_type_client()
{
    delete pimpl_;
}


// ==========================================================================
// TERMINAL_TYPE_CLIENT::ON_SUBNEGOTIATION
// ==========================================================================
void terminal_type_client::on_subnegotiation(
    subnegotiation_type const &subnegotiation)
{
    if (subnegotiation.size() < 2
     || subnegotiation[0] != odin::telnet::TERMINAL_TYPE_IS)
    {
        return;
    }
    
    std::string detected_type(
        subnegotiation.begin() + 1
      , subnegotiation.end());
    
    if(pimpl_->detecting_types_)
    {
        if(pimpl_->detected_types_.empty()
        || pimpl_->detected_types_.back() != detected_type)
        {
            pimpl_->detected_types_.push_back(detected_type);
            pimpl_->current_type_ = pimpl_->detected_types_.size() - 1;
            
            if(pimpl_->on_terminal_type_added_)
            {
                pimpl_->on_terminal_type_added_();
            }
            
            if(pimpl_->on_terminal_type_set_)
            {
                pimpl_->on_terminal_type_set_();
            }
            
            pimpl_->request_send();
        }
        else
        {
            pimpl_->detecting_types_ = false;
                
            if(pimpl_->setting_type_)
            {
                pimpl_->request_send();
            }

            if(pimpl_->on_terminal_types_complete_)
            {
                pimpl_->on_terminal_types_complete_();
            }
        }
    }
    else if(pimpl_->setting_type_)
    {
        bool found = false;
        for(unsigned int type = 0; type < pimpl_->detected_types_.size(); ++type)
        {
            if(pimpl_->detected_types_[type] == detected_type)
            {
                pimpl_->current_type_ = type;
                found = true;
            }
        }
        
        if(detected_type == pimpl_->setting_type_to_)
        {
            pimpl_->setting_type_ = false;
            
            if(pimpl_->on_terminal_type_set_)
            {
                pimpl_->on_terminal_type_set_();
            }
        }
        else
        {
            if(!found)
            {
                pimpl_->detected_types_.push_back(detected_type);
                pimpl_->current_type_ = pimpl_->detected_types_.size() - 1;
                
                if(pimpl_->on_terminal_type_added_)
                {
                    pimpl_->on_terminal_type_added_();
                }
            }

            if(detected_type == get_current_terminal_type())
            {
                // Broken client.
                if(pimpl_->on_terminal_type_set_)
                {
                    pimpl_->on_terminal_type_set_();
                }
            }
            else
            {
                pimpl_->request_send();
            }
        }
    }
}

// ==========================================================================
// TERMINAL_TYPE_CLIENT::GET_NEGOTIATION_TYPE
// ==========================================================================
negotiation_type terminal_type_client::get_negotiation_type() const
{
    return odin::telnet::TERMINAL_TYPE;
}
        
// ==========================================================================
// TERMINAL_TYPE_CLIENT::ON_TERMINAL_TYPE_ADDED
// ==========================================================================
void terminal_type_client::on_terminal_type_added(boost::function<void ()> fn)
{
    pimpl_->on_terminal_type_added_ = fn;
}
        
// ==========================================================================
// TERMINAL_TYPE_CLIENT::ON_TERMINAL_TYPES_COMPLETE
// ==========================================================================
void terminal_type_client::on_terminal_types_complete(boost::function<void ()> fn)
{
    pimpl_->on_terminal_types_complete_ = fn;
}

// ==========================================================================
// TERMINAL_TYPE_CLIENT::ON_TERMINAL_TYPE_SET
// ==========================================================================
void terminal_type_client::on_terminal_type_set(boost::function<void ()> fn)
{
    pimpl_->on_terminal_type_set_ = fn;
}

// ==========================================================================
// TERMINAL_TYPE_CLIENT::DETECT_TERMINAL_TYPES
// ==========================================================================
void terminal_type_client::detect_terminal_types()
{
    if(!pimpl_->detecting_types_ && is_active())
    {
        pimpl_->detecting_types_ = true;
        pimpl_->request_send();
    }
}
        
// ==========================================================================
// TERMINAL_TYPE_CLIENT::GET_NUMBER_OF_TERMINAL_TYPES
// ==========================================================================
unsigned int terminal_type_client::get_number_of_terminal_types() const
{
    return pimpl_->detected_types_.size();
}

// ==========================================================================
// TERMINAL_TYPE_CLIENT::GET_TERMINAL_TYPE
// ==========================================================================
std::string terminal_type_client::get_terminal_type(unsigned int index)
{
    if(index < pimpl_->detected_types_.size())
    {
        return pimpl_->detected_types_[index];
    }
    else
    {
        return "Unknown";
    }
}
        
// ==========================================================================
// TERMINAL_TYPE_CLIENT::SET_CURRENT_TERMINAL_TYPE
// ==========================================================================
void terminal_type_client::set_current_terminal_type(std::string type)
{
    if(is_active())
    {
        if(std::find(
            pimpl_->detected_types_.begin()
          , pimpl_->detected_types_.end()
          , type) != pimpl_->detected_types_.end())
        {
            if(pimpl_->detecting_types_ || pimpl_->setting_type_)
            {
                pimpl_->setting_type_pending_ = true;
                pimpl_->setting_type_to_      = type;
            }
            else if(pimpl_->detected_types_[pimpl_->current_type_] == type)
            {
                if(pimpl_->on_terminal_type_set_)
                {
                    pimpl_->on_terminal_type_set_();
                }
            }
            else
            {
                pimpl_->setting_type_    = true;
                pimpl_->setting_type_to_ = type;
                pimpl_->request_send();
            }
        }
    }
}
        
// ==========================================================================
// TERMINAL_TYPE_CLIENT::GET_CURRENT_TERMINAL_TYPE
// ==========================================================================
std::string terminal_type_client::get_current_terminal_type() const
{
    if(pimpl_->detected_types_.empty())
    {
        return "Unknown";
    }
    else
    {
        return pimpl_->detected_types_[pimpl_->current_type_];
    }
}
    
}}}


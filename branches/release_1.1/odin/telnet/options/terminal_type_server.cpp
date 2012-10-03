// ==========================================================================
// Odin Telnet Terminal Type Server Option
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
#include "odin/telnet/options/terminal_type_server.hpp"
#include <algorithm>
#include <vector>

namespace odin { namespace telnet { namespace options {

// ==========================================================================
// TERMINAL_TYPE_SERVER::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct terminal_type_server::impl
{
    boost::shared_ptr<odin::telnet::stream> stream_;
    std::vector<std::string>                terminal_types_;
    unsigned int                            current_type_;
};

// ==========================================================================
// TERMINAL_TYPE_SERVER::CONSTRUCTOR
// ==========================================================================
terminal_type_server::terminal_type_server(
    boost::shared_ptr<odin::telnet::stream> stream)
    : server_option(stream)
    , pimpl_(new impl)
{
    pimpl_->stream_       = stream;
    pimpl_->current_type_ = 0;
}

// ==========================================================================
// TERMINAL_TYPE_SERVER::DESTRUCTOR
// ==========================================================================
terminal_type_server::~terminal_type_server()
{
    delete pimpl_;
}
        
// ==========================================================================
// TERMINAL_TYPE_SERVER::ON_SUBNEGOTIATION
// ==========================================================================
void terminal_type_server::on_subnegotiation(
    subnegotiation_type const &subnegotiation)
{
    if (subnegotiation.size() > 0
     && subnegotiation[0] == odin::telnet::TERMINAL_TYPE_SEND)
    {
        // if(pimpl_->terminal_types_.empty())
        {
            static std::string const terminal_type = "Unknown";

            odin::runtime_array<odin::u8> reply(
                1 // odin::telnet::TERMINAL_TYPE_IS
              + terminal_type.size());

            reply[0] = odin::telnet::TERMINAL_TYPE_IS;

            std::copy(
                terminal_type.begin()
              , terminal_type.end()
              , reply.begin() + 1);
            
            pimpl_->stream_->send_subnegotiation(
                get_negotiation_type()
              , reply);
          
            return;
        }
        
        /* TODO: Unspaghetti this.
        if(pimpl_->current_type_ == pimpl_->terminal_types_.size() + 1)
        {
            std::string negotiation;
            negotiation += odin::telnet::TERMINAL_TYPE_IS;
            negotiation += *pimpl_->terminal_types_.rbegin();
            
            pimpl_->stream_->send_subnegotiation(
                get_negotiation_type()
              , &*string_to_subnegotiation(negotiation).begin()
              , negotiation.length());
              
            ++pimpl_->current_type_;
            return;
        }
        
        if (pimpl_->current_type_ == pimpl_->terminal_types_.size() + 2)
        {
            pimpl_->current_type_ = 0;
        }
        
        std::string negotiation;
        negotiation += odin::telnet::TERMINAL_TYPE_IS;
        negotiation += pimpl_->terminal_types_[pimpl_->current_type_];
        
        pimpl_->stream_->send_subnegotiation(
            get_negotiation_type()
          , &*string_to_subnegotiation(negotiation).begin()
          , negotiation.length());
          
        ++pimpl_->current_type_;
        */
    }
}
    
    
// ==========================================================================
// TERMINAL_TYPE_SERVER::GET_NEGOTIATION_TYPE
// ==========================================================================
negotiation_type terminal_type_server::get_negotiation_type() const
{
    return odin::telnet::TERMINAL_TYPE;
}
    
// ==========================================================================
// TERMINAL_TYPE_SERVER::ADD_TERMINAL_TYPE
// ==========================================================================
void terminal_type_server::add_terminal_type(std::string const &type)
{
    if(std::find(
        pimpl_->terminal_types_.begin()
      , pimpl_->terminal_types_.end()
      , type) == pimpl_->terminal_types_.end())
    {
        pimpl_->terminal_types_.push_back(type);
    }
}
        
// ==========================================================================
// TERMINAL_TYPE_SERVER::GET_CURRENT_TERMINAL_TYPE
// ==========================================================================
std::string terminal_type_server::get_current_terminal_type() const
{
    if(pimpl_->terminal_types_.empty())
    {
        return "";
    }
    
    if(pimpl_->current_type_ >= pimpl_->terminal_types_.size())
    {
        return *pimpl_->terminal_types_.rbegin();
    }
    
    return pimpl_->terminal_types_[pimpl_->current_type_];
}

}}}

// ==========================================================================
// Odin Telnet Server-Option.
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
#include "odin/telnet/server_option.hpp"

namespace odin { namespace telnet {
    
// ==============================================================================
// SERVER_OPTION::IMPLEMENTATION STRUCTURE
// ==============================================================================
struct server_option::impl
{
    boost::shared_ptr<odin::telnet::stream> stream_;
    
    bool active_;
    bool activate_sent_;
    bool activatable_;
    bool deactivate_sent_;

    boost::function<void ()> on_state_change_;
};
    
// ==========================================================================
// SERVER_OPTION::CONSTRUCTOR
// ==========================================================================
server_option::server_option(boost::shared_ptr<odin::telnet::stream> stream)
    : pimpl_(new impl)
{
    pimpl_->stream_          = stream;
    pimpl_->active_          = false;
    pimpl_->activate_sent_   = false;
    pimpl_->activatable_     = false;
    pimpl_->deactivate_sent_ = false;
}

// ==============================================================================
// SERVER_OPTION::DESTRUCTOR
// ==============================================================================
server_option::~server_option()
{
    delete pimpl_;
}
    
// ==============================================================================
// SERVER_OPTION::ACTIVATE
// ==============================================================================
void server_option::activate()
{
    if(!pimpl_->active_ && !pimpl_->activate_sent_)
    {
        pimpl_->stream_->send_negotiation(
            get_negotiation_type(), odin::telnet::WILL);
        pimpl_->activate_sent_ = true;
    }
}
        
// ======================================================================
// SERVER_OPTION::DEACTIVATE
// ======================================================================
void server_option::deactivate()
{
    if(pimpl_->active_ && !pimpl_->deactivate_sent_)
    {
        pimpl_->stream_->send_negotiation(
            get_negotiation_type(), odin::telnet::WONT);
        pimpl_->deactivate_sent_ = true;
        pimpl_->active_          = false;
    }
}
        
// ==============================================================================
// SERVER_OPTION::IS_ACTIVE
// ==============================================================================
bool server_option::is_active() const
{
    return pimpl_->active_;
}

// ==============================================================================
// SERVER_OPTION::IS_NEGOTIATING_ACTIVATION
// ==============================================================================
bool server_option::is_negotiating_activation() const
{
    return pimpl_->activate_sent_;
}
        
// ==============================================================================
// SERVER_OPTION::IS_NEGOTIATING_DEACTIVATION
// ==============================================================================
bool server_option::is_negotiating_deactivation() const
{
    return pimpl_->deactivate_sent_;
}

// ==============================================================================
// SERVER_OPTION::SET_ACTIVATABLE
// ==============================================================================
void server_option::set_activatable(bool activatable)
{
    pimpl_->activatable_ = activatable;
}
        
// ==============================================================================
// SERVER_OPTION::ON_ACTIVATE_REQUESTED
// ==============================================================================
void server_option::on_activate_requested()
{
    if(pimpl_->activatable_)
    {
        pimpl_->stream_->send_negotiation(
            get_negotiation_type(), odin::telnet::WILL);
    }
    else
    {
        pimpl_->stream_->send_negotiation(
            get_negotiation_type(), odin::telnet::WONT);
    }
}
    
// ==============================================================================
// SERVER_OPTION::ON_ACTIVATED
// ==============================================================================
void server_option::on_activated()
{
    pimpl_->active_        = true;
    pimpl_->activate_sent_ = false;
    
    if(pimpl_->on_state_change_)
    {
        pimpl_->on_state_change_();
    }
}
    
// ==============================================================================
// SERVER_OPTION::ON_ACTIVATE_DENIED
// ==============================================================================
void server_option::on_activate_denied()
{
    pimpl_->active_        = false;
    pimpl_->activate_sent_ = false;
    
    if(pimpl_->on_state_change_)
    {
        pimpl_->on_state_change_();
    }
}
    
// ==============================================================================
// SERVER_OPTION::ON_DEACTIVATED
// ==============================================================================
void server_option::on_deactivated()
{
    pimpl_->deactivate_sent_ = false;
    
    if(pimpl_->on_state_change_)
    {
        pimpl_->on_state_change_();
    }
}
        
// ==============================================================================
// SERVER_OPTION::ON_DEACTIVATE_REQUESTED
// ==============================================================================
void server_option::on_deactivate_requested()
{
    pimpl_->stream_->send_negotiation(
        get_negotiation_type(), odin::telnet::WONT);
}
    
// ==============================================================================
// SERVER_OPTION::ON_DEACTIVATE_DENIED
// ==============================================================================
void server_option::on_deactivate_denied()
{
    pimpl_->active_          = true;
    pimpl_->deactivate_sent_ = false;
    
    if(pimpl_->on_state_change_)
    {
        pimpl_->on_state_change_();
    }
}

// ==========================================================================
// SERVER_OPTION::ON_STATE_CHANGE
// ==========================================================================
void server_option::on_state_change(boost::function<void ()> fn)
{
    pimpl_->on_state_change_ = fn;
}


} }


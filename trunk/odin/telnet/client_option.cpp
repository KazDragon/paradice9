// ==========================================================================
// Odin Telnet Client-Option.
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
#include "odin/telnet/client_option.hpp"

namespace odin { namespace telnet {
    
// ==============================================================================
// CLIENT_OPTION::IMPLEMENTATION STRUCTURE
// ==============================================================================
struct client_option::impl
{
    boost::shared_ptr<odin::telnet::stream> stream_;
    
    bool active_;
    bool activate_sent_;
    bool activatable_;
    bool deactivate_sent_;
    
    boost::function<void ()> on_state_change_;
};
    
// ==========================================================================
// CLIENT_OPTION::CONSTRUCTOR
// ==========================================================================
client_option::client_option(boost::shared_ptr<odin::telnet::stream> stream)
    : pimpl_(new impl)
{
    pimpl_->stream_          = stream;
    pimpl_->active_          = false;
    pimpl_->activate_sent_   = false;
    pimpl_->activatable_     = false;
    pimpl_->deactivate_sent_ = false;
}

// ==============================================================================
// CLIENT_OPTION::DESTRUCTOR
// ==============================================================================
client_option::~client_option()
{
    delete pimpl_;
}
    
// ==============================================================================
// CLIENT_OPTION::ACTIVATE
// ==============================================================================
void client_option::activate()
{
    if(!pimpl_->active_ && !pimpl_->activate_sent_)
    {
        pimpl_->stream_->send_negotiation(
            get_negotiation_type(), odin::telnet::DO);
        pimpl_->activate_sent_ = true;
    }
}
        
// ======================================================================
// CLIENT_OPTION::DEACTIVATE
// ======================================================================
void client_option::deactivate()
{
    if(pimpl_->active_ && !pimpl_->deactivate_sent_)
    {
        pimpl_->stream_->send_negotiation(
            get_negotiation_type(), odin::telnet::DONT);
        pimpl_->deactivate_sent_ = true;
        pimpl_->active_          = false;
    }
}
        
// ==============================================================================
// CLIENT_OPTION::IS_ACTIVE
// ==============================================================================
bool client_option::is_active() const
{
    return pimpl_->active_;
}
        
// ==============================================================================
// CLIENT_OPTION::IS_NEGOTIATING_ACTIVATION
// ==============================================================================
bool client_option::is_negotiating_activation() const
{
    return pimpl_->activate_sent_;
}
        
// ==============================================================================
// CLIENT_OPTION::IS_NEGOTIATING_DEACTIVATION
// ==============================================================================
bool client_option::is_negotiating_deactivation() const
{
    return pimpl_->deactivate_sent_;
}

// ==============================================================================
// CLIENT_OPTION::SET_ACTIVATABLE
// ==============================================================================
void client_option::set_activatable(bool activatable)
{
    pimpl_->activatable_ = activatable;
}
        
// ==============================================================================
// CLIENT_OPTION::ON_ACTIVATE_REQUESTED
// ==============================================================================
void client_option::on_activate_requested()
{
    if(pimpl_->activatable_)
    {
        pimpl_->stream_->send_negotiation(
            get_negotiation_type(), odin::telnet::DO);
    }
    else
    {
        pimpl_->stream_->send_negotiation(
            get_negotiation_type(), odin::telnet::DONT);
    }
}
    
// ==============================================================================
// CLIENT_OPTION::ON_ACTIVATED
// ==============================================================================
void client_option::on_activated()
{
    pimpl_->active_        = true;
    pimpl_->activate_sent_ = false;
    
    if(pimpl_->on_state_change_)
    {
        pimpl_->on_state_change_();
    }
}
    
// ==============================================================================
// CLIENT_OPTION::ON_ACTIVATE_DENIED
// ==============================================================================
void client_option::on_activate_denied()
{
    pimpl_->active_        = false;
    pimpl_->activate_sent_ = false;
   
    if(pimpl_->on_state_change_)
    {
        pimpl_->on_state_change_();
    }
}
    
// ==============================================================================
// CLIENT_OPTION::ON_DEACTIVATED
// ==============================================================================
void client_option::on_deactivated()
{
    pimpl_->deactivate_sent_ = false;

    if(pimpl_->on_state_change_)
    {
        pimpl_->on_state_change_();
    }
}
        
// ==============================================================================
// CLIENT_OPTION::ON_DEACTIVATE_REQUESTED
// ==============================================================================
void client_option::on_deactivate_requested()
{
    pimpl_->stream_->send_negotiation(
        get_negotiation_type(), odin::telnet::DONT);
}
    
// ==============================================================================
// CLIENT_OPTION::ON_DEACTIVATE_DENIED
// ==============================================================================
void client_option::on_deactivate_denied()
{
    pimpl_->active_          = true;
    pimpl_->deactivate_sent_ = false;
    
    if(pimpl_->on_state_change_)
    {
        pimpl_->on_state_change_();
    }
}

// ==========================================================================
// CLIENT_OPTION::ON_STATE_CHANGE
// ==========================================================================
void client_option::on_state_change(boost::function<void ()> fn)
{
    pimpl_->on_state_change_ = fn;
}

} }


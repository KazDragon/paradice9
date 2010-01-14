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
#include "odin/telnet/stream.hpp"
#include "odin/telnet/router.hpp"
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace boost;

namespace odin { namespace telnet {
    
// ==============================================================================
// CLIENT_OPTION IMPLEMENTATION STRUCTURE
// ==============================================================================
struct client_option::impl
    : public enable_shared_from_this<impl>
{
    shared_ptr<odin::telnet::stream>     stream_;
    shared_ptr<odin::telnet::router>     router_;
    option_id_type                       option_id_;
    bool                                 active_;
    bool                                 activate_sent_;
    bool                                 activatable_;
    bool                                 deactivate_sent_;
    function<void ()>                    on_request_complete_;
    function<void (subnegotiation_type)> on_subnegotiation_;
    
    void on_will()
    {
        if (activate_sent_ || deactivate_sent_)
        {
            active_          = true;
            activate_sent_   = false;
            deactivate_sent_ = false;
            
            if (on_request_complete_)
            {
                on_request_complete_();
            }
        }
        else
        {
            if (active_)
            {
                stream_->send_negotiation(DO, option_id_);
            }
            else if (activatable_)
            {
                active_ = true;
                
                stream_->send_negotiation(DO, option_id_);
                
                if (on_request_complete_)
                {
                    on_request_complete_();
                }
            }
            else
            {
                stream_->send_negotiation(DONT, option_id_);
            }
        }
    }

    void on_wont()
    {
        bool was_active          = active_;
        bool activate_was_sent   = activate_sent_;
        bool deactivate_was_sent = deactivate_sent_;
        
        active_          = false;
        activate_sent_   = false;
        deactivate_sent_ = false;
        
        if (!activate_was_sent && !deactivate_was_sent)
        {
            // Since we have neither sent an activate nor a deactivate,
            // this is an unsolicited message.  Therefore, we must respond to
            // it.
            stream_->send_negotiation(DONT, option_id_);
        }

        // We must update the client only if this is a response to a request
        // from the client, OR if this is an unsolicited message from the
        // remote side that has resulted in a state change.
        if (was_active || activate_was_sent || deactivate_was_sent)
        {
            if (on_request_complete_)
            {
                on_request_complete_();
            }
        }
    }
    
    void on_subnegotiation(subnegotiation_type const &subnegotiation)
    {
        if (active_)
        {
            on_subnegotiation_(subnegotiation);
        }
    }
};
    
// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client_option::client_option(
    shared_ptr<odin::telnet::stream> stream
  , shared_ptr<odin::telnet::router> router
  , option_id_type                   option_id)
    : pimpl_(new impl)
{
    pimpl_->stream_            = stream;
    pimpl_->router_            = router;
    pimpl_->option_id_         = option_id;
    pimpl_->active_            = false;
    pimpl_->activate_sent_     = false;
    pimpl_->activatable_       = false;
    pimpl_->deactivate_sent_   = false;
    
    pimpl_->on_subnegotiation_ = 
        bind(&client_option::on_subnegotiation, this, _1);
    
    pimpl_->router_->register_negotiation(
        WILL
      , get_option_id()
      , bind(&impl::on_will, pimpl_->shared_from_this()));
    
    pimpl_->router_->register_negotiation(
        WONT
      , get_option_id()
      , bind(&impl::on_wont, pimpl_->shared_from_this()));
    
    pimpl_->router_->register_subnegotiation(
        get_option_id()
      , bind(&impl::on_subnegotiation, pimpl_->shared_from_this(), _1));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
client_option::~client_option()
{
    pimpl_->router_->unregister_negotiation(WILL, get_option_id());
    pimpl_->router_->unregister_negotiation(WONT, get_option_id());
    pimpl_->router_->unregister_subnegotiation(get_option_id());
}

// ==========================================================================
// GET_OPTION_ID
// ==========================================================================
option_id_type client_option::get_option_id() const
{
    return pimpl_->option_id_;
}

// ==========================================================================
// ACTIVATE
// ==========================================================================
void client_option::activate()
{
    if(!pimpl_->active_ && !pimpl_->activate_sent_)
    {
        pimpl_->stream_->send_negotiation(
            odin::telnet::DO, get_option_id());
        pimpl_->activate_sent_ = true;
    }
}

// ==========================================================================
// DEACTIVATE
// ==========================================================================
void client_option::deactivate()
{
    if (pimpl_->active_)
    {
        pimpl_->stream_->send_negotiation(
            odin::telnet::DONT, get_option_id());
        pimpl_->deactivate_sent_ = true;
    }
    else
    {
        if (pimpl_->on_request_complete_)
        {
            pimpl_->on_request_complete_();
        }
    }
}

// ==========================================================================
// IS_ACTIVE
// ==========================================================================
bool client_option::is_active() const
{
    return pimpl_->active_;
}

// ==========================================================================
// SET_ACTIVATABLE
// ==========================================================================
void client_option::set_activatable(bool activatable)
{
    pimpl_->activatable_ = activatable;
}

// ==========================================================================
// IS_ACTIVATABLE
// ==========================================================================
bool client_option::is_activatable() const
{
    return pimpl_->activatable_;
}

// ==========================================================================
// IS_NEGOTIATING_ACTIVATION
// ==========================================================================
bool client_option::is_negotiating_activation() const
{
    return pimpl_->activate_sent_;
}
        
// ==========================================================================
// IS_NEGOTIATING_DEACTIVATION
// ==========================================================================
bool client_option::is_negotiating_deactivation() const
{
    return pimpl_->deactivate_sent_;
}

// ==========================================================================
// ON_STATE_CHANGE
// ==========================================================================
void client_option::on_request_complete(function<void ()> const &callback)
{
    pimpl_->on_request_complete_ = callback;
}

}}

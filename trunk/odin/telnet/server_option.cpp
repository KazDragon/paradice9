// ==========================================================================
// Odin Telnet Server-Option.
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
#include "odin/telnet/server_option.hpp"
#include "odin/telnet/stream.hpp"
#include "odin/telnet/negotiation_router.hpp"
#include "odin/telnet/subnegotiation_router.hpp"
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace boost;

namespace odin { namespace telnet {
    
// ==============================================================================
// SERVER_OPTION IMPLEMENTATION STRUCTURE
// ==============================================================================
class server_option::impl
    : public enable_shared_from_this<impl>
{
public :
    impl(
        shared_ptr<stream> const                   &stream
      , shared_ptr<negotiation_router> const       &negotiation_router
      , shared_ptr<subnegotiation_router> const    &subnegotiation_router
      , option_id_type                              option_id)
      : stream_(stream)
      , negotiation_router_(negotiation_router)
      , subnegotiation_router_(subnegotiation_router)
      , option_id_(option_id)
      , active_(false)
      , activate_sent_(false)
      , activatable_(false)
      , deactivate_sent_(false)
      , will_negotiation_(1)
      , wont_negotiation_(1)
    {
        negotiation_type will_negotiation_content;
        will_negotiation_content.request_   = WILL;
        will_negotiation_content.option_id_ = option_id_;
        will_negotiation_[0] = will_negotiation_content;
        
        negotiation_type wont_negotiation_content;
        wont_negotiation_content.request_   = WONT;
        wont_negotiation_content.option_id_ = option_id_;
        wont_negotiation_[0] = wont_negotiation_content;
    }

    void register_routes(
        function<void (subnegotiation_type)> const &subnegotiation_callback)
    {
        negotiation_type do_negotiation;
        do_negotiation.request_   = DO;
        do_negotiation.option_id_ = option_id_;

        negotiation_router_->register_route(
            do_negotiation
          , bind(&impl::on_do, shared_from_this()));

        negotiation_type dont_negotiation;
        dont_negotiation.request_   = DONT;
        dont_negotiation.option_id_ = option_id_;
        
        negotiation_router_->register_route(
            dont_negotiation
          , bind(&impl::on_dont, shared_from_this()));
        
        subnegotiation_callback_ = subnegotiation_callback;
        
        subnegotiation_router_->register_route(
            option_id_
          , bind(&impl::on_subnegotiation, shared_from_this(), _1));
    }

    void unregister_routes()
    {
        negotiation_type do_negotiation;
        do_negotiation.request_   = DO;
        do_negotiation.option_id_ = option_id_;
        
        negotiation_router_->unregister_route(do_negotiation);

        negotiation_type dont_negotiation;
        dont_negotiation.request_   = DONT;
        dont_negotiation.option_id_ = option_id_;
        
        negotiation_router_->unregister_route(dont_negotiation);
        
        subnegotiation_router_->unregister_route(option_id_);
    }
    
    option_id_type get_option_id() const
    {
        return option_id_;
    }
    
    void activate()
    {
        if(!active_ && !activate_sent_)
        {
            stream_->write(will_negotiation_);
            activate_sent_ = true;
        }
    }
    
    void deactivate()
    {
        if (active_)
        {
            stream_->write(wont_negotiation_);
            deactivate_sent_ = true;
        }
        else
        {
            if (on_request_complete_)
            {
                on_request_complete_();
            }
        }
    }
    
    bool is_active() const
    {
        return active_;
    }
    
    void set_activatable(bool activatable)
    {
        activatable_ = activatable;
    }
    
    bool is_activatable() const
    {
        return activatable_;
    }

    bool is_negotiating_activation() const
    {
        return activate_sent_;
    }
            
    bool is_negotiating_deactivation() const
    {
        return deactivate_sent_;
    }
    
    void on_request_complete(function<void ()> const &callback)
    {
        on_request_complete_ = callback;
    }
    
    void send_subnegotiation(subnegotiation_content_type const &content)
    {
        subnegotiation_type subnegotiation;
        subnegotiation.option_id_ = option_id_;
        subnegotiation.content_   = content;
        
        stream::input_value_type input[] = { subnegotiation };
        stream_->async_write(input, NULL);
    }
    
private :
    shared_ptr<stream>                   stream_;
    shared_ptr<negotiation_router>       negotiation_router_;
    shared_ptr<subnegotiation_router>    subnegotiation_router_;
    option_id_type                       option_id_;
    bool                                 active_;
    bool                                 activate_sent_;
    bool                                 activatable_;
    bool                                 deactivate_sent_;
    function<void ()>                    on_request_complete_;
    function<void (subnegotiation_type)> subnegotiation_callback_;
    
    odin::runtime_array<stream::output_value_type> will_negotiation_;
    odin::runtime_array<stream::output_value_type> wont_negotiation_;
    
    void on_do()
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
                stream_->write(will_negotiation_);
            }
            else if (activatable_)
            {
                active_ = true;
                
                stream_->write(will_negotiation_);
                
                if (on_request_complete_)
                {
                    on_request_complete_();
                }
            }
            else
            {
                stream_->write(wont_negotiation_);
            }
        }
    }

    void on_dont()
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
            stream_->write(wont_negotiation_);
        }

        // We must update the server only if this is a response to a request
        // from the server, OR if this is an unsolicited message from the
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
        if (active_ && subnegotiation_callback_)
        {
            subnegotiation_callback_(subnegotiation);
        }
    }
};
    
// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server_option::server_option(
    shared_ptr<stream> const                &stream
  , shared_ptr<negotiation_router> const    &negotiation_router
  , shared_ptr<subnegotiation_router> const &subnegotiation_router
  , option_id_type                           option_id)
    : pimpl_(new impl(
          stream
        , negotiation_router
        , subnegotiation_router
        , option_id))
{
    pimpl_->register_routes(bind(&server_option::on_subnegotiation, this, _1));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
server_option::~server_option()
{
    pimpl_->unregister_routes();
}

// ==========================================================================
// GET_OPTION_ID
// ==========================================================================
option_id_type server_option::get_option_id() const
{
    return pimpl_->get_option_id();
}

// ==========================================================================
// ACTIVATE
// ==========================================================================
void server_option::activate()
{
    pimpl_->activate();
}

// ==========================================================================
// DEACTIVATE
// ==========================================================================
void server_option::deactivate()
{
    pimpl_->deactivate();
}

// ==========================================================================
// IS_ACTIVE
// ==========================================================================
bool server_option::is_active() const
{
    return pimpl_->is_active();
}

// ==========================================================================
// SET_ACTIVATABLE
// ==========================================================================
void server_option::set_activatable(bool activatable)
{
    pimpl_->set_activatable(activatable);
}

// ==========================================================================
// IS_ACTIVATABLE
// ==========================================================================
bool server_option::is_activatable() const
{
    return pimpl_->is_activatable();
}

// ==========================================================================
// IS_NEGOTIATING_ACTIVATION
// ==========================================================================
bool server_option::is_negotiating_activation() const
{
    return pimpl_->is_negotiating_activation();
}
        
// ==========================================================================
// IS_NEGOTIATING_DEACTIVATION
// ==========================================================================
bool server_option::is_negotiating_deactivation() const
{
    return pimpl_->is_negotiating_deactivation();
}

// ==========================================================================
// ON_STATE_CHANGE
// ==========================================================================
void server_option::on_request_complete(function<void ()> const &callback)
{
    pimpl_->on_request_complete(callback);
}

// ==========================================================================
// SEND_SUBNEGOTIATION
// ==========================================================================
void server_option::send_subnegotiation(
    subnegotiation_content_type const &content)
{
    pimpl_->send_subnegotiation(content);
}

}}

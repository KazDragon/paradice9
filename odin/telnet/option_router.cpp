// ==========================================================================
// Odin Telnet Option Router
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
#include "odin/telnet/option_router.hpp"
#include "odin/telnet/stream.hpp"
#include "odin/telnet/client_option.hpp"
#include "odin/telnet/server_option.hpp"
#include "odin/telnet/initiated_negotiation.hpp"
#include "odin/telnet/completed_negotiation.hpp"
#include <boost/bind.hpp>
#include <cassert>
#include <map>

namespace odin { namespace telnet {

// ==========================================================================
// OPTION_ROUTER::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct option_router::impl
{
    typedef std::map< negotiation_type, boost::shared_ptr<option> > option_map_t;
    option_map_t client_options_;
    option_map_t server_options_;
    
    // ======================================================================
    // ON_NEGOTIATION_INITIATED
    //  FUNCTION : Called when a negotiation is initiated.  Routes the
    //             relevant message to the relevant registered option, or
    //             responds appropriately if no such component is present.
    // ======================================================================
    void on_negotiation_initiated(initiated_negotiation const &negotiation)
    {
        negotiation_type type = negotiation.get_type();
        option_map_t::iterator pclient = client_options_.find(type);
        option_map_t::iterator pserver = server_options_.find(type);

        switch(negotiation.get_request())
        {
            case odin::telnet::WILL :
                if(pclient != client_options_.end())
                {
                    pclient->second->on_activate_requested();
                }
                else
                {
                    stream_->send_negotiation(type, odin::telnet::DONT);
                }
                
                break;
                
            case odin::telnet::WONT :
                if(pclient != client_options_.end())
                {
                    pclient->second->on_deactivate_requested();
                }
                else
                {
                    stream_->send_negotiation(type, odin::telnet::DONT);
                }
                
                break;
                
            case odin::telnet::DO :
                if(pserver != server_options_.end())
                {
                    pserver->second->on_activate_requested();
                }
                else
                {
                    stream_->send_negotiation(type, odin::telnet::WONT);
                }
                
                break;
                
            case odin::telnet::DONT :
                if(pserver != server_options_.end())
                {
                    pserver->second->on_deactivate_requested();
                }
                else
                {
                    stream_->send_negotiation(type, odin::telnet::WONT);
                }
                
                break;
                
            default :
                assert(!"Invalid request in option_router::impl::"
                        "on_negotiation_initiated()");
                break;
        }
    }
    
    // ======================================================================
    // ON_NEGOTIATION_COMPLETED
    //  FUNCTION : Called when a negotiation is completed.  Forwards the
    //             relevant message to the relevant registered option.
    // ======================================================================
    void on_negotiation_completed(completed_negotiation const &negotiation)
    {
        negotiation_type type = negotiation.get_type();
        option_map_t::iterator pclient = client_options_.find(type);
        option_map_t::iterator pserver = server_options_.find(type);
        int local_request  = negotiation.get_local_request();
        int remote_request = negotiation.get_remote_request();
        
        switch(local_request)
        {
            case odin::telnet::WILL :
                if(pserver != server_options_.end())
                {
                    if(remote_request == odin::telnet::DO)
                    {
                        pserver->second->on_activated();
                    }
                    else // odin::telnet::DONT
                    {
                        pserver->second->on_activate_denied();
                    }
                }
                break;

            case odin::telnet::WONT :
                if(pserver != server_options_.end())
                {
                    if(remote_request == odin::telnet::DONT)
                    {
                        pserver->second->on_deactivated();
                    }
                    else // odin::telnet::DO
                    {
                        pserver->second->on_deactivate_denied();
                    }
                }
                break;
                
            case odin::telnet::DO :
                if(pclient != client_options_.end())
                {
                    if(remote_request == odin::telnet::WILL)
                    {
                        pclient->second->on_activated();
                    }
                    else // odin::telnet::WONT
                    {
                        pserver->second->on_activate_denied();
                    }
                }
                break;
                
            case odin::telnet::DONT :
                if(pclient != client_options_.end())
                {
                    if(remote_request == odin::telnet::WONT)
                    {
                        pclient->second->on_deactivated();
                    }
                    else // odin::telnet::WILL
                    {
                        pserver->second->on_deactivate_denied();
                    }
                }
                break;
                
            default :
                assert(!"Invalid request in option_router::impl::"
                        "on_negotiation_completed()");
                break;
        }
    }
    
    // ======================================================================
    // ON_SUBNEGOTIATION
    //  FUNCTION : Called when a subnegotiation is received.  Forwards the
    //             negotiation to the relevant registered options.  Note:
    //             without specific knowledge of all options here, it is not
    //             possible to know whether the client or server portion
    //             should be informed, so both are.  It is up to the option
    //             itself to filter out messages not meant for it.
    // ======================================================================
    void on_subnegotiation(
        subnegotiation_id_type const &id
      , subnegotiation_type const    &subnegotiation)
    {
        // TODO: Reinstate
        /*
        option_map_t::iterator pclient = client_options_.find(type);
        option_map_t::iterator pserver = server_options_.find(type);
        
        if(pclient != client_options_.end())
        {
            pclient->second->on_subnegotiation(data, length);
        }
        
        if(pserver != server_options_.end())
        {
            pserver->second->on_subnegotiation(data, length);
        }
        */
    }
    
    boost::shared_ptr<stream> stream_;
};

// ==========================================================================
// OPTION_ROUTER::CONSTRUCTOR
// ==========================================================================
option_router::option_router(boost::shared_ptr<stream> stream)
    : pimpl_(new impl)
{
    pimpl_->stream_ = stream;
    
    // set up call-backs
    /*
    stream->on_negotiation_initiated(
        boost::bind(&impl::on_negotiation_initiated, pimpl_, _1));
        
    stream->on_negotiation_completed(
        boost::bind(&impl::on_negotiation_completed, pimpl_, _1));
        
    stream->on_subnegotiation(
        boost::bind(&impl::on_subnegotiation, pimpl_, _1, _2));
    */
}

// ==========================================================================
// OPTION_ROUTER::DESTRUCTOR
// ==========================================================================
option_router::~option_router()
{
    delete pimpl_;
}

// ==========================================================================
// OPTION_ROUTER::REGISTER_CLIENT_OPTION
// ==========================================================================
void option_router::register_client_option(boost::shared_ptr<client_option> option)
{
    pimpl_->client_options_[option->get_negotiation_type()] = option;
}

// ==========================================================================
// OPTION_ROUTER::REGISTER_SERVER_OPTION
// ==========================================================================
void option_router::register_server_option(boost::shared_ptr<server_option> option)
{
    pimpl_->server_options_[option->get_negotiation_type()] = option;
}

// ==========================================================================
// OPTION_ROUTER::UNREGISTER_CLIENT_OPTION
// ==========================================================================
void option_router::unregister_client_option(boost::shared_ptr<client_option> option)
{
    pimpl_->client_options_.erase(option->get_negotiation_type());
}

// ==========================================================================
// OPTION_ROUTER::UNREGISTER_SERVER_OPTION
// ==========================================================================
void option_router::unregister_server_option(boost::shared_ptr<server_option> option)
{
    pimpl_->server_options_.erase(option->get_negotiation_type());
}
        
}}


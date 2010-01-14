// ==========================================================================
// Odin Telnet Router
//
// Copyright (C) 2009 Matthew Chaplain, All Rights Reserved.
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
#include "odin/telnet/router.hpp"
#include "odin/telnet/stream.hpp"
#include <boost/bind.hpp>
#include <boost/typeof/typeof.hpp>
#include <map>
#include <utility>

#include <iostream>

using namespace std;
using namespace boost;

namespace odin { namespace telnet {

// ==========================================================================
// ROUTER IMPLEMENTATION STRUCTURE
// ==========================================================================
struct router::impl
{
    typedef pair
    <
        odin::telnet::negotiation_request_type
      , odin::telnet::option_id_type
    > negotiation_key_type;
    
    typedef map
    <
        negotiation_key_type
      , function<void ()>
    > negotiation_map_type; 
    
    typedef odin::telnet::option_id_type subnegotiation_key_type;
    
    typedef map
    <
        subnegotiation_key_type
      , function<void (subnegotiation_type)>
    > subnegotiation_map_type;
    
    shared_ptr<stream>      underlying_stream_;
    negotiation_map_type    negotiation_map_;
    subnegotiation_map_type subnegotiation_map_;
    
    void default_response(
        negotiation_request_type negotiation_request
      , option_id_type           option_id)
    {
        if (negotiation_request == DO
         || negotiation_request == DONT)
        {
            underlying_stream_->send_negotiation(WONT, option_id);
        }
        else
        {
            underlying_stream_->send_negotiation(DONT, option_id);
        }
    }
        
    void on_negotiation(
        negotiation_request_type negotiation_request
      , option_id_type           option_id)
    {
        negotiation_map_type::const_iterator pcallback =
            negotiation_map_.find(
                make_pair(negotiation_request, option_id));
            
        if (pcallback != negotiation_map_.end())
        {
            function<void ()> callback = pcallback->second;
            
            if (callback)
            {
                callback();
            }
        }
        else
        {
            default_response(negotiation_request, option_id);
        }
    }
    
    void on_subnegotiation(
        option_id_type      option_id
      , subnegotiation_type subnegotiation)
    {
        function<void (subnegotiation_type)> callback = subnegotiation_map_[
            option_id
        ];
        
        if (callback)
        {
            callback(subnegotiation);
        }
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
router::router(shared_ptr<stream> const &underlying_stream)
    : pimpl_(new impl)
{
    pimpl_->underlying_stream_ = underlying_stream;
    
    underlying_stream->on_negotiation(
        bind(&impl::on_negotiation, pimpl_, _1, _2));
    
    underlying_stream->on_subnegotiation(
        bind(&impl::on_subnegotiation, pimpl_, _1, _2));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
router::~router()
{
}

// ==========================================================================
// REGISTER_NEGOTIATION
// ==========================================================================
void router::register_negotiation(
    negotiation_request_type negotiation_request
  , option_id_type           option_id
  , function<void ()>        callback)
{
    pimpl_->negotiation_map_[
        make_pair(negotiation_request, option_id)
    ] = callback;
}

// ==========================================================================
// UNREGISTER_NEGOTIATION
// ==========================================================================
void router::unregister_negotiation(
    negotiation_request_type negotiation_request
  , option_id_type           option_id)
{
    BOOST_AUTO(entry, pimpl_->negotiation_map_.find(
        make_pair(negotiation_request, option_id)));

    if (entry != pimpl_->negotiation_map_.end())
    {
        pimpl_->negotiation_map_.erase(entry);
    }
}

// ==========================================================================
// REGISTER_SUBNEGOTIATION
// ==========================================================================
void router::register_subnegotiation(
    option_id_type                              option_id
  , function<void (subnegotiation_type)> const &callback)
{
    pimpl_->subnegotiation_map_[option_id] = callback;
}

// ==========================================================================
// UNREGISTER_SUBNEGOTIATION
// ==========================================================================
void router::unregister_subnegotiation(option_id_type option_id)
{
    BOOST_AUTO(entry, pimpl_->subnegotiation_map_.find(option_id));

    if (entry != pimpl_->subnegotiation_map_.end())
    {
        pimpl_->subnegotiation_map_.erase(entry);
    }
}

}}

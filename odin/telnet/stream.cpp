// ==========================================================================
// Odin Telnet Stream
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
#include "odin/telnet/stream.hpp"
#include "boost/asio/io_service.hpp"

using namespace boost;

namespace odin { namespace telnet {

// ==========================================================================
// CONSTRUCTOR 
// ==========================================================================
stream::stream(
    shared_ptr<odin::io::byte_stream> const &underlying_stream
  , boost::asio::io_service                 &io_service)
{
}

// ==========================================================================
// DESTRUCTOR 
// ==========================================================================
stream::~stream()
{
}

// ==========================================================================
// AVAILABLE
// ==========================================================================
optional<stream::input_size_type> stream::available() const
{
    return optional<input_size_type>();
}

// ==========================================================================
// READ
// ==========================================================================
odin::runtime_array<stream::input_value_type> stream::read(input_size_type size)
{
    return odin::runtime_array<input_value_type>();
}

// ==========================================================================
// ASYNC_READ
// ==========================================================================
void stream::async_read(
    input_size_type            size
  , input_callback_type const &callback)
{
}

// ==========================================================================
// WRITE
// ==========================================================================
stream::output_size_type stream::write(
    odin::runtime_array<output_value_type> const& values)
{
    return stream::output_size_type();
}

// ==========================================================================
// ASYNC_WRITE
// ==========================================================================
void stream::async_write(
    odin::runtime_array<output_value_type> const &values
  , output_callback_type const                   &callback)
{
}

// ==========================================================================
// IS_ALIVE
// ==========================================================================
bool stream::is_alive() const
{
    return true;
}

// ==========================================================================
// SEND_COMMAND
// ==========================================================================
void stream::send_command(command cmd)
{
}

// ==========================================================================
// SEND_NEGOTIATION
// ==========================================================================
void stream::send_negotiation(
    negotiation_type    type
  , negotiation_request request)
{
}

// ==========================================================================
// SEND_SUBNEGOTIATION
// ==========================================================================
void stream::send_subnegotiation(
    subnegotiation_id_type const &id
  , subnegotiation_type    const &subnegotiation)
{
}

// ==========================================================================
// GET_LOCAL_NEGOTIATION
// ==========================================================================
negotiation_request stream::get_local_negotiation(negotiation_type) const
{
    return WONT;
}

// ==========================================================================
// GET_REMOTE_NEGOTIATION
// ==========================================================================
negotiation_request stream::get_remote_negotiation(negotiation_type) const
{
    return WONT;
}

// ==========================================================================
// ON_COMMAND
// ==========================================================================
void stream::on_command(command_callback const &callback)
{
}

// ==========================================================================
// ON_NEGOTIATION_INITIATED
// ==========================================================================
void stream::on_negotiation_initiated(
    initiated_negotiation_callback const &callback)
{
}

// ==========================================================================
// ON_NEGOTIATION_COMPLETED
// ==========================================================================
void stream::on_negotiation_completed(
    completed_negotiation_callback const &callback)
{
}

// ==========================================================================
// ON_SUBNEGOTIATION
// ==========================================================================
void stream::on_subnegotiation(subnegotiation_callback const &callback)
{
}

}}

/*
#include "odin/telnet/stream.hpp"
#include "odin/telnet/filter.hpp"
#include "odin/telnet/initiated_negotiation.hpp"
#include "odin/telnet/completed_negotiation.hpp"
#include <boost/bind.hpp>
#include <algorithm>
#include <deque>
#include <iterator>
#include <list>
#include <map>

namespace odin { namespace telnet {

namespace {
// ======================================================================
// REQUEST_IS_OPPOSITE
//  FUNCTION : Returns true if the left-hand negotiation is complementary
//             of the right-hand negotiation.  For example, if the left-
//             hand negotiation is WILL or WONT, then DO or DONT is
//             complementary.
// ======================================================================
bool request_is_opposite(negotiation_request lhs, negotiation_request rhs)
{
    return ((lhs == WILL || lhs == WONT) && (rhs == DO || rhs == DONT))
        || ((rhs == WILL || rhs == WONT) && (lhs == DO || lhs == DONT));
}

// ======================================================================
// COMPLETE
//  FUNCTION : Completes a negotiation.
// ======================================================================
completed_negotiation complete(
    initiated_negotiation const &negotiation
  , negotiation_request          request)
{
    if(negotiation.get_initiator() == local)
    {
        return completed_negotiation(
            negotiation.get_initiator()
          , negotiation.get_type()
          , negotiation.get_request()
          , request);
    }
    else // negotiation.get_initiator() == remote
    {
        return completed_negotiation(
            negotiation.get_initiator()
          , negotiation.get_type()
          , request
          , negotiation.get_request());
    }
}
}
    
// ==========================================================================
// STREAM::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct stream::impl
{
typedef std::list<initiated_negotiation>                  negotiation_list;
typedef std::map<negotiation_type, negotiation_request>   negotiation_map;

shared_ptr<char_stream>   stream_;
std::deque<char>                 buffer_;

filter                           filter_;
bool                             alive_;

stream::command_fn               on_command_;
stream::initiated_negotiation_fn on_negotiation_initiated_;
stream::completed_negotiation_fn on_negotiation_completed_;
stream::subnegotiation_fn        on_subnegotiation_;

negotiation_list                 negotiations_underway_;
negotiation_map                  local_negotiations_;
negotiation_map                  remote_negotiations_;

// ======================================================================
// IMPL::CONSTRUCTOR
// ======================================================================
impl()
    : filter_()
    , alive_(true)
    , on_command_(NULL)
    , on_negotiation_initiated_(NULL)
    , on_negotiation_completed_(NULL)
    , on_subnegotiation_(NULL)
{
    filter_.on_command(
        bind(&impl::on_command, this, _1));
        
    filter_.on_negotiation(
        bind(&impl::on_negotiation, this, _1, _2));
        
    filter_.on_subnegotiation(
        bind(&impl::on_subnegotiation, this, _1, _2, _3));
}

// ======================================================================
// IMPL::SET_ON_COMMAND
// ======================================================================
void set_on_command(stream::command_fn const &fn)
{
    on_command_ = fn;
}

// ======================================================================
// IMPL::ON_COMMAND
// ======================================================================
void on_command(command command)
{
    if(on_command_ != NULL)
    {
        on_command_(command);
    }
}

// ======================================================================
// IMPL::SET_ON_NEGOTIATION_INITIATED
// ======================================================================
void set_on_negotiation_initiated(stream::initiated_negotiation_fn const &f)
{
    on_negotiation_initiated_ = f;
}

// ======================================================================
// IMPL::SET_ON_NEGOTIATION_COMPLETED
// ======================================================================
void set_on_negotiation_completed(stream::completed_negotiation_fn const &f)
{
    on_negotiation_completed_ = f;
}

// ======================================================================
// IMPL::ON_NEGOTIATION
// ======================================================================
void on_negotiation(negotiation_type type, negotiation_request request)
{
    bool found = false;
    
    // We are looking for negotiations that:
    // a) are initiated by local
    // b) have the same negotiation type
    // c) have a complementary request type
    for(negotiation_list::iterator iter = negotiations_underway_.begin();
        iter != negotiations_underway_.end() && !found;
        ++iter)
    {
        if(iter->get_initiator() == local
        && iter->get_type()      == type
        && request_is_opposite(iter->get_request(), request))
        {
            if(request == WILL || request == WONT)
            {
                remote_negotiations_.insert(
                    negotiation_map::value_type(type, request));
            }
            else if(request == DO)
            {
                local_negotiations_.insert(
                    negotiation_map::value_type(type, iter->get_request()));
            }
            else // request == DONT
            {
                local_negotiations_.insert(
                    negotiation_map::value_type(type, WONT));
            }

            completed_negotiation neg(complete(*iter, request));
            negotiations_underway_.erase(iter);
            found = true;
            
            if(on_negotiation_completed_)
            {
                on_negotiation_completed_(neg);
            }
        }
    }

    if(!found)
    {
        initiated_negotiation neg(remote, type, request);
        negotiations_underway_.push_back(neg);
        
        if(on_negotiation_initiated_)
        {
            on_negotiation_initiated_(neg);
        }
    }
}

// ======================================================================
// IMPL::SET_ON_SUBNEGOTIATION
// ======================================================================
void set_on_subnegotiation(stream::subnegotiation_fn const &fn)
{
    on_subnegotiation_ = fn;
}

// ======================================================================
// IMPL::ON_SUBNEGOTIATION
// ======================================================================
void on_subnegotiation(subnegotiation_type type
                     , ksubnegotiation     sub
                     , unsigned int        len)
{
    if(on_subnegotiation_ != NULL)
    {
        on_subnegotiation_(type, sub, len);
    }
}

// ======================================================================
// IMPL::IS_ALIVE
// ======================================================================
bool is_alive() const
{
    return alive_ && (!buffer_.empty() || stream_->is_alive());
}

// ======================================================================
// IMPL::AVAILABLE
// ======================================================================
int available() const
{
    return !buffer_.empty() ? buffer_.size() : stream_->available();
}

// ======================================================================
// IMPL::LOCK
// ======================================================================
odin::locked lock()
{
    return stream_->lock();
}


// ======================================================================
// IMPL::SEND_COMMAND
// ======================================================================
void send_command(command cmd)
{
    // Format: IAC <command>
    char command_string[2] = {IAC, cmd};
    stream_->write(command_string, 2);
}

// ======================================================================
// IMPL::NEGOTIATE
// ======================================================================
void negotiate(negotiation_type type, negotiation_request request)
{
    bool found = false;
    
    // look for: initiated by remote, type is same, request is opposite.
    for(negotiation_list::iterator iter = negotiations_underway_.begin();
        iter != negotiations_underway_.end() && !found;
        ++iter)
    {
        // We are looking for negotiations that:
        // a) are initiated by remote
        // b) have the same negotiation type
        // c) have a complementary request type
        if(iter->get_initiator() == remote
        && iter->get_type()      == type
        && request_is_opposite(iter->get_request(), request))
        {
            if(request == WILL || request == WONT)
            {
                local_negotiations_.insert(
                    negotiation_map::value_type(type, request));
            }
            else if(request == DO)
            {
                remote_negotiations_.insert(
                    negotiation_map::value_type(type, iter->get_request()));
            }
            else // request == DONT
            {
                remote_negotiations_.insert(
                    negotiation_map::value_type(type, WONT));
            }

            completed_negotiation neg(complete(*iter, request));
            negotiations_underway_.erase(iter);
            found = true;
            
            if(on_negotiation_completed_)
            {
                on_negotiation_completed_(neg);
            }
        }
    }
      
    if(!found)
    {
        negotiations_underway_.push_back(
            initiated_negotiation(local, type, request));
    }
    
    // Format: IAC <request> <type>
    char cmd[3] = {IAC, request, type};
    stream_->write(cmd, 3);
}

// ======================================================================
// IMPL::SEND_SUBNEGOTIATION
// ======================================================================
void send_subnegotiation(
    subnegotiation_type type
  , ksubnegotiation     subnegotiation
  , unsigned int        length)
{
    // Format: IAC SB <type> <subnegotiation...> IAC SE
    // IACs are duplicated in <subnegotiation...>
    std::string str;
    str.reserve((length * 2) + 5);
    
    str.push_back(IAC);
    str.push_back(SB);
    str.push_back(type);
    
    for(unsigned int i = 0; i < length; ++i)
    {
        if(subnegotiation[i] == IAC)
        {
            str.push_back(subnegotiation[i]);
        }
        
        str.push_back(subnegotiation[i]);
    }
    
    str.push_back(IAC);
    str.push_back(SE);
    
    stream_->write(str.c_str(), str.size());
}

// ======================================================================
// IMPL::GET_LOCAL_NEGOTIATION
// ======================================================================
negotiation_request get_local_negotiation(negotiation_type type) const
{
    negotiation_map::const_iterator iter = local_negotiations_.find(type);
    
    if(iter != local_negotiations_.end())
    {
        return iter->second;
    }
    else
    {
        return WONT;
    }
}

// ======================================================================
// IMPL::GET_REMOTE_NEGOTIATION
// ======================================================================
negotiation_request get_remote_negotiation(negotiation_type type) const
{
    negotiation_map::const_iterator iter = remote_negotiations_.find(type);
    
    if(iter != remote_negotiations_.end())
    {
        return iter->second;
    }
    else
    {
        return WONT;
    }
}

// ======================================================================
// IMPL::READ
// ======================================================================
unsigned int read(char *data, unsigned int length)
{
    if(!is_alive())
    {
        return 0;
    }
    
    optional<char> ch(get_next_char());
    
    if(!ch)
    {
        return 0;
    }
    
    ch = filter_(*ch);
    
    // We are in a telnet negotiation if non-char was returned.
    if(!ch)
    {
        // Read and filter the negotiation
        while(!ch)
        {
            ch = get_next_char();
            
            if(!ch)
            {
                return 0;
            }
            
            ch = filter_(*ch);
        }
    }
    
    // Any telnet transactions are now complete, and ch contains
    // a normal character.  Read until the next telnet transaction
    // starts, we run out of characters, or get length characters.
    if(length == 0)
    {
        // We only wanted to read over the telnet transaction.
        buffer_.push_front(*ch);
        return 0;
    }
    
    unsigned int pos = 0;

    data[pos++] = *ch;

    while(pos < length)
    {
        ch = get_next_char();
        
        if(!ch)
        {
            return pos;
        }
        
        
        ch = filter_(*ch);
        
        if(!ch)
        {
            return pos;
        }
        
        data[pos++] = *ch;
    }

    return pos;
}

// ======================================================================
// IMPL::CLOSE
// ======================================================================
void close()
{
    stream_->close();
    alive_ = false;
}

// ======================================================================
// IMPL::WRITE
// ======================================================================
unsigned int write(char const *data, unsigned int length)
{
    // Since we are writing plain text, it is necessary to "escape" any
    // IACs with another IAC, so that it is not interpreted as a command
    // on the remote terminal.
    std::string escaped_data;
    escaped_data.reserve(length * 2);
    
    while(length--)
    {
        if((unsigned char)(*data) == (unsigned char)(IAC))
        {
            escaped_data += *data;
        }
        
        escaped_data += *data++;
    }
    
    return stream_->write(escaped_data.c_str(), escaped_data.size());
}

// ======================================================================
// IMPL::UNDERFLOW
// ======================================================================
void underflow()
{
    static size_t const telnet_buffer_size = 1024;
    
    assert(buffer_.empty());

    if(stream_->available() > 0)
    {
        char local_buffer_[telnet_buffer_size];
        int  len = stream_->read(local_buffer_, telnet_buffer_size);
            
        if(len > 0)
        {
            std::copy(local_buffer_, local_buffer_ + len,
                std::back_inserter(buffer_));
        }
    }
}

// ======================================================================
// IMPL::GET_NEXT_CHAR
// ======================================================================
optional<char> get_next_char()
{
    if(buffer_.empty())
    {
        underflow();
        
        if(buffer_.empty())
        {
            return optional<char>();
        }
    }
    
    optional<char> ch(buffer_.front());
    buffer_.pop_front();
    return ch;
}
};

// ==========================================================================
// STREAM::CONSTRUCTOR
// ==========================================================================
stream::stream(shared_ptr<odin::char_stream> const &stream)
: pimpl_(new impl)
{
pimpl_->stream_ = stream;
}

// ==========================================================================
// STREAM::DESTRUCTOR
// ==========================================================================
stream::~stream()
{
delete pimpl_;
}

// ==========================================================================
// STREAM::WRITE
// ==========================================================================
unsigned int stream::write(char const *data, unsigned int length)
{
return pimpl_->write(data, length);
}

// ==========================================================================
// STREAM::READ
// ==========================================================================
unsigned int stream::read(char *data, unsigned int length)
{
return pimpl_->read(data, length);
}

// ==========================================================================
// STREAM::CLOSE
// ==========================================================================
void stream::close()
{
pimpl_->close();
}

// ==========================================================================
// STREAM::IS_ALIVE
// ==========================================================================
bool stream::is_alive() const
{
return pimpl_->is_alive();
}

// ==========================================================================
// STREAM::AVAILABLE
// ==========================================================================
int stream::available() const
{
return pimpl_->available();
}

// ==========================================================================
// STREAM::LOCK
// ==========================================================================
odin::locked stream::lock()
{
return pimpl_->lock();
}

// ==========================================================================
// STREAM::SEND_COMMAND
// ==========================================================================
void stream::send_command(command cmd)
{
pimpl_->send_command(cmd);
}

// ==========================================================================
// STREAM::NEGOTIATE
// ==========================================================================
void stream::negotiate(negotiation_type type, negotiation_request request)
{
pimpl_->negotiate(type, request);
}

// ==========================================================================
// STREAM::SEND_SUBNEGOTIATION
// ==========================================================================
void stream::send_subnegotiation(
subnegotiation_type type
, ksubnegotiation     subnegotiation
, unsigned int        length)
{
pimpl_->send_subnegotiation(type, subnegotiation, length);
}

// ===========================================================================
// STREAM::GET_LOCAL_NEGOTIATION
// ===========================================================================
negotiation_request stream::get_local_negotiation(negotiation_type type) const
{
return pimpl_->get_local_negotiation(type);
}
        
// ===========================================================================
// STREAM::GET_REMOTE_NEGOTIATION
// ===========================================================================
negotiation_request stream::get_remote_negotiation(negotiation_type type) const
{
return pimpl_->get_remote_negotiation(type);
}

// ==========================================================================
// STREAM::ON_COMMAND
// ==========================================================================
void stream::on_command(command_fn const &fn)
{
pimpl_->set_on_command(fn);
}

// ==========================================================================
// STREAM::ON_NEGOTIATION_INITIATED
// ==========================================================================
void stream::on_negotiation_initiated(initiated_negotiation_fn const &fn)
{
pimpl_->set_on_negotiation_initiated(fn);
}

// ==========================================================================
// STREAM::ON_NEGOTIATION_COMPLETED
// ==========================================================================
void stream::on_negotiation_completed(completed_negotiation_fn const &fn)
{
pimpl_->set_on_negotiation_completed(fn);
}

// ==========================================================================
// STREAM::ON_SUBNEGOTIATION
// ==========================================================================
void stream::on_subnegotiation(subnegotiation_fn const &fn)
{
pimpl_->set_on_subnegotiation(fn);
}

}}
*/

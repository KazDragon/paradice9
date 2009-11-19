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
#include "odin/telnet/filter.hpp"
#include "odin/telnet/initiated_negotiation.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>
#include <algorithm>
#include <deque>
#include <functional>

using namespace std;
using namespace boost;
using namespace odin;

namespace odin { namespace telnet {

typedef odin::io::byte_stream byte_stream;
typedef odin::io::char_stream char_stream;

class stream::impl 
    : public enable_shared_from_this<stream::impl>
{
public :    
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    impl(
        shared_ptr<byte_stream> const &underlying_stream
      , boost::asio::io_service       &io_service)
      : underlying_stream_(underlying_stream)
      , io_service_(io_service)
    {
    }
 
    //* =====================================================================
    /// \brief Returns the number of bytes available to be read without
    /// blocking.
    //* =====================================================================
    optional<stream::input_size_type> available() const
    {
        // First, we query the underlying stream for any data it has available.
        // These we add to our own input buffer.
        BOOST_AUTO(underlying_available, underlying_stream_->available());
        
        if (underlying_available.is_initialized())
        {
            BOOST_AUTO(
                data
              , underlying_stream_->read(underlying_available.get()));
            
            copy(
                data.begin()
              , data.end()
              , back_inserter(input_buffer_));
        }
        
        // Now that we have all the data in the input buffer, we query how
        // much of that can be read without blocking.
        stream::input_size_type readable_size = get_readable_size();
        
        return readable_size == 0
             ? optional<stream::input_size_type>()
             : optional<stream::input_size_type>(readable_size);
    }
    
    //* =====================================================================
    /// \brief Returns an array of size number of non-telnet bytes from the
    /// stream. Telnet bytes are filtered out during the read and appropriate
    /// callbacks are made.
    //* =====================================================================
    runtime_array<stream::input_value_type> read(
        stream::input_size_type size)
    {
        runtime_array<stream::input_value_type> result(size);
        
        stream::input_size_type amount_copied = 0;
        
        while (amount_copied < size)
        {
            if (input_buffer_.empty())
            {
                // There is no data in the input buffer.  Read up to the
                // amount available in the underlying stream.
                BOOST_AUTO(available, underlying_stream_->available());
                
                BOOST_AUTO(
                    data
                  , underlying_stream_->read(available ? *available : 1));
                
                copy(data.begin(), data.end(), back_inserter(input_buffer_));
            }
            else
            {
                // There is data in the input buffer.  Parse the front byte.
                BOOST_AUTO(raw_value,      input_buffer_.front());
                BOOST_AUTO(filtered_value, filter_(raw_value));
                
                if (filtered_value)
                {
                    result[amount_copied++] = *filtered_value;
                }
                
                input_buffer_.pop_front();
            }
        }
        
        return result;
    }
        
    //* =====================================================================
    /// \brief Asynchronously reads from the stream.  The callback function
    /// is called after size non-telnet bytes have been received.  Any
    /// telnet bytes are filtered out during this read and appropriate
    /// callbacks are made.
    //* =====================================================================
    void async_read(
        stream::input_size_type            size
      , stream::input_callback_type const &callback)
    {
        read_request request = { size, callback };
        read_requests_.push_back(request);
        
        post_schedule_read_request();
    }
    
    //* =====================================================================
    /// \brief Registers a callback to be made when telnet commands are
    /// filtered out during reading.
    //* =====================================================================
    void on_command(stream::command_callback const &callback)
    {
        filter_.on_command(callback);
    }

    //* =====================================================================
    /// \brief Registers a callback for when telnet negotiations are
    /// filtered out during reading.
    //* =====================================================================
    void on_negotiation(stream::negotiation_callback const &callback)
    {
        filter_.on_negotiation(callback);
    }

    //* =====================================================================
    /// \brief Registers a callback to be made when telnet subnegotiations
    /// are filtered out during reading.
    //* =====================================================================
    void on_subnegotiation(stream::subnegotiation_callback const &callback)
    {
        filter_.on_subnegotiation(callback);
    }
        
private :    
    //* =====================================================================
    /// \brief Returns the amount of data readable without encountering
    /// telnet protocol bytes, except for those that would begin at the
    /// very start of the data.
    //* =====================================================================
    stream::input_size_type get_readable_size() const
    {
        // Count until we reach the first telnet byte.
        stream::input_size_type amount       = 0;
        bool                    was_filtered = false;
        odin::telnet::filter    filter;
        
        BOOST_FOREACH(odin::u8 value, input_buffer_)
        {
            if (was_filtered)
            {
                if (filter(value))
                {
                    was_filtered = false;
                    ++amount;
                }
                else
                {
                    // We have received a telnet sequence.  If we have 
                    // previously read a normal character, then we must stop
                    // reading here.
                    if (amount != 0)
                    {
                        break;
                    }
                }
            }
            else
            {
                if (filter(value))
                {
                    ++amount;
                }
                else
                {
                    was_filtered = true;
                }
            }
        }
        
        return amount;
    }

    //* =====================================================================
    /// \brief Helper function to schedule an asynchronous read request.
    //* =====================================================================
    void post_schedule_read_request()
    {
        io_service_.post(bind(
            &impl::schedule_read_request
          , shared_from_this()));
    }
        
    //* =====================================================================
    /// \brief Schedules a read request, if necessary.
    //* =====================================================================
    void schedule_read_request()
    {
        if (read_requests_.empty())
        {
            return;
        }
        
        if (!read_requests_[0].handling_)
        {
            underlying_stream_->async_read(
                read_requests_[0].size_
              , bind(&impl::data_received, shared_from_this(), _1));
                
            read_requests_[0].handling_ = true;
        }
    }
    
    //* =====================================================================
    /// \brief Called when data is received from the underlying stream.
    //* =====================================================================
    void data_received(
        runtime_array<byte_stream::input_value_type> const &values)
    {
        if (read_requests_.empty())
        {
            return;
        }
        
        if (read_requests_[0].callback_)
        {
            read_requests_[0].callback_(values);
        }
        
        read_requests_.pop_front();
        
        post_schedule_read_request();
    }
        
    struct read_request
    {
        stream::input_size_type     size_;
        stream::input_callback_type callback_;
        bool                        handling_;
    };
    
    shared_ptr<odin::io::byte_stream>       underlying_stream_;
    boost::asio::io_service                &io_service_;
    deque<read_request>                     read_requests_;
    
    mutable deque<odin::u8>                 input_buffer_;
    odin::telnet::filter                    filter_;
};

// ==========================================================================
// CONSTRUCTOR 
// ==========================================================================
stream::stream(
    shared_ptr<odin::io::byte_stream> const &underlying_stream
  , boost::asio::io_service                 &io_service)
  : pimpl_(new impl(underlying_stream, io_service))
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
    return pimpl_->available();
}

// ==========================================================================
// READ
// ==========================================================================
odin::runtime_array<stream::input_value_type> stream::read(input_size_type size)
{
    return pimpl_->read(size);
}

// ==========================================================================
// ASYNC_READ
// ==========================================================================
void stream::async_read(
    input_size_type            size
  , input_callback_type const &callback)
{
    pimpl_->async_read(size, callback);
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
// ON_COMMAND
// ==========================================================================
void stream::on_command(command_callback const &callback)
{
    pimpl_->on_command(callback);
}

// ==========================================================================
// ON_NEGOTIATION
// ==========================================================================
void stream::on_negotiation(negotiation_callback const &callback)
{
    pimpl_->on_negotiation(callback);
}

// ==========================================================================
// ON_SUBNEGOTIATION
// ==========================================================================
void stream::on_subnegotiation(subnegotiation_callback const &callback)
{
    pimpl_->on_subnegotiation(callback);
}

}}

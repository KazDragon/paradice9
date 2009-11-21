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
      , async_read_request_fired_(false)
      , async_write_request_fired_(false)
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
        read_request request(size, callback);
        read_requests_.push_back(request);
        
        io_service_.post(bind(
            &impl::check_async_read_requests
          , shared_from_this()));
    }
    
    //* =====================================================================
    /// \brief Perform a synchronous write to the stream.
    /// \return the number of objects written to the stream.
    /// Write an array of WriteValues to the stream.  
    //* =====================================================================
    stream::output_size_type write(
        odin::runtime_array<stream::output_value_type> const &values)
    {
        underlying_stream_->write(duplicate_iacs(values));

        return values.size();
    }

    //* =====================================================================
    /// \brief Schedules an asynchronous write to the stream.
    /// 
    /// Writes an array of WriteValues to the stream.  Returns immediately.
    /// Calls callback upon completion of the write operation, passing
    /// the amount of data written as a value.
    /// \warning async_write MAY NOT return the amount of data written 
    /// synchronously, since this invalidates a set of operations.
    //* =====================================================================
    void async_write(
        odin::runtime_array<stream::output_value_type> const &values
      , stream::output_callback_type const                   &callback)
    {
        write_request request(values, callback);
        write_requests_.push_back(request);

        io_service_.post(bind(
            &impl::check_async_write_requests
          , shared_from_this()));
    }

    //* =====================================================================
    /// \brief Send a Telnet command to the datastream.
    //* =====================================================================
    void send_command(command cmd)
    {
        stream::output_value_type data[] = { IAC, cmd };
        underlying_stream_->write(data);
    }

    //* =====================================================================
    /// \brief Initiate or complete a Telnet negotiation.
    //* =====================================================================
    void send_negotiation(negotiation_request request, negotiation_type type)
    {
        stream::output_value_type data[] = { IAC, request, type };
        underlying_stream_->write(data);
    }

    //* =====================================================================
    /// \brief Send a Telnet subnegotiation to the datastream.
    //* =====================================================================
    void send_subnegotiation(
        subnegotiation_id_type const &id
      , subnegotiation_type    const &subnegotiation)
    {
        // The subnegotiation itself must have its IACs duplicated so that
        // we don't accidentally end the sequence early or something.
        odin::runtime_array<odin::u8> array = duplicate_iacs(subnegotiation);

        // The full sequence is IAC SB <id> <subnegotiation> IAC SE.
        odin::runtime_array<odin::u8> sequence(array.size() + 5);
        sequence[0] = IAC;
        sequence[1] = SB;
        sequence[2] = id;

        copy(array.begin(), array.end(), sequence.begin() + 3);

        sequence[sequence.size() - 2] = IAC;
        sequence[sequence.size() - 1] = SE;

        underlying_stream_->write(sequence);
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
    /// \brief Called when data is received from the underlying stream.
    //* =====================================================================
    void read_complete(
        runtime_array<byte_stream::input_value_type> const &values)
    {
        copy(values.begin(), values.end(), back_inserter(input_buffer_));
        async_read_request_fired_ = false;
        
        io_service_.post(bind(
            &impl::check_async_read_requests
          , shared_from_this()));
    }
        
    //* =====================================================================
    /// \brief Called when data is received from the underlying stream.
    //* =====================================================================
    void write_complete(stream::output_size_type const &amount)
    {
        write_request &request = write_requests_[0];

        if (request.callback_)
        {
            request.callback_(request.values_.size());
        }

        write_requests_.pop_front();
        async_write_request_fired_ = false;

        io_service_.post(bind(
            &impl::check_async_write_requests
          , shared_from_this()));
    }

    //* =====================================================================
    /// \brief Checks whether the top async read request can be fulfilled.
    //* =====================================================================
    void check_async_read_requests()
    {
        // If there are no read requests, then we have nothing to do.
        if (read_requests_.empty())
        {
            return;
        }
        
        // Fulfill the top request as much as possible.
        BOOST_AUTO(input_buffer_pos, input_buffer_.begin());
        read_request &request = read_requests_[0];
        
        while (input_buffer_pos   != input_buffer_.end()
            && request.fulfilled_ != request.values_.size())
        {
            BOOST_AUTO(value, filter_(*input_buffer_pos++));
            
            if (value)
            {
                request.values_[request.fulfilled_++] = *value;
            }
        }
        
        // Erase any data used to fulfill the top request. 
        input_buffer_.erase(
            input_buffer_.begin()
          , input_buffer_pos);
        
        // If the request was fulfilled, perform the necessary callback.
        if (request.fulfilled_ == request.values_.size())
        {
            if (request.callback_)
            {
                request.callback_(request.values_);
            }
            
            read_requests_.pop_front();

            // It may be possible to fulfill more requests from the data in
            // the input buffer.  Schedule another check.
            io_service_.post(
                bind(&impl::check_async_read_requests, shared_from_this()));
        }
        else
        {
            // Otherwise, schedule an asynchronous operation which will read
            // the smallest amount of data necessary to fulfill the request.
            BOOST_AUTO(amount, request.values_.size() - request.fulfilled_);

            underlying_stream_->async_read(
                amount
              , bind(&impl::read_complete, shared_from_this(), _1));
            
            async_read_request_fired_ = true;
        }
    }

    //* =====================================================================
    /// \brief Checks whether the top async write request can be fulfilled.
    //* =====================================================================
    void check_async_write_requests()
    {
        if (write_requests_.empty() || async_write_request_fired_)
        {
            return;
        }

        write_request &request = write_requests_[0];

        underlying_stream_->async_write(
            duplicate_iacs(request.values_)
          , bind(&impl::write_complete, shared_from_this(), _1));

        async_write_request_fired_ = true;
    }

    //* =====================================================================
    /// \brief Returns an array of values identical to the one passed, except
    /// that any IAC values have been duplicated.
    //* =====================================================================
    odin::runtime_array<odin::u8> duplicate_iacs(
        odin::runtime_array<odin::u8> const &array)
    {
        odin::runtime_array<odin::u8>::size_type number_of_iacs = 0;

        BOOST_FOREACH(odin::u8 value, array)
        {
            if (value == IAC)
            {
                ++number_of_iacs;
            }
        }

        odin::runtime_array<odin::u8> duplicate_array(
            array.size() + number_of_iacs);

        odin::runtime_array<odin::u8>::size_type pos = 0;

        BOOST_FOREACH(odin::u8 value, array)
        {
            duplicate_array[pos++] = value;

            if (value == IAC)
            {
                duplicate_array[pos++] = value;
            }
        }

        return duplicate_array;
    }

    struct read_request
    {
        read_request(
            stream::input_size_type            size
          , stream::input_callback_type const &callback)
          : values_(size)
          , callback_(callback)
          , fulfilled_(0)
        {
        }

        odin::runtime_array<stream::input_value_type> values_;
        stream::input_callback_type                   callback_;
        stream::input_size_type                       fulfilled_;
    };
    
    struct write_request
    {
        write_request(
            odin::runtime_array<stream::output_value_type> const &values
          , stream::output_callback_type const                   &callback)
          : values_(values)
          , callback_(callback)
        {
        }

        odin::runtime_array<stream::input_value_type> values_;
        stream::output_callback_type                  callback_;
    };

    shared_ptr<odin::io::byte_stream>       underlying_stream_;
    boost::asio::io_service                &io_service_;
    deque<read_request>                     read_requests_;
    deque<write_request>                    write_requests_;
    
    mutable deque<odin::u8>                 input_buffer_;
    odin::telnet::filter                    filter_;
    bool                                    async_read_request_fired_;
    bool                                    async_write_request_fired_;
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
    return pimpl_->write(values);
}

// ==========================================================================
// ASYNC_WRITE
// ==========================================================================
void stream::async_write(
    odin::runtime_array<output_value_type> const &values
  , output_callback_type const                   &callback)
{
    pimpl_->async_write(values, callback);
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
    pimpl_->send_command(cmd);
}

// ==========================================================================
// SEND_NEGOTIATION
// ==========================================================================
void stream::send_negotiation(
    negotiation_request request
  , negotiation_type    type)
{
    pimpl_->send_negotiation(request, type);
}

// ==========================================================================
// SEND_SUBNEGOTIATION
// ==========================================================================
void stream::send_subnegotiation(
    subnegotiation_id_type const &id
  , subnegotiation_type    const &subnegotiation)
{
    pimpl_->send_subnegotiation(id, subnegotiation);
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

// ==========================================================================
// Odin Telnet Stream
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
#include "odin/telnet/stream.hpp"
#include "odin/telnet/detail/generator.hpp"
#include "odin/telnet/detail/parser.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/typeof/typeof.hpp>
#include <algorithm>
#include <functional>
#include <deque>

using namespace std;
using namespace boost;

namespace odin { namespace telnet {

// ==========================================================================
// STREAM IMPLEMENTATION STRUCTURE
// ==========================================================================
class stream::impl
    : public enable_shared_from_this<impl>
{
public :
    typedef odin::io::byte_stream::input_value_type     underlying_input_value_type;
    typedef odin::io::byte_stream::input_size_type      underlying_input_size_type;
    typedef odin::io::byte_stream::input_callback_type  underlying_input_callback_type;
    typedef odin::io::byte_stream::output_value_type    underlying_output_value_type;
    typedef odin::io::byte_stream::output_size_type     underlying_output_size_type;
    typedef odin::io::byte_stream::output_callback_type underlying_output_callback_type;

    typedef odin::telnet::stream::input_value_type      input_value_type;
    typedef odin::telnet::stream::input_size_type       input_size_type;
    typedef odin::telnet::stream::input_callback_type   input_callback_type;
    typedef odin::telnet::stream::output_value_type     output_value_type;
    typedef odin::telnet::stream::output_size_type      output_size_type;
    typedef odin::telnet::stream::output_callback_type  output_callback_type;
    
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(
        shared_ptr<odin::io::byte_stream> const &underlying_stream
      , boost::asio::io_service                 &io_service)
        : underlying_stream_(underlying_stream)
        , io_service_(io_service)
        , read_request_active_(false)
        , write_request_active_(false)
    {
    }
        
    // ======================================================================
    // AVAILABLE
    // ======================================================================
    optional<input_size_type> available() const
    {
        // If there is a read request ongoing, we must return a "would block"
        // message.
        if (!read_requests_.empty())
        {
            return optional<input_size_type>();
        }
        
        // Grab any data that is available on the underlying datastream.
        BOOST_AUTO(underlying_available, underlying_stream_->available());
        
        if (underlying_available.is_initialized())
        {
            BOOST_AUTO(
                values 
              , underlying_stream_->read(underlying_available.get()));
            
            copy(
                values.begin()
              , values.end()
              , back_inserter(unparsed_input_buffer_));
            
            parse_input_buffer();
        }
        
        return parsed_input_buffer_.empty()
             ? optional<input_size_type>()
             : optional<input_size_type>(parsed_input_buffer_.size());
    }
    
    // ======================================================================
    // READ
    // ======================================================================
    odin::runtime_array<input_value_type> read(input_size_type size)
    {
        // Block until enough data has been read to fulfill this request.
        while (parsed_input_buffer_.size() < size)
        {
            sync_underflow();
            parse_input_buffer();

            // TODO: there is a possibility of a malformed input stream
            // causing this to block forever, but still fill up with data.
            // There should be a user-configurable point at which an exception
            // is thrown.
            
            // For example:
            // if (unparsed_input_stream_.size() > 64000) { throw something; }
            
            // However, this would only occur if a client made a call to
            // read() which actually did block.  Most use cases involve calling
            // available() first to see how much can be read without blocking.
            // However, available() suffers from the same memory issue.
        }
        
        odin::runtime_array<input_value_type> result(size);
        
        copy(
            parsed_input_buffer_.begin()
          , parsed_input_buffer_.begin() + size
          , result.begin());
        
        parsed_input_buffer_.erase(
            parsed_input_buffer_.begin()
          , parsed_input_buffer_.begin() + size);
        
        return result;
    }

    // ======================================================================
    // ASYNC_READ
    // ======================================================================
    void async_read(
        input_size_type            size
      , input_callback_type const &callback)
    {
        read_requests_.push_back(read_request(size, callback));
        schedule_read_request();
    }

    // ======================================================================
    // WRITE
    // ======================================================================
    output_size_type write(odin::runtime_array<output_value_type> const &values)
    {
        BOOST_AUTO(begin, values.begin());
        BOOST_AUTO(end,   values.end());
        
        BOOST_AUTO(generated_output, (generate_(begin, end)));
        
        odin::runtime_array<
            odin::io::byte_stream::output_value_type
        > output_values(generated_output.size());
        
        copy(
            generated_output.begin()
          , generated_output.end()
          , output_values.begin());
        
        underlying_stream_->write(output_values);
        
        return values.size();
    }
    
    // ======================================================================
    // ASYNC_WRITE
    // ======================================================================
    void async_write(
        odin::runtime_array<output_value_type> const &values
      , output_callback_type const                   &callback)
    {
        write_requests_.push_back(write_request(values, callback));
        schedule_write_request();
    }
    
    // ======================================================================
    // IS_ALIVE
    // ======================================================================
    bool is_alive() const
    {
        return underlying_stream_->is_alive();
    }
    
private :
    // ======================================================================
    // PARSE
    // ======================================================================
    template <class ForwardInputIterator>
    vector<input_value_type> parse(
        ForwardInputIterator &start
      , ForwardInputIterator  end)
    {
        return parse_(start, end);
    }
    
    // ======================================================================
    // SYNC_UNDERFLOW
    // ======================================================================
    void sync_underflow()
    {
        // Read as much data as is available from the underlying 
        // datastream, to a minimum of one byte.
        BOOST_AUTO(available, underlying_stream_->available());
        
        BOOST_AUTO(amount_to_read,
            available.is_initialized()
          ? *available 
          : 1);
          
        BOOST_AUTO(data, underlying_stream_->read(amount_to_read));
            
        copy(
            data.begin()
          , data.end()
          , back_inserter(unparsed_input_buffer_));
    }
    
    // ======================================================================
    // PARSE_INPUT_BUFFER
    // ======================================================================
    void parse_input_buffer() const
    {
        // Take input from the unparsed input buffer and parse as much as
        // it as possible.  Then erase any consumed input.
        BOOST_AUTO(begin, unparsed_input_buffer_.begin());
        BOOST_AUTO(end,   unparsed_input_buffer_.end());
        
        BOOST_AUTO(parsed_elements, parse_(begin, end));
        
        copy(
            parsed_elements.begin()
          , parsed_elements.end()
          , back_inserter(parsed_input_buffer_));
        
        // Finally, erase the consumed input.
        unparsed_input_buffer_.erase(
            unparsed_input_buffer_.begin()
          , begin);
    }
    
    // ======================================================================
    // SCHEDULE_READ_REQUEST
    // ======================================================================
    void schedule_read_request()
    {
        // Schedules a call to look at the read requests asynchronously.
        io_service_.post(boost::bind(
            &impl::check_async_read_requests
          , shared_from_this()));
    }
    
    // ======================================================================
    // CHECK_ASYNC_READ_REQUESTS
    // ======================================================================
    void check_async_read_requests()
    {
        if (read_requests_.empty())
        {
            // There are no read requests.  Return immediately.
            return;
        }
        
        read_request &request = read_requests_[0];
        
        // If there is already an async read requested, then this flag will
        // be set and we don't need to do anything yet.
        if (!read_request_active_)
        {
            // See if this can be fulfilled from the currently read data.
            if (parsed_input_buffer_.size() >= request.size_)
            {
                // We can fulfill the request from the data we have available.
                odin::runtime_array<input_value_type> result(request.size_);
                
                copy(
                    parsed_input_buffer_.begin()
                  , parsed_input_buffer_.begin() + request.size_
                  , result.begin());
                
                parsed_input_buffer_.erase(
                    parsed_input_buffer_.begin()
                  , parsed_input_buffer_.begin() + request.size_);

                // If this is the only request, then it is valid for the client
                // to call available() during the callback.  Therefore, this
                // request needs to be popped off the queue before the callback
                // takes place.  So we need to copy the callback in order to
                // call it.
                input_callback_type callback = request.callback_;
                read_requests_.pop_front();
                
                if (callback != NULL)
                {
                    callback(result);
                }
            }
            else
            {
                // There is insufficient data to fulfill the request.  Perform
                // an asynchronous read on the underlying input stream.
                BOOST_AUTO(
                    underlying_available
                  , underlying_stream_->available());
                
                BOOST_AUTO(
                    amount
                  , underlying_available.is_initialized()
                      ? underlying_available.get()
                      : 1);
                
                underlying_stream_->async_read(
                    amount
                  , bind(
                        &impl::async_read_complete
                      , shared_from_this()
                      , _1));
                
                read_request_active_ = true;
            }
        }
    }
    
    // ======================================================================
    // ASYNC_READ_COMPLETE
    // ======================================================================
    void async_read_complete(
        odin::runtime_array<
            odin::io::byte_stream::input_value_type
        > const &values)
    {
        copy(
            values.begin()
          , values.end()
          , back_inserter(unparsed_input_buffer_));
        
        parse_input_buffer();

        // The read requests is now complete.  Unset the flag that records
        // this.
        read_request_active_ = false;
        
        // Schedule a check for the read requests.  One might now have been
        // fulfilled.
        schedule_read_request();
    }
    
    // ======================================================================
    // SCHEDULE_WRITE_REQUEST
    // ======================================================================
    void schedule_write_request()
    {
        // Schedules a call to look at the write requests asynchronously.
        io_service_.post(boost::bind(
            &impl::check_async_write_requests
          , shared_from_this()));
    }
    
    // ======================================================================
    // CHECK_ASYNC_WRITE_REQUESTS
    // ======================================================================
    void check_async_write_requests()
    {
        if (write_requests_.empty())
        {
            // There are no write requests.  Return immediately.
            return;
        }
        
        if (!write_request_active_)
        {
            // No request is active, so generate output for the next request
            // and send it to the underlying stream.
            write_request &request = write_requests_[0];
            
            BOOST_AUTO(begin, request.values_.begin());
            BOOST_AUTO(end,   request.values_.end());
        
            BOOST_AUTO(generated_output, (generate_(begin, end)));
            
            odin::runtime_array<
                odin::io::byte_stream::output_value_type
            > output_values(generated_output.size());
            
            copy(
                generated_output.begin()
              , generated_output.end()
              , output_values.begin());
            
            underlying_stream_->async_write(
                output_values
              , bind(
                    &impl::async_write_complete
                  , shared_from_this()
                  , _1));
            
            write_request_active_ = true;
        }
    }
    
    // ======================================================================
    // ASYNC_WRITE_COMPLETE
    // ======================================================================
    void async_write_complete(output_size_type)
    {
        write_request &request = write_requests_[0];
        
        if (request.callback_)
        {
            request.callback_(request.values_.size());
        }

        write_requests_.pop_front();        
        write_request_active_ = false;
        
        schedule_write_request();
    }
    
    // READ_REQUEST =========================================================
    //  A structure to encapsulate read requests.
    // ======================================================================
    struct read_request
    {
        read_request(
            input_size_type            size
          , input_callback_type const &callback)
            : size_(size)
            , callback_(callback)
        {
        }
            
        input_size_type     size_;
        input_callback_type callback_;
    };
    
    // WRITE_REQUEST ========================================================
    //  A structure to encapsulate write requests.
    // ======================================================================
    struct write_request
    {
        write_request(
            odin::runtime_array<output_value_type> const &values
          , output_callback_type const                   &callback)
            : values_(values)
            , callback_(callback)
        {
        }
          
        odin::runtime_array<output_value_type> values_;
        output_callback_type                   callback_;
    };
    
    // The underlying byte stream that we read from and write to.
    shared_ptr<odin::io::byte_stream> underlying_stream_;
    
    // The IO service we use for asynchronous dispatch.
    boost::asio::io_service &io_service_;
    
    // A parser for dealing with the input stream.
    odin::telnet::detail::parser parse_;
    
    // A generator for dealing with the output stream.
    odin::telnet::detail::generator generate_;
    
    // This buffer is full of unparsed input from the underlying stream.
    mutable deque<odin::u8> unparsed_input_buffer_;
    
    // This buffer is full of parsed input.
    mutable deque<input_value_type> parsed_input_buffer_;
    
    // This is the queue of asynchronous input requests.
    deque<read_request> read_requests_;
    
    // This is the queue of asynchronous output requests.
    deque<write_request> write_requests_;
    
    // A flag to record whether an asynchronous read is currently in progress.
    bool read_request_active_;
    
    // A flag to record whether an asynchronous write is currently in progress.
    bool write_request_active_;
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
boost::optional<stream::input_size_type> stream::available() const
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
    return pimpl_->is_alive();
}

}}

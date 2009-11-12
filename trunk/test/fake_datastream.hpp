#ifndef FAKE_DATASTREAM_HPP_
#define FAKE_DATASTREAM_HPP_

#include "odin/io/datastream.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <algorithm>
#include <deque>
#include <functional>
#include <stdexcept>

//* =========================================================================
/// \brief A fake datastream should not block during testing, otherwise the
/// testing blocks.  Therefore, if we detect a blocking situation, we throw a 
/// blocking error instead.
//* =========================================================================
class blocking_error
    : public std::runtime_error
{
public :
    blocking_error()
        : std::runtime_error("Blocked!")
    {
    }
};

//* =========================================================================
/// \brief A datastream for testing purposes that can be quizzed for what has
/// been fed into it and can have data inserted into it so that it can be
/// read from.
//* =========================================================================
template <class ReadValue, class WriteValue>
class fake_datastream
    : public odin::io::datastream<ReadValue, WriteValue>
    , public boost::enable_shared_from_this< 
          fake_datastream<ReadValue, WriteValue> 
      >
{
public :    
    // Some helpful typedefs to allow base classes to reduce ambiguity.
    typedef typename odin::io::datastream<ReadValue, WriteValue>::input_value_type    input_value_type;
    typedef typename odin::io::datastream<ReadValue, WriteValue>::input_size_type     input_size_type;
    typedef typename odin::io::datastream<ReadValue, WriteValue>::input_callback_type input_callback_type;

    typedef typename odin::io::datastream<ReadValue, WriteValue>::output_value_type    output_value_type;
    typedef typename odin::io::datastream<ReadValue, WriteValue>::output_size_type     output_size_type;
    typedef typename odin::io::datastream<ReadValue, WriteValue>::output_callback_type output_callback_type;
    
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    fake_datastream(boost::asio::io_service &io_service)
        : io_service_(io_service)
    {
    }

    //* =====================================================================
    /// \brief Returns the number of objects that are available to be read.
    /// \return the number of objects that are available to be read without 
    /// blocking, or no value if this is unknown and a call to read() might
    /// block.  The value 0 must only be returned in the case that there is 
    /// no more data to be read, and the stream is dead.  
    /// \warning This operation MUST NOT block.
    //* =====================================================================
    virtual boost::optional<input_size_type> available() const
    {
        return input_buffer_.empty() 
             ? boost::optional<input_size_type>()
             : boost::optional<input_size_type>(input_buffer_.size());
    }

    //* =====================================================================
    /// \brief Performs a synchronous read on the stream.
    /// \return an array of values read frmo the stream.
    /// Reads up to size items from the stream and returns them in a
    /// runtime_array.  This may block, unless a previous call to available()
    /// since the last read() yielded a positive value, which was less than or 
    /// equal to size, in which case it MUST NOT block.
    //* =====================================================================
    virtual odin::runtime_array<input_value_type> read(input_size_type size)
    {
        if (input_buffer_.size() < size)
        {
            throw blocking_error(); 
        }
        
        odin::runtime_array<input_value_type> result(size);
        
        std::copy(
            input_buffer_.begin()
          , input_buffer_.begin() + size
          , result.begin());
        
        input_buffer_.erase(
            input_buffer_.begin()
          , input_buffer_.begin() + size);
        
        return result;
    }

    //* =====================================================================
    /// \brief Schedules an asynchronous read on the stream.
    /// 
    /// Sends a request to read size elements from the stream.  Returns
    /// immediately.  Calls callback upon completion of the read operation,
    /// passing an array of the values read as a value.
    /// \warning async_read MUST NOT return the read values synchronously, 
    /// since this invalidates a set of operations.
    //* =====================================================================
    virtual void async_read(
        input_size_type            size
      , input_callback_type const &callback)
    {
        read_request request = { size, callback };
        read_requests_.push_back(request);
        
        post_check_read();
    }

    //* =====================================================================
    /// \brief Perform a synchronous write to the stream.
    /// \return the number of objects written to the stream.
    /// Write an array of WriteValues to the stream.  
    //* =====================================================================
    virtual output_size_type write(
        odin::runtime_array<output_value_type> const& values)
    {
        std::copy(
            values.begin()
          , values.end()
          , std::back_inserter(output_buffer_));
        
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
    virtual void async_write(
        odin::runtime_array<output_value_type> const &values
      , output_callback_type const                   &callback)
    {
        write_request request = { values, callback };
        write_requests_.push_back(request);
        
        post_check_write();
    }
    
    //* =====================================================================
    /// \brief Check to see if the underlying stream is still alive.
    /// \return true if the underlying stream is alive, false otherwise.
    //* =====================================================================
    virtual bool is_alive() const
    {
        return true;
    }

    //* =====================================================================
    /// \brief Inserts the data into the read buffer so that read() and
    /// async_read() can return it.
    //* =====================================================================
    void write_data_to_read(
        odin::runtime_array<input_value_type> const &values)
    {
        std::copy(
            values.begin()
          , values.end()
          , std::back_inserter(input_buffer_));
        
        post_check_read();
    }
    
    //* =====================================================================
    /// \brief Retrieves the data that has been written through this
    /// datastream.  Note: Destructive read.
    //* =====================================================================
    odin::runtime_array<output_value_type> read_data_written()
    {
        odin::runtime_array<output_value_type> result(output_buffer_.size());
        
        std::copy(
            output_buffer_.begin()
          , output_buffer_.end()
          , result.begin());
        
        output_buffer_.clear();
        
        return result;
    }
    
private :
    //* =====================================================================
    /// \brief Models an async_read request from a client.
    //* =====================================================================
    struct read_request
    {
        input_size_type     size_;
        input_callback_type callback_;
    };
    
    //* =====================================================================
    /// \brief Models an async_write request from a client.
    //* =====================================================================
    struct write_request
    {
        odin::runtime_array<output_value_type> values_;
        output_callback_type                   callback_;
    };
    
    //* =====================================================================
    /// \brief Helper function to post a check_read() function call to the
    /// io_service.
    //* =====================================================================
    void post_check_read()
    {
        io_service_.post(
            boost::bind(
                &fake_datastream::check_read
              , this->shared_from_this()));
    }
    
    //* =====================================================================
    /// \brief Checks to see if there is enough data in the input buffer to
    /// fulfil the topmost read request.  If there is, that is handled and
    /// another check is posted.
    //* =====================================================================
    void check_read()
    {
        if (read_requests_.empty())
        {
            return;
        }
        
        read_request const &request = read_requests_.front();
        
        if (input_buffer_.size() >= request.size_)
        {
            odin::runtime_array<input_value_type> result(request.size_);
            
            std::copy(
                input_buffer_.begin()
              , input_buffer_.begin() + request.size_
              , result.begin());
            
            input_buffer_.erase(
                input_buffer_.begin()
              , input_buffer_.begin() + request.size_);
            
            if (request.callback_)
            {
                request.callback_(result);
            }
            
            read_requests_.pop_front();
            
            post_check_read();
        }
    }
    
    //* =====================================================================
    /// \brief Helper function to post a check_write() function call to the
    /// io_service.
    //* =====================================================================
    void post_check_write()
    {
        io_service_.post(
            boost::bind(
                &fake_datastream::check_write
              , this->shared_from_this()));
    }
    
    //* =====================================================================
    /// \brief Checks to see if there is enough data in the input buffer to
    /// fulfil the topmost write request.  If there is, that is handled and
    /// another check is posted.
    //* =====================================================================
    void check_write()
    {
        if (write_requests_.empty())
        {
            return;
        }
        
        write_request const &request = write_requests_.front();
        
        std::copy(
            request.values_.begin()
          , request.values_.end()
          , std::back_inserter(output_buffer_));
        
        if (request.callback_)
        {
            request.callback_(request.values_.size());
        }
        
        write_requests_.pop_front();
        
        post_check_write();
    }
        
    boost::asio::io_service       &io_service_;
    std::deque<read_request>       read_requests_;
    std::deque<write_request>      write_requests_;
    std::deque<input_value_type>   input_buffer_;
    std::deque<output_value_type>  output_buffer_;
};

#endif

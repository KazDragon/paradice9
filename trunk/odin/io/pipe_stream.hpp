// ==========================================================================
// Odin Pipe Stream.
//
// Copyright (C) 2003 Matthew Chaplain, All Rights Reserved.
// This file is covered by the MIT Licence:
//
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.
// ==========================================================================
#ifndef ODIN_PIPE_STREAM_HPP_
#define ODIN_PIPE_STREAM_HPP_

#include "odin/io/datastream.hpp"
#include <boost/shared_ptr.hpp>

namespace odin { namespace io {

// PIPE_STREAM ==============================================================
//  FUNCTION : A datastream which simply passes the data on to a containing
//             datastream.  This could, for example, be used to explicitly
//             denote the top of a protocol stack.
//
//  CONCEPTS : ReadValue  - the type of data that is read from the stream.
//             WriteValue - the type of data that is written to the stream.
//
//  USAGE    : Construct, passing in a shared_ptr<> to a datastream.  use
//             as a normal stream. 
// ==========================================================================
template <class ReadValue, class WriteValue>
class pipe_stream : public odin::io::datastream<ReadValue, WriteValue>
{

private :
    typedef boost::shared_ptr<
        odin::io::datastream<ReadValue, WriteValue>
    > stream_ptr;

public :
    /*
    pipe_stream()
        : stream_(stream_ptr(new sink_stream<element_t>))
    {
    }
    */
    
    explicit pipe_stream(stream_ptr const &stream)
        : stream_(stream)
    {
    }

    virtual ~pipe_stream() {}

public :
    // Returns the number of objects that are available to be read without
    // blocking, or no value if this is unknown and a call to read() might
    // block.  The value 0 must only be returned in the case that there is
    // no more data to be read, and the stream is dead.  This operation must 
    // not block.
    virtual boost::optional
    <
        typename odin::runtime_array<ReadValue>::size_type
    > available() const
    {
        return boost::optional<
            typename odin::runtime_array<ReadValue>::size_type
        >();
    }

    // Reads up to size items from the stream and returns them in a runtime
    // array.  This may block, unless a previous call to Available() since the
    // last Read() yielded a positive value, which was less than or equal to
    // size, in which case it must not block.
    virtual odin::runtime_array<ReadValue> read(
        typename odin::runtime_array<ReadValue>::size_type size)
    {
        return stream_->read(size);
    }

    // Sends a request to read size elements from the stream.  Returns
    // immediately.  Calls callback upon completion of the read operation,
    // passing an array of the values read as a value.
    // Note: AsyncRead MAY NOT return the read values synchronously, since this
    // invalidates a set of operations.
    virtual void async_read(
        typename odin::runtime_array<ReadValue>::size_type            size,
        boost::function<void (odin::runtime_array<ReadValue>)> const& callback)
    {
    }

    // Write an array of WriteValues to the stream.  Returns the
    // actual number of objects written.
    virtual typename odin::runtime_array<WriteValue>::size_type write(
        odin::runtime_array<WriteValue> const& values)
    {
        return stream_->write(values);
    }

    // Writes an array of WriteValues to the stream.  Returns immediately.
    // Calls callback upon completion of the write operation, passing
    // the amount of data written as a value.
    // Note: async_write MAY NOT return the amount of data written 
    // synchronously, since this invalidates a set of operations.
    virtual void async_write(
        odin::runtime_array<WriteValue>                                             const& values,
        boost::function<void (typename odin::runtime_array<WriteValue>::size_type)> const& callback)
    {
    }

    // Returns true if the underlying stream is alive, false otherwise.
    virtual bool is_alive() const
    {
        return stream_->is_alive();
    }

    /*
    // Write an array of WriteValues to the stream.  Returns the
    // actual number of objects written.
    virtual typename odin::runtime_array<WriteValue>::size_type write(
        odin::runtime_array<WriteValue> const& values) = 0;
    virtual unsigned int write(
        element_t const *data
      , unsigned int        length)
    {
        return stream_->write(data, length);
    }

    virtual unsigned int read(element_t *data, unsigned int length)
    {
        return stream_->read(data, length);
    }

    virtual void close()
    {
        stream_->close();
    }

    virtual int available() const
    {
        return stream_->available();
    }

    virtual bool is_alive() const
    {
        return stream_->is_alive();
    }
    
    virtual locked lock()
    {
        return stream_->lock();
    }
    */
    
private :
    stream_ptr stream_;
};

}}

#endif


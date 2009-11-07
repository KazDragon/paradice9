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

//* =========================================================================
/// \brief A datastream that passes data onto a containing datastream.
/// \param ReadValue the type of data that is read from the stream.
/// \param WriteValue the type of data that is written to the stream.
//* =========================================================================
template <class ReadValue, class WriteValue>
class pipe_stream : public odin::io::datastream<ReadValue, WriteValue>
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    explicit pipe_stream(
        boost::shared_ptr<
            odin::io::datastream<ReadValue, WriteValue>
        > const &underlying_stream)
        : underlying_stream_(underlying_stream)
    {
    }

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~pipe_stream() {}

public :
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
        return underlying_stream_->available();
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
        return underlying_stream_->read(size);
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
        underlying_stream_->async_read(size, callback);
    }

    //* =====================================================================
    /// \brief Perform a synchronous write to the stream.
    /// \return the number of objects written to the stream.
    /// Write an array of WriteValues to the stream.  
    //* =====================================================================
    virtual output_size_type write(
        odin::runtime_array<output_value_type> const &values)
    {
        return underlying_stream_->write(values);
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
        underlying_stream_->async_write(values, callback);
    }

    //* =====================================================================
    /// \brief Check to see if the underlying stream is still alive.
    /// \return true if the underlying stream is alive, false otherwise.
    //* =====================================================================
    virtual bool is_alive() const
    {
        return underlying_stream_->is_alive();
    }
    
private :
    boost::shared_ptr<
        odin::io::datastream<ReadValue, WriteValue>
    > underlying_stream_;
};

}}

#endif


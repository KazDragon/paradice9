// ==========================================================================
// Odin Input Datastream.
//
// Copyright (C) 2009 Matthew Chaplain, All Rights Reserved.
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
#ifndef ODIN_INPUT_DATASTREAM_H_
#define ODIN_INPUT_DATASTREAM_H_

#include "odin/runtime_array.hpp"
#include <boost/function.hpp>
#include <boost/optional.hpp>

namespace odin { namespace io {

//* =========================================================================
/// \interface input_datastream
/// \brief An interface that models an input datastream.
//* =========================================================================
template <class ReadValue>
struct input_datastream
{
    typedef ReadValue                                          value_type;
    typedef typename odin::runtime_array<ReadValue>::size_type size_type;
    
    typedef boost::function
    <
        void (odin::runtime_array<ReadValue>)
    > callback_type;
    
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~input_datastream() {}

    //* =====================================================================
    /// \brief Returns the number of objects that are available to be read.
    /// \return the number of objects that are available to be read without 
    /// blocking, or no value if this is unknown and a call to read() might
    /// block.  The value 0 must only be returned in the case that there is 
    /// no more data to be read, and the stream is dead.  
    /// \warning This operation MUST NOT block.
    //* =====================================================================
    virtual boost::optional<size_type> available() const = 0;

    //* =====================================================================
    /// \brief Performs a synchronous read on the stream.
    /// \return an array of values read frmo the stream.
    /// Reads up to size items from the stream and returns them in a
    /// runtime_array.  This may block, unless a previous call to available()
    /// since the last read() yielded a positive value, which was less than or 
    /// equal to size, in which case it MUST NOT block.
    //* =====================================================================
    virtual odin::runtime_array<value_type> read(size_type size) = 0;

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
        size_type            size
      , callback_type const &callback) = 0;

    //* =====================================================================
    /// \brief Check to see if the underlying stream is still alive.
    /// \return true if the underlying stream is alive, false otherwise.
    //* =====================================================================
    virtual bool is_alive() const = 0;
};

}}

#endif

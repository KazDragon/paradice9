// ==========================================================================
// Odin Datastream.
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
#ifndef ODIN_DATASTREAM_H_
#define ODIN_DATASTREAM_H_

#include "odin/io/input_datastream.hpp"
#include "odin/io/output_datastream.hpp"
#include "odin/types.hpp"

namespace odin { namespace io {

//* =========================================================================
/// \interface datastream
/// \brief An interface that models an input/output datastream.
//* =========================================================================
template <class ReadValue, class WriteValue>
struct datastream
    : odin::io::input_datastream<ReadValue>
    , odin::io::output_datastream<WriteValue>
{
    // Some helpful typedefs to allow derived classes to reduce ambiguity.
    typedef typename odin::io::input_datastream<ReadValue>::value_type      input_value_type;
    typedef typename odin::io::input_datastream<ReadValue>::storage_type    input_storage_type;
    typedef typename odin::io::input_datastream<ReadValue>::size_type       input_size_type;
    typedef typename odin::io::input_datastream<ReadValue>::callback_type   input_callback_type;

    typedef typename odin::io::output_datastream<WriteValue>::value_type    output_value_type;
    typedef typename odin::io::output_datastream<WriteValue>::storage_type  output_storage_type;
    typedef typename odin::io::output_datastream<WriteValue>::size_type     output_size_type;
    typedef typename odin::io::output_datastream<WriteValue>::callback_type output_callback_type;

    // We must re-define is_alive here to remove an ambiguity of clients as to
    // whether they are calling input_datastream<>::is_alive() or
    // output_datastream<>::is_alive(), even though they're both the same 
    // function.

    //* =====================================================================
    /// \brief Check to see if the underlying stream is still alive.
    /// \return true if the underlying stream is alive, false otherwise.
    //* =====================================================================
    virtual bool is_alive() const = 0;
};

// Some handy typedefs for everyone to use.
typedef datastream<odin::u8, odin::u8> byte_stream;
typedef datastream<char, char>         char_stream;

}}

#endif

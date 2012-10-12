// ==========================================================================
// Odin Net Socket
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
#ifndef ODIN_NET_SOCKET_HPP_
#define ODIN_NET_SOCKET_HPP_

#include "odin/io/datastream.hpp"
#include "odin/types.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/shared_ptr.hpp>

namespace odin { namespace net {

class socket 
    : public odin::io::datastream<odin::u8, odin::u8>
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    socket(
        boost::shared_ptr<boost::asio::ip::tcp::socket> const &socket);

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~socket();

    //* =====================================================================
    /// \brief Close the socket, allowing no further transmission.
    //* =====================================================================
    void close();

    //* =====================================================================
    /// \brief Returns the number of objects that are available to be read.
    /// \return the number of objects that are available to be read without 
    /// blocking, or no value if this is unknown and a call to read() might
    /// block.  The value 0 must only be returned in the case that there is 
    /// no more data to be read, and the stream is dead.  
    /// \warning This operation MUST NOT block.
    //* =====================================================================
    virtual boost::optional<input_size_type> available() const;

    //* =====================================================================
    /// \brief Performs a synchronous read on the stream.
    /// \return an array of values read frmo the stream.
    /// Reads up to size items from the stream and returns them in a
    /// vector.  This may block, unless a previous call to available()
    /// since the last read() yielded a positive value, which was less than or 
    /// equal to size, in which case it MUST NOT block.
    //* =====================================================================
    virtual input_storage_type read(input_size_type size);

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
      , input_callback_type const &callback);

    //* =====================================================================
    /// \brief Perform a synchronous write to the stream.
    /// \return the number of objects written to the stream.
    /// Write an array of odin::u8s to the stream.  
    //* =====================================================================
    virtual output_size_type write(output_storage_type const& values);

    //* =====================================================================
    /// \brief Schedules an asynchronous write to the stream.
    /// 
    /// Writes an array of odin::u8s to the stream.  Returns immediately.
    /// Calls callback upon completion of the write operation, passing
    /// the amount of data written as a value.
    /// \warning async_write MAY NOT return the amount of data written 
    /// synchronously, since this invalidates a set of operations.
    //* =====================================================================
    virtual void async_write(
        output_storage_type  const &values
      , output_callback_type const &callback);

    //* =====================================================================
    /// \brief Check to see if the underlying stream is still alive.
    /// \return true if the underlying stream is alive, false otherwise.
    //* =====================================================================
    virtual bool is_alive() const;

    //* =====================================================================
    /// \brief Retrieve the io_service instance that this socket uses for
    /// synchronisation.
    //* =====================================================================
    boost::asio::io_service &get_io_service();
    
    //* =====================================================================
    /// \brief Register a callback to be performed when the socket is closed.
    //* =====================================================================
    void on_death(boost::function<void ()> callback);

private :
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}}

#endif

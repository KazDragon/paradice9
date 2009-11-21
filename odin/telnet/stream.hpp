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
#ifndef ODIN_TELNET_STREAM_HPP_
#define ODIN_TELNET_STREAM_HPP_

#include "odin/io/datastream.hpp"
#include "odin/telnet/protocol.hpp"
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace boost { namespace asio {

class io_service;

}}

namespace odin { namespace telnet {

class initiated_negotiation;
class completed_negotiation;
        
//* =========================================================================
/// \brief A wrapper around a datastream that provides Telnet protocol
/// capabilities.
///
/// \par Behaviour
/// For odin::telnet::stream, the parts of data that are Telnet data are
/// considered to be blocking - it is possible that a read of these data
/// yield no bytes of actual data.
//* =========================================================================
class stream 
    : public  odin::io::byte_stream
    , private boost::noncopyable
{
public :
    //* =====================================================================
    /// \brief The callback type used to register for notification of when
    /// a Telnet command has been received.
    //* =====================================================================
    typedef boost::function
    <
        void (command)
    > command_callback;

    //* =====================================================================
    /// \brief The callback type used to register for notification of when
    /// a Telnet negotiation has been made by the remote end point.
    //* =====================================================================
    typedef boost::function
    <
        void (negotiation_request, negotiation_type)
    > negotiation_callback;
    
    //* =====================================================================
    /// \brief The callback type used to register for notification of when
    /// a Telnet subnegotiation has been received.
    //* =====================================================================
    typedef ::boost::function
    <
        void (subnegotiation_id_type, subnegotiation_type)
    > subnegotiation_callback;

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    explicit stream(
        boost::shared_ptr<odin::io::byte_stream> const &underlying_stream
      , boost::asio::io_service                        &io_service);

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~stream();
    
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
    /// runtime_array.  This may block, unless a previous call to available()
    /// since the last read() yielded a positive value, which was less than or 
    /// equal to size, in which case it MUST NOT block.
    //* =====================================================================
    virtual odin::runtime_array<input_value_type> read(input_size_type size);

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
    /// Write an array of WriteValues to the stream.  
    //* =====================================================================
    virtual output_size_type write(
        odin::runtime_array<output_value_type> const& values);

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
      , output_callback_type const                   &callback);
    
    //* =====================================================================
    /// \brief Check to see if the underlying stream is still alive.
    /// \return true if the underlying stream is alive, false otherwise.
    //* =====================================================================
    virtual bool is_alive() const;

    //* =====================================================================
    /// \brief Send a Telnet command to the datastream.
    //* =====================================================================
    void send_command(command cmd);

    //* =====================================================================
    /// \brief Initiate or complete a Telnet negotiation.
    //* =====================================================================
    void send_negotiation(negotiation_request request, negotiation_type type);
    
    //* =====================================================================
    /// \brief Send a Telnet subnegotiation to the datastream.
    //* =====================================================================
    void send_subnegotiation(
        subnegotiation_id_type const &id
      , subnegotiation_type    const &subnegotiation);
    
    //* =====================================================================
    /// \brief Register for notification of incoming Telnet commands.
    //* =====================================================================
    void on_command(command_callback const &callback);
    
    //* =====================================================================
    /// \brief Register for notification of incoming negotiation requests.
    //* =====================================================================
    void on_negotiation(negotiation_callback const &callback);

    //* =====================================================================
    /// \brief Register for notification of incoming subnegotiations.
    //* =====================================================================
    void on_subnegotiation(subnegotiation_callback const &callback);
    
private :
    class impl;
    boost::shared_ptr<impl> pimpl_;
};

}}

#endif


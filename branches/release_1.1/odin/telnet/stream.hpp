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
#ifndef ODIN_TELNET_STREAM_HPP_
#define ODIN_TELNET_STREAM_HPP_

#include "odin/telnet/protocol.hpp"
#include "odin/io/datastream.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>
#include <string>

namespace boost { namespace asio {
    class io_service;
}}

namespace odin { namespace telnet {

namespace detail {
    typedef boost::variant<
        odin::telnet::command
      , odin::telnet::negotiation_type
      , odin::telnet::subnegotiation_type
      , std::string
    > element_type;
}

//* =========================================================================
/// \brief A class that represents a telnet datastream.
///
/// \par Summary
/// odin::telnet::stream reads and writes elements that are a variant of
/// the protocol units contained within the Telnet protocol: commands,
/// negotiation, subnegotiations and plain text.  These are converted to
/// and from the underlying byte stream.
/// \par Usage
/// Several classes have been made to help the usage of odin::telnet::stream.
/// \par
/// In the odin::telnet::options directory are implementations of various
/// options, such as odin::telnet::option::naws_client, which implementes the
/// client portion of the Telnet Negotiation About Window Size option (that is,
/// the receiver of information from a remote client that will send information
/// on window size updates).
/// \par
/// These automatically hook into router objects contained in the odin::telnet
/// namespace whose job is to forward negotiations and subnegotiations on to
/// the correct options.
/// \par
/// Finally, the odin::telnet::input_visitor class is used to forward input
/// received from the stream to the correct handler.  A sample set up might
/// be:
///
/// \code
/// // Construct the telnet stream around a given underlying byte stream.
/// boost::shared_ptr<odin::telnet::stream> stream(
///     new odin::telnet::stream(underlying_stream));
///
/// // Construct the various routers used by the options and the input_visitor.
/// boost::shared_ptr<odin::telnet::command_router> command_router(
///     new odin::telnet::command_router);
/// boost::shared_ptr<odin::telnet::negotiation_router> negotiation_router(
///     new odin::telnet::negotiation_router);
/// boost::shared_ptr<odin::telnet::subnegotiation_router> subnegotiation_router(
///     new odin::telnet::subnegotiation_router);
///
/// // Also set up a callback for any plain text that comes through.
/// boost::function<void (std::string)> string_handler = boost::bind(
///     &my_string_handler, _1);
///
/// Construct an input_visitor to handle the input.
/// odin::telnet::input_visitor input_visitor(
///     command_router, negotiation_router, subnegotiation_router, string_handler);
///
/// // Create, allow to be activated, and activate the NAWS option.  Activation
/// // sends a DO NAWS over the telnet stream.
/// boost::shared_ptr<odin::telnet::options::naws_client> naws_client(
///     new odin::telnet::options::naws_client(
///         stream, negotiation_router, subnegotiation_router));
/// naws_client->set_activatable(true);
/// naws_client->activate();
///
/// // Read some data from the stream (quite possibly WILL NAWS or WONT NAWS
/// // in response to the naws_client requesting activation).
/// std::vector<odin::telnet::stream::input_value_type> input_values =
///     stream->read(odin::telnet::stream::input_size_type(1));
///
/// // Hand this input to the input handler.  There is a helper function
/// // to apply the visitor across the entire received input array.
/// odin::telnet::apply_input_range(input_visitor, input_values);
/// \endcode
/// \par
/// Assuming that the response is WILL NAWS, then the following occurs:
/// A vector containing a single element, the WILL NAWS negotiation,
/// is read from the stream.  The individual elements are handed off to the
/// input_visitor, which discriminates the negotiation from the other possible
/// inputs (such as subnegotiations or plain text).  This causes it to be
/// handed off to the negotiation_router.  That router detects that negotiations
/// of that type should be handed off to the naws_client, and does so.
/// The naws_client is now active.
///  
//* =========================================================================
class stream
    : public odin::io::datastream<
          odin::telnet::detail::element_type
        , odin::telnet::detail::element_type
      >
{
public :
    
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    stream(
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
    /// block.  Note: if a stream is serving any asynchronous read requests,
    /// it must return at most the amount already buffered by the stream,
    /// otherwise that it might block. The value 0 must only be returned
    /// in the case that there is no more data to be read, and the stream is 
    /// dead. 
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
    /// Write an array of WriteValues to the stream.  
    //* =====================================================================
    virtual output_size_type write(output_storage_type const &values);

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
        input_storage_type const   &values
      , output_callback_type const &callback);
    
    //* =====================================================================
    /// \brief Check to see if the underlying stream is still alive.
    /// \return true if the underlying stream is alive, false otherwise.
    //* =====================================================================
    virtual bool is_alive() const;
    
private :
    class impl;
    boost::shared_ptr<impl> pimpl_;
};

}}

#endif


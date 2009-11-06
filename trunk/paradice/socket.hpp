#pragma once

#include "odin/io/datastream.hpp"
#include "odin/types.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/shared_ptr.hpp>

namespace paradice {

class socket 
    : public odin::io::datastream<odin::u8, odin::u8>
{
public :
    socket(
        boost::shared_ptr<boost::asio::ip::tcp::socket> const &socket);

    virtual ~socket();

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
    virtual odin::runtime_array<odin::u8> read(input_size_type size);

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
    virtual output_size_type write(
        odin::runtime_array<odin::u8> const& values);

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
        odin::runtime_array<odin::u8> const &values
      , output_callback_type const          &callback);

    //* =====================================================================
    /// \brief Check to see if the underlying stream is still alive.
    /// \return true if the underlying stream is alive, false otherwise.
    //* =====================================================================
    virtual bool is_alive() const;

    //
    boost::asio::io_service &get_io_service();
    
    //
    void on_death(boost::function<void ()> callback);

    void kill();

private :
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}

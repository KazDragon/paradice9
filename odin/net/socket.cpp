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
#include "socket.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <deque>

using namespace boost;
using namespace boost::asio;
using namespace boost::system;
using namespace odin;
using namespace odin::io;
using namespace std;

namespace odin { namespace net {

// ==========================================================================
// SOCKET::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct socket::impl : enable_shared_from_this<impl>
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(shared_ptr<asio::ip::tcp::socket> const &socket)
        : socket_(socket) 
        , is_alive_(true)
    {
    }
        
    // ======================================================================
    // CLOSE
    // ======================================================================
    void close()
    {
        socket_->close();
    }

    // ======================================================================
    // READ_REQUEST
    // ======================================================================
    struct read_request
    {
        // ==================================================================
        // CONSTRUCTOR
        // ==================================================================
        read_request(
            runtime_array<u8>::size_type               amount_requested
          , input_datastream<u8>::callback_type const &callback)
          : values_(amount_requested)
          , callback_(callback)
          , scheduled_(false)
        {
        }

        runtime_array<u8>                   values_;
        input_datastream<u8>::callback_type callback_;
        bool                                scheduled_;
    };

    // ======================================================================
    // WRITE_REQUEST
    // ======================================================================
    struct write_request
    {
        // ==================================================================
        // CONSTRUCTOR
        // ==================================================================
        write_request(
            runtime_array<u8> const                    &values
          , output_datastream<u8>::callback_type const &callback)
          : values_(values)
          , callback_(callback)
        {
        }

        runtime_array<u8>                    values_;
        output_datastream<u8>::callback_type callback_;
    };

    // ======================================================================
    // AVAILABLE
    // ======================================================================
    optional<socket::input_size_type> available() const
    {
        if (!is_alive())
        {
            return optional<input_size_type>(0);
        }
        else if (read_requests_.empty())
        {
            asio::socket_base::bytes_readable command(true);
            boost::system::error_code ec;
            socket_->io_control(command, ec);
            
            if (!ec)
            {
                std::size_t bytes_readable = command.get();
        
                return bytes_readable == 0
                     ? optional<input_size_type>()
                     : optional<input_size_type>(bytes_readable);
            }
            else
            {
                return optional<input_size_type>(0);
            }
        }
        else
        {
            return optional<input_size_type>();
        }
    }
    
    // ======================================================================
    // READ
    // ======================================================================
    runtime_array<u8> read(input_size_type size)
    {
        runtime_array<u8> data(size);
    
        boost::system::error_code ec;
        socket_->read_some(boost::asio::buffer(data.begin(), size), ec);

        /* INPUT DEBUGGING 
        for(size_t i = 0; i < data.size(); ++i)
        {
            char ch[2] = { data[i], 0 };
            
            printf("[0x%02X] %s\n", 
                (int)(unsigned char)ch[0]
              , isprint(ch[0]) ? ch : "(*)");
        }
        */
        
        return data;
    }
    
    // ======================================================================
    // ASYNC_READ
    // ======================================================================
    void async_read(
        runtime_array<u8>::size_type               amount_requested
      , input_datastream<u8>::callback_type const &callback)
    {
        read_request request(amount_requested, callback);
        read_requests_.push_back(request);

        if (!read_requests_[0].scheduled_)
        {
            boost::asio::async_read(
                *socket_.get()
              , buffer(
                    read_requests_[0].values_.begin()
                  , read_requests_[0].values_.size())
              , bind(
                    &impl::read_complete
                  , shared_from_this()
                  , boost::asio::placeholders::error
                  , boost::asio::placeholders::bytes_transferred));
            
            read_requests_[0].scheduled_ = true;
        }
    }

    // ======================================================================
    // WRITE
    // ======================================================================
    socket::output_size_type write(runtime_array<u8> const &values)
    {
        boost::system::error_code ec;
        return socket_->write_some(
            asio::buffer(values.begin(), values.size()), ec);
    }
    
    // ======================================================================
    // ASYNC_WRITE
    // ======================================================================
    void async_write(
        runtime_array<u8> const                    &values
      , output_datastream<u8>::callback_type const &callback)
    {
        bool write_now = write_requests_.empty();

        write_request request(values, callback);
        write_requests_.push_back(request);

        if (write_now)
        {
            write_first_request();
        }
    }

    // ======================================================================
    // IS_ALIVE
    // ======================================================================
    bool is_alive() const
    {
        return is_alive_;
    }

    // ======================================================================
    // GET_IO_SERVICE
    // ======================================================================
    io_service &get_io_service()
    {
        return socket_->get_io_service();
    }
    
    // ======================================================================
    // ON_DEATH
    // ======================================================================
    void on_death(boost::function<void ()> callback)
    {
        on_death_ = callback;
    }
    
private :
    // ======================================================================
    // WRITE_FIRST_REQUEST
    // ======================================================================
    void write_first_request()
    {
        boost::asio::async_write(
            *socket_.get()
          , buffer(
                write_requests_.front().values_.begin()
              , write_requests_.front().values_.size())
          , bind(
                &impl::write_complete
              , shared_from_this()
              , boost::asio::placeholders::error
              , boost::asio::placeholders::bytes_transferred));
    }

    // ======================================================================
    // WRITE_COMPLETE
    // ======================================================================
    void write_complete(
        error_code const &error
      , size_t            bytes_transferred)
    {
        if (!is_alive_)
        {
            return;
        }

        if (!error)
        {
            if (write_requests_.front().callback_)
            {
                write_requests_.front().callback_(bytes_transferred);
            }

            write_requests_.pop_front();

            if (!write_requests_.empty())
            {
                write_first_request();
            }
        }
        else
        {
            is_alive_ = false;
            
            if (on_death_)
            {
                on_death_();
            }
        }
    }

    // ======================================================================
    // READ_COMPLETE
    // ======================================================================
    void read_complete(
        error_code const &error
      , size_t            bytes_transferred)
    {
        if (!is_alive_)
        {
            return;
        }

        if (!error)
        {
            /* INPUT DEBUGGING 
            for(size_t i = 0; i < bytes_transferred; ++i)
            {
                char ch[2] = { read_requests_.front().values_[i], 0 };
                
                printf("[0x%02X] %s\n", 
                    (int)(unsigned char)ch[0]
                  , isprint(ch[0]) ? ch : "(*)");
            }
            */

            if (bytes_transferred >= read_requests_.front().values_.size())
            {
                if (read_requests_.front().callback_)
                {
                    read_requests_.front().callback_(
                        read_requests_.front().values_);
                }

                read_requests_.pop_front();

                if (!read_requests_.empty())
                {
                    boost::asio::async_read(
                        *socket_.get()
                      , buffer(
                            read_requests_.front().values_.begin()
                          , read_requests_.front().values_.size())
                      , bind(
                            &impl::read_complete
                          , shared_from_this()
                          , boost::asio::placeholders::error
                          , boost::asio::placeholders::bytes_transferred));
                    
                    read_requests_.front().scheduled_ = true;
                }
            }
        }
        else
        {
            is_alive_ = false;
            
            if (on_death_)
            {
                on_death_();
            }
        }
    }

    shared_ptr<asio::ip::tcp::socket> socket_;
    bool                              is_alive_;
    function<void ()>                 on_death_;

    deque<read_request>  read_requests_;
    deque<write_request> write_requests_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
socket::socket(
    shared_ptr<asio::ip::tcp::socket> const &socket)
    : pimpl_(new impl(socket))
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
socket::~socket()
{
}

// ==========================================================================
// CLOSE
// ==========================================================================
void socket::close()
{
    pimpl_->close();
}

// ==========================================================================
// AVAILABLE
// ==========================================================================
optional<socket::input_size_type> socket::available() const
{
    return pimpl_->available();
}

// ==========================================================================
// READ
// ==========================================================================
runtime_array<u8> socket::read(input_size_type size)
{
    return pimpl_->read(size);
}

// ==========================================================================
// ASYNC_READ
// ==========================================================================
void socket::async_read(
    input_size_type            size
  , input_callback_type const &callback)
{
    pimpl_->async_read(size, callback);
}

// ==========================================================================
// WRITE
// ==========================================================================
socket::output_size_type socket::write(
    runtime_array<u8> const& values)
{
    return pimpl_->write(values);
}

// ==========================================================================
// ASYNC_WRITE
// ==========================================================================
void socket::async_write(
    runtime_array<u8> const    &values
  , output_callback_type const &callback)
{
    pimpl_->async_write(values, callback);
}

// ==========================================================================
// IS_ALIVE
// ==========================================================================
bool socket::is_alive() const
{
    return pimpl_->is_alive();
}

// ==========================================================================
// GET_IO_SERVICE
// ==========================================================================
io_service &socket::get_io_service()
{
    return pimpl_->get_io_service();
}

// ==========================================================================
// ON_DEATH
// ==========================================================================
void socket::on_death(boost::function<void ()> callback)
{
    pimpl_->on_death(callback);
}

}}


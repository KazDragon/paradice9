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
#include "odin/net/socket.hpp"
#include <boost/asio.hpp>
#include <deque>

namespace odin { namespace net {

// ==========================================================================
// SOCKET::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct socket::impl : std::enable_shared_from_this<impl>
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(std::shared_ptr<boost::asio::ip::tcp::socket> const &socket)
        : socket_(socket)
    {
    }

    // ======================================================================
    // CLOSE
    // ======================================================================
    void close()
    {
        boost::system::error_code unused_error_code;
        socket_->close(unused_error_code);

        // Now we need to inform all readers and writers that the socket has
        // died by informing them that no bytes have been read or written.

        while (!read_requests_.empty())
        {
            read_request &top_read_request = read_requests_[0];

            if (top_read_request.callback_ != NULL)
            {
                top_read_request.callback_(socket::input_storage_type());
            }

            read_requests_.pop_front();
        }

        while (!write_requests_.empty())
        {
            write_request &top_write_request = write_requests_[0];

            if (top_write_request.callback_ != NULL)
            {
                top_write_request.callback_(socket::input_size_type(0));
            }

            write_requests_.pop_front();
        }

        if (on_death_ != NULL)
        {
            on_death_();
        }
    }

    // ======================================================================
    // AVAILABLE
    // ======================================================================
    boost::optional<socket::input_size_type> available() const
    {
        if (!is_alive())
        {
            return {};
        }
        else if (read_requests_.empty())
        {
            boost::asio::socket_base::bytes_readable command(true);
            boost::system::error_code ec;
            socket_->io_control(command, ec);

            if (!ec)
            {
                std::size_t bytes_readable = command.get();

                return bytes_readable == 0
                     ? boost::optional<input_size_type>()
                     : boost::optional<input_size_type>(bytes_readable);
            }
            else
            {
                return boost::optional<input_size_type>(0);
            }
        }
        else
        {
            return {};
        }
    }

    // ======================================================================
    // READ
    // ======================================================================
    socket::input_storage_type read(input_size_type size)
    {
        socket::input_storage_type data(size);

        boost::system::error_code ec;
        socket_->read_some(boost::asio::buffer(&*data.begin(), size), ec);

        /* INPUT DEBUGGING
        for(size_t i = 0; i < data.size(); ++i)
        {
            unsigned char ch[2] = { data[i], 0 };

            printf(" IN [0x%02X] %s\n",
                (int)(unsigned char)ch[0]
              , isprint(char(ch[0])) ? (char*)ch : "(*)");
        }
        //*/

        return data;
    }

    // ======================================================================
    // ASYNC_READ
    // ======================================================================
    void async_read(
        socket::input_size_type            amount_requested,
        socket::input_callback_type const &callback)
    {
        read_requests_.emplace_back(amount_requested, callback);

        if (!read_requests_[0].scheduled_)
        {
            boost::asio::async_read(
                *socket_.get(),
                boost::asio::buffer(
                    &*read_requests_[0].values_.begin(),
                    read_requests_[0].values_.size()),
                [this](
                    boost::system::error_code const &ec,
                    std::size_t bytes_transferred)
                {
                    read_complete(ec, bytes_transferred);
                });

            read_requests_[0].scheduled_ = true;
        }
    }

    // ======================================================================
    // WRITE
    // ======================================================================
    socket::output_size_type write(output_storage_type const &values)
    {
        /* OUTPUT DEBUGGING
        for(size_t i = 0; i < values.size(); ++i)
        {
            char ch[2] = { char(values[i]), 0 };

            printf("OUT [0x%02X] %s\n",
                (int)(unsigned char)ch[0]
              , isprint(ch[0]) ? ch : "(*)");
            fflush(stdout);
        }
        //*/

        boost::system::error_code ec;
        return socket_->write_some(
            boost::asio::buffer(&*values.begin(), values.size()), ec);
    }

    // ======================================================================
    // ASYNC_WRITE
    // ======================================================================
    void async_write(
        output_storage_type const  &values
      , output_callback_type const &callback)
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
        return socket_->is_open();
    }

    // ======================================================================
    // GET_IO_SERVICE
    // ======================================================================
    boost::asio::io_service &get_io_service()
    {
        return socket_->get_io_service();
    }

    // ======================================================================
    // ON_DEATH
    // ======================================================================
    void on_death(std::function<void ()> const &callback)
    {
        on_death_ = callback;
    }

private :
    // ======================================================================
    // READ_REQUEST
    // ======================================================================
    struct read_request
    {
        // ==================================================================
        // CONSTRUCTOR
        // ==================================================================
        read_request(
            socket::input_size_type            amount_requested
          , socket::input_callback_type const &callback)
          : values_(amount_requested)
          , callback_(callback)
          , scheduled_(false)
        {
        }

        socket::input_storage_type  values_;
        socket::input_callback_type callback_;
        bool                        scheduled_;
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
            socket::output_storage_type const  &values
          , socket::output_callback_type const &callback)
          : values_(values)
          , callback_(callback)
        {
        }

        socket::output_storage_type  values_;
        socket::output_callback_type callback_;
    };

    // ======================================================================
    // WRITE_FIRST_REQUEST
    // ======================================================================
    void write_first_request()
    {
        /* OUTPUT DEBUGGING
        for(size_t i = 0; i < write_requests_.front().values_.size(); ++i)
        {
            unsigned char ch[2] = { write_requests_.front().values_[i], 0 };

            printf("OUT [0x%02X] %s\n",
                (int)(unsigned char)ch[0]
              , isprint(ch[0]) ? (char*)ch : "(*)");
            fflush(stdout);
        }
        //*/

        boost::asio::async_write(
            *socket_.get(),
            boost::asio::buffer(
                &*write_requests_.front().values_.begin(),
                write_requests_.front().values_.size()),
            [this](
                boost::system::error_code const &ec,
                std::size_t bytes_transferred)
            {
                write_complete(ec, bytes_transferred);
            });
    }

    // ======================================================================
    // WRITE_COMPLETE
    // ======================================================================
    void write_complete(
        boost::system::error_code const &error
      , size_t                           bytes_transferred)
    {
        if (!is_alive())
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
            close();
        }
    }

    // ======================================================================
    // READ_COMPLETE
    // ======================================================================
    void read_complete(
        boost::system::error_code const &error
      , size_t                           bytes_transferred)
    {
        if (!is_alive())
        {
            return;
        }

        if (!error)
        {
            /* INPUT DEBUGGING
            for(size_t i = 0; i < bytes_transferred; ++i)
            {
                unsigned char ch[2] = { read_requests_.front().values_[i], 0 };

                printf(" IN [0x%02X] %s\n",
                    (int)(unsigned char)ch[0]
                  , isprint(char(ch[0])) ? (char*)ch : "(*)");
            }
            //*/

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
                        *socket_.get(),
                        boost::asio::buffer(
                            &*read_requests_.front().values_.begin(),
                            read_requests_.front().values_.size()),
                      [this](
                          boost::system::error_code const &ec,
                          std::size_t bytes_transferred)
                      {
                          read_complete(ec, bytes_transferred);
                      });

                    read_requests_.front().scheduled_ = true;
                }
            }
        }
        else
        {
            close();
        }
    }

    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
    std::function<void ()>                        on_death_;

    std::deque<read_request>  read_requests_;
    std::deque<write_request> write_requests_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
socket::socket(
    std::shared_ptr<boost::asio::ip::tcp::socket> const &socket)
    : pimpl_(std::make_shared<impl>(socket))
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
boost::optional<socket::input_size_type> socket::available() const
{
    return pimpl_->available();
}

// ==========================================================================
// READ
// ==========================================================================
socket::input_storage_type socket::read(input_size_type size)
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
socket::output_size_type socket::write(output_storage_type const &values)
{
    return pimpl_->write(values);
}

// ==========================================================================
// ASYNC_WRITE
// ==========================================================================
void socket::async_write(
    output_storage_type const  &values
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
boost::asio::io_service &socket::get_io_service()
{
    return pimpl_->get_io_service();
}

// ==========================================================================
// ON_DEATH
// ==========================================================================
void socket::on_death(std::function<void ()> const &callback)
{
    pimpl_->on_death(callback);
}

}}


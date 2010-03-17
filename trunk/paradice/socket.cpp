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

namespace paradice {

struct socket::impl : enable_shared_from_this<impl>
{
    impl()
        : is_alive_(true)
    {
    }
        
    struct read_request
    {
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

    struct write_request
    {
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

    void kill()
    {
        socket_->close();
    }

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

    void write_complete(
        error_code const &error
      , size_t            bytes_transferred)
    {
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

    void read_complete(
        error_code const &error
      , size_t            bytes_transferred)
    {
        if (!error)
        {
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

socket::socket(
    shared_ptr<asio::ip::tcp::socket> const &socket)
    : pimpl_(new impl)
{
    pimpl_->socket_ = socket;
}

socket::~socket()
{
}

optional<socket::input_size_type> socket::available() const
{
    if (!is_alive())
    {
        return optional<input_size_type>(0);
    }
    else if (pimpl_->read_requests_.empty())
    {
        asio::socket_base::bytes_readable command(true);
        boost::system::error_code ec;
        pimpl_->socket_->io_control(command, ec);
        
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

runtime_array<u8> socket::read(input_size_type size)
{
    runtime_array<u8> data(size);

    pimpl_->socket_->read_some(boost::asio::buffer(data.begin(), size));

    return data;
}

void socket::async_read(
    input_size_type            size
  , input_callback_type const &callback)
{
    pimpl_->async_read(size, callback);
}

socket::output_size_type socket::write(
    runtime_array<u8> const& values)
{
    return pimpl_->socket_->write_some(
        asio::buffer(values.begin(), values.size()));
}

void socket::async_write(
    runtime_array<u8> const    &values
  , output_callback_type const &callback)
{
    pimpl_->async_write(values, callback);
}

bool socket::is_alive() const
{
    return pimpl_->is_alive_;
}

io_service &socket::get_io_service()
{
    return pimpl_->socket_->get_io_service();
}

void socket::on_death(boost::function<void ()> callback)
{
    pimpl_->on_death_ = callback;
}

void socket::kill()
{
    pimpl_->kill();
}

}

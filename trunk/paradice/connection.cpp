#include "connection.hpp"
#include "socket.hpp"
#include "odin/telnet/stream.hpp"
#include "odin/telnet/router.hpp"
#include "odin/telnet/options/naws_client.hpp"
#include <boost/bind.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <deque>

using namespace odin;
using namespace odin::io;
using namespace boost;
using namespace std;

namespace paradice {

struct connection::impl
{
    impl()
        : window_width_(80)
        , window_height_(24)
    {
    }
        
    void schedule_next_read()
    {
        BOOST_AUTO(available, telnet_stream_->available());
        
        if (available.is_initialized())
        {
            telnet_stream_->async_read(
                input_datastream<u8>::size_type(available.get())
              , bind(&impl::data_read, this, _1));
        }
        else
        {
            telnet_stream_->async_read(
                input_datastream<u8>::size_type(1)
              , bind(&impl::data_read, this, _1));
        }
    }
    
    void data_read(runtime_array<u8> const &data)
    {
        copy(data.begin(), data.end(), back_inserter(read_buffer_));
        schedule_next_read();
        lex_input();
    }

    void lex_input()
    {
        deque<u8>::iterator line_end = 
            find(read_buffer_.begin(), read_buffer_.end(), u8('\n'));
        
        if (line_end != read_buffer_.end())
        {
            // Convert to string
            string text;

            // Parse backspaces
            for (deque<u8>::iterator pos = read_buffer_.begin();
                 pos != line_end;
                 ++pos)
            {
                if (*pos == '\b')
                {
                    if (!text.empty())
                    {
                        text.erase(text.end() - 1, text.end());
                    }
                }
                else
                {
                    text += *pos;
                }
            }

            // Trim extra space from the beginning and end.
            boost::algorithm::trim(text);

            // Finally, trim the associated text out of the input buffer.
            read_buffer_.erase(read_buffer_.begin(), ++line_end);

            // Announce to the listener.
            if (on_data_)
            {
                on_data_(text);
            }
        }
    }
    
    void write(string const &text)
    {
        copy(text.begin(), text.end(), back_inserter(write_buffer_));
        socket_->get_io_service().post(bind(&impl::do_write, this));
    }
    
    void do_write()
    {
        if (!write_buffer_.empty())
        {
            runtime_array<u8> output(write_buffer_.size());
            copy(write_buffer_.begin(), write_buffer_.end(), output.begin());
            
            telnet_stream_->async_write(output, NULL);
            
            write_buffer_.clear();
        }
    }
    
    void on_window_size_changed(u16 width, u16 height)
    {
        window_width_  = width;
        window_height_ = height;
        
        if (on_window_size_changed_)
        {
            on_window_size_changed_(window_width_, window_height_);
        }
    }
    
    shared_ptr<socket>                             socket_;
    shared_ptr<odin::telnet::stream>               telnet_stream_;
    shared_ptr<odin::telnet::router>               telnet_router_;
    shared_ptr<odin::telnet::options::naws_client> telnet_naws_client_;
    function<void (string)>                        on_data_;
    
    deque<u8>                                      read_buffer_;
    vector<u8>                                     write_buffer_;
    
    u16                                            window_width_;
    u16                                            window_height_;
    function<void (u16, u16)>                      on_window_size_changed_;
};

connection::connection(
    shared_ptr<socket>      connection_socket
  , function<void (string)> data_callback)
    : pimpl_(new impl)
{
    reconnect(connection_socket);
    pimpl_->on_data_ = data_callback;
}
    
connection::~connection()
{
}
    
void connection::write(string const &text)
{
    pimpl_->write(text);
}

pair<u16, u16> connection::get_window_size() const
{
    return make_pair(pimpl_->window_width_, pimpl_->window_height_);
}

void connection::on_window_size_changed(
    function<void (u16, u16)> const &callback)
{
    pimpl_->on_window_size_changed_ = callback;
}

void connection::keepalive()
{
    pimpl_->telnet_stream_->send_command(odin::telnet::NOP);
}

void connection::disconnect()
{
    pimpl_->socket_->kill();
    pimpl_->socket_.reset();
}

void connection::reconnect(shared_ptr<socket> connection_socket)
{
    pimpl_->socket_  = connection_socket;
    pimpl_->telnet_stream_ = shared_ptr<odin::telnet::stream>(
        new odin::telnet::stream(
            pimpl_->socket_
          , pimpl_->socket_->get_io_service()));
    pimpl_->telnet_router_ = shared_ptr<odin::telnet::router>(
        new odin::telnet::router(pimpl_->telnet_stream_));
    pimpl_->telnet_naws_client_ = 
        shared_ptr<odin::telnet::options::naws_client>(
            new odin::telnet::options::naws_client(
                pimpl_->telnet_stream_, pimpl_->telnet_router_));
    pimpl_->telnet_naws_client_->on_size(
        bind(&impl::on_window_size_changed, pimpl_, _1, _2));
    pimpl_->telnet_naws_client_->set_activatable(true);
    pimpl_->telnet_naws_client_->activate();
    pimpl_->schedule_next_read();
}

}

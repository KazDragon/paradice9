#include "client.hpp"
#include "socket.hpp"
#include <boost/bind.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <deque>

using namespace odin;
using namespace odin::io;
using namespace boost;
using namespace std;

namespace paradice {

struct client::impl
{
    void schedule_next_read()
    {
        BOOST_AUTO(available, socket_->available());
        
        if (available.is_initialized())
        {
            socket_->async_read(
                input_datastream<u8>::size_type(available.get())
              , bind(&impl::data_read, this, _1));
        }
        else
        {
            socket_->async_read(
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
            
            socket_->async_write(output, NULL);
            
            write_buffer_.clear();
        }
    }
    
    shared_ptr<socket>      socket_;
    function<void (string)> on_data_;
    
    deque<u8>               read_buffer_;
    vector<u8>              write_buffer_;

    string                  name_;
    string                  title_;
};

client::client(
    shared_ptr<socket>      client_socket
  , function<void (string)> data_callback)
    : pimpl_(new impl)
{
    pimpl_->socket_  = client_socket;
    pimpl_->on_data_ = data_callback;
    
    pimpl_->schedule_next_read();
}
    
client::~client()
{
}
    
void client::set_name(string const &name)
{
    pimpl_->name_ = name;
}

string client::get_name() const
{
    return pimpl_->name_;
}

void client::set_title(string const &title)
{
    pimpl_->title_ = title;
}

string client::get_title() const
{
    return pimpl_->title_;
}

void client::write(string const &text)
{
    pimpl_->write(text);
}

}

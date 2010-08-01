#include "connection.hpp"
#include "socket.hpp"
#include "odin/telnet/command_router.hpp"
#include "odin/telnet/input_visitor.hpp"
#include "odin/telnet/negotiation_router.hpp"
#include "odin/telnet/stream.hpp"
#include "odin/telnet/subnegotiation_router.hpp"
#include "odin/telnet/options/naws_client.hpp"
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <deque>

using namespace odin;
using namespace odin::io;
using namespace boost;
using namespace std;

namespace paradice {

// ==========================================================================
// CONNECTION IMPLEMENTATION STRUCTURE
// ==========================================================================
struct connection::impl
    : public enable_shared_from_this<impl>
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl()
        : window_width_(80)
        , window_height_(24)
    {
    }
        
    // ======================================================================
    // SCHEDULE_NEXT_READ
    // ======================================================================
    void schedule_next_read()
    {
        BOOST_AUTO(available, telnet_stream_->available());
        
        if (available.is_initialized())
        {
            telnet_stream_->async_read(
                odin::telnet::stream::input_size_type(available.get())
              , bind(&impl::data_read, shared_from_this(), _1));
        }
        else
        {
            telnet_stream_->async_read(
                odin::telnet::stream::input_size_type(1)
              , bind(&impl::data_read, shared_from_this(), _1));
        }
    }
    
    // ======================================================================
    // DATA_READ
    // ======================================================================
    void data_read(
        runtime_array<odin::telnet::stream::input_value_type> const &data)
    {
        apply_input_range(*telnet_input_visitor_, data);
        
        schedule_next_read();
        lex_input();
    }

    // ======================================================================
    // LEX_INPUT
    // ======================================================================
    void lex_input()
    {
        string::iterator line_end = 
            find(input_buffer_.begin(), input_buffer_.end(), '\n');
        
        if (line_end != input_buffer_.end())
        {
            // Parse backspaces out of the input line.
            string text;
            
            for (string::iterator pos = input_buffer_.begin();
                 pos != line_end;
                 ++pos)
            {
                if (*pos == '\b')
                {
                    if (!text.empty())
                    {
                        text.erase(text.size() - 1, 1);
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
            input_buffer_.erase(input_buffer_.begin(), ++line_end);

            // Announce to the listener.
            if (on_data_)
            {
                on_data_(text);
            }
        }
    }
    
    // ======================================================================
    // WRITE
    // ======================================================================
    void write(string const &text)
    {
        odin::telnet::stream::output_value_type value(text);
        runtime_array<odin::telnet::stream::output_value_type> array(&value, 1);

        telnet_stream_->write(array);        
    }
    
    // ======================================================================
    // RECONNECT
    // ======================================================================
    void reconnect(shared_ptr<socket> const &connection_socket)
    {
        socket_ = connection_socket;
        
        telnet_stream_ = shared_ptr<odin::telnet::stream>(
            new odin::telnet::stream(
                socket_
              , socket_->get_io_service()));
        
        telnet_command_router_ = 
            shared_ptr<odin::telnet::command_router>(
                new odin::telnet::command_router);
    
        telnet_negotiation_router_ = 
            shared_ptr<odin::telnet::negotiation_router>(
                new odin::telnet::negotiation_router);
    
        telnet_subnegotiation_router_ = 
            shared_ptr<odin::telnet::subnegotiation_router>(
                new odin::telnet::subnegotiation_router);

        telnet_input_visitor_ =
            shared_ptr<odin::telnet::input_visitor>(
                new odin::telnet::input_visitor(
                    telnet_command_router_
                  , telnet_negotiation_router_
                  , telnet_subnegotiation_router_
                  , bind(&impl::on_text, shared_from_this(), _1)));
            
        telnet_naws_client_ =
            shared_ptr<odin::telnet::options::naws_client>(
                new odin::telnet::options::naws_client(
                    telnet_stream_
                  , telnet_negotiation_router_
                  , telnet_subnegotiation_router_));
        
        telnet_naws_client_->on_size(
            bind(&impl::on_window_size_changed, shared_from_this(), _1, _2));
        telnet_naws_client_->set_activatable(true);
        telnet_naws_client_->activate();
        
        schedule_next_read();
    }
    
    // ======================================================================
    // ON_TEXT
    // ======================================================================
    void on_text(string const &text)
    {
        input_buffer_ += text;
    }
    
    // ======================================================================
    // ON_WINDOW_SIZE_CHANGED
    // ======================================================================
    void on_window_size_changed(u16 width, u16 height)
    {
        window_width_  = width;
        window_height_ = height;
        
        if (on_window_size_changed_)
        {
            on_window_size_changed_(window_width_, window_height_);
        }
    }
    
    shared_ptr<socket>                              socket_;
    shared_ptr<odin::telnet::stream>                telnet_stream_;
    shared_ptr<odin::telnet::command_router>        telnet_command_router_;
    shared_ptr<odin::telnet::negotiation_router>    telnet_negotiation_router_;
    shared_ptr<odin::telnet::subnegotiation_router> telnet_subnegotiation_router_;
    shared_ptr<odin::telnet::input_visitor>         telnet_input_visitor_;
    shared_ptr<odin::telnet::options::naws_client>  telnet_naws_client_;

    function<void (string)>                        on_data_;
    string                                         input_buffer_;
    
    u16                                            window_width_;
    u16                                            window_height_;
    function<void (u16, u16)>                      on_window_size_changed_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
connection::connection(
    shared_ptr<socket>      connection_socket
  , function<void (string)> data_callback)
    : pimpl_(new impl)
{
    reconnect(connection_socket);
    pimpl_->on_data_ = data_callback;
}
    
// ==========================================================================
// DESTRUCTOR
// ==========================================================================
connection::~connection()
{
}
    
// ==========================================================================
// WRITE
// ==========================================================================
void connection::write(string const &text)
{
    pimpl_->write(text);
}

// ==========================================================================
// GET_WINDOW_SIZE
// ==========================================================================
pair<u16, u16> connection::get_window_size() const
{
    return make_pair(pimpl_->window_width_, pimpl_->window_height_);
}

// ==========================================================================
// ON_WINDOW_SIZE_CHANGED
// ==========================================================================
void connection::on_window_size_changed(
    function<void (u16, u16)> const &callback)
{
    pimpl_->on_window_size_changed_ = callback;
}

// ==========================================================================
// KEEPALIVE
// ==========================================================================
void connection::keepalive()
{
    odin::telnet::stream::output_value_type value(odin::telnet::NOP);
    runtime_array<odin::telnet::stream::output_value_type> array(&value, 1);
    
    pimpl_->telnet_stream_->write(array);
}

// ==========================================================================
// DISCONNECT
// ==========================================================================
void connection::disconnect()
{
    pimpl_->socket_->kill();
    pimpl_->socket_.reset();
}

// ==========================================================================
// RECONNECT
// ==========================================================================
void connection::reconnect(shared_ptr<socket> connection_socket)
{
    pimpl_->reconnect(connection_socket);
}

}

// ==========================================================================
// GuiBuilder Client
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
#include "client.hpp"
#include "munin/ansi/window.hpp"
#include "odin/ansi/parser.hpp"
#include "odin/telnet/stream.hpp"
#include "odin/telnet/command_router.hpp"
#include "odin/telnet/negotiation_router.hpp"
#include "odin/telnet/subnegotiation_router.hpp"
#include "odin/telnet/input_visitor.hpp"
#include "odin/telnet/options/echo_server.hpp"
#include "odin/telnet/options/naws_client.hpp"
#include "odin/telnet/options/suppress_goahead_server.hpp"
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>
#include <deque>
#include <string>

using namespace std;
using namespace boost;
using namespace munin::ansi;

namespace guibuilder {

namespace {
    
struct ansi_input_visitor
    : public static_visitor<>
{
    ansi_input_visitor(
        function<void (odin::ansi::control_sequence)> on_control_sequence
      , function<void (string)>                       on_text)
        : on_control_sequence_(on_control_sequence)
        , on_text_(on_text)
    {
    }
    
    void operator()(odin::ansi::control_sequence const &control_sequence)
    {
        printf("[META: %s CSI: 0x%02X ('%c') CMD: 0x%02X('%c')\n"
          , (control_sequence.meta_ ? "Yes" : "No")
          , int(control_sequence.initiator_)
          , control_sequence.initiator_
          , int(control_sequence.command_)
          , control_sequence.command_
          );

        if (!control_sequence.arguments_.empty())
        {
            printf("ARGUMENTS:\n");
            
            BOOST_FOREACH(char ch, control_sequence.arguments_)
            {
                printf("    [0x%02X]", int(ch));
                
                if (isprint(ch))
                {
                    printf(" (%c)", ch);
                }
                
                printf("\n");
            }
        }
        
        if (on_control_sequence_)
        {
            on_control_sequence_(control_sequence);
        }
    }
    
    void operator()(string const &text)
    {
        if (on_text_)
        {
            on_text_(text);
        }
    }
    
    function<void (odin::ansi::control_sequence)> on_control_sequence_;
    function<void (string)>                       on_text_;
};

}

struct client::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(shared_ptr<stream>       connection
       , boost::asio::io_service &io_service)
        : connection_(connection)
    {
        telnet_stream_ =
            make_shared<odin::telnet::stream>(connection, ref(io_service));

        telnet_command_router_ = 
            make_shared<odin::telnet::command_router>();

        telnet_negotiation_router_ = 
            make_shared<odin::telnet::negotiation_router>();

        telnet_subnegotiation_router_ = 
            make_shared<odin::telnet::subnegotiation_router>();

        telnet_input_visitor_ = make_shared<odin::telnet::input_visitor>(
            telnet_command_router_
          , telnet_negotiation_router_
          , telnet_subnegotiation_router_
          , boost::bind(&impl::on_text, this, _1));

        telnet_echo_server_ = make_shared<odin::telnet::options::echo_server>(
            telnet_stream_
          , telnet_negotiation_router_
          , telnet_subnegotiation_router_);
        telnet_echo_server_->set_activatable(true);
        telnet_echo_server_->activate();

        telnet_suppress_ga_server_ = 
            make_shared<odin::telnet::options::suppress_goahead_server>(
                telnet_stream_
              , telnet_negotiation_router_
              , telnet_subnegotiation_router_);
        telnet_suppress_ga_server_->set_activatable(true);
        telnet_suppress_ga_server_->activate();

        telnet_naws_client_ = make_shared<odin::telnet::options::naws_client>(
            telnet_stream_
          , telnet_negotiation_router_
          , telnet_subnegotiation_router_);
        telnet_naws_client_->set_activatable(true);
        telnet_naws_client_->activate();
        telnet_naws_client_->on_size(bind(
            &impl::on_window_size_changed
          , this
          , _1
          , _2));

        window_  = make_shared<window>(ref(io_service));
    
        schedule_next_read();
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
              , bind(&impl::data_read, this, _1));
        }
        else
        {
            telnet_stream_->async_read(
                odin::telnet::stream::input_size_type(1)
              , bind(&impl::data_read, this, _1));
        }
    }
    
    // ======================================================================
    // DATA_READ
    // ======================================================================
    void data_read(
        odin::runtime_array<odin::telnet::stream::input_value_type> const &data)
    {
        apply_input_range(*telnet_input_visitor_, data);
        schedule_next_read();
    }

    // ======================================================================
    // ON_TEXT
    // ======================================================================
    void on_text(string const &text)
    {
        input_buffer_.insert(
            input_buffer_.end()
          , text.begin()
          , text.end());
        
        static odin::ansi::parser parse;
        
        BOOST_AUTO(begin, input_buffer_.begin());
        BOOST_AUTO(end,   input_buffer_.end());
        BOOST_AUTO(result, parse(begin, end));
        
        ansi_input_visitor visitor(on_control_sequence_, on_text_);

        BOOST_FOREACH(odin::ansi::parser::element_type value, result)
        {
            apply_visitor(visitor, value);
        }

        // Erase any input that has been successfully parsed.
        input_buffer_.erase(input_buffer_.begin(), begin);
    }

    // ======================================================================
    // ON_WINDOW_SIZE_CHANGED
    // ======================================================================
    void on_window_size_changed(odin::u16 width, odin::u16 height)
    {
        if (on_window_size_changed_)
        {
            on_window_size_changed_(width, height);
        }
    }

    shared_ptr<stream>                              connection_;
    shared_ptr<odin::telnet::stream>                telnet_stream_;
    shared_ptr<odin::telnet::command_router>        telnet_command_router_;
    shared_ptr<odin::telnet::negotiation_router>    telnet_negotiation_router_;
    shared_ptr<odin::telnet::subnegotiation_router> telnet_subnegotiation_router_;
    shared_ptr<odin::telnet::input_visitor>         telnet_input_visitor_;
    shared_ptr<odin::telnet::options::echo_server>  telnet_echo_server_;
    shared_ptr<odin::telnet::options::suppress_goahead_server> telnet_suppress_ga_server_;
    shared_ptr<odin::telnet::options::naws_client>  telnet_naws_client_;

    shared_ptr<window> window_;

    function<void (odin::u16, odin::u16)>           on_window_size_changed_;
    function<void (string)>                         on_text_;
    function<void (odin::ansi::control_sequence)>   on_control_sequence_;
   
    deque<char>                                     input_buffer_;
};

client::client(
    shared_ptr<stream>       connection
  , boost::asio::io_service &io_service)
    : pimpl_(new impl(connection, io_service))
{
}

client::~client()
{
}

shared_ptr<window> client::get_window()
{
    return pimpl_->window_;
}

void client::on_window_size_changed(
    function<void (odin::u16, odin::u16)> callback)
{
    pimpl_->on_window_size_changed_ = callback;
}

void client::on_text(function<void (string)> callback)
{
    pimpl_->on_text_ = callback;
}

void client::on_control_sequence(
    function<void (odin::ansi::control_sequence)> callback)
{
    pimpl_->on_control_sequence_ = callback;
}

}

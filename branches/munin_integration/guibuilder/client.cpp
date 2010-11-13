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
#include <string>

using namespace std;
using namespace boost;
using namespace munin::ansi;

namespace guibuilder {

struct client::impl
    : public boost::enable_shared_from_this<client::impl>
{
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
        if (on_text_)
        {
            on_text_(text);
        }
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

};

client::client(
    shared_ptr<stream>       connection
  , boost::asio::io_service &io_service)
    : pimpl_(new impl)
{
    pimpl_->connection_ = connection;

    pimpl_->telnet_stream_ = 
        make_shared<odin::telnet::stream>(
            pimpl_->connection_, ref(io_service));

    pimpl_->telnet_command_router_ = 
        make_shared<odin::telnet::command_router>();

    pimpl_->telnet_negotiation_router_ =
        make_shared<odin::telnet::negotiation_router>();

    pimpl_->telnet_subnegotiation_router_ =
        make_shared<odin::telnet::subnegotiation_router>();

    pimpl_->telnet_input_visitor_ =
        make_shared<odin::telnet::input_visitor>(
            pimpl_->telnet_command_router_
          , pimpl_->telnet_negotiation_router_
          , pimpl_->telnet_subnegotiation_router_
          , bind(&impl::on_text, pimpl_, _1));

    pimpl_->telnet_echo_server_ = 
        make_shared<odin::telnet::options::echo_server>(
            pimpl_->telnet_stream_
          , pimpl_->telnet_negotiation_router_
          , pimpl_->telnet_subnegotiation_router_);

    pimpl_->telnet_echo_server_->set_activatable(true);
    pimpl_->telnet_echo_server_->activate();

    pimpl_->telnet_naws_client_ =
        make_shared<odin::telnet::options::naws_client>(
            pimpl_->telnet_stream_
          , pimpl_->telnet_negotiation_router_
          , pimpl_->telnet_subnegotiation_router_);

    pimpl_->telnet_naws_client_->set_activatable(true);
    pimpl_->telnet_naws_client_->activate();
    pimpl_->telnet_naws_client_->on_size(
        bind(
            &impl::on_window_size_changed
          , pimpl_->shared_from_this()
          , _1
          , _2));

    pimpl_->telnet_suppress_ga_server_ =
        make_shared<odin::telnet::options::suppress_goahead_server>(
            pimpl_->telnet_stream_
          , pimpl_->telnet_negotiation_router_
          , pimpl_->telnet_subnegotiation_router_);

    pimpl_->telnet_suppress_ga_server_->set_activatable(true);
    pimpl_->telnet_suppress_ga_server_->activate();

    pimpl_->window_ = make_shared<window>(ref(io_service));

    pimpl_->schedule_next_read();
}

client::~client()
{
}

shared_ptr<window> client::get_window()
{
    return pimpl_->window_;
}

void client::on_window_size_changed(
    boost::function<void (odin::u16, odin::u16)> callback)
{
    pimpl_->on_window_size_changed_ = callback;
}

void client::on_text(boost::function<void (string)> callback)
{
    pimpl_->on_text_ = callback;
}

}

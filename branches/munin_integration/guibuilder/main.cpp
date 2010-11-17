// ==========================================================================
// GuiBuilder Main
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
#include "server.hpp"
#include "socket.hpp"
#include "client.hpp"
#include "ui.hpp"
#include "odin/telnet/protocol.hpp"
#include "munin/ansi/window.hpp"
#include "munin/grid_layout.hpp"
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>
#include <boost/program_options.hpp>
#include <boost/typeof/typeof.hpp>
#include <iostream>
#include <list>

using namespace std;
using namespace boost;
namespace po = boost::program_options;

boost::asio::io_service io_service;

vector< shared_ptr<guibuilder::client> > clients;

static void schedule_keepalive(
    boost::shared_ptr<guibuilder::socket>   socket
  , shared_ptr<boost::asio::deadline_timer> keepalive_timer);

void on_repaint(
    weak_ptr<guibuilder::socket>  weak_socket
  , string const                 &paint_data)
{
    // TODO: This should take a client instead so that it can filter down the
    // stream chain rather than writing directly to the socket.
    shared_ptr<guibuilder::socket> socket = weak_socket.lock();

    if (socket)
    {
        odin::runtime_array<odin::u8> data(paint_data.size());
        copy(paint_data.begin(), paint_data.end(), data.begin());

        socket->async_write(data, NULL);
    }
}

void on_keepalive(
    weak_ptr<guibuilder::socket>             weak_socket
  , shared_ptr<boost::asio::deadline_timer>  keepalive_timer
  , boost::system::error_code const         &error)
{
    boost::shared_ptr<guibuilder::socket> socket = weak_socket.lock();

    if (socket)
    {
        guibuilder::socket::output_value_type values[] = {
            odin::telnet::IAC, odin::telnet::NOP
        };

        odin::runtime_array<guibuilder::socket::output_value_type> data(values);

        socket->async_write(data, NULL);

        schedule_keepalive(socket, keepalive_timer);
    }
}

void on_death(weak_ptr<guibuilder::client> weak_client)
{
    shared_ptr<guibuilder::client> client = weak_client.lock();

    if (client)
    {
        clients.erase(
            find(clients.begin(), clients.end(), client)
          , clients.end());
    }
}

void on_window_size_changed(
    weak_ptr<guibuilder::client> weak_client,
    odin::u16                    width,
    odin::u16                    height)
{
    shared_ptr<guibuilder::client> client = weak_client.lock();

    if (client)
    {
        client->get_window()->get_content()->set_size(
            munin::extent(width, height));
    }
}

void on_text(
    weak_ptr<guibuilder::client>  weak_client
  , string const                 &text)
{
    shared_ptr<guibuilder::client> client = weak_client.lock();

    if (client)
    {
        BOOST_AUTO(window, client->get_window());

        BOOST_FOREACH(char ch, text)
        {
            window->event(ch);
        }
    }
}

void on_accept(shared_ptr<guibuilder::socket> socket)
{
    schedule_keepalive(
        socket
      , make_shared<boost::asio::deadline_timer>(ref(io_service)));

    BOOST_AUTO(client, make_shared<guibuilder::client>(socket, ref(io_service)));
    
    socket->on_death(bind(on_death, weak_ptr<guibuilder::client>(client)));
    clients.push_back(client);

    client->on_window_size_changed(
        bind(
            &on_window_size_changed
          , boost::weak_ptr<guibuilder::client>(client)
          , _1
          , _2));

    client->on_text(
        bind(
            &on_text
          , boost::weak_ptr<guibuilder::client>(client)
          , _1));

    boost::shared_ptr<munin::ansi::window>    window  = client->get_window();
    boost::shared_ptr<munin::ansi::container> content = window->get_content();

    window->on_repaint.connect(
        bind(&on_repaint, weak_ptr<guibuilder::socket>(socket), _1));

    BOOST_AUTO(user_interface, make_shared<guibuilder::ui>());

    content->set_size(munin::extent(80, 24));
    content->set_layout(
        make_shared< munin::grid_layout<munin::ansi::element_type> >(1, 1));

    content->add_component(user_interface);
    content->set_focus();
}

static void schedule_keepalive(
    boost::shared_ptr<guibuilder::socket>   socket
  , boost::shared_ptr<asio::deadline_timer> keepalive_timer)
{
    keepalive_timer->expires_from_now(boost::posix_time::seconds(30));
    keepalive_timer->async_wait(
        bind(
            on_keepalive
          , boost::weak_ptr<guibuilder::socket>(socket)
          , keepalive_timer
          , boost::asio::placeholders::error));
}

int main(int argc, char **argv)
{
    unsigned int port = 4000;
    
    po::options_description description("Available options");
    description.add_options()
        ( "help,h", "show this help message" )
        ( "port,p", po::value<unsigned int>(&port), "port number" )
        ;

    po::positional_options_description pos_description;
    pos_description.add("port", -1);
    
    try
    {
        po::variables_map vm;
        po::store(
            po::command_line_parser(argc, argv)
                .options(description)
                .positional(pos_description)
                .run()
          , vm);
        
        po::notify(vm);
        
        if (vm.count("help") != 0)
        {
            throw po::error("");
        }
        else if (vm.count("port") == 0)
        {
            throw po::error("Port number must be specified");
        }
    }
    catch(po::error &err)
    {
        if (strlen(err.what()) == 0)
        {
            cout << format("USAGE: %s <port number>|<options>\n")
                        % argv[0]
                 << description
                 << endl;
                 
            return EXIT_SUCCESS;
        }
        else
        {
            cerr << format("ERROR: %s\n\nUSAGE: %s <port number>|<options>\n")
                        % err.what()
                        % argv[0]
                 << description
                 << endl;
        }
        
        return EXIT_FAILURE;
    }

    guibuilder::server server(io_service, port, bind(&on_accept, _1));

    io_service.run();

    return EXIT_SUCCESS;
}

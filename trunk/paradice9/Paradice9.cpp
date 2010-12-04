// ==========================================================================
// Paradice9 Executable Driver
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
#include "context_impl.hpp"
#include "paradice/server.hpp"
#include "paradice/socket.hpp"
#include "paradice/connection.hpp"
#include "paradice/client.hpp"
#include "paradice/communication.hpp"
#include "paradice/configuration.hpp"
#include "paradice/rules.hpp"
#include "paradice/who.hpp"
#include "hugin/user_interface.hpp"
#include "munin/ansi/protocol.hpp"
#include "munin/ansi/window.hpp"
#include "munin/grid_layout.hpp"
#include "odin/telnet/protocol.hpp"
#include "odin/tokenise.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/typeof/typeof.hpp>
#include <iostream>
#include <string>

using namespace boost;
using namespace std;
using namespace odin;

namespace po = program_options;

#define PARADICE_CMD_ENTRY_NOP(name) \
    { name,        NULL                                   }
    
#define PARADICE_CMD_ENTRY(func) \
    { #func,       bind(&paradice::do_##func, _1, _2, _3) }
    
#define PARADICE_CMD_ALIAS(func, alias) \
    { alias,       bind(&paradice::do_##func, _1, _2, _3) }
    
typedef function<void (
    shared_ptr<paradice::context> ctx
  , std::string                   args
  , shared_ptr<paradice::client>  player)> paradice_command;

static struct command
{
    string           command_;
    paradice_command function_;
} const command_list[] =
{
    PARADICE_CMD_ENTRY_NOP("!")

  , PARADICE_CMD_ENTRY(say)    
  , PARADICE_CMD_ALIAS(say, ".")
  
  , PARADICE_CMD_ENTRY(whisper)
  , PARADICE_CMD_ALIAS(whisper, ">")
  
  , PARADICE_CMD_ENTRY(emote)
  , PARADICE_CMD_ALIAS(emote, ":")

  , PARADICE_CMD_ENTRY(set)
  , PARADICE_CMD_ENTRY(title)
  , PARADICE_CMD_ALIAS(title, "surname")
  , PARADICE_CMD_ENTRY(rename)
  , PARADICE_CMD_ENTRY(prefix)
  , PARADICE_CMD_ALIAS(prefix, "honorific")
  , PARADICE_CMD_ENTRY(roll)
  , PARADICE_CMD_ENTRY(rollprivate)
  , PARADICE_CMD_ENTRY(showrolls)
  , PARADICE_CMD_ENTRY(clearrolls)

  , PARADICE_CMD_ENTRY(quit)
};

void on_input_entered(
    weak_ptr<paradice::context>  weak_context
  , weak_ptr<paradice::client>   weak_client
  , string const                &input);

void on_command(
    shared_ptr<paradice::context>  ctx
  , shared_ptr<paradice::client>   client
  , string const                  &input)
{
    client->get_user_interface()->add_command_history(input);

    BOOST_AUTO(arg, odin::tokenise(input));

    // Transform the command to lower case.
    for (string::iterator ch = arg.first.begin();
         ch != arg.first.end();
         ++ch)
    {
        *ch = tolower(*ch);
    }

    if (arg.first == "!")
    {
        weak_ptr<paradice::client> weak_client(client);
        on_input_entered(ctx, client, client->get_last_command());
        return;
    }

    // Search through the list for commands
    BOOST_FOREACH(command const &cur_command, command_list)
    {
        if (cur_command.command_ == arg.first)
        {
            cur_command.function_(ctx, arg.second, client);
            client->set_last_command(input);
            return;
        }
    }

    string text = 
        "\nDidn't understand that.  Available commands are:\n";

    BOOST_FOREACH(command const &cur_command, command_list)
    {
        text += cur_command.command_ + " ";
    }

    text += "\n";

    paradice::send_to_player(ctx, text, client);
}
    
void on_input_entered(
    weak_ptr<paradice::context>      weak_context
  , weak_ptr<paradice::client>       weak_client
  , string const                    &input)
{
    BOOST_AUTO(client, weak_client.lock());
    BOOST_AUTO(ctx, weak_context.lock());
    
    if (client != NULL && ctx != NULL)
    {
        if (input.empty())
        {
            return;
        }
        
        if (client->get_command_mode() == paradice::client::command_mode_mud)
        {
            on_command(ctx, client, input);
        }
        else
        {
            if (input[0] == '/')
            {
                if (input.size() >= 2 && input[1] == '!')
                {
                    on_input_entered(
                        weak_context
                      , weak_client
                      , "/" + client->get_last_command());
                }
                else
                {
                    on_command(ctx, client, input.substr(1));
                }
            }
            else
            {
                INVOKE_PARADICE_COMMAND(say, ctx, input, client);
            }
        }
    }
}

void capitalise(string &name)
{
    if (!name.empty())
    {
        name[0] = toupper(name[0]);
        
        for (string::size_type index = 1; index < name.size(); ++index)
        {
            name[index] = tolower(name[index]);
        }
    }
}

void on_username_entered(
    weak_ptr<paradice::context> weak_context
  , weak_ptr<paradice::client>  weak_client
  , string                      username)
{
    BOOST_AUTO(ctx, weak_context.lock());
    BOOST_AUTO(client, weak_client.lock());
    
    if (ctx != NULL && client != NULL)
    {
        BOOST_AUTO(user_interface, client->get_user_interface());
        BOOST_AUTO(arg, odin::tokenise(username).first);
        capitalise(arg);
        
        if (!paradice::is_acceptible_name(arg))
        {
            munin::ansi::attribute pen;
            pen.foreground_colour = odin::ansi::graphics::COLOUR_RED;

            client->get_user_interface()->set_statusbar_text(
                munin::ansi::elements_from_string(
                    "Name must be over two characters long and "
                    "comprise only alphabetical characters"
                  , pen));
        }
        else
        {
            BOOST_AUTO(clients, ctx->get_clients());
            munin::ansi::attribute pen;
            pen.foreground_colour = odin::ansi::graphics::COLOUR_RED;
            
            BOOST_FOREACH(shared_ptr<paradice::client> cur_client, clients)
            {
                if (cur_client->get_name() == arg)
                {
                    client->get_user_interface()->set_statusbar_text(
                        munin::ansi::elements_from_string(
                            "A user with that name is already online."
                          , pen));
                    return;
                }
            }

            client->set_name(arg);
            ctx->update_names();
            
            user_interface->select_face(hugin::FACE_MAIN);
            user_interface->set_focus();
            
            paradice::send_to_all(
                ctx
              , "#SERVER: " 
              + client->get_name()
              + " has entered Paradice!\n");
        }
    }
}

void on_repaint(
    weak_ptr<paradice::socket>  weak_socket
  , string const               &paint_data)
{
    // TODO: This should take a connection instead so that it can filter down the
    // stream chain rather than writing directly to the socket.
    BOOST_AUTO(socket, weak_socket.lock());

    if (socket)
    {
        runtime_array<u8> data(paint_data.size());
        copy(paint_data.begin(), paint_data.end(), data.begin());

        socket->async_write(data, NULL);
    }
}

void on_control_sequence(
    weak_ptr<paradice::connection>      weak_connection
  , odin::ansi::control_sequence const &control_sequence)
{
    BOOST_AUTO(connection, weak_connection.lock());
    
    if (connection)
    {
        connection->get_window()->event(control_sequence);
    }
}

void on_text(
    weak_ptr<paradice::connection>  weak_connection
  , string const                   &text)
{
    BOOST_AUTO(connection, weak_connection.lock());

    if (connection)
    {
        BOOST_AUTO(window, connection->get_window());

        BOOST_FOREACH(char ch, text)
        {
            window->event(ch);
        }
    }
}

void on_window_size_changed(
    weak_ptr<paradice::connection> weak_connection,
    u16                            width,
    u16                            height)
{
    BOOST_AUTO(connection, weak_connection.lock());

    if (connection)
    {
        connection->get_window()->get_content()->set_size(
            munin::extent(width, height));
    }
}

static void on_death(
    weak_ptr<paradice::context> &weak_context
  , weak_ptr<paradice::client>  &weak_client)

{
    BOOST_AUTO(ctx,    weak_context.lock());
    BOOST_AUTO(client, weak_client.lock());

    if (ctx != NULL && client != NULL)
    {
        ctx->remove_client(client);
        ctx->update_names();

        BOOST_AUTO(name, client->get_name());
        
        if (!name.empty())
        {
            paradice::send_to_all(
                ctx
              , "#SERVER: "
              + name
              + " has left Paradice.\n");
        }
    }
}

static void schedule_keepalive(
    boost::shared_ptr<paradice::socket>     socket
  , boost::shared_ptr<asio::deadline_timer> keepalive_timer);

static void on_keepalive(
    weak_ptr<paradice::socket>               weak_socket
  , shared_ptr<boost::asio::deadline_timer>  keepalive_timer
  , boost::system::error_code const         &error)
{
    BOOST_AUTO(socket, weak_socket.lock());

    if (socket)
    {
        paradice::socket::output_value_type values[] = {
            odin::telnet::IAC, odin::telnet::NOP
        };

        socket->async_write(values, NULL);

        schedule_keepalive(socket, keepalive_timer);
    }
}

static void schedule_keepalive(
    boost::shared_ptr<paradice::socket>     socket
  , boost::shared_ptr<asio::deadline_timer> keepalive_timer)
{
    keepalive_timer->expires_from_now(boost::posix_time::seconds(30));
    keepalive_timer->async_wait(
        bind(
            on_keepalive
          , boost::weak_ptr<paradice::socket>(socket)
          , keepalive_timer
          , boost::asio::placeholders::error));
}

static void on_accept(
    weak_ptr<paradice::context>   weak_context
  , shared_ptr<paradice::socket>  socket
  , boost::asio::io_service      &io_service)
{
    schedule_keepalive(
        socket
      , make_shared<boost::asio::deadline_timer>(ref(io_service)));

    BOOST_AUTO(connection, make_shared<paradice::connection>(socket));
    BOOST_AUTO(client, make_shared<paradice::client>());
    BOOST_AUTO(ctx, weak_context.lock());
    client->set_connection(connection);

    socket->on_death(bind(
        on_death
      , weak_context
      , weak_ptr<paradice::client>(client)));

    connection->on_window_size_changed(
        bind(
            &on_window_size_changed
          , boost::weak_ptr<paradice::connection>(connection)
          , _1
          , _2));

    connection->on_text(
        bind(
            &on_text
          , boost::weak_ptr<paradice::connection>(connection)
          , _1));

    connection->on_control_sequence(
        bind(
            &on_control_sequence
          , boost::weak_ptr<paradice::connection>(connection)
          , _1));
    
    BOOST_AUTO(window,  connection->get_window());
    BOOST_AUTO(content, window->get_content());

    window->on_repaint.connect(
        bind(&on_repaint, weak_ptr<paradice::socket>(socket), _1));

    BOOST_AUTO(user_interface, make_shared<hugin::user_interface>());
    user_interface->on_username_entered(bind(
        &on_username_entered
      , weak_context
      , weak_ptr<paradice::client>(client)
      , _1));

    user_interface->on_input_entered(bind(
        &on_input_entered
      , weak_context
      , weak_ptr<paradice::client>(client)
      , _1));
    
    client->set_user_interface(user_interface);
    ctx->add_client(client);
    ctx->update_names();
    
    content->set_size(munin::extent(80, 24));
    content->set_layout(
        make_shared< munin::grid_layout<munin::ansi::element_type> >(1, 1));

    content->add_component(user_interface);
    content->set_focus();

    std::string string_data = munin::ansi::set_window_title("Paradice9");
    
    runtime_array<u8> data(string_data.size());
    copy(string_data.begin(), string_data.end(), data.begin());
    socket->async_write(data, NULL);
}

int main(int argc, char *argv[])
{
    srand(static_cast<unsigned int>(time(NULL)));

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

    asio::io_service              io_service;
    shared_ptr<paradice::context> ctx(new context_impl);
    weak_ptr<paradice::context>   weak_context(ctx);
    
    paradice::server server(
        io_service
      , weak_context
      , port
      , bind(&on_accept, weak_context, _1, ref(io_service)));
    
    io_service.run();

    return EXIT_SUCCESS;
}

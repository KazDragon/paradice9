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
#include "paradice/client.hpp"
#include "paradice/connection.hpp"
#include "paradice/communication.hpp"
#include "paradice/configuration.hpp"
#include "paradice/rules.hpp"
#include "paradice/who.hpp"
#include "odin/tokenise.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <boost/typeof/typeof.hpp>
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace boost;
using namespace odin;

namespace po = program_options;

static string const intro =
"             _                                                         \r\n"
"    |/ _._  | \\.__. _  _ ._/ _                                        \r\n"
"    |\\(_|/_ |_/|(_|(_|(_)| |_>                                        \r\n"
"                    _|                                                 \r\n"
"                                                                       \r\n"
"                   ___                   ___           ___             \r\n"
"                  / _ \\___ ________ ____/ (_)______   / _ \\          \r\n"
"                 / ___/ _ `/ __/ _ `/ _  / / __/ -_)  \\_, /           \r\n"
"                /_/   \\_,_/_/  \\_,_/\\_,_/_/\\__/\\__/  /___/        \r\n"
"                                                           v0.2        \r\n"
"                                                                       \r\n"
"Enter a name by which you wish to be known: ";

#define PARADICE_CMD_ENTRY_NOP(name) \
    { name,        NULL                                   }
    
#define PARADICE_CMD_ENTRY(func) \
    { #func,        bind(&paradice::do_##func, _1, _2, _3) }
    
#define PARADICE_CMD_ALIAS(func, alias) \
    { alias,        bind(&paradice::do_##func, _1, _2, _3) }
    
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
  
  , PARADICE_CMD_ENTRY(sayto)
  , PARADICE_CMD_ALIAS(sayto, ">")
  
  , PARADICE_CMD_ENTRY(emote)
  , PARADICE_CMD_ALIAS(emote, ":")

  , PARADICE_CMD_ENTRY(who)
  
  , PARADICE_CMD_ENTRY(set)
  , PARADICE_CMD_ENTRY(backtrace)
  , PARADICE_CMD_ENTRY(rename)
  , PARADICE_CMD_ENTRY(prefix)
  , PARADICE_CMD_ENTRY(roll)
  
  , PARADICE_CMD_ENTRY(quit)
};

void on_name_entered(
    shared_ptr<paradice::context> &ctx
  , shared_ptr<paradice::client>  &client
  , string const                  &input)
{
    BOOST_AUTO(name, odin::tokenise(input).first);

    if (!paradice::is_acceptible_name(name))
    {
        client->get_connection()->write(str(format(
            "Your name must be between %d and %d characters long and comprise "
            "only alphabetical characters.\r\n"
            "Enter a name by which you wish to be known: ")
                % paradice::minimum_name_size
                % paradice::maximum_name_size));
    }
    else
    {
        // Ensure correct capitalisation
        name[0] = toupper(name[0]);

        client->set_name(name);

        pair<u16, u16> window_size = 
            client->get_connection()->get_window_size();
            
        string text = str(format(
            "\x1B[2J"      // Erase screen
            "\x1B[9;%dr"   // Set scrolling region
            "\x1B[%d;0f") // Force to end.
            % window_size.second
            % window_size.second);

        client->get_connection()->write(text);

        paradice::message_to_room(
            ctx
          , "\r\n#SERVER: " 
          + client->get_name() 
          + " has entered Paradice.\r\n",
            client);

        client->set_level(paradice::client::level_in_game);

        BOOST_FOREACH(
            shared_ptr<paradice::client> curr_client
          , ctx->get_clients())
        {
            INVOKE_PARADICE_COMMAND(who, ctx, "auto", curr_client);
        }
    }
}

void on_data(
    weak_ptr<paradice::context> &weak_context
  , weak_ptr<paradice::client>  &weak_client
  , std::string const          &input);

void on_command(
    weak_ptr<paradice::context>  &weak_context
  , shared_ptr<paradice::client> &client
  , std::string const            &input)
{
    BOOST_AUTO(ctx, weak_context.lock());
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
        on_data(weak_context, weak_client, client->get_last_command());
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
        "\r\nDidn't understand that.  Available commands are:\r\n";

    BOOST_FOREACH(command const &cur_command, command_list)
    {
        text += cur_command.command_ + " ";
    }

    text += "\r\n";

    paradice::send_to_player(ctx, text, client);
}

void on_data(
    weak_ptr<paradice::context> &weak_context
  , weak_ptr<paradice::client>  &weak_client
  , std::string const           &input)
{
    BOOST_AUTO(ctx,    weak_context.lock());
    BOOST_AUTO(client, weak_client.lock());
    
    if (ctx && client)
    {
        if (client->get_level() == paradice::client::level_intro_screen)
        {
            on_name_entered(ctx, client, input);
        }
        else // client->get_level() == paradice::client::level_in_game
        {
            if (input.empty())
            {
                return;
            }

            if (client->get_command_mode() == paradice::client::command_mode_mud)
            {
                on_command(weak_context, client, input);
            }
            else
            {
                if (input[0] == '/')
                {
                    if (input.size() >= 2 && input[1] == '!')
                    {
                        on_data(
                            weak_context
                          , weak_client, "/" + client->get_last_command());
                    }
                    else
                    {
                        on_command(weak_context, client, input.substr(1));
                    }
                }
                else
                {
                    INVOKE_PARADICE_COMMAND(say, ctx, input, client);
                }
            }
        }
    }
}

void on_socket_death(
    weak_ptr<paradice::context> &weak_context
  , weak_ptr<paradice::client>  &weak_client)
{
    BOOST_AUTO(ctx,    weak_context.lock());
    BOOST_AUTO(client, weak_client.lock());
    
    if (ctx && client)
    {
        if (client->get_name() != "")
        {
            paradice::message_to_room(
                ctx
              , "\r\n#SERVER: " 
              + client->get_name()
              + " has left Paradice.\r\n",
                client);
        }

        ctx->remove_client(client);

        BOOST_FOREACH(
            shared_ptr<paradice::client> curr_client
          , ctx->get_clients())
        {
            INVOKE_PARADICE_COMMAND(who, ctx, "auto", curr_client);
        }
    }
}

static void on_window_size_changed(
    weak_ptr<paradice::context> weak_context
  , weak_ptr<paradice::client>  weak_client
  , u16                         width
  , u16                         height)
{
    BOOST_AUTO(ctx,    weak_context.lock());
    BOOST_AUTO(client, weak_client.lock());
    
    if (ctx && client)
    {
        if (client->get_level() == paradice::client::level_in_game)
        {
            string text = str(format(
                "\x1B[2J"      // Erase screen
                "\x1B[9;%dr"   // Set scrolling region
                "\x1B[%d;0f")  // Force to end.
              % height
              % height);
            
            client->get_connection()->write(text);

            INVOKE_PARADICE_COMMAND(who, ctx, "auto", client);
            INVOKE_PARADICE_COMMAND(backtrace, ctx, "", client);
        }
    }
}

void on_accept(
    weak_ptr<paradice::context>        &weak_context
  , shared_ptr<paradice::socket> const &socket)
{
    shared_ptr<paradice::context> ctx(weak_context.lock());
    shared_ptr<paradice::client>  client(new paradice::client);

    socket->on_death(bind(
        &on_socket_death
      , weak_context
      , weak_ptr<paradice::client>(client)));

    client->set_connection(shared_ptr<paradice::connection>(
        new paradice::connection(
            socket
          , bind(
               &on_data
             , weak_context
             , weak_ptr<paradice::client>(client)
             , _1))));

    ctx->add_client(client);    
    
    client->get_connection()->write(intro);
    
    client->get_connection()->on_window_size_changed(
        bind(&on_window_size_changed, 
            weak_context
          , weak_ptr<paradice::client>(client)
          , _1
          , _2));
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
      , bind(&on_accept, weak_context, _1));
    
    io_service.run();

    return EXIT_SUCCESS;
}

// ==========================================================================
// Paradice Communication
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
#include "communication.hpp"
#include "client.hpp"
#include "connection.hpp"
#include "context.hpp"
#include "utility.hpp"
#include "odin/tokenise.hpp"
#include "odin/types.hpp"
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

using namespace std;
using namespace boost;
using namespace odin;

namespace paradice {
    
// ==========================================================================
// MESSAGE_TO_ALL
// ==========================================================================
void message_to_all(
    shared_ptr<context> &ctx
  , string const        &text)
{
    runtime_array< shared_ptr<client> > clients = ctx->get_clients();
    
    BOOST_FOREACH(shared_ptr<client> cur_client, clients)
    {
        cur_client->get_connection()->write(text);
        cur_client->add_backtrace(
            boost::algorithm::trim_copy(text));
    }
}

// ==========================================================================
// MESSAGE_TO_PLAYER
// ==========================================================================
void message_to_player(
    shared_ptr<context> &ctx
  , string const        &text
  , shared_ptr<client>  &conn)
{
    conn->get_connection()->write(text);
    conn->add_backtrace(
        boost::algorithm::trim_copy(text));
}

// ==========================================================================
// MESSAGE_TO_ROOM
// ==========================================================================
void message_to_room(
    shared_ptr<context> &ctx
  , string const        &text 
  , shared_ptr<client>  &conn)
{
    runtime_array< shared_ptr<client> > clients = ctx->get_clients();

    BOOST_FOREACH(shared_ptr<client> cur_client, clients)
    {
        if (cur_client != conn)
        {
            if (cur_client->get_level() == client::level_in_game)
            {
                cur_client->get_connection()->write(text);
                cur_client->add_backtrace(
                    boost::algorithm::trim_copy(text));
            }
        }
    }
}

// ==========================================================================
// SEND_TO_ALL
// ==========================================================================
void send_to_all(
    shared_ptr<context> &ctx
  , string const        &text)
{
    runtime_array< shared_ptr<client> > clients = ctx->get_clients();

    BOOST_FOREACH(shared_ptr<client> cur_client, clients)
    {
        cur_client->get_connection()->write(text);
    }
}

// ==========================================================================
// SEND_TO_PLAYER
// ==========================================================================
void send_to_player(
    shared_ptr<context> &ctx
  , string const        &text
  , shared_ptr<client>  &conn)
{
    conn->get_connection()->write(text);
}

// ==========================================================================
// SEND_TO_ROOM
// ==========================================================================
void send_to_room(
    shared_ptr<context> &ctx
  , string const        &text, 
    shared_ptr<client>  &conn)
{
    runtime_array< shared_ptr<client> > clients = ctx->get_clients();

    BOOST_FOREACH(shared_ptr<client> cur_client, clients)
    {
        if (cur_client != conn)
        {
            if (cur_client->get_level() == client::level_in_game)
            {
                cur_client->get_connection()->write(text);
            }
        }
    }
}

// ==========================================================================
// PARADICE COMMAND: SAY
// ==========================================================================
PARADICE_COMMAND_IMPL(say)
{
    message_to_player(ctx, str(
        format("\r\nYou say, \"%s\"\r\n")
            % arguments)
      , player);


    message_to_room(ctx, str(
        format("\r\n%s says, \"%s\"\r\n")
            % player->get_name()
            % arguments)
      , player);
}

// ==========================================================================
// PARADICE COMMAND: SAYTO
// ==========================================================================
PARADICE_COMMAND_IMPL(sayto)
{
    pair<string, string> arg = odin::tokenise(arguments);

    if (arg.first == "")
    {
        send_to_player(
            ctx
          , "\r\n Usage: sayto [player] [message]"
            "\r\n     or"
            "\r\n Usage: > [player] [message]"
            "\r\n"
          , player);

        return;
    }

    runtime_array< shared_ptr<client> > clients = ctx->get_clients();

    BOOST_FOREACH(shared_ptr<client> cur_client, clients)
    {
        if(is_iequal(cur_client->get_name(), arg.first))
        {
            message_to_player(ctx, str(
                format("\r\nYou say to %s, \"%s\"\r\n")
                    % cur_client->get_name()
                    % arg.second)
              , player);

            message_to_player(ctx, str(
                format("\r\n%s says to you, \"%s\"\r\n")
                    % player->get_name()
                    % arg.second)
              , cur_client);

            return;
        }
    }

    send_to_player(
        ctx, "\r\nCouldn't find anyone by that name to talk to.\r\n", player);
}

// ==========================================================================
// PARADICE COMMAND: EMOTE
// ==========================================================================
PARADICE_COMMAND_IMPL(emote)
{
    if (arguments.empty())
    {
        static string const usage_message =
            "\r\n USAGE:   emote <some action>"
            "\r\n EXAMPLE: emote bounces off the walls."
            "\r\n\r\n";

        send_to_player(ctx, usage_message, player);
    }

    message_to_all(ctx, str(
        format("\r\n%s %s\r\n")
            % player->get_name()
            % arguments));
}

// ==========================================================================
// PARADICE COMMAND: BACKTRACE
// ==========================================================================
PARADICE_COMMAND_IMPL(backtrace)
{
    pair<u16, u16> window_size = player->get_connection()->get_window_size();
    u16 window_width = window_size.first;
    
    string backtrace = "=== Backtrace: ";
    
    if (window_width > u16(backtrace.size()))
    {
        backtrace += string((window_width - 1) - backtrace.size(), '='); 
    }
    
    backtrace = "\r\n" + backtrace + "\r\n";
    
    send_to_player(ctx, backtrace + player->get_backtrace(), player);
}


}

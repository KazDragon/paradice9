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
#include "hugin/user_interface.hpp"
#include "munin/ansi/protocol.hpp"
#include "odin/tokenise.hpp"
#include "odin/types.hpp"
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/typeof/typeof.hpp>

using namespace std;
using namespace boost;
using namespace odin;

namespace paradice {
    
// ==========================================================================
// SEND_TO_ALL
// ==========================================================================
void send_to_all(
    shared_ptr<context> &ctx
  , string const        &text)
{
    send_to_all(ctx, munin::ansi::elements_from_string(text));
}

// ==========================================================================
// SEND_TO_ALL
// ==========================================================================
void send_to_all(
    shared_ptr<context>                            &ctx
  , runtime_array<munin::ansi::element_type> const &text)
{
    BOOST_AUTO(clients, ctx->get_clients());
    
    BOOST_FOREACH(shared_ptr<client> cur_client, clients)
    {
        cur_client->get_user_interface()->add_output_text(text);
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
    send_to_player(ctx, munin::ansi::elements_from_string(text), conn);
}

// ==========================================================================
// SEND_TO_PLAYER
// ==========================================================================
void send_to_player(
    shared_ptr<context>                            &ctx
  , runtime_array<munin::ansi::element_type> const &text
  , shared_ptr<client>                             &conn)
{
    conn->get_user_interface()->add_output_text(text);
}

// ==========================================================================
// SEND_TO_ROOM
// ==========================================================================
void send_to_room(
    shared_ptr<context> &ctx
  , string const        &text 
  , shared_ptr<client>  &conn)
{
    send_to_room(ctx, munin::ansi::elements_from_string(text), conn);
}

// ==========================================================================
// SEND_TO_ROOM
// ==========================================================================
void send_to_room(
    shared_ptr<context> &ctx
  , runtime_array<munin::ansi::element_type> const &text
  , shared_ptr<client>  &conn)
{
    BOOST_AUTO(clients, ctx->get_clients());
    
    BOOST_FOREACH(shared_ptr<client> cur_client, clients)
    {
        if (cur_client != conn)
        {
            cur_client->get_user_interface()->add_output_text(text);
        }
    }
}

// ==========================================================================
// PARADICE COMMAND: SAY
// ==========================================================================
PARADICE_COMMAND_IMPL(say)
{
    send_to_player(ctx, str(
        format("You say, \"%s\"\n")
            % arguments)
      , player);


    send_to_room(ctx, str(
        format("%s says, \"%s\"\n")
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
          , "\n Usage: sayto [player] [message]"
            "\n     or"
            "\n Usage: > [player] [message]"
            "\n"
          , player);

        return;
    }

    runtime_array< shared_ptr<client> > clients = ctx->get_clients();

    BOOST_FOREACH(shared_ptr<client> cur_client, clients)
    {
        if(is_iequal(cur_client->get_name(), arg.first))
        {
            send_to_player(ctx, str(
                format("You say to %s, \"%s\"\n")
                    % cur_client->get_name()
                    % arg.second)
              , player);

            send_to_player(ctx, str(
                format("%s says to you, \"%s\"\n")
                    % player->get_name()
                    % arg.second)
              , cur_client);

            return;
        }
    }

    send_to_player(
        ctx, "\nCouldn't find anyone by that name to talk to.\n", player);
}

// ==========================================================================
// PARADICE COMMAND: EMOTE
// ==========================================================================
PARADICE_COMMAND_IMPL(emote)
{
    if (arguments.empty())
    {
        static string const usage_message =
            "\n USAGE:   emote <some action>"
            "\n EXAMPLE: emote bounces off the walls."
            "\n\n";

        send_to_player(ctx, usage_message, player);
    }

    send_to_all(ctx, str(
        format("%s %s\n")
            % player->get_name()
            % arguments));
}

}

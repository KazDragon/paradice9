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
#include "character.hpp"
#include "client.hpp"
#include "connection.hpp"
#include "context.hpp"
#include "utility.hpp"
#include "hugin/user_interface.hpp"
#include "munin/ansi/protocol.hpp"
#include "munin/algorithm.hpp"
#include "odin/tokenise.hpp"
#include "odin/types.hpp"
#include <boost/algorithm/string/trim.hpp>
#include <boost/format.hpp>

namespace paradice {

// ==========================================================================
// SEND_TO_ALL
// ==========================================================================
void send_to_all(
    std::shared_ptr<context> &ctx,
    std::string const        &text)
{
    send_to_all(ctx, munin::string_to_elements(text));
}

// ==========================================================================
// SEND_TO_ALL
// ==========================================================================
void send_to_all(
    std::shared_ptr<context>               &ctx,
    std::vector<munin::element_type> const &text)
{
    for (auto cur_client : ctx->get_clients())
    {
        cur_client->get_user_interface()->add_output_text(text);
    }
}

// ==========================================================================
// SEND_TO_PLAYER
// ==========================================================================
void send_to_player(
    std::shared_ptr<context> &ctx,
    std::string const        &text,
    std::shared_ptr<client>  &conn)
{
    send_to_player(ctx, munin::string_to_elements(text), conn);
}

// ==========================================================================
// SEND_TO_PLAYER
// ==========================================================================
void send_to_player(
    std::shared_ptr<context>               &ctx,
    std::vector<munin::element_type> const &text,
    std::shared_ptr<client>                &conn)
{
    conn->get_user_interface()->add_output_text(text);
}

// ==========================================================================
// SEND_TO_ROOM
// ==========================================================================
void send_to_room(
    std::shared_ptr<context> &ctx,
    std::string const        &text,
    std::shared_ptr<client>  &conn)
{
    send_to_room(ctx, munin::string_to_elements(text), conn);
}

// ==========================================================================
// SEND_TO_ROOM
// ==========================================================================
void send_to_room(
    std::shared_ptr<context>               &ctx,
    std::vector<munin::element_type> const &text,
    std::shared_ptr<client>                &conn)
{
    for (auto cur_client : ctx->get_clients())
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
    if (arguments.empty())
    {
        send_to_player(
            ctx
          , "\n Usage: say [message]"
            "\n     or"
            "\n Usage: . [message]"
            "\n"
          , player);

        return;
    }

    send_to_player(ctx, boost::str(
        boost::format("You say, \"%s\\x\"\n")
            % arguments)
      , player);


    send_to_room(ctx, boost::str(
        boost::format("%s says, \"%s\\x\"\n")
            % player->get_character()->get_name()
            % arguments)
      , player);
}

// ==========================================================================
// PARADICE COMMAND: WHISPER
// ==========================================================================
PARADICE_COMMAND_IMPL(whisper)
{
    auto arg = odin::tokenise(arguments);

    if (arg.first.empty() || arg.second.empty())
    {
        send_to_player(
            ctx
          , "\n Usage: whisper [player] [message]"
            "\n     or"
            "\n Usage: > [player] [message]"
            "\n"
          , player);

        return;
    }

    for (auto cur_client : ctx->get_clients())
    {
        if(is_iequal(cur_client->get_character()->get_name(), arg.first))
        {
            send_to_player(ctx, boost::str(
                boost::format("You say to %s, \"%s\\x\"\n")
                    % cur_client->get_character()->get_name()
                    % arg.second)
              , player);

            send_to_player(ctx, boost::str(
                boost::format("%s says to you, \"%s\\x\"\n")
                    % player->get_character()->get_name()
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
        static std::string const usage_message =
            "\n USAGE:   emote <some action>"
            "\n EXAMPLE: emote bounces off the walls."
            "\n\n";

        send_to_player(ctx, usage_message, player);
    }

    send_to_all(ctx, boost::str(
        boost::format("%s %s\n")
            % player->get_character()->get_name()
            % arguments));
}

}

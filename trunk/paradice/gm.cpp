// ==========================================================================
// Paradice Encounters
//
// Copyright (C) 2013 Matthew Chaplain, All Rights Reserved.  
// This file is covered by the MIT Licence:
//
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.
// ==========================================================================
#include "paradice/gm.hpp"
#include "paradice/active_encounter.hpp"
#include "paradice/client.hpp"
#include "paradice/communication.hpp"
#include "paradice/context.hpp"
#include "hugin/user_interface.hpp"
#include "odin/tokenise.hpp"
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/typeof/typeof.hpp>

using namespace odin;
using namespace boost;
using namespace std;

namespace paradice {

namespace {

PARADICE_COMMAND_IMPL(gm_tools)
{
    BOOST_AUTO(user_interface, player->get_user_interface());
    user_interface->select_face(hugin::FACE_GM_TOOLS);
    user_interface->set_focus();
}

PARADICE_COMMAND_IMPL(gm_encounter_show)
{
    ctx->set_active_encounter_visible(true);
}

PARADICE_COMMAND_IMPL(gm_encounter_hide)
{
    ctx->set_active_encounter_visible(false);
}

static void add_encounter_player(
    shared_ptr<character> ch
  , shared_ptr<context> ctx)
{
    // Don't add the GM.
    if (ch->get_gm_level() != 0)
    {
        return;
    }

    bool found = false;

    BOOST_AUTO(enc, ctx->get_active_encounter());
    BOOST_FOREACH(active_encounter::entry &entry, enc->entries_)
    {
        BOOST_AUTO(
            participant_player
          , get<active_encounter::player>(&entry.participant_));

        // If this entry is not a character, then move to the next entry.
        if (!participant_player)
        {
            continue;
        }

        BOOST_AUTO(
            participant_character
          , participant_player->character_.lock());

        // If this entry is the same as the character, then it has already 
        // been added, so we can skip to the next player.
        if (participant_character == ch)
        {
            found = true;
            break;
        }
    }

    // Only add characters that are not already in the encounter.
    if (!found)
    {
        add_character(enc, ch);
    }
}

static void remove_encounter_participant(
    u32 id
  , shared_ptr<context> ctx)
{
    namespace bll = boost::lambda;

    BOOST_AUTO(enc, ctx->get_active_encounter());

    enc->entries_.erase(
        remove_if(
            enc->entries_.begin()
          , enc->entries_.end()
          , bll::bind(&active_encounter::entry::id_, bll::_1) == id)
      , enc->entries_.end());

    ctx->update_active_encounter();
}

PARADICE_COMMAND_IMPL(gm_encounter_add_player)
{
    BOOST_AUTO(arg0, tokenise(arguments));
    string argument = arg0.first;

    BOOST_FOREACH(shared_ptr<client> cli, ctx->get_clients())
    {
        BOOST_AUTO(ch, cli->get_character());

        if (ch->get_name() == argument)
        {
            add_encounter_player(ch, ctx);
            break;
        }
    }

    ctx->update_active_encounter();
}

PARADICE_COMMAND_IMPL(gm_encounter_add_players)
{
    BOOST_FOREACH(shared_ptr<client> cli, ctx->get_clients())
    {
        add_encounter_player(cli->get_character(), ctx);
    }

    ctx->update_active_encounter();
}

PARADICE_COMMAND_IMPL(gm_encounter_add)
{
    BOOST_AUTO(arg0, tokenise(arguments));
    string argument = arg0.first;

#define DISPATCH_GM_ENCOUNTER_ADD_COMMAND(cmd) \
    if (argument == #cmd) \
    { \
        INVOKE_PARADICE_COMMAND(gm_encounter_add_##cmd, ctx, arg0.second, player); \
        return; \
    }

    DISPATCH_GM_ENCOUNTER_ADD_COMMAND(player);
    DISPATCH_GM_ENCOUNTER_ADD_COMMAND(players);
#undef DISPATCH_GM_ENCOUNTER_ADD_COMMAND

    send_to_player(
        ctx
      , "Unknown: gm encounter add " + argument
      , player);

}

PARADICE_COMMAND_IMPL(gm_encounter_remove)
{
    BOOST_AUTO(arg0, tokenise(arguments));
    string argument = arg0.first;

    BOOST_AUTO(id, lexical_cast<u32>(argument));

    if (id != 0)
    {
        remove_encounter_participant(id, ctx);
        return;
    }

    send_to_player(
        ctx
      , "USAGE: gm encounter remove <id>"
      , player);
}

PARADICE_COMMAND_IMPL(gm_encounter)
{
    BOOST_AUTO(arg0, tokenise(arguments));
    string argument = arg0.first;

#define DISPATCH_GM_ENCOUNTER_COMMAND(cmd) \
    if (argument == #cmd) \
    { \
        INVOKE_PARADICE_COMMAND(gm_encounter_##cmd, ctx, arg0.second, player); \
        return; \
    }

    DISPATCH_GM_ENCOUNTER_COMMAND(show);
    DISPATCH_GM_ENCOUNTER_COMMAND(hide);
    DISPATCH_GM_ENCOUNTER_COMMAND(add);
    DISPATCH_GM_ENCOUNTER_COMMAND(remove);

#undef DISPATCH_GM_ENCOUNTER_COMMAND

    send_to_player(
        ctx
      , "Unknown: gm encounter " + argument
      , player);
}

}

// ==========================================================================
// PARADICE COMMAND: GM
// ==========================================================================
PARADICE_COMMAND_IMPL(gm)
{
    BOOST_AUTO(arg0, tokenise(arguments));
    string argument = arg0.first;

#define DISPATCH_GM_COMMAND(cmd) \
    if (argument == #cmd) \
    { \
        INVOKE_PARADICE_COMMAND(gm_##cmd, ctx, arg0.second, player); \
        return; \
    }
    
    DISPATCH_GM_COMMAND(tools);
    DISPATCH_GM_COMMAND(encounter);
#undef DISPATCH_GM_COMMAND

    send_to_player(
        ctx
      , "USAGE:   gm <command> [<arguments>...]\n"
        "EXAMPLE: gm tools\n"
        "EXAMPLE: gm encounter add player <player>\n"
        "EXAMPLE: gm encounter add players\n"
        "EXAMPLE: gm encounter remove <id>\n"
      , player);
}

}

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
#include "paradice/tokenise.hpp"
// #include "hugin/user_interface.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

namespace paradice {

namespace {

PARADICE_COMMAND_IMPL(gm_tools)
{
    // auto user_interface = player->get_user_interface();
    // user_interface->select_face(hugin::FACE_GM_TOOLS);
    // user_interface->set_focus();
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
    std::shared_ptr<character> ch,
    std::shared_ptr<context> ctx)
{
    // Don't add the GM.
    if (ch->get_gm_level() != 0)
    {
        return;
    }

    bool found = false;

    auto enc = ctx->get_active_encounter();
    for (auto &entry : enc->entries_)
    {
        auto participant_player = 
            boost::get<active_encounter::player>(&entry.participant_);

        // If this entry is not a character, then move to the next entry.
        if (!participant_player)
        {
            continue;
        }

        auto participant_character =
            participant_player->character_.lock();

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
    std::uint32_t id,
    std::shared_ptr<context> ctx)
{
    auto enc = ctx->get_active_encounter();

    enc->entries_.erase(
        std::remove_if(
            enc->entries_.begin(),
            enc->entries_.end(),
            [id](auto const &entry)
            {
                return entry.id_ == id;
            }),
        enc->entries_.end());

    ctx->update_active_encounter();
}

PARADICE_COMMAND_IMPL(gm_encounter_add_player)
{
    auto arg0 = paradice::tokenise(arguments);
    auto argument = arg0.first;

    for (auto cli : ctx->get_clients())
    {
        auto ch = cli->get_character();

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
    for (auto cli : ctx->get_clients())
    {
        add_encounter_player(cli->get_character(), ctx);
    }

    ctx->update_active_encounter();
}

PARADICE_COMMAND_IMPL(gm_encounter_add)
{
    auto arg0 = paradice::tokenise(arguments);
    auto argument = arg0.first;

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
    auto arg0 = paradice::tokenise(arguments);
    auto argument = arg0.first;

    if (argument == "all")
    {
        auto enc = ctx->get_active_encounter();
        enc->entries_.clear();
        ctx->update_active_encounter();
    }
    else
    {
        std::uint32_t id = 0;
 
        try
        {
            id = boost::lexical_cast<std::uint32_t>(argument);
        }
        catch(boost::bad_lexical_cast const &)
        {
            send_to_player(
                ctx
              , "USAGE: gm encounter remove (all|<id>)\n"
              , player);
            return;
        }

        remove_encounter_participant(id, ctx);
        ctx->update_active_encounter();
        return;
    }

    send_to_player(
        ctx
      , "USAGE: gm encounter remove (all|<id>)\n"
      , player);
}

PARADICE_COMMAND_IMPL(gm_encounter_move)
{
    auto arg0 = paradice::tokenise(arguments);
    auto id_arg = arg0.first;

    std::uint32_t id = 0;

    try
    {
        id = boost::lexical_cast<std::uint32_t>(id_arg);
    }
    catch(boost::bad_lexical_cast const &)
    {
        send_to_player(
            ctx
          , "USAGE: gm encounter move <id> (up|down|top|bottom)\n"
          , player);
        return;
    }

    auto enc = ctx->get_active_encounter();
    auto entry_it = enc->entries_.begin();

    for (; entry_it != enc->entries_.end(); ++entry_it)
    {
        if (entry_it->id_ == id)
        {
            break;
        }
    }

    if (entry_it == enc->entries_.end())
    {
        send_to_player(
            ctx
          , boost::str(boost::format(
                "Error: No entry in the active encounter with id %d\n")
                % id)
          , player);
        return;
    }

    auto arg1 = paradice::tokenise(arg0.second);
    auto dir_arg = arg1.first;

    if (dir_arg == "up")
    {
        if (entry_it != enc->entries_.begin())
        {
            std::iter_swap(entry_it, entry_it - 1);
            ctx->update_active_encounter();
        }
    }
    else if (dir_arg == "down")
    {
        if (entry_it + 1 != enc->entries_.end())
        {
            std::iter_swap(entry_it, entry_it + 1);
            ctx->update_active_encounter();
        }
    }
    else if (dir_arg == "top")
    {
        active_encounter::entry cloned_entry = *entry_it;
        enc->entries_.erase(entry_it);
        enc->entries_.insert(enc->entries_.begin(), cloned_entry);
        ctx->update_active_encounter();
    }
    else if (dir_arg == "bottom")
    {
        active_encounter::entry cloned_entry = *entry_it;
        enc->entries_.erase(entry_it);
        enc->entries_.insert(enc->entries_.end(), cloned_entry);
        ctx->update_active_encounter();
    }
    else
    {
        send_to_player(
            ctx
          , "USAGE: gm encounter move <id> (up|down|top|bottom)\n"
          , player);
    }
}

PARADICE_COMMAND_IMPL(gm_encounter)
{
    auto arg0 = paradice::tokenise(arguments);
    auto argument = arg0.first;

#define DISPATCH_GM_ENCOUNTER_COMMAND(cmd) \
    if (argument == #cmd) \
    { \
        INVOKE_PARADICE_COMMAND(gm_encounter_##cmd, ctx, arg0.second, player); \
        return; \
    }

    DISPATCH_GM_ENCOUNTER_COMMAND(add);
    DISPATCH_GM_ENCOUNTER_COMMAND(hide);
    DISPATCH_GM_ENCOUNTER_COMMAND(move);
    DISPATCH_GM_ENCOUNTER_COMMAND(remove);
    DISPATCH_GM_ENCOUNTER_COMMAND(show);

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
    auto arg0 = paradice::tokenise(arguments);
    auto argument = arg0.first;

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
        "EXAMPLE: gm encounter show\n"
        "EXAMPLE: gm encounter hide\n"
        "EXAMPLE: gm encounter add player <player>\n"
        "EXAMPLE: gm encounter add players\n"
        "EXAMPLE: gm encounter remove (all|<id>)\n"
        "EXAMPLE: gm encounter move <id> (up|down|top|bottom)\n"
        "\n"
      , player);
}

}

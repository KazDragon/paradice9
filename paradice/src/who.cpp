// ==========================================================================
// Paradice Who
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
#include "paradice/who.hpp"
#include "paradice/character.hpp"
#include "paradice/client.hpp"
#include "paradice/communication.hpp"
#include "paradice/connection.hpp"
#include "paradice/context.hpp"
#include "paradice/utility.hpp"
#include "munin/algorithm.hpp"
#include "odin/tokenise.hpp"
#include "odin/core.hpp"
#include "terminalpp/string.hpp"
#include <boost/algorithm/string/trim.hpp>
#include <boost/format.hpp>
#include <cstdio>

namespace paradice {

// ==========================================================================
// IS_ACCEPTIBLE_NAME
// ==========================================================================
bool is_acceptible_name(std::string const &name)
{
    if (name.length() < 3)
    {
        return false;
    }
    
    for (auto ch : name)
    {
        if (!boost::is_alpha()(ch))
        {
            return false;
        }
    }
    
    // Profanity filter HERE!
    return true;
}

// ==========================================================================
// PARADICE COMMAND: TITLE
// ==========================================================================
PARADICE_COMMAND_IMPL(title)
{
    using namespace terminalpp::literals;
    
    auto character = player->get_character();
    character->set_suffix(boost::algorithm::trim_copy(arguments));

    send_to_player(ctx, boost::str(
        boost::format("\r\nYou are now %s.\r\n")
            % ctx->get_moniker(character))
      , player);
    
    send_to_room(ctx, boost::str(
        boost::format("\r\n%s is now %s.\r\n")
            % player->get_character()->get_name()
            % ctx->get_moniker(character))
      , player);

    try
    {
        ctx->save_character(character);
    }
    catch(std::exception &ex)
    {
        printf("Error saving character %s: %s\n",
            character->get_name().c_str(), ex.what());

        send_to_player(
            ctx
          , "\\[1There was an error saving your character."_ets
          , player);
    }

    ctx->update_names();
}

// ==========================================================================
// PARADICE COMMAND: PREFIX
// ==========================================================================
PARADICE_COMMAND_IMPL(prefix)
{
    using namespace terminalpp::literals;

    auto character = player->get_character();
    character->set_prefix(boost::algorithm::trim_copy(arguments));

    send_to_player(ctx, boost::str(
        boost::format("\r\nYou are now %s.\r\n")
            % ctx->get_moniker(character))
      , player);
    
    send_to_room(ctx, boost::str(
        boost::format("\r\n%s is now %s.\r\n")
            % player->get_character()->get_name()
            % ctx->get_moniker(character))
      , player);


    try
    {
        ctx->save_character(character);
    }
    catch(std::exception &ex)
    {
        printf("Error saving character %s: %s\n",
            character->get_name().c_str(), ex.what());

        send_to_player(
            ctx
          , "\\[1There was an error saving your character."_ets
          , player);
    }

    ctx->update_names();
}

}


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
#include "paradice/client.hpp"
#include "paradice/communication.hpp"
#include "paradice/context.hpp"
#include "hugin/user_interface.hpp"
#include "odin/tokenise.hpp"
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
    BOOST_AUTO(clients, ctx->get_clients());
    BOOST_FOREACH(shared_ptr<client> cli, clients)
    {
        if (cli)
        {
            cli->get_user_interface()->show_active_encounter_window();
        }
    }
}

PARADICE_COMMAND_IMPL(gm_encounter_hide)
{
    BOOST_AUTO(clients, ctx->get_clients());
    BOOST_FOREACH(shared_ptr<client> cli, clients)
    {
        if (cli)
        {
            cli->get_user_interface()->hide_active_encounter_window();
        }
    }
}

PARADICE_COMMAND_IMPL(gm_encounter_add_player)
{
    BOOST_AUTO(arg0, tokenise(arguments));
    string argument = arg0.first;


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
#undef DISPATCH_GM_ENCOUNTER_ADD_COMMAND

    send_to_player(
        ctx
      , "Unknown: gm encounter add " + argument
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
      , "<TODO: usage statement>\n"
      , player);
}

}

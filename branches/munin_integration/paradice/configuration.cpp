// ==========================================================================
// Paradice Configuration
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
#include "configuration.hpp"
#include "client.hpp"
#include "communication.hpp"
#include "connection.hpp"
#include "who.hpp"
#include "odin/tokenise.hpp"
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

using namespace std;
using namespace boost;

namespace paradice {

// ==========================================================================
// PARADICE COMMAND: SET
// ==========================================================================
PARADICE_COMMAND_IMPL(set)
{
    static string const usage =
        "\r\n USAGE:   set <setting> [<argument>]"
        "\r\n Current setting are:"
        "\r\n     commandmode (mud|mmo)"
        "\r\n\r\n";

    pair<string, string> token0 = odin::tokenise(arguments);

    if (token0.first.empty())
    {
        send_to_player(ctx, usage, player);
        return;
    }

    // TODO: when we have more settings, factor them out into a map of
    // settings functions.
    if (token0.first == "commandmode")
    {
        static string const commandmode_usage =
            "\r\n USAGE:   set commandmode (mud|mmo)"
            "\r\n Example: set commandmode mmo"
            "\r\n\r\n";

        pair<string, string> token1 = odin::tokenise(token0.second);
    
        if (token1.first.empty())
        {
            send_to_player(ctx, commandmode_usage, player);
            return;
        }

        // TODO: make case sensitive, tie settings to a map, etc.
        if (token1.first == "mud")
        {
            player->set_command_mode(paradice::client::command_mode_mud);
        }
        else if (token1.first == "mmo")
        {
            player->set_command_mode(paradice::client::command_mode_mmo);
        }
        else
        {
            send_to_player(ctx, commandmode_usage, player);
        }
    }
}

// ==========================================================================
// PARADICE COMMAND: QUIT
// ==========================================================================
PARADICE_COMMAND_IMPL(quit)
{
    player->get_connection()->disconnect();
}


}

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
#include "admin.hpp"
#include "account.hpp"
#include "character.hpp"
#include "client.hpp"
#include "communication.hpp"
#include "connection.hpp"
#include "context.hpp"
#include "who.hpp"
#include "hugin/user_interface.hpp"
#include "odin/tokenise.hpp"
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/typeof/typeof.hpp>

using namespace std;
using namespace boost;

namespace paradice {

// ==========================================================================
// CAPITALISE
// ==========================================================================
static void capitalise(string &name)
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
    
// ==========================================================================
// PARADICE COMMAND: ADMIN_SET_PASSWORD
// ==========================================================================
PARADICE_COMMAND_IMPL(admin_set_password)
{
    static string const usage =
        "\n USAGE:    admin_set_password <account> <argument> <verify>"
        "\n EXAMPLE:  admin_set_password bob foobar foobar"
        "\n\n";
        
    pair<string, string> token0 = odin::tokenise(arguments);
    pair<string, string> token1 = odin::tokenise(token0.second);
    pair<string, string> token2 = odin::tokenise(token1.second);
    
    if (token0.first.empty() || token1.first.empty() || token2.first.empty())
    {
        send_to_player(ctx, usage, player);
        return;
    }
    
    BOOST_AUTO(account_name, token0.first);
    capitalise(account_name);
    
    BOOST_AUTO(account, ctx->load_account(account_name));
    
    if (account == NULL)
    {
        send_to_player(ctx, "No such account with that name.\n", player);
        return;
    }
    
    BOOST_AUTO(password, token1.first);
    BOOST_AUTO(password_verify, token2.first);
    
    if (password != password_verify)
    {
        send_to_player(ctx, "Passwords did not match.\n", player);
        return;
    }
    
    account->set_password(password);
    ctx->save_account(account);
    send_to_player(ctx, "Passwords changed.\n", player);
}

// ==========================================================================
// PARADICE COMMAND: ADMIN_SHUTDOWN
// ==========================================================================
PARADICE_COMMAND_IMPL(admin_shutdown)
{
    static string const usage =
        "\n USAGE:    admin_shutdown now"
        "\n\n";

    pair<string, string> token = odin::tokenise(arguments);
    
    if (token.first != "now")
    {
        send_to_player(ctx, usage, player);
        return;
    }
    
    // For each client, save its character (if possible), then close its 
    // socket.
    BOOST_AUTO(clients, ctx->get_clients());
    
    BOOST_FOREACH(shared_ptr<client> current_client, clients)
    {
        BOOST_AUTO(ch, current_client->get_character());
        
        if (ch != NULL)
        {
            ctx->save_character(ch);
        }
        
        BOOST_AUTO(conn, current_client->get_connection());
        
        if (conn != NULL)
        {
            conn->disconnect();
        }
    }

    // shutdown
    ctx->shutdown();
}

}


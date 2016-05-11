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
#include "paradice/admin.hpp"
#include "paradice/account.hpp"
#include "paradice/character.hpp"
#include "paradice/client.hpp"
#include "paradice/communication.hpp"
#include "paradice/connection.hpp"
#include "paradice/context.hpp"
#include "paradice/who.hpp"
#include "odin/tokenise.hpp"
#include <terminalpp/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/format.hpp>
#include <cstdio>

namespace paradice {

// ==========================================================================
// CAPITALISE
// ==========================================================================
static void capitalise(std::string &name)
{
    if (!name.empty())
    {
        name[0] = toupper(name[0]);

        for (std::string::size_type index = 1; index < name.size(); ++index)
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
    static std::string const usage =
        "\n USAGE:    admin_set_password <account> <argument> <verify>"
        "\n EXAMPLE:  admin_set_password bob foobar foobar"
        "\n\n";

    auto token0 = odin::tokenise(arguments);
    auto token1 = odin::tokenise(token0.second);
    auto token2 = odin::tokenise(token1.second);

    if (token0.first.empty() || token1.first.empty() || token2.first.empty())
    {
        send_to_player(ctx, usage, player);
        return;
    }

    auto account_name = token0.first;
    capitalise(account_name);

    std::shared_ptr<account> account;

    try
    {
        account = ctx->load_account(account_name);
    }
    catch(std::exception &ex)
    {
        // TODO: Use an actual logging library for this message.
        std::printf("Error loading account: %s\n", ex.what());

        send_to_player(
            ctx
          , boost::str(boost::format("Error loading that account: %s\n") % ex.what())
          , player);
        return;
    }

    if (account == NULL)
    {
        send_to_player(ctx, "No such account with that name.\n", player);
        return;
    }

    auto password = token1.first;
    auto password_verify = token2.first;

    if (password != password_verify)
    {
        send_to_player(ctx, "Passwords did not match.\n", player);
        return;
    }

    account->set_password(password);

    try
    {
        ctx->save_account(account);
    }
    catch(std::exception &ex)
    {
        // TODO: Use an actual logging library for this message.
        std::printf("Error saving account: %s\n", ex.what());

        send_to_player(
            ctx
          , "Unexpected error setting saving your account.  Please try again."
          , player);
        return;
    }

    send_to_player(ctx, "Passwords changed.\n", player);
}

// ==========================================================================
// PARADICE COMMAND: ADMIN_SHUTDOWN
// ==========================================================================
PARADICE_COMMAND_IMPL(admin_shutdown)
{
    static std::string const usage =
        "\n USAGE:    admin_shutdown now"
        "\n\n";

    auto token = odin::tokenise(arguments);

    if (token.first != "now")
    {
        send_to_player(ctx, usage, player);
        return;
    }

    // For each client, save its character (if possible), then close its
    // socket.
    for (auto current_client : ctx->get_clients())
    {
        auto ch = current_client->get_character();

        if (ch != NULL)
        {
            try
            {
                ctx->save_character(ch);
            }
            catch(std::exception &ex)
            {
                std::printf("Error saving character %s: %s\n",
                    ch->get_name().c_str(), ex.what());

                send_to_player(
                    ctx
                  , terminalpp::string(boost::str(boost::format(
                        "\\[1Error saving character: %s.")
                            % ch->get_name()))
                  , player);

                return;
            }
        }

        current_client->disconnect();
    }

    // shutdown
    ctx->shutdown();
}

}


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
#ifndef PARADICE_COMMUNICATION_HPP_
#define PARADICE_COMMUNICATION_HPP_

#include "command.hpp"
#include "munin/ansi/protocol.hpp"
#include <string>

namespace paradice {

class client;
class context;

//* =========================================================================
/// \brief Send a text message to all connected players.
//* =========================================================================
void send_to_all(
    std::shared_ptr<context> &ctx
  , std::string const        &text);

//* =========================================================================
/// \brief Send a text message to all connected players.
//* =========================================================================
void send_to_all(
    std::shared_ptr<context>               &ctx
  , std::vector<munin::element_type> const &text);

//* =========================================================================
/// \brief Send a text message to a single player.
//* =========================================================================
void send_to_player(
    std::shared_ptr<context> &ctx
  , std::string const        &text
  , std::shared_ptr<client>  &player);

//* =========================================================================
/// \brief Send a text message to a single player.
//* =========================================================================
void send_to_player(
    std::shared_ptr<context>               &ctx
  , std::vector<munin::element_type> const &text
  , std::shared_ptr<client>                &player);

//* =========================================================================
/// \brief Send a text message to all players in the same room as player,
/// but not to player.
//* =========================================================================
void send_to_room(
    std::shared_ptr<context> &ctx
  , std::string const        &text
  , std::shared_ptr<client>  &player);

//* =========================================================================
/// \brief Send a text message to all players in the same room as player,
/// but not to player.
//* =========================================================================
void send_to_room(
    std::shared_ptr<context>               &ctx
  , std::vector<munin::element_type> const &text
  , std::shared_ptr<client>                &player);

PARADICE_COMMAND_DECL(say);
PARADICE_COMMAND_DECL(whisper);
PARADICE_COMMAND_DECL(emote);

}

#endif

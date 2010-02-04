// ==========================================================================
// Paradice Rules
//
// Copyright (C) 2009 Matthew Chaplain, All Rights Reserved.  
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
#include "rules.hpp"
#include "client.hpp"
#include "communication.hpp"
#include "connection.hpp"
#include "dice_roll_parser.hpp"
#include <boost/format.hpp>
#include <boost/typeof/typeof.hpp>

using namespace std;
using namespace boost;

namespace paradice {

// ==========================================================================
// PARADICE COMMAND: ROLL
// ==========================================================================
PARADICE_COMMAND_IMPL(roll)
{
    static string const usage_message =
        "\r\n Usage:   roll <dice>d<sides>[<bonuses...>]"
        "\r\n Example: roll 2d6+3-20"
        "\r\n\r\n";

    BOOST_AUTO(rolls, parse_dice_roll(arguments));

    if (!rolls)
    {
        player->get_connection()->write(usage_message);
        return;
    }

    BOOST_AUTO(dice_roll, rolls.get());

    if (dice_roll.sides_ == 0)
    {
        player->get_connection()->write(usage_message);
        return;
    }

    odin::s32 total = dice_roll.bonus_;
    string roll_description;

    for (odin::u32 current_roll = 0; current_roll < dice_roll.amount_; ++current_roll)
    {
        odin::s32 score = (rand() % dice_roll.sides_) + 1;

        roll_description += str(format("%s%d") 
            % (roll_description.empty() ? "" : ", ")
            % score);

        total += score;
    }

    message_to_player(
        ctx
      , str(format("\r\nYou roll %dd%d%s%d and score %d [%s]\r\n") 
            % dice_roll.amount_
            % dice_roll.sides_
            % (dice_roll.bonus_ >= 0 ? "+" : "")
            % dice_roll.bonus_
            % total
            % roll_description)
      , player);

    message_to_room(
        ctx
      , str(format("\r\n%s rolls %dd%d%s%d and scores %d [%s]\r\n")
            % player->get_name()
            % dice_roll.amount_
            % dice_roll.sides_
            % (dice_roll.bonus_ >= 0 ? "+" : "")
            % dice_roll.bonus_
            % total
            % roll_description)
      , player);
}

}

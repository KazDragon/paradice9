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
#include "paradice/random.hpp"
#include "odin/tokenise.hpp"
#include "odin/ansi/protocol.hpp"
#include "munin/ansi/protocol.hpp"
#include <boost/format.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/weak_ptr.hpp>
#include <map>
#include <vector>

using namespace std;
using namespace boost;
using namespace odin;

namespace paradice {

namespace {
    struct roll_data
    {
        weak_ptr<client> roller;
        string           name;
        string           roll_text;
        s32              raw_score;
        s32              score;
        bool             max_roll;
    };
    
    static map<
        string            // category.
      , vector<roll_data> // list of rolls in that category.
    > roll_category;
    
    static bool sort_roll_data_by_score_ascending(
        roll_data const &lhs
      , roll_data const &rhs)
    {
        return lhs.score < rhs.score;
    }
    
    static bool sort_roll_data_by_score_descending(
        roll_data const &lhs
      , roll_data const &rhs)
    {
        return lhs.score > rhs.score;
    }
}

// ==========================================================================
// PARADICE COMMAND: ROLL
// ==========================================================================
PARADICE_COMMAND_IMPL(roll)
{
    static string const usage_message =
        "\n Usage:   roll [n*]<dice>d<sides>[<bonuses...>] [<category>]"
        "\n Example: roll 2d6+3-20"
        "\n Example: roll 20*2d6"
        "\n Example: roll 1d10+4 initiative"
        "\n";

    string::const_iterator begin = arguments.begin();
    string::const_iterator end   = arguments.end();
    
    BOOST_AUTO(rolls, parse_dice_roll(begin, end));

    if (!rolls)
    {
        send_to_player(ctx, usage_message, player);
        return;
    }

    // Store a category if the user entered one.
    string category = tokenise(string(begin, end)).first;

    BOOST_AUTO(dice_roll, rolls.get());

    if (dice_roll.repetitions_ == 0
     || dice_roll.amount_      == 0)
    {
        send_to_player(
            ctx
          , "You roll no dice and score nothing.\n"
          , player);
        
        send_to_room(
            ctx
          , str(format(
                "%s rolls no dice and scores nothing.\n")
              % player->get_name())
          , player);
        
        return;
    }

    if (dice_roll.sides_ == 0)
    {
        send_to_player(
            ctx
          , "You fumble your roll and spill all your zero-sided dice on "
            "the floor.\n"
          , player);
        
        send_to_room(
            ctx
          , str(format(
                "%s fumbles their roll and spills a pile of zero-"
                "sided dice on the floor.\n")
              % player->get_name())
          , player);
        
        return;
    }
    
    if (dice_roll.repetitions_ * dice_roll.amount_ > 200)
    {
        send_to_player(
            ctx
          , "You can only roll at most 200 dice at once.\n"
          , player);
        
        return;
    }

    using namespace munin::ansi;
    
    attribute normal_pen;
    attribute max_roll_pen;
    attribute total_pen;
    
    max_roll_pen.intensity         = odin::ansi::graphics::INTENSITY_BOLD;
    max_roll_pen.foreground_colour = odin::ansi::graphics::COLOUR_GREEN;
    
    total_pen.intensity = odin::ansi::graphics::INTENSITY_BOLD;
    
    runtime_array<munin::ansi::element_type> total_description;    
    s32                                      total_score = 0;
    
    for (u32 repetition = 0; repetition < dice_roll.repetitions_; ++repetition)
    {
        runtime_array<munin::ansi::element_type> roll_description;
        s32                                      total = dice_roll.bonus_;

        for (u32 current_roll = 0; 
             current_roll < dice_roll.amount_; 
             ++current_roll)
        {
            s32 score = s32(random_number(1, dice_roll.sides_));
    
            roll_description += elements_from_string(
                current_roll == 0 ? "" : ", "
              , normal_pen);
            
            roll_description += elements_from_string(
                str(format("%d") % score)
              , dice_roll.bonus_ == 0 ? total_pen : normal_pen);
    
            total += score;
        }

        bool max_roll = 
            total == s32(dice_roll.amount_ * dice_roll.sides_)
                   + dice_roll.bonus_;
            
        if (dice_roll.bonus_ == 0)
        {
            total_description += elements_from_string(
                repetition == 0 ? "" : ", "
              , normal_pen);
            total_description += elements_from_string(
                str(format("%d") % total)
              , total_pen);
            total_description += elements_from_string(
                max_roll ? "!" : ""
              , max_roll_pen);
        }
        else
        {
            total_description += elements_from_string(
                repetition == 0 ? "" : ", "
              , normal_pen);
            total_description += elements_from_string(
                str(format("%d") % total)
              , total_pen);
            total_description += elements_from_string(
                " ["
              , normal_pen);
            total_description += roll_description;
            total_description += elements_from_string(
                max_roll ? "!" : ""
              , max_roll_pen);
            total_description += elements_from_string(
                "]"
              , normal_pen);
        }

        total_score += total;
        
        // If the player has rolled in a category, add the roll to the list.
        if (category != "")
        {
            roll_data data;
            data.roller = player;
            data.name   = player->get_name();
            data.roll_text = str(format("%dd%d%s%d")
                % dice_roll.amount_
                % dice_roll.sides_
                % (dice_roll.bonus_ >= 0 ? "+" : "")
                % dice_roll.bonus_);
            data.raw_score = total - dice_roll.bonus_;
            data.score     = total;
            data.max_roll  = max_roll;
            
            roll_category[category].push_back(data);
        }
    }

    runtime_array<munin::ansi::element_type> player_output;
    player_output += elements_from_string(
        str(format("You roll %dd%d%s%d %s%sand score ")
            % dice_roll.amount_
            % dice_roll.sides_
            % (dice_roll.bonus_ >= 0 ? "+" : "")
            % dice_roll.bonus_
            % (category == "" 
                ? "" 
                : str(format("(category: %s) ") % category))
            % (dice_roll.repetitions_ == 1 
                ? ""
                : str(format("%d times ") % dice_roll.repetitions_)))
      , normal_pen);
    player_output += total_description;
    player_output += elements_from_string(
        dice_roll.repetitions_ == 1
      ? ""
      : " for a grand total of "
      , normal_pen);
    player_output += elements_from_string(
        dice_roll.repetitions_ == 1
      ? ""
      : str(format("%d") % total_score)
      , total_pen);
    player_output += elements_from_string("\n", normal_pen);
    
    send_to_player(ctx, player_output, player);

    runtime_array<munin::ansi::element_type> room_output;
    room_output += elements_from_string(
        player->get_name()
      , normal_pen);
    room_output += elements_from_string(
        str(format(" rolls %dd%d%s%d %s%sand scores ")
            % dice_roll.amount_
            % dice_roll.sides_
            % (dice_roll.bonus_ >= 0 ? "+" : "")
            % dice_roll.bonus_
            % (category == "" 
                ? "" 
                : str(format("(category: %s) ") % category))
            % (dice_roll.repetitions_ == 1 
                ? ""
                : str(format("%d times ") % dice_roll.repetitions_)))
      , normal_pen);
    room_output += total_description;
    room_output += elements_from_string(
        dice_roll.repetitions_ == 1
      ? ""
      : " for a grand total of "
      , normal_pen);
    room_output += elements_from_string(
        dice_roll.repetitions_ == 1
      ? ""
      : str(format("%d") % total_score)
      , total_pen);
    room_output += elements_from_string("\n", normal_pen);
    
    send_to_room(ctx, room_output, player);
}

// ==========================================================================
// PARADICE COMMAND: ROLLPRIVATE
// ==========================================================================
PARADICE_COMMAND_IMPL(rollprivate)
{
    static string const usage_message =
        "\n Usage:   rollprivate [n*]<dice>d<sides>[<bonuses...>] [<category>]"
        "\n Example: rollprivate 2d6+3-20"
        "\n Example: rollprivate 20*2d6"
        "\n";

    string::const_iterator begin = arguments.begin();
    string::const_iterator end   = arguments.end();
    
    BOOST_AUTO(rolls, parse_dice_roll(begin, end));

    if (!rolls)
    {
        send_to_player(ctx, usage_message, player);
        return;
    }

    BOOST_AUTO(dice_roll, rolls.get());

    if (dice_roll.repetitions_ == 0
     || dice_roll.amount_      == 0)
    {
        send_to_player(
            ctx
          , "You roll no dice and score nothing.\n"
          , player);
        
        return;
    }

    if (dice_roll.sides_ == 0)
    {
        send_to_player(
            ctx
          , "You fumble your roll and spill all your zero-sided dice on "
            "the floor.\n"
          , player);
        
        return;
    }
    
    if (dice_roll.repetitions_ * dice_roll.amount_ > 200)
    {
        send_to_player(
            ctx
          , "You can only roll at most 200 dice at once.\n"
          , player);
        
        return;
    }

    using namespace munin::ansi;
    
    attribute normal_pen;
    attribute max_roll_pen;
    attribute total_pen;
    
    max_roll_pen.intensity         = odin::ansi::graphics::INTENSITY_BOLD;
    max_roll_pen.foreground_colour = odin::ansi::graphics::COLOUR_GREEN;
    
    total_pen.intensity = odin::ansi::graphics::INTENSITY_BOLD;
    
    runtime_array<munin::ansi::element_type> total_description;    
    s32                                      total_score = 0;
    
    for (u32 repetition = 0; repetition < dice_roll.repetitions_; ++repetition)
    {
        runtime_array<munin::ansi::element_type> roll_description;
        s32                                      total = dice_roll.bonus_;

        for (u32 current_roll = 0; 
             current_roll < dice_roll.amount_; 
             ++current_roll)
        {
            s32 score = s32(random_number(1, dice_roll.sides_));
    
            roll_description += elements_from_string(
                current_roll == 0 ? "" : ", "
              , normal_pen);
            
            roll_description += elements_from_string(
                str(format("%d") % score)
              , dice_roll.bonus_ == 0 ? total_pen : normal_pen);
    
            total += score;
        }

        bool max_roll = 
            total == s32(dice_roll.amount_ * dice_roll.sides_)
                   + dice_roll.bonus_;
            
        if (dice_roll.bonus_ == 0)
        {
            total_description += elements_from_string(
                repetition == 0 ? "" : ", "
              , normal_pen);
            total_description += elements_from_string(
                str(format("%d") % total)
              , total_pen);
            total_description += elements_from_string(
                max_roll ? "!" : ""
              , max_roll_pen);
        }
        else
        {
            total_description += elements_from_string(
                repetition == 0 ? "" : ", "
              , normal_pen);
            total_description += elements_from_string(
                str(format("%d") % total)
              , total_pen);
            total_description += elements_from_string(
                " ["
              , normal_pen);
            total_description += roll_description;
            total_description += elements_from_string(
                max_roll ? "!" : ""
              , max_roll_pen);
            total_description += elements_from_string(
                "]"
              , normal_pen);
        }

        total_score += total;
    }

    runtime_array<munin::ansi::element_type> player_output;
    player_output += elements_from_string(
        str(format("You roll privately %dd%d%s%d %sand score ")
            % dice_roll.amount_
            % dice_roll.sides_
            % (dice_roll.bonus_ >= 0 ? "+" : "")
            % dice_roll.bonus_
            % (dice_roll.repetitions_ == 1 
                ? ""
                : str(format("%d times ") % dice_roll.repetitions_)))
      , normal_pen);
    player_output += total_description;
    player_output += elements_from_string(
        dice_roll.repetitions_ == 1
      ? ""
      : " for a grand total of "
      , normal_pen);
    player_output += elements_from_string(
        dice_roll.repetitions_ == 1
      ? ""
      : str(format("%d") % total_score)
      , total_pen);
    player_output += elements_from_string("\n", normal_pen);
    
    send_to_player(ctx, player_output, player);
}

// ==========================================================================
// PARADICE COMMAND: SHOWROLLS
// ==========================================================================
PARADICE_COMMAND_IMPL(showrolls)
{
    pair<string, string> tokens = tokenise(arguments);
    string category             = tokens.first;
    
    tokens                      = tokenise(tokens.second);
    string order                = tokens.first;
    
    if (category == "")
    {
        static string const usage_message = 
            "\n USAGE:   showrolls <category> [desc|asc]"
            "\n EXAMPLE: showrolls initiative"
            "\n EXAMPLE: showrolls combat desc"
            "\n";
              
        send_to_player(ctx, usage_message, player);
        return;
    }
    
    vector<roll_data> rolls = roll_category[category];
    
    if (rolls.empty())
    {
        send_to_player(
            ctx
          , str(format("There are no rolls in the %s category\n")
                % category)
          , player);
        return;
    }

    if (order == "desc")
    {
        sort(
            rolls.begin()
          , rolls.end()
          , ptr_fun(&sort_roll_data_by_score_descending));
    }
    else if (order == "asc")
    {
        sort(
            rolls.begin()
          , rolls.end()
          , ptr_fun(&sort_roll_data_by_score_ascending));
    }
    
    string output = str(format(
        "===== Rolls in the %s category ====="
        "\n")
        % category);
    
    for (u32 index = 0; index < rolls.size(); ++index)
    {
        roll_data &data = rolls[index];
        shared_ptr<client> roller = data.roller.lock();
        
        string name = (roller == NULL ? data.name : roller->get_name());
        
        output += str(format(
            "\n%s rolled %s and scored %d [%d%s]")
            % name
            % data.roll_text
            % data.score
            % data.raw_score
            % (data.max_roll ? "!" : ""));
    }
    
    output += "\n";

    send_to_player(ctx, output, player);
}

// ==========================================================================
// PARADICE COMMAND: CLEARROLLS
// ==========================================================================
PARADICE_COMMAND_IMPL(clearrolls)
{
    string category = tokenise(arguments).first;
    
    if (category == "")
    {
        static string const usage_message = 
            "\n USAGE:   clearrolls <category>"
            "\n EXAMPLE: clearrolls initiative"
            "\n";
              
        send_to_player(ctx, usage_message, player);
        return;
    }
    
    roll_category[category].clear();

    send_to_player(
        ctx
      , str(format("You clear the rolls for the %s category\n") % category)
      , player);
    
    send_to_room(
        ctx
      , str(format("%s clears the rolls for the %s category\n")
          % player->get_name()
          % category)
      , player);
}

}


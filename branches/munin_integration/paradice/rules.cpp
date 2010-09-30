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
    static string const ANSI_DEFAULT = str(format("%c%c%c")
        % munin::ansi::ESCAPE
        % munin::ansi::CONTROL_SEQUENCE_INTRODUCER
        // No argument = 0, which is DEFAULT_ATTRIBUTES.
        % munin::ansi::SELECT_GRAPHICS_RENDITION);

    static string const ANSI_BOLD = str(format("%c%c%d%c")
        % munin::ansi::ESCAPE
        % munin::ansi::CONTROL_SEQUENCE_INTRODUCER
        % int(munin::ansi::graphics::INTENSITY_BOLD)
        % munin::ansi::SELECT_GRAPHICS_RENDITION);
    
    static string const ANSI_GREEN = str(format("%c%c%d%c")
        % munin::ansi::ESCAPE
        % munin::ansi::CONTROL_SEQUENCE_INTRODUCER
        % int(munin::ansi::graphics::FOREGROUND_COLOUR_BASE
            + munin::ansi::graphics::COLOUR_GREEN)
        % munin::ansi::SELECT_GRAPHICS_RENDITION);
    
    static string const MAX_ROLL_INDICATOR = str(format("%s%s!%s")
        % ANSI_BOLD
        % ANSI_GREEN
        % ANSI_DEFAULT);
    
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
        "\r\n Usage:   roll [n*]<dice>d<sides>[<bonuses...>] [<category>]"
        "\r\n Example: roll 2d6+3-20"
        "\r\n Example: roll 20*2d6"
        "\r\n Example: roll 1d10+4 initiative"
        "\r\n\r\n";

    string::const_iterator begin = arguments.begin();
    string::const_iterator end   = arguments.end();
    
    BOOST_AUTO(rolls, parse_dice_roll(begin, end));

    if (!rolls)
    {
        player->get_connection()->write(usage_message);
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
          , "You roll no dice and score nothing.\r\n"
          , player);
        
        send_to_room(
            ctx
          , str(format(
                "%s rolls no dice and scores nothing.\r\n")
              % player->get_name())
          , player);
        
        return;
    }

    if (dice_roll.sides_ == 0)
    {
        send_to_player(
            ctx
          , "You fumble your roll and spill all your zero-sided dice on "
            "the floor.\r\n"
          , player);
        
        send_to_room(
            ctx
          , str(format(
                "%s fumbles their roll and spills a pile of zero-"
                "sided dice on the floor.\r\n")
              % player->get_name())
          , player);
        
        return;
    }
    
    if (dice_roll.repetitions_ * dice_roll.amount_ > 200)
    {
        send_to_player(
            ctx
          , "You can only roll at most 200 dice at once.\r\n"
          , player);
        
        return;
    }

    string total_description;
    s32    total_score = 0;
    
    for (u32 repetition = 0; repetition < dice_roll.repetitions_; ++repetition)
    {
        string roll_description;
        s32    total = dice_roll.bonus_;

        for (u32 current_roll = 0; 
             current_roll < dice_roll.amount_; 
             ++current_roll)
        {
            s32 score = s32(random_number(1, dice_roll.sides_));
    
            roll_description += str(format("%s%d") 
                % (current_roll == 0 ? "" : ", ")
                % score);
    
            total += score;
        }

        bool max_roll = 
            total == s32(dice_roll.amount_ * dice_roll.sides_)
                   + dice_roll.bonus_;
            
        if (dice_roll.bonus_ == 0)
        {
            total_description +=
                str(format("%s%s%d%s%s")
                    % (repetition == 0 ? "" : ", ")
                    % ANSI_BOLD
                    % total
                    % (max_roll ? MAX_ROLL_INDICATOR : "")
                    % ANSI_DEFAULT);


        }
        else
        {
            total_description +=
                str(format("%s%s%d%s [%s%s]")
                    % (repetition == 0 ? "" : ", ")
                    % ANSI_BOLD
                    % total
                    % ANSI_DEFAULT
                    % roll_description
                    % (max_roll ? MAX_ROLL_INDICATOR : ""));
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

    message_to_player(
        ctx
      , str(format("\r\nYou roll %dd%d%s%d %s%sand score %s%s\r\n\r\n")
            % dice_roll.amount_
            % dice_roll.sides_
            % (dice_roll.bonus_ >= 0 ? "+" : "")
            % dice_roll.bonus_
            % (category == "" 
                ? "" 
                : str(format("(category: %s) ") % category))
            % (dice_roll.repetitions_ == 1 
                ? ""
                : str(format("%d times ") % dice_roll.repetitions_))
            % total_description
            % (dice_roll.repetitions_ == 1
                ? ""
                : str(format(" for a grand total of %s%d%s") 
                      % ANSI_BOLD
                      % total_score
                      % ANSI_DEFAULT)))
      , player);
          
    message_to_room(
        ctx
      , str(format("\r\n%s rolls %dd%d%s%d %s%sand scores %s%s\r\n\r\n")
            % player->get_name()
            % dice_roll.amount_
            % dice_roll.sides_
            % (dice_roll.bonus_ >= 0 ? "+" : "")
            % dice_roll.bonus_
            % (category == "" 
                ? "" 
                : str(format("(category: %s) ") % category))
            % (dice_roll.repetitions_ == 1 
                ? ""
                : str(format("%d times ") % dice_roll.repetitions_))
            % total_description
            % (dice_roll.repetitions_ == 1
                ? ""
                : str(format(" for a grand total of %s%d%s") 
                      % ANSI_BOLD
                      % total_score
                      % ANSI_DEFAULT)))
      , player);
}

// ==========================================================================
// PARADICE COMMAND: ROLL
// ==========================================================================
PARADICE_COMMAND_IMPL(rollprivate)
{
    static string const usage_message =
        "\r\n Usage:   rollprivate [n*]<dice>d<sides>[<bonuses...>]"
        "\r\n Example: rollprivate 2d6+3-20"
        "\r\n Example: rollprivate 20*2d6"
        "\r\n\r\n";

    string::const_iterator begin = arguments.begin();
    string::const_iterator end   = arguments.end();
    
    BOOST_AUTO(rolls, parse_dice_roll(begin, end));

    if (!rolls)
    {
        player->get_connection()->write(usage_message);
        return;
    }

    BOOST_AUTO(dice_roll, rolls.get());

    if (dice_roll.repetitions_ == 0
     || dice_roll.amount_      == 0)
    {
        send_to_player(
            ctx
          , "You roll no dice and score nothing.\r\n"
          , player);
        
        return;
    }

    if (dice_roll.sides_ == 0)
    {
        send_to_player(
            ctx
          , "You fumble your roll and spill all your zero-sided dice on "
            "the floor.\r\n"
          , player);
        
        return;
    }
    
    if (dice_roll.repetitions_ * dice_roll.amount_ > 200)
    {
        send_to_player(
            ctx
          , "You can only roll at most 200 dice at once.\r\n"
          , player);
        
        return;
    }

    string total_description;
    s32    total_score = 0;
    
    for (u32 repetition = 0; repetition < dice_roll.repetitions_; ++repetition)
    {
        string roll_description;
        s32    total = dice_roll.bonus_;

        for (u32 current_roll = 0; 
             current_roll < dice_roll.amount_; 
             ++current_roll)
        {
            s32 score = s32(random_number(1, dice_roll.sides_));
    
            roll_description += str(format("%s%d") 
                % (current_roll == 0 ? "" : ", ")
                % score);
    
            total += score;
        }

        bool max_roll = 
            total == s32(dice_roll.amount_ * dice_roll.sides_)
                   + dice_roll.bonus_;
            
        if (dice_roll.bonus_ == 0)
        {
            total_description +=
                str(format("%s%s%d%s%s")
                    % (repetition == 0 ? "" : ", ")
                    % ANSI_BOLD
                    % total
                    % (max_roll ? MAX_ROLL_INDICATOR : "")
                    % ANSI_DEFAULT);


        }
        else
        {
            total_description +=
                str(format("%s%s%d%s [%s%s]")
                    % (repetition == 0 ? "" : ", ")
                    % ANSI_BOLD
                    % total
                    % ANSI_DEFAULT
                    % roll_description
                    % (max_roll ? MAX_ROLL_INDICATOR : ""));
        }

        total_score += total;
    }

    message_to_player(
        ctx
      , str(format("\r\nYou privately roll %dd%d%s%d %sand score %s%s\r\n\r\n")
            % dice_roll.amount_
            % dice_roll.sides_
            % (dice_roll.bonus_ >= 0 ? "+" : "")
            % dice_roll.bonus_
            % (dice_roll.repetitions_ == 1 
                ? ""
                : str(format("%d times ") % dice_roll.repetitions_))
            % total_description
            % (dice_roll.repetitions_ == 1
                ? ""
                : str(format(" for a grand total of %s%d%s") 
                      % ANSI_BOLD
                      % total_score
                      % ANSI_DEFAULT)))
      , player);
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
            "\r\n USAGE:   showrolls <category> [desc|asc]"
            "\r\n EXAMPLE: showrolls initiative"
            "\r\n EXAMPLE: showrolls combat desc"
            "\r\n\r\n";
              
        player->get_connection()->write(usage_message);
        return;
    }
    
    vector<roll_data> rolls = roll_category[category];
    
    if (rolls.empty())
    {
        send_to_player(
            ctx
          , str(format("There are no rolls in the %s category\r\n\r\n")
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
        "\r\n")
        % category);
    
    for (u32 index = 0; index < rolls.size(); ++index)
    {
        roll_data &data = rolls[index];
        shared_ptr<client> roller = data.roller.lock();
        
        string name = (roller == NULL ? data.name : roller->get_name());
        
        output += str(format(
            "\r\n%s rolled %s and scored %s%d%s [%d%s]")
            % name
            % data.roll_text
            % ANSI_BOLD
            % data.score
            % ANSI_DEFAULT
            % data.raw_score
            % (data.max_roll ? MAX_ROLL_INDICATOR : ""));
    }
    
    output += "\r\n\r\n";

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
            "\r\n USAGE:   clearrolls <category>"
            "\r\n EXAMPLE: clearrolls initiative"
            "\r\n\r\n";
              
        player->get_connection()->write(usage_message);
        return;
    }
    
    roll_category[category].clear();

    message_to_player(
        ctx
      , str(format("You clear the rolls for the %s category\r\n\r\n") % category)
      , player);
    
    message_to_room(
        ctx
      , str(format("%s clears the rolls for the %s category\r\n\r\n")
          % player->get_name()
          % category)
      , player);
}

}


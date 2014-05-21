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
#include "character.hpp"
#include "client.hpp"
#include "communication.hpp"
#include "connection.hpp"
#include "dice_roll_parser.hpp"
#include "paradice/active_encounter.hpp"
#include "paradice/context.hpp"
#include "paradice/random.hpp"
#include "odin/tokenise.hpp"
#include "odin/ansi/protocol.hpp"
#include "munin/ansi/protocol.hpp"
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <boost/typeof/typeof.hpp>
#include <map>
#include <numeric>
#include <vector>

using namespace munin::ansi;
using namespace munin;
using namespace odin;
using namespace boost;
using namespace std;

namespace paradice {

namespace {
    BOOST_STATIC_CONSTANT(u32, max_encounter_rolls = 10);
    
    static map<
        string            // category.
      , vector<roll_data> // list of rolls in that category.
    > roll_category;
    mutex roll_category_mutex;
    
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

    static string const DEFAULT_ATTRIBUTE   = "\\x";      // Default
    static string const MAXIMUM_HIT         = "\\i>\\[2!\\x"; // Bold, Green "!"
    static string const TOTAL_ATTRIBUTE     = "\\i>";     // Bold
}

// ==========================================================================
// CONCAT_TEXT
// ==========================================================================
void concat_text(
    vector<element_type>       &dest
  , vector<element_type> const &source)
{
    dest.insert(dest.end(), source.begin(), source.end());
}

// ==========================================================================
// CONCAT_TEXT
// ==========================================================================
void concat_text(
    vector<element_type> &dest
  , string const         &source
  , attribute const      &attr = attribute())
{
    concat_text(dest, elements_from_string(source, attr));
}

// ==========================================================================
// DESCRIBE_DICE
// ==========================================================================
string describe_dice(dice_roll const &roll)
{
    string description;

    if (roll.repetitions_ != 1)
    {
        description += str(format("%d*") % roll.repetitions_);
    }

    description += str(format("%dd%d") 
        % roll.amount_
        % roll.sides_);

    if (roll.bonus_ > 0)
    {
        description += "+";
    }

    if (roll.bonus_ != 0)
    {
        description += str(format("%d") % roll.bonus_);
    }

    return description;
}

// ==========================================================================
// THROW_DICE
// ==========================================================================
static dice_result throw_dice(roller const &rlr, dice_roll const &roll)
{
    dice_result result;
    result.roller_ = rlr;
    result.roll_ = roll;

    for (u32 repetition = 0; repetition < roll.repetitions_; ++repetition)
    {
        vector<s32> raw_dice;

        for (u32 die = 0; die < roll.amount_; ++die)
        {
            raw_dice.push_back(random_number(1, roll.sides_));
        }

        result.results_.push_back(raw_dice);
    }

    return result;
}

// ==========================================================================
// EXECUTE_ROLL
// ==========================================================================
static void execute_roll(
    boost::shared_ptr<context> &ctx
  , std::string const          &category
  , dice_roll const            &roll
  , boost::shared_ptr<client>  &player
  , bool                        is_rolling_privately)
{
    if (roll.repetitions_ == 0
     || roll.amount_      == 0)
    {
        send_to_player(
            ctx
          , "You roll no dice and score nothing.\n"
          , player);
        
        if (!is_rolling_privately)
        {
            send_to_room(
                ctx
              , str(format(
                    "%s rolls no dice and scores nothing.\n")
                  % player->get_character()->get_name())
              , player);
        }

        return;
    }

    if (roll.sides_ == 0)
    {
        send_to_player(
            ctx
          , "You fumble your roll and spill all your zero-sided dice on "
            "the floor.\n"
          , player);
        
        if (!is_rolling_privately)
        {
            send_to_room(
                ctx
              , str(format(
                    "%s fumbles their roll and spills a pile of zero-"
                    "sided dice on the floor.\n")
                  % player->get_character()->get_name())
              , player);
        }

        return;
    }
    
    if (roll.repetitions_ * roll.amount_ > 200)
    {
        send_to_player(
            ctx
          , "You can only roll at most 200 dice at once.\n"
          , player);
        
        return;
    }

    dice_result result = throw_dice(player, roll);
    string dice_description = describe_dice(roll);

    s32 total_score = 0;

    string dice_text;

    s32 const theoretical_max_roll = 
        (roll.sides_ * roll.amount_) + roll.bonus_;
    s32 const theoretical_max_total =
        theoretical_max_roll * roll.repetitions_;

    for (vector< vector<s32> >::const_iterator repetition = result.results_.begin();
         repetition != result.results_.end();
         ++repetition)
    {
        if (repetition != result.results_.begin())
        {
            dice_text += ", ";
        }

        s32 const subtotal = std::accumulate(
            repetition->begin(),
            repetition->end(), 
            0);
        s32 const total = subtotal + roll.bonus_;

        total_score += total;

        dice_text += TOTAL_ATTRIBUTE;
        dice_text += str(format("%d") % total);
        dice_text += DEFAULT_ATTRIBUTE;

        if (total == theoretical_max_roll)
        {
            dice_text += MAXIMUM_HIT;
        }

        if (roll.amount_ > 1)
        {
            dice_text += " [";

            for (vector<s32>::const_iterator current_roll = repetition->begin();
                 current_roll != repetition->end();
                 ++current_roll)
            {
                if (current_roll != repetition->begin())
                {
                    dice_text += ", ";
                }

                dice_text += str(format("%d") % *current_roll);

                if (*current_roll == roll.sides_)
                {
                    dice_text += MAXIMUM_HIT;
                }
            }

            dice_text += "]";
        }

        if (!category.empty())
        {
            dice_roll temp_roll = roll;
            temp_roll.repetitions_ = 1;

            roll_data data;
            data.roller = player;
            data.name = player->get_character()->get_name();
            data.roll_text = describe_dice(temp_roll);
            data.score = total;
            data.raw_score = subtotal;
            data.max_roll = (total == theoretical_max_roll);
            roll_category[category].push_back(data);
        }
    }

    if (roll.repetitions_ != 1)
    {
        dice_text += " for a grand total of ";
        dice_text += TOTAL_ATTRIBUTE;
        dice_text += str(format("%d") % total_score);
        dice_text += DEFAULT_ATTRIBUTE;

        if (total_score == theoretical_max_total)
        {
            dice_text += MAXIMUM_HIT;
        }
    }

    if (total_score != theoretical_max_total)
    {
        dice_text += ".";
    }

    dice_text += "\n";

    string second_person_lead = 
        "You roll " 
      + dice_description
      + (category.empty() ? "" : ("(category: " + category + ")"))
      + " and score ";

    send_to_player(ctx, second_person_lead + dice_text, player);

    if (!is_rolling_privately)
    {
        string third_person_lead = 
            player->get_character()->get_name() 
          + " rolls " 
          + dice_description 
          + (category.empty() ? "" : ("(category: " + category + ")"))
          + " and scores ";
            send_to_room(ctx, third_person_lead + dice_text, player);

        BOOST_AUTO(enc, ctx->get_active_encounter());
        BOOST_FOREACH(active_encounter::entry &entry, enc->entries_)
        {
            BOOST_AUTO(ply, get<active_encounter::player>(&entry.participant_));
     
            if (ply != NULL)
            {
                BOOST_AUTO(ch, ply->character_.lock());

                if (ch != NULL)
                {
                    if (ch->get_name() == player->get_character()->get_name())
                    {
                        entry.roll_data_.push_back(result);

                        if (entry.roll_data_.size() > max_encounter_rolls)
                        {
                            entry.roll_data_.pop_front();
                        }

                        ctx->update_active_encounter();
                    }
                }
            }
        }
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

    execute_roll(ctx, category, rolls.get(), player, false);
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

    execute_roll(ctx, "", rolls.get(), player, true);
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
    
    vector<roll_data> rolls;
    
    {
        unique_lock<mutex> lock(roll_category_mutex);
        rolls = roll_category[category];
    }
    
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
        "\n===== Rolls in the %s category =====")
        % category);
    
    for (u32 index = 0; index < rolls.size(); ++index)
    {
        roll_data &data = rolls[index];
        shared_ptr<client> roller = data.roller.lock();
        
        string name = (
            roller == NULL 
          ? data.name 
          : roller->get_character()->get_name());
        
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
    
    {
        unique_lock<mutex> lock(roll_category_mutex);
        roll_category[category].clear();
    }

    send_to_player(
        ctx
      , str(format("You clear the rolls for the %s category\n") % category)
      , player);
    
    send_to_room(
        ctx
      , str(format("%s clears the rolls for the %s category\n")
          % player->get_character()->get_name()
          % category)
      , player);
}

}


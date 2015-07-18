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
#include <boost/format.hpp>
#include <map>
#include <mutex>
#include <numeric>
#include <thread>
#include <vector>

namespace paradice {

namespace {
    BOOST_STATIC_CONSTANT(odin::u32, max_encounter_rolls = 10);
    
    static std::map<
        std::string,           // category.
        std::vector<roll_data> // list of rolls in that category.
    > roll_category;
    std::mutex roll_category_mutex;
    
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

    static std::string const DEFAULT_ATTRIBUTE   = "\\x";      // Default
    static std::string const MAXIMUM_HIT         = "\\i>\\[2!\\x"; // Bold, Green "!"
    static std::string const TOTAL_ATTRIBUTE     = "\\i>";     // Bold
}

// ==========================================================================
// CONCAT_TEXT
// ==========================================================================
void concat_text(
    std::vector<munin::element_type>       &dest,
    std::vector<munin::element_type> const &source)
{
    dest.insert(dest.end(), source.begin(), source.end());
}

// ==========================================================================
// CONCAT_TEXT
// ==========================================================================
void concat_text(
    std::vector<munin::element_type> &dest,
    std::string const                &source,
    munin::attribute const           &attr = {})
{
    concat_text(dest, munin::ansi::elements_from_string(source, attr));
}

// ==========================================================================
// DESCRIBE_DICE
// ==========================================================================
std::string describe_dice(dice_roll const &roll)
{
    std::string description;

    if (roll.repetitions_ != 1)
    {
        description += boost::str(boost::format("%d*") % roll.repetitions_);
    }

    description += boost::str(boost::format("%dd%d") 
        % roll.amount_
        % roll.sides_);

    if (roll.bonus_ > 0)
    {
        description += "+";
    }

    if (roll.bonus_ != 0)
    {
        description += boost::str(boost::format("%d") % roll.bonus_);
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

    for (odin::u32 repetition = 0; repetition < roll.repetitions_; ++repetition)
    {
        std::vector<odin::s32> raw_dice;

        for (odin::u32 die = 0; die < roll.amount_; ++die)
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
    std::shared_ptr<context> &ctx
  , std::string const        &category
  , dice_roll const          &roll
  , std::shared_ptr<client>  &player
  , bool                      is_rolling_privately)
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
              , boost::str(boost::format(
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
              , boost::str(boost::format(
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
    auto dice_description = describe_dice(roll);

    odin::s32 total_score = 0;

    std::string dice_text;

    auto const theoretical_max_roll = 
        (roll.sides_ * roll.amount_) + roll.bonus_;
    auto const theoretical_max_total =
        theoretical_max_roll * roll.repetitions_;

    for (auto repetition = result.results_.begin();
         repetition != result.results_.end();
         ++repetition)
    {
        if (repetition != result.results_.begin())
        {
            dice_text += ", ";
        }

        auto const subtotal = std::accumulate(
            repetition->begin(),
            repetition->end(), 
            0);
        auto const total = subtotal + roll.bonus_;

        total_score += total;

        dice_text += TOTAL_ATTRIBUTE;
        dice_text += boost::str(boost::format("%d") % total);
        dice_text += DEFAULT_ATTRIBUTE;

        if (total == theoretical_max_roll)
        {
            dice_text += MAXIMUM_HIT;
        }

        if (roll.amount_ > 1)
        {
            dice_text += " [";

            for (auto current_roll = repetition->begin();
                 current_roll != repetition->end();
                 ++current_roll)
            {
                if (current_roll != repetition->begin())
                {
                    dice_text += ", ";
                }

                dice_text += boost::str(boost::format("%d") % *current_roll);

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
        dice_text += boost::str(boost::format("%d") % total_score);
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

    std::string second_person_lead = 
        "You roll " 
      + dice_description
      + (category.empty() ? "" : ("(category: " + category + ")"))
      + " and score ";

    send_to_player(ctx, second_person_lead + dice_text, player);

    if (!is_rolling_privately)
    {
        std::string third_person_lead = 
            player->get_character()->get_name() 
          + " rolls " 
          + dice_description 
          + (category.empty() ? "" : ("(category: " + category + ")"))
          + " and scores ";
            send_to_room(ctx, third_person_lead + dice_text, player);

        auto enc = ctx->get_active_encounter();
        for (auto &entry : enc->entries_)
        {
            auto ply = boost::get<active_encounter::player>(&entry.participant_);
     
            if (ply != NULL)
            {
                auto ch = ply->character_.lock();

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
    static std::string const usage_message =
        "\n Usage:   roll [n*]<dice>d<sides>[<bonuses...>] [<category>]"
        "\n Example: roll 2d6+3-20"
        "\n Example: roll 20*2d6"
        "\n Example: roll 1d10+4 initiative"
        "\n";

    auto begin = arguments.begin();
    auto end   = arguments.end();
    
    auto rolls = parse_dice_roll(begin, end);

    if (!rolls)
    {
        send_to_player(ctx, usage_message, player);
        return;
    }

    // Store a category if the user entered one.
    auto category = odin::tokenise(std::string(begin, end)).first;

    execute_roll(ctx, category, rolls.get(), player, false);
}

// ==========================================================================
// PARADICE COMMAND: ROLLPRIVATE
// ==========================================================================
PARADICE_COMMAND_IMPL(rollprivate)
{
    static std::string const usage_message =
        "\n Usage:   rollprivate [n*]<dice>d<sides>[<bonuses...>] [<category>]"
        "\n Example: rollprivate 2d6+3-20"
        "\n Example: rollprivate 20*2d6"
        "\n";

    auto begin = arguments.begin();
    auto end   = arguments.end();
    
    auto rolls = parse_dice_roll(begin, end);

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
    auto tokens   = odin::tokenise(arguments);
    auto category = tokens.first;
    
    tokens        = odin::tokenise(tokens.second);
    auto order    = tokens.first;
    
    if (category == "")
    {
        static std::string const usage_message = 
            "\n USAGE:   showrolls <category> [desc|asc]"
            "\n EXAMPLE: showrolls initiative"
            "\n EXAMPLE: showrolls combat desc"
            "\n";
              
        send_to_player(ctx, usage_message, player);
        return;
    }
    
    std::vector<roll_data> rolls;
    
    {
        std::unique_lock<std::mutex> lock(roll_category_mutex);
        rolls = roll_category[category];
    }
    
    if (rolls.empty())
    {
        send_to_player(
            ctx
          , boost::str(boost::format("There are no rolls in the %s category\n")
                % category)
          , player);
        return;
    }

    if (order == "desc")
    {
        std::sort(
            rolls.begin()
          , rolls.end()
          , sort_roll_data_by_score_descending);
    }
    else if (order == "asc")
    {
        sort(
            rolls.begin()
          , rolls.end()
          , sort_roll_data_by_score_ascending);
    }
    
    auto output = boost::str(boost::format(
        "\n===== Rolls in the %s category =====")
        % category);
    
    for (odin::u32 index = 0; index < rolls.size(); ++index)
    {
        roll_data &data = rolls[index];
        auto roller = data.roller.lock();
        
        std::string name = (
            roller == NULL 
          ? data.name 
          : roller->get_character()->get_name());
        
        output += boost::str(boost::format(
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
    auto category = odin::tokenise(arguments).first;
    
    if (category == "")
    {
        static std::string const usage_message = 
            "\n USAGE:   clearrolls <category>"
            "\n EXAMPLE: clearrolls initiative"
            "\n";
              
        send_to_player(ctx, usage_message, player);
        return;
    }
    
    {
        std::unique_lock<std::mutex> lock(roll_category_mutex);
        roll_category[category].clear();
    }

    send_to_player(
        ctx
      , boost::str(boost::format("You clear the rolls for the %s category\n") 
          % category)
      , player);
    
    send_to_room(
        ctx
      , boost::str(boost::format("%s clears the rolls for the %s category\n")
          % player->get_character()->get_name()
          % category)
      , player);
}

}


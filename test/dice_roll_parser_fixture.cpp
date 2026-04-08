#include "paradice/dice_roll_parser.hpp"

#include <gtest/gtest.h>

TEST(a_dice_roll_parser, does_not_parse_an_empty_string)
{
    auto const text = std::string();
    auto begin = text.begin();
    auto end = text.end();

    auto roll = paradice::parse_dice_roll(begin, end);

    ASSERT_FALSE(roll.is_initialized());
}

TEST(a_dice_roll_parser, parses_a_basic_roll_without_bonus)
{
    auto const text = std::string("2d6");
    auto begin = text.begin();
    auto end = text.end();

    auto roll = paradice::parse_dice_roll(begin, end);

    ASSERT_TRUE(roll.is_initialized());
    ASSERT_EQ(1u, roll->repetitions_);
    ASSERT_EQ(2u, roll->amount_);
    ASSERT_EQ(6u, roll->sides_);
    ASSERT_EQ(0, roll->bonus_);
}

TEST(a_dice_roll_parser, parses_a_roll_with_a_positive_bonus)
{
    auto const text = std::string("3D9+3");
    auto begin = text.begin();
    auto end = text.end();

    auto roll = paradice::parse_dice_roll(begin, end);

    ASSERT_TRUE(roll.is_initialized());
    ASSERT_EQ(1u, roll->repetitions_);
    ASSERT_EQ(3u, roll->amount_);
    ASSERT_EQ(9u, roll->sides_);
    ASSERT_EQ(3, roll->bonus_);
}

TEST(a_dice_roll_parser, preserves_trailing_text_after_the_parsed_roll)
{
    auto const text = std::string("10d20+30-35FOO");
    auto begin = text.begin();
    auto end = text.end();

    auto roll = paradice::parse_dice_roll(begin, end);

    ASSERT_TRUE(roll.is_initialized());
    ASSERT_EQ(1u, roll->repetitions_);
    ASSERT_EQ(10u, roll->amount_);
    ASSERT_EQ(20u, roll->sides_);
    ASSERT_EQ(-5, roll->bonus_);
    ASSERT_EQ(std::string("FOO"), std::string(begin, end));
}

#include "paradice/dice_roll_parser.hpp"
#include <gtest/gtest.h>

//* =========================================================================
//  An empty string must not match a dice roll.
//* =========================================================================
TEST(dice_parser, test_empty_string)
{
    auto const str = std::string();
    auto begin = str.begin();
    auto end   = str.end();
    
    auto roll = paradice::parse_dice_roll(begin, end);
    
    ASSERT_FALSE(roll.is_initialized());
}

//* =========================================================================
//  A roll of xdy is equivalent to xdy+0
//* =========================================================================
TEST(dice_parser, test_roll_no_bonus)
{
    auto const str = std::string("2d6");
    auto begin = str.begin();
    auto end   = str.end();
    
    auto roll = paradice::parse_dice_roll(begin, end);
        
    ASSERT_TRUE(roll.is_initialized());
    
    paradice::dice_roll result = roll.get();
    
    ASSERT_EQ(std::uint32_t(2), result.amount_);
    ASSERT_EQ(std::uint32_t(6), result.sides_);
    ASSERT_EQ(std::int32_t(0), result.bonus_);
}

//* =========================================================================
//  A roll of xdy+z must parse correctly.
//* =========================================================================
TEST(dice_parser, test_roll_with_bonus)
{
    auto const str = std::string("3D9+3");
    auto begin = str.begin();
    auto end   = str.end();
    
    auto roll = paradice::parse_dice_roll(begin, end);
        
    ASSERT_TRUE(roll.is_initialized());
    
    paradice::dice_roll result = roll.get();
    
    ASSERT_EQ(std::uint32_t(3), result.amount_);
    ASSERT_EQ(std::uint32_t(9), result.sides_);
    ASSERT_EQ(std::int32_t(3), result.bonus_);
}

//* =========================================================================
//  A roll of xdy+(expr) must parse correctly.
//* =========================================================================
TEST(dice_parser, test_roll_extended_bonus)
{
    auto const str = std::string("10d20+30-35FOO");
    auto begin = str.begin();
    auto end   = str.end();
    
    auto roll = paradice::parse_dice_roll(begin, end);
        
    ASSERT_TRUE(roll.is_initialized());
    
    paradice::dice_roll result = roll.get();
    
    ASSERT_EQ(std::uint32_t(10), result.amount_);
    ASSERT_EQ(std::uint32_t(20), result.sides_);
    ASSERT_EQ(std::int32_t(-5), result.bonus_);
    
    // Also check that the text after the roll is preserved.
    ASSERT_EQ(std::string("FOO"), std::string(begin, end));
}


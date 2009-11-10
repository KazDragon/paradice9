#include "dice_parser_fixture.hpp"
#include "paradice/dice_roll_parser.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(dice_parser_fixture);

using namespace boost;
using namespace std;

//* =========================================================================
//  An empty string must not match a dice roll.
//* =========================================================================
void dice_parser_fixture::test_empty_string()
{
    optional<paradice::dice_roll> roll = 
        paradice::parse_dice_roll(string(""));
    
    CPPUNIT_ASSERT_EQUAL(false, roll.is_initialized());
}

//* =========================================================================
//  A roll of xdy is equivalent to xdy+0
//* =========================================================================
void dice_parser_fixture::test_roll_no_bonus()
{
    optional<paradice::dice_roll> roll =
        paradice::parse_dice_roll(string("2d6"));
        
    CPPUNIT_ASSERT_EQUAL(true, roll.is_initialized());
    
    paradice::dice_roll result = roll.get();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(2), result.amount_);
    CPPUNIT_ASSERT_EQUAL(odin::u32(6), result.sides_);
    CPPUNIT_ASSERT_EQUAL(odin::s32(0), result.bonus_);
}

//* =========================================================================
//  A roll of xdy+z must parse correctly.
//* =========================================================================
void dice_parser_fixture::test_roll_with_bonus()
{
    optional<paradice::dice_roll> roll =
        paradice::parse_dice_roll(string("3D9+3"));
        
    CPPUNIT_ASSERT_EQUAL(true, roll.is_initialized());
    
    paradice::dice_roll result = roll.get();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(3), result.amount_);
    CPPUNIT_ASSERT_EQUAL(odin::u32(9), result.sides_);
    CPPUNIT_ASSERT_EQUAL(odin::s32(3), result.bonus_);
}

//* =========================================================================
//  A roll of xdy+(expr) must parse correctly.
//* =========================================================================
void dice_parser_fixture::test_roll_extended_bonus()
{
    optional<paradice::dice_roll> roll =
        paradice::parse_dice_roll(string("10d20+30-35"));
        
    CPPUNIT_ASSERT_EQUAL(true, roll.is_initialized());
    
    paradice::dice_roll result = roll.get();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(10), result.amount_);
    CPPUNIT_ASSERT_EQUAL(odin::u32(20), result.sides_);
    CPPUNIT_ASSERT_EQUAL(odin::s32(-5), result.bonus_);
}


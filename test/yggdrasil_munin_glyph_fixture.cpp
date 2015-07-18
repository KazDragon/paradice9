#include "yggdrasil_munin_glyph_fixture.hpp"
#include "yggdrasil/munin/glyph.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(yggdrasil_munin_glyph_fixture);

void yggdrasil_munin_glyph_fixture::test_default_constructor()
{
    auto gly = yggdrasil::munin::glyph();
    
    CPPUNIT_ASSERT_EQUAL(' ', gly.character_);
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::character_set::CHARACTER_SET_G0,
        gly.character_set_);
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::character_set::LOCALE_US_ASCII,
        gly.locale_);
}

static yggdrasil::munin::glyph return_glyph(yggdrasil::munin::glyph gly)
{
    return gly;
}

void yggdrasil_munin_glyph_fixture::test_char_constructor()
{
    auto gly = return_glyph('X');
    
    CPPUNIT_ASSERT_EQUAL('X', gly.character_);
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::character_set::CHARACTER_SET_G0,
        gly.character_set_);
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::character_set::LOCALE_US_ASCII,
        gly.locale_);
}

void yggdrasil_munin_glyph_fixture::test_charset_locale_constructor()
{
    auto gly = yggdrasil::munin::glyph(
        'X',
        yggdrasil::character_set::CHARACTER_SET_G1,
        yggdrasil::character_set::LOCALE_UK);
    
    CPPUNIT_ASSERT_EQUAL('X', gly.character_);
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::character_set::CHARACTER_SET_G1,
        gly.character_set_);
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::character_set::LOCALE_UK,
        gly.locale_);
}

void yggdrasil_munin_glyph_fixture::test_equality_operator()
{
    auto lhs = yggdrasil::munin::glyph(
        ' ',
        yggdrasil::character_set::CHARACTER_SET_G0,
        yggdrasil::character_set::LOCALE_US_ASCII);
    
    auto rhs = yggdrasil::munin::glyph(
        'X',
        yggdrasil::character_set::CHARACTER_SET_G1,
        yggdrasil::character_set::LOCALE_UK);
    
    CPPUNIT_ASSERT(lhs == lhs);
    CPPUNIT_ASSERT(!(lhs == rhs));
}

void yggdrasil_munin_glyph_fixture::test_inequality_operator()
{
    auto lhs = yggdrasil::munin::glyph(
        ' ',
        yggdrasil::character_set::CHARACTER_SET_G0,
        yggdrasil::character_set::LOCALE_US_ASCII);
    
    auto rhs = yggdrasil::munin::glyph(
        'X',
        yggdrasil::character_set::CHARACTER_SET_G1,
        yggdrasil::character_set::LOCALE_UK);
    
    CPPUNIT_ASSERT(lhs != rhs);
    CPPUNIT_ASSERT(!(lhs != lhs));
}

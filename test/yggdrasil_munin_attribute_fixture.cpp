#include "yggdrasil_munin_attribute_fixture.hpp"
#include "yggdrasil/munin/attribute.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(yggdrasil_munin_attribute_fixture);

void yggdrasil_munin_attribute_fixture::test_default_constructor()
{
    auto elem = yggdrasil::munin::attribute{};
    
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::munin::attribute::colour(
            yggdrasil::munin::attribute::low_colour(
                yggdrasil::graphics::colour::default_)),
        elem.foreground_colour_);
                
    
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::munin::attribute::colour(
            yggdrasil::munin::attribute::low_colour(
                yggdrasil::graphics::colour::default_)),
        elem.background_colour_);
    
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::graphics::intensity::normal,
        elem.intensity_);
    
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::graphics::underlining::not_underlined,
        elem.underlining_);
    
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::graphics::polarity::positive,
        elem.polarity_);
    
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::graphics::blinking::steady,
        elem.blinking_);
}

void yggdrasil_munin_attribute_fixture::test_foreground_colour_constructor()
{
    auto lhs = yggdrasil::munin::attribute{};
    lhs.foreground_colour_ = yggdrasil::graphics::colour::yellow;
    
    auto rhs = yggdrasil::munin::attribute(yggdrasil::graphics::colour::yellow);
    
    CPPUNIT_ASSERT_EQUAL(lhs, rhs);
}

void yggdrasil_munin_attribute_fixture::test_all_colours_constructor()
{
    auto lhs = yggdrasil::munin::attribute{};
    lhs.foreground_colour_ = yggdrasil::graphics::colour::yellow;
    lhs.background_colour_ = yggdrasil::graphics::colour::blue;
    
    auto rhs = yggdrasil::munin::attribute(
        yggdrasil::graphics::colour::yellow,
        yggdrasil::graphics::colour::blue);
    
    CPPUNIT_ASSERT_EQUAL(lhs, rhs);
    
}

void yggdrasil_munin_attribute_fixture::test_equality_operator()
{
    auto lhs = yggdrasil::munin::attribute{};
    lhs.foreground_colour_ = yggdrasil::graphics::colour::black;
    lhs.background_colour_ = yggdrasil::graphics::colour::white;
    lhs.intensity_ = yggdrasil::graphics::intensity::normal;
    lhs.underlining_ = yggdrasil::graphics::underlining::underlined;
    lhs.polarity_ = yggdrasil::graphics::polarity::positive;
    lhs.blinking_ = yggdrasil::graphics::blinking::blink;
    
    auto rhs = yggdrasil::munin::attribute{};
    rhs.foreground_colour_ = yggdrasil::graphics::colour::white;
    rhs.background_colour_ = yggdrasil::graphics::colour::black;
    rhs.intensity_ = yggdrasil::graphics::intensity::faint;
    rhs.underlining_ = yggdrasil::graphics::underlining::not_underlined;
    rhs.polarity_ = yggdrasil::graphics::polarity::negative;
    rhs.blinking_ = yggdrasil::graphics::blinking::steady;
    
    CPPUNIT_ASSERT(lhs == lhs);
    CPPUNIT_ASSERT(!(lhs == rhs));
}

void yggdrasil_munin_attribute_fixture::test_inequality_operator()
{
    auto lhs = yggdrasil::munin::attribute{};
    lhs.foreground_colour_ = yggdrasil::graphics::colour::black;
    lhs.background_colour_ = yggdrasil::graphics::colour::white;
    lhs.intensity_ = yggdrasil::graphics::intensity::normal;
    lhs.underlining_ = yggdrasil::graphics::underlining::underlined;
    lhs.polarity_ = yggdrasil::graphics::polarity::positive;
    lhs.blinking_ = yggdrasil::graphics::blinking::blink;
    
    auto rhs = yggdrasil::munin::attribute{};
    rhs.foreground_colour_ = yggdrasil::graphics::colour::white;
    rhs.background_colour_ = yggdrasil::graphics::colour::black;
    rhs.intensity_ = yggdrasil::graphics::intensity::faint;
    rhs.underlining_ = yggdrasil::graphics::underlining::not_underlined;
    rhs.polarity_ = yggdrasil::graphics::polarity::negative;
    rhs.blinking_ = yggdrasil::graphics::blinking::steady;
    
    CPPUNIT_ASSERT(lhs != rhs);
    CPPUNIT_ASSERT(!(lhs != lhs));
}

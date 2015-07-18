#include "yggdrasil_munin_element_fixture.hpp"
#include "yggdrasil/munin/element.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(yggdrasil_munin_element_fixture);

void yggdrasil_munin_element_fixture::test_default_constructor()
{
    auto elem = yggdrasil::munin::element{};
    CPPUNIT_ASSERT_EQUAL(
        elem.glyph_, yggdrasil::munin::glyph{});
    CPPUNIT_ASSERT_EQUAL(
        elem.attribute_, yggdrasil::munin::attribute{});
}

void yggdrasil_munin_element_fixture::test_glyph_constructor()
{
    auto elem = yggdrasil::munin::element(yggdrasil::munin::glyph('X'));
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::munin::glyph('X'),
        elem.glyph_);
}

void yggdrasil_munin_element_fixture::test_glyph_attr_constructor()
{

    auto elem = yggdrasil::munin::element(
        yggdrasil::munin::glyph('Y'),
        yggdrasil::munin::attribute(yggdrasil::graphics::colour::white));
    
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::munin::glyph('Y'),
        elem.glyph_);
    
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::munin::attribute(yggdrasil::graphics::colour::white),
        elem.attribute_);
}

void yggdrasil_munin_element_fixture::test_char_constructor()
{
    auto elem = yggdrasil::munin::element('Y');

    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::munin::glyph('Y'),
        elem.glyph_);

    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::munin::attribute{},
        elem.attribute_);
}

yggdrasil::munin::element return_element(yggdrasil::munin::element const &elem)
{
    return elem;
}

void yggdrasil_munin_element_fixture::test_initializer_list_constructor()
{
    auto elem = return_element({{}, {}});
    
    CPPUNIT_ASSERT_EQUAL(
        elem.glyph_, yggdrasil::munin::glyph{});
    CPPUNIT_ASSERT_EQUAL(
        elem.attribute_, yggdrasil::munin::attribute{});
}

void yggdrasil_munin_element_fixture::test_equality_operator()
{
    auto lhs = yggdrasil::munin::element{
        'X',
        yggdrasil::graphics::colour::white};
    
    auto rhs = yggdrasil::munin::element(
        'Y',
        yggdrasil::graphics::colour::black);
    
    CPPUNIT_ASSERT(lhs == lhs);
    CPPUNIT_ASSERT(!(lhs == rhs));
}

void yggdrasil_munin_element_fixture::test_inequality_operator()
{
    auto lhs = yggdrasil::munin::element(
        'X',
        yggdrasil::graphics::colour::white);
    
    auto rhs = yggdrasil::munin::element(
        'Y',
        yggdrasil::graphics::colour::black);
    
    CPPUNIT_ASSERT(lhs != rhs);
    CPPUNIT_ASSERT(!(lhs != lhs));
}

void yggdrasil_munin_element_fixture::test_assignment()
{
    auto lhs = yggdrasil::munin::element(
        'X',
        yggdrasil::graphics::colour::white);
    
    auto const rhs = yggdrasil::munin::element(
        'Y',
        yggdrasil::graphics::colour::black);
    
    lhs = rhs;
    
    CPPUNIT_ASSERT_EQUAL(lhs, rhs);
}

void yggdrasil_munin_element_fixture::test_glyph_assignment()
{
    auto lhs = yggdrasil::munin::element(
        'X',
        yggdrasil::graphics::colour::white);

    // Note: colour is default.  That is because, in 'elem = 'X', the
    // can be considered an element with glyph 'X' and default attributes.
    auto const rhs = yggdrasil::munin::element(
        'Y',
        yggdrasil::graphics::colour::default_);
    
    lhs = yggdrasil::munin::glyph('Y');
    
    CPPUNIT_ASSERT_EQUAL(lhs, rhs);
}

void yggdrasil_munin_element_fixture::test_char_assignment()
{
    auto lhs = yggdrasil::munin::element(
        'X',
        yggdrasil::graphics::colour::white);
    
    // Note: colour is default.  That is because, in 'elem = 'X', the
    // can be considered an element with glyph 'X' and default attributes.
    auto const rhs = yggdrasil::munin::element(
        'Y',
        yggdrasil::graphics::colour::default_);
    
    lhs = 'Y';
    
    CPPUNIT_ASSERT_EQUAL(lhs, rhs);
}

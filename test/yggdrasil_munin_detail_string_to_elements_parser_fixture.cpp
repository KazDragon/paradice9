#include "yggdrasil_munin_detail_string_to_elements_parser_fixture.hpp"
#include "yggdrasil/munin/detail/string_to_elements.hpp"
#include "yggdrasil/munin/element.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(
    yggdrasil_munin_detail_string_to_elements_parser_fixture);

void yggdrasil_munin_detail_string_to_elements_parser_fixture::test_empty_string()
{
    // An empty string should yield an empty vector of elements.
    auto elements = yggdrasil::munin::detail::string_to_elements("");
    
    CPPUNIT_ASSERT_EQUAL(true, elements.empty());
}

void yggdrasil_munin_detail_string_to_elements_parser_fixture::test_normal_string()
{
    // A string of normal text should be converted to an equivalent
    // array of elements, with each attribute being the default attribute.
    auto const test_string = std::string("test_string");
    auto elements = yggdrasil::munin::detail::string_to_elements(test_string);
    
    auto expected_array = std::vector<yggdrasil::munin::element>();
    
    for (auto const &ch : test_string)
    {
        expected_array.push_back({ch});
    }
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void yggdrasil_munin_detail_string_to_elements_parser_fixture::test_character_code()
{
    // A string ot the form \C??? where ??? is 000..255 should yield
    // an equivalent character.  For example, \C097 should yield the 'a'
    // character, and so on.
    auto const test_string = std::string("\\C097\\C098\\C099");
    auto const elements = yggdrasil::munin::detail::string_to_elements(
        test_string);
    
    auto expected_array = std::vector<yggdrasil::munin::element> {
        {'a'}, {'b'}, {'c'}
    };
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void yggdrasil_munin_detail_string_to_elements_parser_fixture::test_escaped_slash()
{
    // If there is a "\\" in the text, then we treat it as a single slash.
    // To test this, we will deliberately introduce a test case where a
    // misinterpretation would otherwise cause a character code to be
    // read.
    auto const test_string = std::string("\\\\C097");
    auto const elements = yggdrasil::munin::detail::string_to_elements(
        test_string);
    
    auto expected_array = std::vector<yggdrasil::munin::element> {
        {'\\'}, {'C'}, {'0'}, {'9'}, {'7'}
    };
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void yggdrasil_munin_detail_string_to_elements_parser_fixture::test_character_set()
{
    // A character set change follows the form "\c?" where ? is either
    // x, 0, or 1.  This affects the character_set_ member in the following
    // elements' glyphs.
    auto const test_string = std::string("\\c0a\\c1b\\cxc");
    auto const elements = yggdrasil::munin::detail::string_to_elements(
        test_string);
    
    auto glyph_a = yggdrasil::munin::glyph('a');
    glyph_a.character_set_ = yggdrasil::character_set::CHARACTER_SET_G0;
    
    auto glyph_b = yggdrasil::munin::glyph('b');
    glyph_b.character_set_ = yggdrasil::character_set::CHARACTER_SET_G1;
    
    auto glyph_c = yggdrasil::munin::glyph('c');
    glyph_c.character_set_ = yggdrasil::character_set::CHARACTER_SET_G0;
    
    auto const expected_array = std::vector<yggdrasil::munin::element> { 
        glyph_a, glyph_b, glyph_c
    };
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void yggdrasil_munin_detail_string_to_elements_parser_fixture::test_locale()
{
    // A locale change follows the form "\l?" where ? is either x, 0, A, B,
    // or U.  There are potentially others, but these are the only supported
    // ones for now.  This affects the locale_ member in the following
    // elements' glyphs.
    auto const test_string = std::string("\\l0a\\lAb\\lBc\\lUd\\lxe");
    auto const elements = yggdrasil::munin::detail::string_to_elements(
        test_string);
    
    auto glyph_a = yggdrasil::munin::glyph('a');
    glyph_a.locale_ = yggdrasil::character_set::LOCALE_DEC;
    
    auto glyph_b = yggdrasil::munin::glyph('b');
    glyph_b.locale_ = yggdrasil::character_set::LOCALE_UK;
    
    auto glyph_c = yggdrasil::munin::glyph('c');
    glyph_c.locale_ = yggdrasil::character_set::LOCALE_US_ASCII;
    
    auto glyph_d = yggdrasil::munin::glyph('d');
    glyph_d.locale_ = yggdrasil::character_set::LOCALE_SCO;
    
    auto glyph_e = yggdrasil::munin::glyph('e');
    glyph_e.locale_ = yggdrasil::character_set::LOCALE_DEC;
    
    auto const expected_array = std::vector<yggdrasil::munin::element> { 
        glyph_a, glyph_b, glyph_c, glyph_d, glyph_e
    };
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void yggdrasil_munin_detail_string_to_elements_parser_fixture::test_intensity()
{
    // An intensity change follows the form "i?" where ? is either 
    // x, >, =, or <.  x is the default, which the same as =.  > is bold,
    // = is normal, and < is faint.
    auto const test_string = std::string("\\i>a\\i=b\\i<c\\ixd");
    auto const elements = yggdrasil::munin::detail::string_to_elements(
        test_string);
    
    auto attribute_a = yggdrasil::munin::attribute();
    attribute_a.intensity_ = yggdrasil::graphics::intensity::bold;
    
    auto attribute_b = yggdrasil::munin::attribute();
    attribute_b.intensity_ = yggdrasil::graphics::intensity::normal;
    
    auto attribute_c = yggdrasil::munin::attribute();
    attribute_c.intensity_ = yggdrasil::graphics::intensity::faint;
    
    auto attribute_d = yggdrasil::munin::attribute();
    attribute_d.intensity_ = yggdrasil::graphics::intensity::normal;
    
    auto const expected_array = std::vector<yggdrasil::munin::element> { 
        {'a', attribute_a},
        {'b', attribute_b},
        {'c', attribute_c},
        {'d', attribute_d}
    };
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void yggdrasil_munin_detail_string_to_elements_parser_fixture::test_polarity()
{
    // A polarity change follows the form "\p?" where ? is either x, +, or -.
    // x is the default, which is the same as +.  + is positive polarity,
    // - is negative polarity.
    auto const test_string = std::string("\\p+a\\p-b\\pxc");
    auto const elements = yggdrasil::munin::detail::string_to_elements(
        test_string);
    
    auto attribute_a = yggdrasil::munin::attribute();
    attribute_a.polarity_ = yggdrasil::graphics::polarity::positive;
    
    auto attribute_b = yggdrasil::munin::attribute();
    attribute_b.polarity_ = yggdrasil::graphics::polarity::negative;
    
    auto attribute_c = yggdrasil::munin::attribute();
    attribute_c.polarity_ = yggdrasil::graphics::polarity::positive;
    
    auto const expected_array = std::vector<yggdrasil::munin::element> { 
        {'a', attribute_a},
        {'b', attribute_b},
        {'c', attribute_c}
    };
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void yggdrasil_munin_detail_string_to_elements_parser_fixture::test_underlining()
{
    // An underlining change follows the form "\u?" where ? is either 
    // x, +, or -.  x is the default, which is the same as -.  + is underlining
    // on, - is underlining off.
    auto const test_string = std::string("\\u+a\\u-b\\uxc");
    auto const elements = yggdrasil::munin::detail::string_to_elements(
        test_string);
    
    auto attribute_a = yggdrasil::munin::attribute();
    attribute_a.underlining_ = yggdrasil::graphics::underlining::underlined;
    
    auto attribute_b = yggdrasil::munin::attribute();
    attribute_b.underlining_ = yggdrasil::graphics::underlining::not_underlined;
    
    auto attribute_c = yggdrasil::munin::attribute();
    attribute_c.underlining_ = yggdrasil::graphics::underlining::not_underlined;
    
    auto const expected_array = std::vector<yggdrasil::munin::element> {
        {'a', attribute_a},
        {'b', attribute_b},
        {'c', attribute_c}
    };
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void yggdrasil_munin_detail_string_to_elements_parser_fixture::test_foreground_colour_ansi()
{
    // ansi (low) foreground colour follows the form \[? where ? is 0..9.
    auto const test_string = std::string("\\[0a\\[7b");
    auto const elements = yggdrasil::munin::detail::string_to_elements(
        test_string);
    
    auto attribute_a = yggdrasil::munin::attribute();
    attribute_a.foreground_colour_ = yggdrasil::graphics::colour::black;
    
    auto attribute_b = yggdrasil::munin::attribute();
    attribute_b.foreground_colour_ = yggdrasil::graphics::colour::white;
    
    auto const expected_array = std::vector<yggdrasil::munin::element> { 
        {'a', attribute_a},
        {'b', attribute_b}
    };
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void yggdrasil_munin_detail_string_to_elements_parser_fixture::test_foreground_colour_216()
{
    // 216-colour (high) foreground follows the form \<?R?G?B where
    // ?R is the red component, ?G the green, and ?B the blue.  These
    // components take the form of 0..5.
    auto const test_string = std::string("\\<012a\\<345b");
    auto const elements = yggdrasil::munin::detail::string_to_elements(
        test_string);
    
    auto attribute_a = yggdrasil::munin::attribute();
    attribute_a.foreground_colour_ =
        yggdrasil::munin::attribute::high_colour(0, 1, 2);
    
    auto attribute_b = yggdrasil::munin::attribute();
    attribute_b.foreground_colour_ =
        yggdrasil::munin::attribute::high_colour(3, 4, 5);
    
    auto const expected_array = std::vector<yggdrasil::munin::element> { 
        {'a', attribute_a},
        {'b', attribute_b}
    };
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void yggdrasil_munin_detail_string_to_elements_parser_fixture::test_foreground_colour_greyscale()
{
    // Greyscale colour foreground follows the form \{?? where
    // ?? is 0..23.
    auto const test_string = std::string("\\{1a\\{09b\\{23c");
    auto const elements = yggdrasil::munin::detail::string_to_elements(
        test_string);
    
    auto attribute_a = yggdrasil::munin::attribute();
    attribute_a.foreground_colour_ =
        yggdrasil::munin::attribute::greyscale_colour(1);
    
    auto attribute_b = yggdrasil::munin::attribute();
    attribute_b.foreground_colour_ =
        yggdrasil::munin::attribute::greyscale_colour(9);
    
    auto attribute_c = yggdrasil::munin::attribute();
    attribute_c.foreground_colour_ =
        yggdrasil::munin::attribute::greyscale_colour(23);
    
    auto const expected_array = std::vector<yggdrasil::munin::element> { 
        {'a', attribute_a},
        {'b', attribute_b},
        {'c', attribute_c}
    };
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void yggdrasil_munin_detail_string_to_elements_parser_fixture::test_background_colour_ansi()
{
    // ansi (low) background colour follows the form \]? where ? is 0..9.
    auto const test_string = std::string("\\]0a\\]7b");
    auto const elements = yggdrasil::munin::detail::string_to_elements(
        test_string);
    
    auto attribute_a = yggdrasil::munin::attribute();
    attribute_a.background_colour_ = yggdrasil::graphics::colour::black;
    
    auto attribute_b = yggdrasil::munin::attribute();
    attribute_b.background_colour_ = yggdrasil::graphics::colour::white;
    
    auto const expected_array = std::vector<yggdrasil::munin::element> { 
        {'a', attribute_a},
        {'b', attribute_b}
    };
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void yggdrasil_munin_detail_string_to_elements_parser_fixture::test_background_colour_216()
{
    // 216-colour (high) background follows the form \>?R?G?B where
    // ?R is the red component, ?G the green, and ?B the blue.  These
    // components take the form of 0..5.
    auto const test_string = std::string("\\>012a\\>345b");
    auto const elements = yggdrasil::munin::detail::string_to_elements(
        test_string);
    
    auto attribute_a = yggdrasil::munin::attribute();
    attribute_a.background_colour_ =
        yggdrasil::munin::attribute::high_colour(0, 1, 2);
    
    auto attribute_b = yggdrasil::munin::attribute();
    attribute_b.background_colour_ =
        yggdrasil::munin::attribute::high_colour(3, 4, 5);
    
    auto const expected_array = std::vector<yggdrasil::munin::element> { 
        {'a', attribute_a},
        {'b', attribute_b}
    };
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void yggdrasil_munin_detail_string_to_elements_parser_fixture::test_background_colour_greyscale()
{
    // Greyscale colour background follows the form \}?? where
    // ?? is 0..23.
    auto const test_string = std::string("\\}1a\\}09b\\}23c");
    auto const elements = yggdrasil::munin::detail::string_to_elements(
        test_string);
    
    auto attribute_a = yggdrasil::munin::attribute();
    attribute_a.background_colour_ =
        yggdrasil::munin::attribute::greyscale_colour(1);
    
    auto attribute_b = yggdrasil::munin::attribute();
    attribute_b.background_colour_ =
        yggdrasil::munin::attribute::greyscale_colour(9);
    
    auto attribute_c = yggdrasil::munin::attribute();
    attribute_c.background_colour_ =
        yggdrasil::munin::attribute::greyscale_colour(23);

    auto const expected_array = std::vector<yggdrasil::munin::element> { 
        {'a', attribute_a},
        {'b', attribute_b},
        {'c', attribute_c}
    };
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void yggdrasil_munin_detail_string_to_elements_parser_fixture::test_default()
{
    // Default is \x, and resets all attributes and glyph characteristics
    // to their default value.
    auto const test_string = std::string("\\c1\\lU\\i>\\u+\\p-\\[1\\]7a\\xb");
    auto const elements = yggdrasil::munin::detail::string_to_elements(
        test_string);
    
    auto glyph_a = yggdrasil::munin::glyph('a');
    glyph_a.character_set_ = yggdrasil::character_set::CHARACTER_SET_G1;
    glyph_a.locale_        = yggdrasil::character_set::LOCALE_SCO;
    
    auto attribute_a = yggdrasil::munin::attribute();
    attribute_a.intensity_   = yggdrasil::graphics::intensity::bold;
    attribute_a.underlining_ = yggdrasil::graphics::underlining::underlined;
    attribute_a.polarity_    = yggdrasil::graphics::polarity::negative;
    attribute_a.foreground_colour_ = yggdrasil::graphics::colour::red;
    attribute_a.background_colour_ = yggdrasil::graphics::colour::white;
    
    auto attribute_b = yggdrasil::munin::attribute();
    
    auto const expected_array = std::vector<yggdrasil::munin::element> { 
        {glyph_a, attribute_a},
        {'b', attribute_b}
    };
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

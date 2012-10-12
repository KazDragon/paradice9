#include "munin_string_to_elements_fixture.hpp"
#include "munin/algorithm.hpp"
#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <vector>

using namespace boost::assign;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(munin_string_to_elements_fixture);

void munin_string_to_elements_fixture::test_empty_string()
{
    // An empty string should yield an empty vector of elements.
    vector<munin::element_type> elements = 
        munin::string_to_elements(string(""));
        
    CPPUNIT_ASSERT_EQUAL(true, elements.empty());
}

void munin_string_to_elements_fixture::test_normal_string()
{
    // A string of normal text should be converted to an equivalent
    // array of elements, with each attribute being the default attribute.
    string test_string = "test_string";
    vector<munin::element_type> elements(
        munin::string_to_elements(test_string));
    
    vector<munin::element_type> expected_array;
    
    BOOST_FOREACH(char ch, test_string)
    {
        expected_array.push_back(munin::element_type(ch));
    }
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void munin_string_to_elements_fixture::test_character_code()
{
    // A string ot the form \C??? where ??? is 000..255 should yield
    // an equivalent character.  For example, \C097 should yield the 'a'
    // character, and so on.
    string test_string = "\\C097\\C098\\C099";
    vector<munin::element_type> elements(
        munin::string_to_elements(test_string));
    
    vector<munin::element_type> expected_array = 
        list_of(munin::element_type('a'))
               (munin::element_type('b'))
               (munin::element_type('c'));

    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void munin_string_to_elements_fixture::test_escaped_slash()
{
    // If there is a "\\" in the text, then we treat it as a single slash.
    // To test this, we will deliberately introduce a test case where a
    // misinterpretation would otherwise cause a character code to be
    // read.
    string test_string("\\\\C097");
    vector<munin::element_type> elements(
        munin::string_to_elements(test_string));
    
    vector<munin::element_type> expected_array = 
        list_of(munin::element_type('\\'))
               (munin::element_type('C'))
               (munin::element_type('0'))
               (munin::element_type('9'))
               (munin::element_type('7'));

    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void munin_string_to_elements_fixture::test_character_set()
{
    // A character set change follows the form "\c?" where ? is either
    // x, 0, or 1.  This affects the character_set_ member in the following
    // elements' glyphs.
    string test_string("\\c0a\\c1b\\cxc");
    vector<munin::element_type> elements(
        munin::string_to_elements(test_string));

    munin::glyph glyph_a('a');
    glyph_a.character_set_ = odin::ansi::character_set::CHARACTER_SET_G0;

    munin::glyph glyph_b('b');
    glyph_b.character_set_ = odin::ansi::character_set::CHARACTER_SET_G1;

    munin::glyph glyph_c('c');
    glyph_c.character_set_ = odin::ansi::character_set::CHARACTER_SET_G0;
    
    vector<munin::element_type> expected_array = 
        list_of(munin::element_type(glyph_a))
               (munin::element_type(glyph_b))
               (munin::element_type(glyph_c));

    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void munin_string_to_elements_fixture::test_locale()
{
    // A locale change follows the form "\l?" where ? is either x, 0, A, B,
    // or U.  There are potentially others, but these are the only supported
    // ones for now.  This affects the locale_ member in the following
    // elements' glyphs.
    string test_string("\\l0a\\lAb\\lBc\\lUd\\lxe");
    vector<munin::element_type> elements(
        munin::string_to_elements(test_string));
    
    munin::glyph glyph_a('a');
    glyph_a.locale_ = odin::ansi::character_set::LOCALE_DEC;
    
    munin::glyph glyph_b('b');
    glyph_b.locale_ = odin::ansi::character_set::LOCALE_UK;
    
    munin::glyph glyph_c('c');
    glyph_c.locale_ = odin::ansi::character_set::LOCALE_US_ASCII;
    
    munin::glyph glyph_d('d');
    glyph_d.locale_ = odin::ansi::character_set::LOCALE_SCO;
    
    munin::glyph glyph_e('e');
    glyph_e.locale_ = odin::ansi::character_set::LOCALE_DEC;

    vector<munin::element_type> expected_array = 
        list_of(munin::element_type(glyph_a))
               (munin::element_type(glyph_b))
               (munin::element_type(glyph_c))
               (munin::element_type(glyph_d))
               (munin::element_type(glyph_e));

    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void munin_string_to_elements_fixture::test_intensity()
{
    // An intensity change follows the form "i?" where ? is either 
    // x, >, =, or <.  x is the default, which the same as =.  > is bold,
    // = is normal, and < is faint.
    string test_string("\\i>a\\i=b\\i<c\\ixd");
    vector<munin::element_type> elements(
        munin::string_to_elements(test_string));
    
    munin::attribute attribute_a;
    attribute_a.intensity_ = odin::ansi::graphics::INTENSITY_BOLD;
    
    munin::attribute attribute_b;
    attribute_b.intensity_ = odin::ansi::graphics::INTENSITY_NORMAL;
    
    munin::attribute attribute_c;
    attribute_c.intensity_ = odin::ansi::graphics::INTENSITY_FAINT;
    
    munin::attribute attribute_d;
    attribute_d.intensity_ = odin::ansi::graphics::INTENSITY_NORMAL;

    vector<munin::element_type> expected_array = 
        list_of(munin::element_type('a', attribute_a))
               (munin::element_type('b', attribute_b))
               (munin::element_type('c', attribute_c))
               (munin::element_type('d', attribute_d));

    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void munin_string_to_elements_fixture::test_polarity()
{
    // A polarity change follows the form "\p?" where ? is either x, +, or -.
    // x is the default, which is the same as +.  + is positive polarity,
    // - is negative polarity.
    string test_string("\\p+a\\p-b\\pxc");
    vector<munin::element_type> elements(
        munin::string_to_elements(test_string));
    
    munin::attribute attribute_a;
    attribute_a.polarity_ = odin::ansi::graphics::POLARITY_POSITIVE;
    
    munin::attribute attribute_b;
    attribute_b.polarity_ = odin::ansi::graphics::POLARITY_NEGATIVE;
    
    munin::attribute attribute_c;
    attribute_c.polarity_ = odin::ansi::graphics::POLARITY_POSITIVE;

    vector<munin::element_type> expected_array = 
        list_of(munin::element_type('a', attribute_a))
               (munin::element_type('b', attribute_b))
               (munin::element_type('c', attribute_c));

    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void munin_string_to_elements_fixture::test_underlining()
{
    // An underlining change follows the form "\u?" where ? is either 
    // x, +, or -.  x is the default, which is the same as -.  + is underlining
    // on, - is underlining off.
    string test_string("\\u+a\\u-b\\uxc");
    vector<munin::element_type> elements(
        munin::string_to_elements(test_string));
    
    munin::attribute attribute_a;
    attribute_a.underlining_ = odin::ansi::graphics::UNDERLINING_UNDERLINED;
    
    munin::attribute attribute_b;
    attribute_b.underlining_ = odin::ansi::graphics::UNDERLINING_NOT_UNDERLINED;
    
    munin::attribute attribute_c;
    attribute_c.underlining_ = odin::ansi::graphics::UNDERLINING_NOT_UNDERLINED;

    vector<munin::element_type> expected_array = 
        list_of(munin::element_type('a', attribute_a))
               (munin::element_type('b', attribute_b))
               (munin::element_type('c', attribute_c));

    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void munin_string_to_elements_fixture::test_foreground_colour_ansi()
{
    // ansi (low) foreground colour follows the form \[? where ? is 0..9.
    string test_string("\\[0a\\[7b");
    vector<munin::element_type> elements(
        munin::string_to_elements(test_string));
    
    munin::attribute attribute_a;
    attribute_a.foreground_colour_ = odin::ansi::graphics::COLOUR_BLACK;
    
    munin::attribute attribute_b;
    attribute_b.foreground_colour_ = 
        munin::attribute::colour(odin::ansi::graphics::COLOUR_WHITE);
    
    vector<munin::element_type> expected_array = 
        list_of(munin::element_type('a', attribute_a))
               (munin::element_type('b', attribute_b));

    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void munin_string_to_elements_fixture::test_foreground_colour_216()
{
    // 216-colour (high) foreground follows the form \<?R?G?B where
    // ?R is the red component, ?G the green, and ?B the blue.  These
    // components take the form of 0..5.
    string test_string("\\<012a\\<345b");
    vector<munin::element_type> elements(
        munin::string_to_elements(test_string));
    
    munin::attribute attribute_a;
    attribute_a.foreground_colour_ =
        munin::attribute::high_colour(0, 1, 2);

    munin::attribute attribute_b;
    attribute_b.foreground_colour_ =
        munin::attribute::high_colour(3, 4, 5);
        
    vector<munin::element_type> expected_array = 
        list_of(munin::element_type('a', attribute_a))
               (munin::element_type('b', attribute_b));

    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void munin_string_to_elements_fixture::test_foreground_colour_greyscale()
{
    // Greyscale colour foreground follows the form \{?? where
    // ?? is 0..23.
    string test_string("\\{1a\\{09b\\{23c");
    vector<munin::element_type> elements(
        munin::string_to_elements(test_string));
    
    munin::attribute attribute_a;
    attribute_a.foreground_colour_ =
        munin::attribute::greyscale_colour(1);

    munin::attribute attribute_b;
    attribute_b.foreground_colour_ =
        munin::attribute::greyscale_colour(9);
        
    munin::attribute attribute_c;
    attribute_c.foreground_colour_ =
        munin::attribute::greyscale_colour(23);
        
    vector<munin::element_type> expected_array = 
        list_of(munin::element_type('a', attribute_a))
               (munin::element_type('b', attribute_b))
               (munin::element_type('c', attribute_c));

    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void munin_string_to_elements_fixture::test_background_colour_ansi()
{
    // ansi (low) background colour follows the form \]? where ? is 0..9.
    string test_string("\\]0a\\]7b");
    vector<munin::element_type> elements(
        munin::string_to_elements(test_string));
    
    munin::attribute attribute_a;
    attribute_a.background_colour_ = odin::ansi::graphics::COLOUR_BLACK;
    
    munin::attribute attribute_b;
    attribute_b.background_colour_ = 
        munin::attribute::colour(odin::ansi::graphics::COLOUR_WHITE);
    
    vector<munin::element_type> expected_array = 
        list_of(munin::element_type('a', attribute_a))
               (munin::element_type('b', attribute_b));

    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void munin_string_to_elements_fixture::test_background_colour_216()
{
    // 216-colour (high) background follows the form \>?R?G?B where
    // ?R is the red component, ?G the green, and ?B the blue.  These
    // components take the form of 0..5.
    string test_string("\\>012a\\>345b");
    vector<munin::element_type> elements(
        munin::string_to_elements(test_string));
    
    munin::attribute attribute_a;
    attribute_a.background_colour_ =
        munin::attribute::high_colour(0, 1, 2);

    munin::attribute attribute_b;
    attribute_b.background_colour_ =
        munin::attribute::high_colour(3, 4, 5);
        
    vector<munin::element_type> expected_array = 
        list_of(munin::element_type('a', attribute_a))
               (munin::element_type('b', attribute_b));

    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void munin_string_to_elements_fixture::test_background_colour_greyscale()
{
    // Greyscale colour background follows the form \}?? where
    // ?? is 0..23.
    string test_string("\\}1a\\}09b\\}23c");
    vector<munin::element_type> elements(
        munin::string_to_elements(test_string));
    
    munin::attribute attribute_a;
    attribute_a.background_colour_ =
        munin::attribute::greyscale_colour(1);

    munin::attribute attribute_b;
    attribute_b.background_colour_ =
        munin::attribute::greyscale_colour(9);
        
    munin::attribute attribute_c;
    attribute_c.background_colour_ =
        munin::attribute::greyscale_colour(23);
        
    vector<munin::element_type> expected_array = 
        list_of(munin::element_type('a', attribute_a))
               (munin::element_type('b', attribute_b))
               (munin::element_type('c', attribute_c));

    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}

void munin_string_to_elements_fixture::test_default()
{
    // Default is \x, and resets all attributes and glyph characteristics
    // to their default value.
    string test_string("\\c1\\lU\\i>\\u+\\p-\\[1\\]7a\\xb");
    vector<munin::element_type> elements(
        munin::string_to_elements(test_string));
    
    munin::glyph glyph_a('a');
    glyph_a.character_set_ = odin::ansi::character_set::CHARACTER_SET_G1;
    glyph_a.locale_        = odin::ansi::character_set::LOCALE_SCO;
    
    munin::attribute attribute_a;
    attribute_a.intensity_   = odin::ansi::graphics::INTENSITY_BOLD;
    attribute_a.underlining_ = odin::ansi::graphics::UNDERLINING_UNDERLINED;
    attribute_a.polarity_    = odin::ansi::graphics::POLARITY_NEGATIVE;
    attribute_a.foreground_colour_ = odin::ansi::graphics::COLOUR_RED;
    attribute_a.background_colour_ = odin::ansi::graphics::COLOUR_WHITE;

    munin::attribute attribute_b;
        
    vector<munin::element_type> expected_array = 
        list_of(munin::element_type(glyph_a, attribute_a))
               (munin::element_type('b', attribute_b));

    CPPUNIT_ASSERT_EQUAL(expected_array.size(), elements.size());
    CPPUNIT_ASSERT(expected_array == elements);
}


#ifndef YGGDRASIL_MUNIN_STRING_TO_ELEMENTS_PARSER_FIXTURE_HPP_
#define YGGDRASIL_MUNIN_STRING_TO_ELEMENTS_PARSER_FIXTURE_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class yggdrasil_munin_detail_string_to_elements_parser_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(yggdrasil_munin_detail_string_to_elements_parser_fixture);
        CPPUNIT_TEST(test_empty_string);
        CPPUNIT_TEST(test_normal_string);
        CPPUNIT_TEST(test_character_code);
        CPPUNIT_TEST(test_escaped_slash);
        CPPUNIT_TEST(test_character_set);
        CPPUNIT_TEST(test_locale);
        CPPUNIT_TEST(test_intensity);
        CPPUNIT_TEST(test_polarity);
        CPPUNIT_TEST(test_underlining);
        CPPUNIT_TEST(test_foreground_colour_ansi);
        CPPUNIT_TEST(test_foreground_colour_216);
        CPPUNIT_TEST(test_foreground_colour_greyscale);
        CPPUNIT_TEST(test_background_colour_ansi);
        CPPUNIT_TEST(test_background_colour_216);
        CPPUNIT_TEST(test_background_colour_greyscale);
        CPPUNIT_TEST(test_default);
    CPPUNIT_TEST_SUITE_END();
    
public :
    void test_empty_string();
    void test_normal_string();
    void test_character_code();
    void test_escaped_slash();
    void test_character_set();
    void test_locale();
    void test_intensity();
    void test_polarity();
    void test_underlining();
    void test_foreground_colour_ansi();
    void test_foreground_colour_216();
    void test_foreground_colour_greyscale();
    void test_background_colour_ansi();
    void test_background_colour_216();
    void test_background_colour_greyscale();
    void test_default();
};

#endif

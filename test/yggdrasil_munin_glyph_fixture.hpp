#ifndef YGGDRASIL_MUNIN_GLYPH_FIXTURE_HPP_
#define YGGDRASIL_MUNIN_GLYPH_FIXTURE_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class yggdrasil_munin_glyph_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(yggdrasil_munin_glyph_fixture);
        CPPUNIT_TEST(test_default_constructor);
        CPPUNIT_TEST(test_char_constructor);
        CPPUNIT_TEST(test_charset_locale_constructor);
        CPPUNIT_TEST(test_equality_operator);
        CPPUNIT_TEST(test_inequality_operator);
    CPPUNIT_TEST_SUITE_END();
    
public :
    void test_default_constructor();
    void test_char_constructor();
    void test_charset_locale_constructor();
    void test_equality_operator();
    void test_inequality_operator();
};

#endif

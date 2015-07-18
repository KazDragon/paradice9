#ifndef YGGDRASIL_MUNIN_ELEMENT_FIXTURE_HPP_
#define YGGDRASIL_MUNIN_ELEMENT_FIXTURE_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class yggdrasil_munin_element_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(yggdrasil_munin_element_fixture);
        CPPUNIT_TEST(test_default_constructor);
        CPPUNIT_TEST(test_glyph_constructor);
        CPPUNIT_TEST(test_glyph_attr_constructor);
        CPPUNIT_TEST(test_char_constructor);
        CPPUNIT_TEST(test_initializer_list_constructor);
        CPPUNIT_TEST(test_equality_operator);
        CPPUNIT_TEST(test_inequality_operator);
        CPPUNIT_TEST(test_assignment);
        CPPUNIT_TEST(test_glyph_assignment);
        CPPUNIT_TEST(test_char_assignment);
    CPPUNIT_TEST_SUITE_END();
    
public :
    void test_default_constructor();
    void test_glyph_constructor();
    void test_glyph_attr_constructor();
    void test_char_constructor();
    void test_initializer_list_constructor();
    void test_equality_operator();
    void test_inequality_operator();
    void test_assignment();
    void test_glyph_assignment();
    void test_char_assignment();
};

#endif

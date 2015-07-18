#ifndef YGGDRASIL_MUNIN_ATTRIBUTE_FIXTURE_HPP_
#define YGGDRASIL_MUNIN_ATTRIBUTE_FIXTURE_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class yggdrasil_munin_attribute_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(yggdrasil_munin_attribute_fixture);
        CPPUNIT_TEST(test_default_constructor);
        CPPUNIT_TEST(test_foreground_colour_constructor);
        CPPUNIT_TEST(test_all_colours_constructor);
        CPPUNIT_TEST(test_equality_operator);
        CPPUNIT_TEST(test_inequality_operator);
    CPPUNIT_TEST_SUITE_END();
    
public :
    void test_default_constructor();
    void test_foreground_colour_constructor();
    void test_all_colours_constructor();
    void test_equality_operator();
    void test_inequality_operator();
};

#endif

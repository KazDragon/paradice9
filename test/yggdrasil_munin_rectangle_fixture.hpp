#ifndef YGGDRASIL_MUNIN_RECTANGLE_FIXTURE_HPP_
#define YGGDRASIL_MUNIN_RECTANGLE_FIXTURE_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class yggdrasil_munin_rectangle_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(yggdrasil_munin_rectangle_fixture);
        CPPUNIT_TEST(test_default_constructor);
        CPPUNIT_TEST(test_origin_size_constructor);
        CPPUNIT_TEST(test_point_plus_extent_constructor);
        CPPUNIT_TEST(test_initializer_list_constructor);
        CPPUNIT_TEST(test_equality_operator);
        CPPUNIT_TEST(test_inequality_operator);
    CPPUNIT_TEST_SUITE_END();
    
public :
    void test_default_constructor();
    void test_origin_size_constructor();
    void test_point_plus_extent_constructor();
    void test_initializer_list_constructor();
    void test_equality_operator();
    void test_inequality_operator();
};

#endif

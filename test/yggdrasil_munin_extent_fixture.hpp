#ifndef YGGDRASIL_MUNIN_EXTENT_FIXTURE_HPP_
#define YGGDRASIL_MUNIN_EXTENT_FIXTURE_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class yggdrasil_munin_extent_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(yggdrasil_munin_extent_fixture);
        CPPUNIT_TEST(test_default_constructor);
        CPPUNIT_TEST(test_wh_constructor);
        CPPUNIT_TEST(test_initializer_list_constructor);
        CPPUNIT_TEST(test_increment_operator);
        CPPUNIT_TEST(test_decrement_operator);
        CPPUNIT_TEST(test_equality_operator);
        CPPUNIT_TEST(test_inequality_operator);
        CPPUNIT_TEST(test_operator_plus);
        CPPUNIT_TEST(test_operator_minus);
    CPPUNIT_TEST_SUITE_END();

public :
    void test_default_constructor();
    void test_wh_constructor();
    void test_initializer_list_constructor();
    void test_increment_operator();
    void test_decrement_operator();
    void test_equality_operator();
    void test_inequality_operator();
    void test_operator_plus();
    void test_operator_minus();
    
};

#endif

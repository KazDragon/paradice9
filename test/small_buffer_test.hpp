#ifndef ODIN_SMALL_BUFFER_FIXTURE_HPP_
#define ODIN_SMALL_BUFFER_FIXTURE_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class small_buffer_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(small_buffer_fixture);
        CPPUNIT_TEST(test_container_concept);
        CPPUNIT_TEST(test_default_constructor);
        CPPUNIT_TEST(test_size_constructor_short);
        CPPUNIT_TEST(test_size_constructor_long);
        CPPUNIT_TEST(test_static_array_constructor_short);
        CPPUNIT_TEST(test_static_array_constructor_long);
        CPPUNIT_TEST(test_sized_array_constructor_short);
        CPPUNIT_TEST(test_sized_array_constructor_long);
        CPPUNIT_TEST(test_assignment_via_array_operator_short);
        CPPUNIT_TEST(test_assignment_via_array_operator_long);
        CPPUNIT_TEST(test_equality_short);
        CPPUNIT_TEST(test_equality_long);
        CPPUNIT_TEST(test_inequality_short);
        CPPUNIT_TEST(test_inequality_long);
        CPPUNIT_TEST(test_less_than);
        CPPUNIT_TEST(test_copy_constructor_short);
        CPPUNIT_TEST(test_copy_constructor_long);
        CPPUNIT_TEST(test_swap_short_to_short);
        CPPUNIT_TEST(test_swap_long_to_long);
        CPPUNIT_TEST(test_swap_short_to_long);
        CPPUNIT_TEST(test_swap_long_to_short);
        CPPUNIT_TEST(test_assignment_short_to_short);
        CPPUNIT_TEST(test_assignment_long_to_short);
        CPPUNIT_TEST(test_assignment_short_to_long);
        CPPUNIT_TEST(test_assignment_long_to_long);
        CPPUNIT_TEST(test_stl_algorithm_short);
        CPPUNIT_TEST(test_stl_algorithm_long);
        CPPUNIT_TEST(test_access_out_of_bounds);
        CPPUNIT_TEST(test_non_pod);
    CPPUNIT_TEST_SUITE_END();

public :
    void test_container_concept();
    void test_default_constructor();
    void test_size_constructor_short();
    void test_size_constructor_long();
    void test_static_array_constructor_short();
    void test_static_array_constructor_long();
    void test_sized_array_constructor_short();
    void test_sized_array_constructor_long();
    void test_assignment_via_array_operator_short();
    void test_assignment_via_array_operator_long();
    void test_equality_short();
    void test_equality_long();
    void test_inequality_short();
    void test_inequality_long();
    void test_less_than();
    void test_copy_constructor_short();
    void test_copy_constructor_long();
    void test_swap_short_to_short();
    void test_swap_long_to_long();
    void test_swap_short_to_long();
    void test_swap_long_to_short();
    void test_assignment_short_to_short();
    void test_assignment_long_to_short();
    void test_assignment_short_to_long();
    void test_assignment_long_to_long();
    void test_stl_algorithm_short();
    void test_stl_algorithm_long();
    void test_access_out_of_bounds();
    void test_non_pod();
};

#endif

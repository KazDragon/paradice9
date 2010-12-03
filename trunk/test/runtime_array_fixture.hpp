#ifndef RUNTIME_ARRAY_FIXTURE_HPP_
#define RUNTIME_ARRAY_FIXTURE_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class runtime_array_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(runtime_array_fixture);
        CPPUNIT_TEST(test_container_concept);
        CPPUNIT_TEST(test_default_constructor);
        CPPUNIT_TEST(test_null_construction);
        CPPUNIT_TEST(test_sized_array_construction);
        CPPUNIT_TEST(test_size_construction);
        CPPUNIT_TEST(test_array_construction);
        CPPUNIT_TEST(test_construct_different_storage);
        CPPUNIT_TEST(test_carray);
        CPPUNIT_TEST(test_equality);
        CPPUNIT_TEST(test_inequality);
        CPPUNIT_TEST(test_copy_constructor);
        CPPUNIT_TEST(test_swap);
        CPPUNIT_TEST(test_assignment);
        CPPUNIT_TEST(test_concatenation);
        CPPUNIT_TEST(test_vector_storage);
        CPPUNIT_TEST(test_stream_out);
    CPPUNIT_TEST_SUITE_END();

public :
    void test_container_concept();
    void test_default_constructor();
    void test_null_construction();
    void test_sized_array_construction();
    void test_size_construction();
    void test_array_construction();
    void test_construct_different_storage();
    void test_carray();
    void test_equality();
    void test_inequality();
    void test_copy_constructor();
    void test_swap();
    void test_assignment();
    void test_concatenation();
    void test_vector_storage();
    void test_stream_out();
};

#endif

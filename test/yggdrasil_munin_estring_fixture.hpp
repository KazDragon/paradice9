#ifndef YGGDRASIL_MUNIN_ESTRING_FIXTURE_HPP_
#define YGGDRASIL_MUNIN_ESTRING_FIXTURE_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class yggdrasil_munin_estring_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(yggdrasil_munin_estring_fixture);
        CPPUNIT_TEST(test_container_concepts);
        CPPUNIT_TEST(test_default_constructor);
        CPPUNIT_TEST(test_charstar_constructor);
        CPPUNIT_TEST(test_string_constructor);
        CPPUNIT_TEST(test_literal_constructor);
        CPPUNIT_TEST(test_supports_parsed_strings);
        CPPUNIT_TEST(test_string_append_operator);
        CPPUNIT_TEST(test_string_concatenate_operator);
        CPPUNIT_TEST(test_string_assignment);
    CPPUNIT_TEST_SUITE_END();
    
public :
    void test_container_concepts();
    void test_default_constructor();
    void test_charstar_constructor();
    void test_string_constructor();
    void test_literal_constructor();
    void test_supports_parsed_strings();
    void test_string_append_operator();
    void test_string_concatenate_operator();
    void test_string_assignment();
};

#endif

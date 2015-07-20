#ifndef YGGDRASIL_MUNIN_CREATE_MODEL_FROM_PTREE_FIXTURE_HPP_
#define YGGDRASIL_MUNIN_CREATE_MODEL_FROM_PTREE_FIXTURE_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class yggdrasil_munin_create_model_from_ptree_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(yggdrasil_munin_create_model_from_ptree_fixture);
        CPPUNIT_TEST(test_empty_ptree_creates_default_model);
        CPPUNIT_TEST(test_ptree_with_only_name_creates_default_model);
        CPPUNIT_TEST(test_ptree_with_string_property);
        CPPUNIT_TEST(test_ptree_with_array_of_string_property);
    CPPUNIT_TEST_SUITE_END();
    
public :
    void test_empty_ptree_creates_default_model();
    void test_ptree_with_only_name_creates_default_model();
    void test_ptree_with_string_property();
    void test_ptree_with_array_of_string_property();
};

#endif

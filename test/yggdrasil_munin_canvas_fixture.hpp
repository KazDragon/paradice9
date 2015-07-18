#ifndef YGGDRASIL_MUNIN_CANVAS_FIXTURE_HPP_
#define YGGDRASIL_MUNIN_CANVAS_FIXTURE_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class yggdrasil_munin_canvas_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(yggdrasil_munin_canvas_fixture);
        CPPUNIT_TEST(test_default_constructor);
        CPPUNIT_TEST(test_size_constructor);
        CPPUNIT_TEST(test_set_size);
        CPPUNIT_TEST(test_brackets_operator);
        CPPUNIT_TEST(test_offset_by);
    CPPUNIT_TEST_SUITE_END();
    
public :
    void test_default_constructor();
    void test_size_constructor();
    void test_set_size();
    void test_brackets_operator();
    void test_offset_by();
};

#endif

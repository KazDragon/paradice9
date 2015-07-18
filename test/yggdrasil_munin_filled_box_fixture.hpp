#ifndef YGGDRASIL_MUNIN_FILLED_BOX_FIXTURE_HPP_
#define YGGDRASIL_MUNIN_FILLED_BOX_FIXTURE_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class yggdrasil_munin_filled_box_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(yggdrasil_munin_filled_box_fixture);
        CPPUNIT_TEST(test_default_constructor);
        CPPUNIT_TEST(test_element_constructor);
        CPPUNIT_TEST(test_filled_box_is_component);
        CPPUNIT_TEST(test_size);
        CPPUNIT_TEST(test_draw);
    CPPUNIT_TEST_SUITE_END();
    
public :
    void test_default_constructor();
    void test_element_constructor();
    void test_filled_box_is_component();
    void test_size();
    void test_draw();
};

#endif

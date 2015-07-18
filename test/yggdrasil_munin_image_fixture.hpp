#ifndef YGGDRASIL_MUNIN_IMAGE_FIXTURE_HPP_
#define YGGDRASIL_MUNIN_IMAGE_FIXTURE_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class yggdrasil_munin_image_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(yggdrasil_munin_image_fixture);
    CPPUNIT_TEST(test_default_constructor);
    CPPUNIT_TEST(test_vector_constructor);
    CPPUNIT_TEST(test_image_is_component);
    CPPUNIT_TEST(test_size);
    CPPUNIT_TEST(test_background_brush);
    CPPUNIT_TEST(test_set_image);
    CPPUNIT_TEST(test_draw);
    CPPUNIT_TEST_SUITE_END();
    
public :
    void test_default_constructor();
    void test_vector_constructor();
    void test_image_is_component();
    void test_size();
    void test_background_brush();
    void test_set_image();
    void test_draw();
};

#endif

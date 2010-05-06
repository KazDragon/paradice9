#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class munin_component_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(munin_component_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_position);
        CPPUNIT_TEST(test_size);
        CPPUNIT_TEST(test_preferred_size);
        CPPUNIT_TEST(test_draw);
        CPPUNIT_TEST(test_redraw);
        CPPUNIT_TEST(test_set_focus);
        CPPUNIT_TEST(test_set_focus_subobject);
        CPPUNIT_TEST(test_set_focus_cant_focus);
        CPPUNIT_TEST(test_lose_focus_subobject);
        CPPUNIT_TEST(test_focus_next);
        CPPUNIT_TEST(test_focus_previous);
        CPPUNIT_TEST(test_event);
        CPPUNIT_TEST(test_get_focussed_component);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_constructor();
    void test_position();
    void test_size();
    void test_preferred_size();
    void test_draw();
    void test_redraw();
    void test_set_focus();
    void test_set_focus_subobject();
    void test_set_focus_cant_focus();
    void test_lose_focus_subobject();
    void test_focus_next();
    void test_focus_previous();
    void test_event();
    void test_get_focussed_component();
};

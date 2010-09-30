#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class munin_card_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(munin_card_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_inheritance);
        CPPUNIT_TEST(test_add_face);
        CPPUNIT_TEST(test_draw_no_faces);
        CPPUNIT_TEST(test_draw_first_face);
        CPPUNIT_TEST(test_draw_second_face);
        CPPUNIT_TEST(test_select_face);
        CPPUNIT_TEST(test_can_focus);
        CPPUNIT_TEST(test_set_focus);
    CPPUNIT_TEST_SUITE_END();
    
private :
    void test_constructor();
    void test_inheritance();
    void test_add_face();
    void test_draw_no_faces();
    void test_draw_first_face();
    void test_draw_second_face();
    void test_select_face();
    void test_can_focus();
    void test_set_focus();
};


#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class hugin_text_area_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(hugin_text_area_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_inheritance);
        CPPUNIT_TEST(test_add_text);
        //CPPUNIT_TEST(test_text_text);
        /*
        CPPUNIT_TEST(test_draw);
        CPPUNIT_TEST(test_draw_with_offsets);
        */
    CPPUNIT_TEST_SUITE_END();
    
private :
    void test_constructor();
    void test_inheritance();
    void test_add_text();
    //void test_text_text();
    /*
    void test_draw();
    void test_draw_with_offsets();
    */
};


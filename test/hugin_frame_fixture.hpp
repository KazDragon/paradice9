#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class hugin_frame_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(hugin_frame_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_inheritance);
        CPPUNIT_TEST(test_draw);
        CPPUNIT_TEST(test_draw_with_offsets);
    CPPUNIT_TEST_SUITE_END();
    
private :
    void test_constructor();
    void test_inheritance();
    void test_draw();
    void test_draw_with_offsets();
};


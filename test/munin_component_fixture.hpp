#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class munin_component_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(munin_component_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_position);
        CPPUNIT_TEST(test_size);
        CPPUNIT_TEST(test_draw);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_constructor();
    void test_position();
    void test_size();
    void test_draw();
};

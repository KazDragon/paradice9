#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class munin_layout_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(munin_layout_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_add_component);
        CPPUNIT_TEST(test_remove_component);
        CPPUNIT_TEST(test_layout);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_constructor();
    void test_add_component();
    void test_remove_component();
    void test_layout();
};

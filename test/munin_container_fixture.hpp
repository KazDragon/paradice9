#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class munin_container_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(munin_container_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_inheritance);
        CPPUNIT_TEST(test_add_component);
        CPPUNIT_TEST(test_draw);
        CPPUNIT_TEST(test_contained_container);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_constructor();
    void test_inheritance();
    void test_add_component();
    void test_draw();
    void test_contained_container();
};

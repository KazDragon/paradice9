#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class munin_graphics_context_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(munin_graphics_context_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_array_notation);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_constructor();
    void test_array_notation();
};

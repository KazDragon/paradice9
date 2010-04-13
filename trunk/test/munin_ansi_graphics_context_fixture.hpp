#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class munin_ansi_graphics_context_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(munin_ansi_graphics_context_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_inheritance);
        CPPUNIT_TEST(test_size);
        CPPUNIT_TEST(test_array_indexing);
        CPPUNIT_TEST(test_array_resizing);
        CPPUNIT_TEST(test_out_of_range);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_constructor();
    void test_inheritance();
    void test_size();
    void test_array_indexing();
    void test_array_resizing();
    void test_out_of_range();
};

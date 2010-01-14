#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class buffered_datastream_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(buffered_datastream_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_inheritance);
        CPPUNIT_TEST(test_sync_read_empty);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_constructor();
    void test_inheritance();
    void test_sync_read_empty();
};

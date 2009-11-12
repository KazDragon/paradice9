#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class telnet_stream_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(telnet_stream_fixture);
        CPPUNIT_TEST(test_constructor);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_constructor();
};

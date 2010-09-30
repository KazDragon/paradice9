#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class telnet_options_suppress_goahead_server_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(telnet_options_suppress_goahead_server_fixture);
        CPPUNIT_TEST(test_identity);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_identity();
};

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class telnet_options_subnegotiationless_client_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(telnet_options_subnegotiationless_client_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_inheritance);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_constructor();
    void test_inheritance();
};

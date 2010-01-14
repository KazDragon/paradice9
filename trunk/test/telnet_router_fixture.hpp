#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class telnet_router_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(telnet_router_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_register_negotiation);
        CPPUNIT_TEST(test_unregister_negotiation);
        CPPUNIT_TEST(test_register_subnegotiation);
        CPPUNIT_TEST(test_unregister_subnegotiation);
        CPPUNIT_TEST(test_unrouted_negotiation);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_constructor();
    void test_register_negotiation();
    void test_unregister_negotiation();
    void test_register_subnegotiation();
    void test_unregister_subnegotiation();
    void test_unrouted_negotiation();
};

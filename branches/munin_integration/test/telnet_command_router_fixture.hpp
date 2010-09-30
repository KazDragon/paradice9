#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class telnet_command_router_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(telnet_command_router_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_register_route);
        CPPUNIT_TEST(test_unregister_route);
        CPPUNIT_TEST(test_unregistered_route);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_constructor();
    void test_register_route();
    void test_unregister_route();
    void test_unregistered_route();
};

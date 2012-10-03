#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class telnet_client_option_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(telnet_client_option_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_inactive_activate);
        CPPUNIT_TEST(test_inactive_activate_deny);
        CPPUNIT_TEST(test_inactive_activate_allow);
        CPPUNIT_TEST(test_inactive_deactivate);
        CPPUNIT_TEST(test_active_deactivate);
        CPPUNIT_TEST(test_active_deactivate_deny);
        CPPUNIT_TEST(test_active_deactivate_allow);
        
        CPPUNIT_TEST(test_non_activatable_inactive_activate);
        CPPUNIT_TEST(test_non_activatable_active_activate);
        CPPUNIT_TEST(test_non_activatable_inactive_deactivate);
        CPPUNIT_TEST(test_non_activatable_active_deactivate);
        
        CPPUNIT_TEST(test_activatable_inactive_activate);
        CPPUNIT_TEST(test_activatable_active_activate);
        CPPUNIT_TEST(test_activatable_inactive_deactivate);
        CPPUNIT_TEST(test_activatable_active_deactivate);
        
        CPPUNIT_TEST(test_inactive_subnegotiation);
        CPPUNIT_TEST(test_active_subnegotiation);
        
        CPPUNIT_TEST(test_send_subnegotiation);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_constructor();
    void test_inactive_activate();
    void test_inactive_activate_deny();
    void test_inactive_activate_allow();
    void test_inactive_deactivate();
    void test_active_deactivate();
    void test_active_deactivate_deny();
    void test_active_deactivate_allow();
    
    void test_non_activatable_inactive_activate();
    void test_non_activatable_active_activate();
    void test_non_activatable_inactive_deactivate();
    void test_non_activatable_active_deactivate();

    void test_activatable_inactive_activate();
    void test_activatable_active_activate();
    void test_activatable_inactive_deactivate();
    void test_activatable_active_deactivate();
    
    void test_inactive_subnegotiation();
    void test_active_subnegotiation();
    
    void test_send_subnegotiation();
};

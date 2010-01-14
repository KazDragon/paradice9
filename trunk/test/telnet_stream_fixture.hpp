#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class telnet_stream_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(telnet_stream_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_inheritance);
        CPPUNIT_TEST(test_sync_read_normal);
        CPPUNIT_TEST(test_async_read_normal);
        CPPUNIT_TEST(test_available_empty);
        CPPUNIT_TEST(test_available_normal);
        CPPUNIT_TEST(test_available_normal_iac);
        CPPUNIT_TEST(test_available_normal_iac_iac);
        CPPUNIT_TEST(test_available_normal_iac_iac_normal);
        CPPUNIT_TEST(test_available_negotiation);
        CPPUNIT_TEST(test_available_negotiation_normal);
        CPPUNIT_TEST(test_available_during_async_read);
        CPPUNIT_TEST(test_available_during_async_callback);
        CPPUNIT_TEST(test_sync_read_normal_iac);
        CPPUNIT_TEST(test_sync_read_normal_iac_iac);
        CPPUNIT_TEST(test_sync_read_normal_iac_iac_normal);
        CPPUNIT_TEST(test_sync_read_command);
        CPPUNIT_TEST(test_sync_read_command_normal);
        CPPUNIT_TEST(test_sync_read_negotiation);
        CPPUNIT_TEST(test_sync_read_negotiation_normal);
        CPPUNIT_TEST(test_sync_read_subnegotiation);
        CPPUNIT_TEST(test_sync_read_subnegotiation_normal);
        CPPUNIT_TEST(test_async_read_normal_iac);
        CPPUNIT_TEST(test_async_read_normal_iac_iac);
        CPPUNIT_TEST(test_async_read_normal_iac_iac_normal);
        CPPUNIT_TEST(test_async_read_command);
        CPPUNIT_TEST(test_async_read_command_normal);
        CPPUNIT_TEST(test_async_read_negotiation);
        CPPUNIT_TEST(test_async_read_negotiation_normal);
        CPPUNIT_TEST(test_async_read_subnegotiation);
        CPPUNIT_TEST(test_async_read_subnegotiation_normal);
        CPPUNIT_TEST(test_sync_write_normal);
        CPPUNIT_TEST(test_sync_write_normal_iac);
        CPPUNIT_TEST(test_async_write_normal);
        CPPUNIT_TEST(test_async_write_normal_iac);
        CPPUNIT_TEST(test_send_command);
        CPPUNIT_TEST(test_send_negotiation);
        CPPUNIT_TEST(test_send_subnegotiation);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_constructor();
    void test_inheritance();
    void test_sync_read_normal();
    void test_async_read_normal();
    void test_available_empty();
    void test_available_normal();
    void test_available_normal_iac();
    void test_available_normal_iac_iac();
    void test_available_normal_iac_iac_normal();
    void test_available_negotiation();
    void test_available_negotiation_normal();
    void test_available_during_async_read();
    void test_available_during_async_callback();
    void test_sync_read_normal_iac();
    void test_sync_read_normal_iac_iac();
    void test_sync_read_normal_iac_iac_normal();
    void test_sync_read_command();
    void test_sync_read_command_normal();
    void test_sync_read_negotiation();
    void test_sync_read_negotiation_normal();
    void test_sync_read_subnegotiation();
    void test_sync_read_subnegotiation_normal();
    void test_async_read_normal_iac();
    void test_async_read_normal_iac_iac();
    void test_async_read_normal_iac_iac_normal();
    void test_async_read_command();
    void test_async_read_command_normal();
    void test_async_read_negotiation();
    void test_async_read_negotiation_normal();
    void test_async_read_subnegotiation();
    void test_async_read_subnegotiation_normal();
    void test_sync_write_normal();
    void test_sync_write_normal_iac();
    void test_async_write_normal();
    void test_async_write_normal_iac();
    void test_send_command();
    void test_send_negotiation();
    void test_send_subnegotiation();
};

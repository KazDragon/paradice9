#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class telnet_stream_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(telnet_stream_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_inheritance);
        CPPUNIT_TEST(test_sync_read_normal);
        CPPUNIT_TEST(test_sync_read_normal_iac);
        CPPUNIT_TEST(test_sync_read_normal_iac_iac);
        CPPUNIT_TEST(test_sync_read_normal_iac_then_iac);
        CPPUNIT_TEST(test_sync_read_command);
        CPPUNIT_TEST(test_sync_read_normal_command);
        CPPUNIT_TEST(test_sync_read_partial);
        CPPUNIT_TEST(test_sync_read_many);
        CPPUNIT_TEST(test_async_read_normal);
        CPPUNIT_TEST(test_available_empty);
        CPPUNIT_TEST(test_available_iac);
        CPPUNIT_TEST(test_available_normal);
        CPPUNIT_TEST(test_available_many);
        CPPUNIT_TEST(test_available_during_async_read);
        CPPUNIT_TEST(test_available_during_async_callback);
        CPPUNIT_TEST(test_sync_write_nothing);
        CPPUNIT_TEST(test_sync_write_normal);
        CPPUNIT_TEST(test_sync_write_normal_iac);
        CPPUNIT_TEST(test_async_write_normal);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_constructor();
    void test_inheritance();
    void test_sync_read_normal();
    void test_sync_read_normal_iac();
    void test_sync_read_normal_iac_iac();
    void test_sync_read_normal_iac_then_iac();
    void test_sync_read_command();
    void test_sync_read_normal_command();
    void test_sync_read_partial();
    void test_sync_read_many();
    void test_async_read_normal();
    void test_available_empty();
    void test_available_iac();
    void test_available_normal();
    void test_available_many();
    void test_available_during_async_read();
    void test_available_during_async_callback();
    void test_sync_write_nothing();
    void test_sync_write_normal();
    void test_sync_write_normal_iac();
    void test_async_write_normal();
};

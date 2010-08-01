#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class buffered_datastream_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(buffered_datastream_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_inheritance);
        CPPUNIT_TEST(test_available_empty);
        CPPUNIT_TEST(test_available_one);
        CPPUNIT_TEST(test_available_many);
        CPPUNIT_TEST(test_sync_read_empty);
        CPPUNIT_TEST(test_sync_read_one_of_one);
        CPPUNIT_TEST(test_sync_read_one_of_one_available);
        CPPUNIT_TEST(test_sync_read_one_of_many);
        CPPUNIT_TEST(test_sync_read_one_of_many_available);
        CPPUNIT_TEST(test_sync_read_one_of_many_partially_available);
        
        CPPUNIT_TEST(test_sync_write);
        
        CPPUNIT_TEST(test_async_read_empty);
        CPPUNIT_TEST(test_async_read_one_of_one);
        CPPUNIT_TEST(test_async_read_one_of_one_null_callback);
        CPPUNIT_TEST(test_async_read_one_of_one_available);
        CPPUNIT_TEST(test_async_read_one_of_many);
        CPPUNIT_TEST(test_async_read_one_of_many_available);
        CPPUNIT_TEST(test_async_read_many_of_many_partially_available);
        
        CPPUNIT_TEST(test_consecutive_async_read);
        CPPUNIT_TEST(test_many_async_reads);
        
        CPPUNIT_TEST(test_available_during_async_read);
        CPPUNIT_TEST(test_available_during_async_callback);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_constructor();
    void test_inheritance();
    void test_available_empty();
    void test_available_one();
    void test_available_many();
    void test_sync_read_empty();
    void test_sync_read_one_of_one();
    void test_sync_read_one_of_one_available();
    void test_sync_read_one_of_many();
    void test_sync_read_one_of_many_available();
    void test_sync_read_one_of_many_partially_available();
    
    void test_sync_write();
    
    void test_async_read_empty();
    void test_async_read_one_of_one();
    void test_async_read_one_of_one_null_callback();
    void test_async_read_one_of_one_available();
    void test_async_read_one_of_many();
    void test_async_read_one_of_many_available();
    void test_async_read_many_of_many_partially_available();
    
    void test_consecutive_async_read();
    void test_many_async_reads();
    
    void test_available_during_async_read();
    void test_available_during_async_callback();
};

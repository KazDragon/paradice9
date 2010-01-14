#ifndef TELNET_COMPLETED_NEGOTIATION_FIXTURE_HPP_
#define TELNET_COMPLETED_NEGOTIATION_FIXTURE_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class telnet_completed_negotiation_fixture : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(telnet_completed_negotiation_fixture);
        CPPUNIT_TEST(test_initiator);
        CPPUNIT_TEST(test_local_request);
        CPPUNIT_TEST(test_remote_request);
        CPPUNIT_TEST(test_equality);
        CPPUNIT_TEST(test_type);
        CPPUNIT_TEST(test_assign);
        CPPUNIT_TEST(test_copy);
        CPPUNIT_TEST(test_complete);
    CPPUNIT_TEST_SUITE_END();
    
public :
    void test_initiator();
    void test_local_request();
    void test_remote_request();
    void test_equality();
    void test_type();
    void test_assign();
    void test_copy();
    void test_complete();
};

#endif

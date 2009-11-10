#ifndef TELNET_INITIATED_NEGOTIATION_TEST_HPP_
#define TELNET_INITIATED_NEGOTIATION_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class telnet_initiated_negotiation_test : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(telnet_initiated_negotiation_test);
        CPPUNIT_TEST(test_initiator);
        CPPUNIT_TEST(test_request);
        CPPUNIT_TEST(test_type);
        CPPUNIT_TEST(test_equality);
        CPPUNIT_TEST(test_assign);
        CPPUNIT_TEST(test_copy);
    CPPUNIT_TEST_SUITE_END();
    
public :
    void test_initiator();
    void test_request();
    void test_type();
    void test_equality();
    void test_assign();
    void test_copy();
};

#endif

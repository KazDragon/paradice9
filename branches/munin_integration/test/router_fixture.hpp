#ifndef ODIN_ROUTER_FIXTURE_HPP_
#define ODIN_ROUTER_FIXTURE_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class router_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(router_fixture);
        CPPUNIT_TEST(test_registered_route);
        CPPUNIT_TEST(test_unregister_route);
        CPPUNIT_TEST(test_unregistered_route);
        CPPUNIT_TEST(test_multiple_messages);
        CPPUNIT_TEST(test_unregistered_handler);
    CPPUNIT_TEST_SUITE_END();

public :
    void test_registered_route();
    void test_unregister_route();
    void test_unregistered_route();
    void test_multiple_messages();
    void test_unregistered_handler();
};

#endif

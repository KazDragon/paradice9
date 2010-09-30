#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class telnet_input_visitor_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(telnet_input_visitor_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_visit_command);
        CPPUNIT_TEST(test_visit_negotiation);
        CPPUNIT_TEST(test_visit_subnegotiation);
        CPPUNIT_TEST(test_visit_text);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_constructor();
    void test_visit_command();
    void test_visit_negotiation();
    void test_visit_subnegotiation();
    void test_visit_text();
};

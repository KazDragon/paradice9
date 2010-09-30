#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class telnet_parser_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(telnet_parser_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_parse_empty);
        CPPUNIT_TEST(test_parse_normal_one);
        CPPUNIT_TEST(test_parse_normal_two);
        CPPUNIT_TEST(test_parse_iac);
        CPPUNIT_TEST(test_parse_iac_iac);
        CPPUNIT_TEST(test_parse_command);
        CPPUNIT_TEST(test_parse_negotiation);
        CPPUNIT_TEST(test_parse_subnegotiation);
        CPPUNIT_TEST(test_parse_incomplete_subnegotiation);
        CPPUNIT_TEST(test_parse_many);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_constructor();
    void test_parse_empty();
    void test_parse_normal_one();
    void test_parse_normal_two();
    void test_parse_iac();
    void test_parse_iac_iac();
    void test_parse_command();
    void test_parse_negotiation();
    void test_parse_subnegotiation();
    void test_parse_incomplete_subnegotiation();
    void test_parse_many();
};

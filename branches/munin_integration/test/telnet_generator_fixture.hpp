#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class telnet_generator_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(telnet_generator_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_generate_empty);
        CPPUNIT_TEST(test_generate_normal_empty);
        CPPUNIT_TEST(test_generate_normal_one);
        CPPUNIT_TEST(test_generate_normal_two);
        CPPUNIT_TEST(test_generate_normal_iac);
        CPPUNIT_TEST(test_generate_command);
        CPPUNIT_TEST(test_generate_negotiation);
        CPPUNIT_TEST(test_generate_subnegotiation);
        CPPUNIT_TEST(test_generate_many);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_constructor();
    void test_generate_empty();
    void test_generate_normal_empty();
    void test_generate_normal_one();
    void test_generate_normal_two();
    void test_generate_normal_iac();
    void test_generate_command();
    void test_generate_negotiation();
    void test_generate_subnegotiation();
    void test_generate_many();
};

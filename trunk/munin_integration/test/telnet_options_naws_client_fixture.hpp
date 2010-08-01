#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class telnet_options_naws_client_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(telnet_options_naws_client_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_inheritance);
        CPPUNIT_TEST(test_on_size);
        CPPUNIT_TEST(test_callback_no_callback);
        CPPUNIT_TEST(test_callback_width);
        CPPUNIT_TEST(test_callback_height);
        CPPUNIT_TEST(test_subnegotiation_short);
        CPPUNIT_TEST(test_subnegotiation_long);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_constructor();
    void test_inheritance();
    void test_on_size();
    void test_callback_no_callback();
    void test_callback_width();
    void test_callback_height();
    void test_subnegotiation_short();
    void test_subnegotiation_long();
};

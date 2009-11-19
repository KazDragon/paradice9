#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class telnet_filter_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(telnet_filter_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_inheritance);
        CPPUNIT_TEST(test_filter_normal_char);
        CPPUNIT_TEST(test_filter_iac);
        CPPUNIT_TEST(test_filter_iac_iac);
        CPPUNIT_TEST(test_filter_iac_command);
        CPPUNIT_TEST(test_filter_iac_will);
        CPPUNIT_TEST(test_filter_iac_wont);
        CPPUNIT_TEST(test_filter_iac_do);
        CPPUNIT_TEST(test_filter_iac_dont);
        CPPUNIT_TEST(test_filter_iac_sb_empty);
        CPPUNIT_TEST(test_filter_iac_sb_full);
        CPPUNIT_TEST(test_filter_iac_sb_iac);
        CPPUNIT_TEST(test_filter_iac_sb_char);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_constructor();
    void test_inheritance();
    void test_filter_normal_char();
    void test_filter_iac();
    void test_filter_iac_iac();
    void test_filter_iac_command();
    void test_filter_iac_will();
    void test_filter_iac_wont();
    void test_filter_iac_do();
    void test_filter_iac_dont();
    void test_filter_iac_sb_empty();
    void test_filter_iac_sb_full();
    void test_filter_iac_sb_iac();
    void test_filter_iac_sb_char();
};

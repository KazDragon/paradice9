#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class ansi_parser_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(ansi_parser_fixture);
        CPPUNIT_TEST(test_constructor);
        CPPUNIT_TEST(test_plain_string);
        CPPUNIT_TEST(test_csi_no_argument);
        CPPUNIT_TEST(test_csi_one_argument);
        CPPUNIT_TEST(test_csi_two_arguments);
        CPPUNIT_TEST(test_multiple_tokens);
        CPPUNIT_TEST(test_csi_meta_no_argument);
        CPPUNIT_TEST(test_mouse_report);
    CPPUNIT_TEST_SUITE_END();
    
private :
    void test_constructor();
    void test_plain_string();
    void test_csi_no_argument();
    void test_csi_one_argument();
    void test_csi_two_arguments();
    void test_multiple_tokens();
    void test_csi_meta_no_argument();
    void test_mouse_report();
};


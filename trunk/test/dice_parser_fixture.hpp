#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class dice_parser_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(dice_parser_fixture);
        CPPUNIT_TEST(test_empty_string);
        CPPUNIT_TEST(test_roll_no_bonus);
        CPPUNIT_TEST(test_roll_with_bonus);
        CPPUNIT_TEST(test_roll_extended_bonus);
    CPPUNIT_TEST_SUITE_END();
    
private :
    void test_empty_string();
    void test_roll_no_bonus();
    void test_roll_with_bonus();
    void test_roll_extended_bonus();
};


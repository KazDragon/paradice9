#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class munin_algorithm_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(munin_algorithm_fixture);
        CPPUNIT_TEST(test_rectangle_intersection_same);
        CPPUNIT_TEST(test_rectangle_intersection_no_overlap);
        CPPUNIT_TEST(test_rectangle_intersection_overlap_top_left);
        CPPUNIT_TEST(test_rectangle_intersection_overlap_bottom_left);
        CPPUNIT_TEST(test_rectangle_intersection_overlap_bottom_right);
        CPPUNIT_TEST(test_rectangle_intersection_overlap_top_right);
        CPPUNIT_TEST(test_rectangle_intersection_enclose_top);
        CPPUNIT_TEST(test_rectangle_intersection_enclose_left);
        CPPUNIT_TEST(test_rectangle_intersection_enclose_bottom);
        CPPUNIT_TEST(test_rectangle_intersection_enclose_right);
        CPPUNIT_TEST(test_rectangle_intersection_contain);
        
        CPPUNIT_TEST(test_rectangular_slice);
    CPPUNIT_TEST_SUITE_END();

private :
    void test_rectangle_intersection_same();
    void test_rectangle_intersection_no_overlap();
    void test_rectangle_intersection_overlap_top_left();
    void test_rectangle_intersection_overlap_bottom_left();
    void test_rectangle_intersection_overlap_bottom_right();
    void test_rectangle_intersection_overlap_top_right();
    void test_rectangle_intersection_enclose_top();
    void test_rectangle_intersection_enclose_left();
    void test_rectangle_intersection_enclose_bottom();
    void test_rectangle_intersection_enclose_right();
    void test_rectangle_intersection_contain();
    
    void test_rectangular_slice();
};

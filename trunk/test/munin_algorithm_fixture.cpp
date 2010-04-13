#include "munin_algorithm_fixture.hpp"
#include "munin/algorithm.hpp"

using namespace boost;
using namespace odin;

CPPUNIT_TEST_SUITE_REGISTRATION(munin_algorithm_fixture);

void munin_algorithm_fixture::test_rectangle_intersection_same()
{
    // Test that the overlapping area of two similar rectangles is the same
    // as the area of the overlapping rectangles.
    munin::rectangle lhs;
    lhs.origin.x    = 1;
    lhs.origin.y    = 2;
    lhs.size.width  = 3;
    lhs.size.height = 4;
    
    munin::rectangle rhs = lhs;
    
    optional<munin::rectangle> intersection = munin::intersection(lhs, rhs);
    CPPUNIT_ASSERT_EQUAL(true, intersection.is_initialized());
    
    CPPUNIT_ASSERT_EQUAL(u32(1), intersection->origin.x);
    CPPUNIT_ASSERT_EQUAL(u32(2), intersection->origin.y);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->size.width);
    CPPUNIT_ASSERT_EQUAL(u32(4), intersection->size.height);
}

void munin_algorithm_fixture::test_rectangle_intersection_no_overlap()
{
    // Test that the intersection of two rectangles that do overlap is
    // returned as not-a-rectangle.
    munin::rectangle lhs;
    lhs.origin.x    = 0;
    lhs.origin.y    = 0;
    lhs.size.width  = 1;
    lhs.size.height = 1;
    
    munin::rectangle rhs;
    rhs.origin.x    = 1;
    rhs.origin.y    = 1;
    rhs.size.width  = 1;
    rhs.size.height = 1;
    
    optional<munin::rectangle> intersection = munin::intersection(lhs, rhs);
    CPPUNIT_ASSERT_EQUAL(false, intersection.is_initialized());
}

void munin_algorithm_fixture::test_rectangle_intersection_overlap_top_left()
{
    // Test that, if the second rectangle overlaps the top left of the first
    // rectangle, that overlapping region is returned.
    //
    //  0123456789       0123456789
    // 0+-----+         0
    // 1| R +----+      1    +-+
    // 2|   | L  |  ->  2    | |
    // 3+---|    |      3    +-+
    // 4    +----+      4
    //
    munin::rectangle lhs;
    lhs.origin.x    = 4;
    lhs.origin.y    = 1;
    lhs.size.width  = 6;
    lhs.size.height = 4;
    
    munin::rectangle rhs;
    rhs.origin.x    = 0;
    rhs.origin.y    = 0;
    rhs.size.width  = 7;
    rhs.size.height = 4;
    
    optional<munin::rectangle> intersection = munin::intersection(lhs, rhs);
    CPPUNIT_ASSERT_EQUAL(true, intersection.is_initialized());
    
    CPPUNIT_ASSERT_EQUAL(u32(4), intersection->origin.x);
    CPPUNIT_ASSERT_EQUAL(u32(1), intersection->origin.y);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->size.width);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->size.height);
}

void munin_algorithm_fixture::test_rectangle_intersection_overlap_bottom_left()
{
    // Test that, if the second rectangle overlaps the bottom left of the first
    // rectangle, that overlapping region is returned.
    //
    //  0123456789       0123456789
    // 0    +----+      0
    // 1    | L  |      1
    // 2+---|    |  ->  2    +-+
    // 3| R +----+      3    +-+
    // 4+-----+         4    
    munin::rectangle lhs;
    lhs.origin.x    = 4;
    lhs.origin.y    = 0;
    lhs.size.width  = 5;
    lhs.size.height = 4;
    
    munin::rectangle rhs;
    rhs.origin.x    = 0;
    rhs.origin.y    = 2;
    rhs.size.width  = 7;
    rhs.size.height = 3;
    
    optional<munin::rectangle> intersection = munin::intersection(lhs, rhs);
    CPPUNIT_ASSERT_EQUAL(true, intersection.is_initialized());
    
    CPPUNIT_ASSERT_EQUAL(u32(4), intersection->origin.x);
    CPPUNIT_ASSERT_EQUAL(u32(2), intersection->origin.y);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->size.width);
    CPPUNIT_ASSERT_EQUAL(u32(2), intersection->size.height);
}

void munin_algorithm_fixture::test_rectangle_intersection_overlap_bottom_right()
{
    // Test that, if the second rectangle overlaps the bottom right of the 
    // first rectangle, that overlapping region is returned.
    //
    //  0123456789       0123456789
    // 0                0
    // 1 +----+         1
    // 2 | L  |--+      2   +--+
    // 3 |    |  |  ->  3   |  |
    // 4 +----+  |      4   +--+
    // 5   |   R |      5
    // 6   +-----+      6
    
    munin::rectangle lhs;
    lhs.origin.x    = 1;
    lhs.origin.y    = 1;
    lhs.size.width  = 6;
    lhs.size.height = 4;
    
    munin::rectangle rhs;
    rhs.origin.x    = 3;
    rhs.origin.y    = 2;
    rhs.size.width  = 7;
    rhs.size.height = 5;
    
    optional<munin::rectangle> intersection = munin::intersection(lhs, rhs);
    CPPUNIT_ASSERT_EQUAL(true, intersection.is_initialized());
    
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->origin.x);
    CPPUNIT_ASSERT_EQUAL(u32(2), intersection->origin.y);
    CPPUNIT_ASSERT_EQUAL(u32(4), intersection->size.width);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->size.height);
}

void munin_algorithm_fixture::test_rectangle_intersection_overlap_top_right()
{
    // Test that, if the second rectangle overlaps the top right of the 
    // first rectangle, that overlapping region is returned.
    //
    //  0123456789       0123456789
    // 0                0
    // 1    +----+      1
    // 2    |  R |      2
    // 3  +----+ |  ->  3    +--+
    // 4  | L  | |      4    |  |
    // 5  |    |-+      5    +--+
    // 6  +----+        6
    
    munin::rectangle lhs;
    lhs.origin.x    = 2;
    lhs.origin.y    = 3;
    lhs.size.width  = 6;
    lhs.size.height = 4;
    
    munin::rectangle rhs;
    rhs.origin.x    = 4;
    rhs.origin.y    = 1;
    rhs.size.width  = 6;
    rhs.size.height = 5;
    
    optional<munin::rectangle> intersection = munin::intersection(lhs, rhs);
    CPPUNIT_ASSERT_EQUAL(true, intersection.is_initialized());
    
    CPPUNIT_ASSERT_EQUAL(u32(4), intersection->origin.x);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->origin.y);
    CPPUNIT_ASSERT_EQUAL(u32(4), intersection->size.width);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->size.height);
}

void munin_algorithm_fixture::test_rectangle_intersection_enclose_top()
{
    // Test that, if the second rectangle encloses the top of the 
    // first rectangle, that overlapping region is returned.
    //
    //  0123456789       0123456789
    // 0                0
    // 1 +------+       1
    // 2 |   R  |       2
    // 3 | +--+ |   ->  3   +--+
    // 4 | | L| |       4   |  |
    // 5 +-|  |-+       5   +--+
    // 6   +--+         6
    
    munin::rectangle lhs;
    lhs.origin.x    = 3;
    lhs.origin.y    = 3;
    lhs.size.width  = 4;
    lhs.size.height = 4;
    
    munin::rectangle rhs;
    rhs.origin.x    = 1;
    rhs.origin.y    = 1;
    rhs.size.width  = 8;
    rhs.size.height = 5;
    
    optional<munin::rectangle> intersection = munin::intersection(lhs, rhs);
    CPPUNIT_ASSERT_EQUAL(true, intersection.is_initialized());
    
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->origin.x);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->origin.y);
    CPPUNIT_ASSERT_EQUAL(u32(4), intersection->size.width);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->size.height);
    
    // Ensure that this works for the opposite case too, where the first
    // rectangle encloses the second.
    intersection = munin::intersection(rhs, lhs);
    CPPUNIT_ASSERT_EQUAL(true, intersection.is_initialized());
    
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->origin.x);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->origin.y);
    CPPUNIT_ASSERT_EQUAL(u32(4), intersection->size.width);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->size.height);
}

void munin_algorithm_fixture::test_rectangle_intersection_enclose_left()
{
    // Test that, if the second rectangle encloses the left of the 
    // first rectangle, that overlapping region is returned.
    //
    //  0123456789       0123456789
    // 0                0
    // 1 +----+         1
    // 2 | R  |         2
    // 3 | +-----+      3   +--+
    // 4 | | L   |  ->  4   |  |
    // 5 | +-----+      5   +--+
    // 6 |    |         6
    // 7 +----+         7
    
    munin::rectangle lhs;
    lhs.origin.x    = 3;
    lhs.origin.y    = 3;
    lhs.size.width  = 7;
    lhs.size.height = 3;
    
    munin::rectangle rhs;
    rhs.origin.x    = 1;
    rhs.origin.y    = 1;
    rhs.size.width  = 6;
    rhs.size.height = 7;
    
    optional<munin::rectangle> intersection = munin::intersection(lhs, rhs);
    CPPUNIT_ASSERT_EQUAL(true, intersection.is_initialized());
    
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->origin.x);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->origin.y);
    CPPUNIT_ASSERT_EQUAL(u32(4), intersection->size.width);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->size.height);
    
    // Ensure that this works for the opposite case too, where the first
    // rectangle encloses the second.
    intersection = munin::intersection(rhs, lhs);
    CPPUNIT_ASSERT_EQUAL(true, intersection.is_initialized());
    
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->origin.x);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->origin.y);
    CPPUNIT_ASSERT_EQUAL(u32(4), intersection->size.width);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->size.height);
}

void munin_algorithm_fixture::test_rectangle_intersection_enclose_bottom()
{
    // Test that, if the second rectangle encloses the bottom of the 
    // first rectangle, that overlapping region is returned.
    //
    //  0123456789       0123456789
    // 0                0
    // 1  +---+         1
    // 2  |L  |         2
    // 3+-|   |--+  ->  3  +---+
    // 4| +---+ R|      4  +---+
    // 5+--------+      5
    
    munin::rectangle lhs;
    lhs.origin.x    = 0;
    lhs.origin.y    = 3;
    lhs.size.width  = 10;
    lhs.size.height = 3;
    
    munin::rectangle rhs;
    rhs.origin.x    = 2;
    rhs.origin.y    = 1;
    rhs.size.width  = 5;
    rhs.size.height = 4;
    
    optional<munin::rectangle> intersection = munin::intersection(lhs, rhs);
    CPPUNIT_ASSERT_EQUAL(true, intersection.is_initialized());
    
    CPPUNIT_ASSERT_EQUAL(u32(2), intersection->origin.x);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->origin.y);
    CPPUNIT_ASSERT_EQUAL(u32(5), intersection->size.width);
    CPPUNIT_ASSERT_EQUAL(u32(2), intersection->size.height);
    
    // Ensure that this works for the opposite case too, where the first
    // rectangle encloses the second.
    intersection = munin::intersection(rhs, lhs);
    CPPUNIT_ASSERT_EQUAL(true, intersection.is_initialized());
    
    CPPUNIT_ASSERT_EQUAL(u32(2), intersection->origin.x);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->origin.y);
    CPPUNIT_ASSERT_EQUAL(u32(5), intersection->size.width);
    CPPUNIT_ASSERT_EQUAL(u32(2), intersection->size.height);
}

void munin_algorithm_fixture::test_rectangle_intersection_enclose_right()
{
    // Test that, if the second rectangle encloses the right of the 
    // first rectangle, that overlapping region is returned.
    //
    //  0123456789       0123456789
    // 0                0
    // 1     +---+      1
    // 2  +----+ |      2     +-+ 
    // 3  | L  | |  ->  3     | |
    // 4  +----+ |      4     +-+
    // 5     | R |      5
    // 6     +---+      6
    
    munin::rectangle lhs;
    lhs.origin.x    = 5;
    lhs.origin.y    = 1;
    lhs.size.width  = 5;
    lhs.size.height = 6;
    
    munin::rectangle rhs;
    rhs.origin.x    = 2;
    rhs.origin.y    = 2;
    rhs.size.width  = 6;
    rhs.size.height = 3;
    
    optional<munin::rectangle> intersection = munin::intersection(lhs, rhs);
    CPPUNIT_ASSERT_EQUAL(true, intersection.is_initialized());
    
    CPPUNIT_ASSERT_EQUAL(u32(5), intersection->origin.x);
    CPPUNIT_ASSERT_EQUAL(u32(2), intersection->origin.y);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->size.width);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->size.height);
    
    // Ensure that this works for the opposite case too, where the first
    // rectangle encloses the second.
    intersection = munin::intersection(rhs, lhs);
    CPPUNIT_ASSERT_EQUAL(true, intersection.is_initialized());
    
    CPPUNIT_ASSERT_EQUAL(u32(5), intersection->origin.x);
    CPPUNIT_ASSERT_EQUAL(u32(2), intersection->origin.y);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->size.width);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->size.height);
}

void munin_algorithm_fixture::test_rectangle_intersection_contain()
{
    // Test that, if the second rectangle contains the first entirely, 
    // that overlapping region is returned.
    //
    //  0123456789       0123456789
    // 0                0
    // 1 +------+       1
    // 2 | +--+ |       2   +--+
    // 3 | |L |R|   ->  3   |  |
    // 4 | +--+ |       4   +--+
    // 5 +------+       5
    
    munin::rectangle lhs;
    lhs.origin.x    = 3;
    lhs.origin.y    = 2;
    lhs.size.width  = 4;
    lhs.size.height = 3;
    
    munin::rectangle rhs;
    rhs.origin.x    = 1;
    rhs.origin.y    = 1;
    rhs.size.width  = 8;
    rhs.size.height = 5;
    
    optional<munin::rectangle> intersection = munin::intersection(lhs, rhs);
    CPPUNIT_ASSERT_EQUAL(true, intersection.is_initialized());
    
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->origin.x);
    CPPUNIT_ASSERT_EQUAL(u32(2), intersection->origin.y);
    CPPUNIT_ASSERT_EQUAL(u32(4), intersection->size.width);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->size.height);
    
    // Ensure that this works for the opposite case too, where the first
    // rectangle encloses the second.
    intersection = munin::intersection(rhs, lhs);
    CPPUNIT_ASSERT_EQUAL(true, intersection.is_initialized());
    
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->origin.x);
    CPPUNIT_ASSERT_EQUAL(u32(2), intersection->origin.y);
    CPPUNIT_ASSERT_EQUAL(u32(4), intersection->size.width);
    CPPUNIT_ASSERT_EQUAL(u32(3), intersection->size.height);
}

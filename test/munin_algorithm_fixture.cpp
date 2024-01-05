#include "munin/algorithm.hpp"
#include <gtest/gtest.h>

TEST(munin_algorithm, test_rectangle_intersection_same)
{
    // Test that the overlapping area of two similar rectangles is the same
    // as the area of the overlapping rectangles.
    terminalpp::rectangle lhs;
    lhs.origin.x    = 1;
    lhs.origin.y    = 2;
    lhs.size.width  = 3;
    lhs.size.height = 4;
    
    terminalpp::rectangle rhs = lhs;
    
    auto intersection = munin::intersection(lhs, rhs);
    ASSERT_TRUE(intersection.is_initialized());
    
    ASSERT_EQ(1, intersection->origin.x);
    ASSERT_EQ(2, intersection->origin.y);
    ASSERT_EQ(3, intersection->size.width);
    ASSERT_EQ(4, intersection->size.height);
}

TEST(munin_algorithm, test_rectangle_intersection_no_overlap)
{
    // Test that the intersection of two rectangles that do overlap is
    // returned as not-a-rectangle.
    terminalpp::rectangle lhs;
    lhs.origin.x    = 0;
    lhs.origin.y    = 0;
    lhs.size.width  = 1;
    lhs.size.height = 1;
    
    terminalpp::rectangle rhs;
    rhs.origin.x    = 1;
    rhs.origin.y    = 1;
    rhs.size.width  = 1;
    rhs.size.height = 1;
    
    auto intersection = munin::intersection(lhs, rhs);
    ASSERT_FALSE(intersection.is_initialized());
}

TEST(munin_algorithm, test_rectangle_intersection_overlap_top_left)
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
    terminalpp::rectangle lhs;
    lhs.origin.x    = 4;
    lhs.origin.y    = 1;
    lhs.size.width  = 6;
    lhs.size.height = 4;
    
    terminalpp::rectangle rhs;
    rhs.origin.x    = 0;
    rhs.origin.y    = 0;
    rhs.size.width  = 7;
    rhs.size.height = 4;
    
    auto intersection = munin::intersection(lhs, rhs);
    ASSERT_TRUE(intersection.is_initialized());
    
    ASSERT_EQ(4, intersection->origin.x);
    ASSERT_EQ(1, intersection->origin.y);
    ASSERT_EQ(3, intersection->size.width);
    ASSERT_EQ(3, intersection->size.height);
}

TEST(munin_algorithm, test_rectangle_intersection_overlap_bottom_left)
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
    terminalpp::rectangle lhs;
    lhs.origin.x    = 4;
    lhs.origin.y    = 0;
    lhs.size.width  = 5;
    lhs.size.height = 4;
    
    terminalpp::rectangle rhs;
    rhs.origin.x    = 0;
    rhs.origin.y    = 2;
    rhs.size.width  = 7;
    rhs.size.height = 3;
    
    auto intersection = munin::intersection(lhs, rhs);
    ASSERT_TRUE(intersection.is_initialized());
    
    ASSERT_EQ(4, intersection->origin.x);
    ASSERT_EQ(2, intersection->origin.y);
    ASSERT_EQ(3, intersection->size.width);
    ASSERT_EQ(2, intersection->size.height);
}

TEST(munin_algorithm, test_rectangle_intersection_overlap_bottom_right)
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
    
    terminalpp::rectangle lhs;
    lhs.origin.x    = 1;
    lhs.origin.y    = 1;
    lhs.size.width  = 6;
    lhs.size.height = 4;
    
    terminalpp::rectangle rhs;
    rhs.origin.x    = 3;
    rhs.origin.y    = 2;
    rhs.size.width  = 7;
    rhs.size.height = 5;
    
    auto intersection = munin::intersection(lhs, rhs);
    ASSERT_TRUE(intersection.is_initialized());
    
    ASSERT_EQ(3, intersection->origin.x);
    ASSERT_EQ(2, intersection->origin.y);
    ASSERT_EQ(4, intersection->size.width);
    ASSERT_EQ(3, intersection->size.height);
}

TEST(munin_algorithm, test_rectangle_intersection_overlap_top_right)
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
    
    terminalpp::rectangle lhs;
    lhs.origin.x    = 2;
    lhs.origin.y    = 3;
    lhs.size.width  = 6;
    lhs.size.height = 4;
    
    terminalpp::rectangle rhs;
    rhs.origin.x    = 4;
    rhs.origin.y    = 1;
    rhs.size.width  = 6;
    rhs.size.height = 5;
    
    auto intersection = munin::intersection(lhs, rhs);
    ASSERT_TRUE(intersection.is_initialized());
    
    ASSERT_EQ(4, intersection->origin.x);
    ASSERT_EQ(3, intersection->origin.y);
    ASSERT_EQ(4, intersection->size.width);
    ASSERT_EQ(3, intersection->size.height);
}

TEST(munin_algorithm, test_rectangle_intersection_enclose_top)
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
    
    terminalpp::rectangle lhs;
    lhs.origin.x    = 3;
    lhs.origin.y    = 3;
    lhs.size.width  = 4;
    lhs.size.height = 4;
    
    terminalpp::rectangle rhs;
    rhs.origin.x    = 1;
    rhs.origin.y    = 1;
    rhs.size.width  = 8;
    rhs.size.height = 5;
    
    auto intersection = munin::intersection(lhs, rhs);
    ASSERT_TRUE(intersection.is_initialized());
    
    ASSERT_EQ(3, intersection->origin.x);
    ASSERT_EQ(3, intersection->origin.y);
    ASSERT_EQ(4, intersection->size.width);
    ASSERT_EQ(3, intersection->size.height);
    
    // Ensure that this works for the opposite case too, where the first
    // rectangle encloses the second.
    intersection = munin::intersection(rhs, lhs);
    ASSERT_TRUE(intersection.is_initialized());
    
    ASSERT_EQ(3, intersection->origin.x);
    ASSERT_EQ(3, intersection->origin.y);
    ASSERT_EQ(4, intersection->size.width);
    ASSERT_EQ(3, intersection->size.height);
}

TEST(munin_algorithm, test_rectangle_intersection_enclose_left)
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
    
    terminalpp::rectangle lhs;
    lhs.origin.x    = 3;
    lhs.origin.y    = 3;
    lhs.size.width  = 7;
    lhs.size.height = 3;
    
    terminalpp::rectangle rhs;
    rhs.origin.x    = 1;
    rhs.origin.y    = 1;
    rhs.size.width  = 6;
    rhs.size.height = 7;
    
    auto intersection = munin::intersection(lhs, rhs);
    ASSERT_TRUE(intersection.is_initialized());
    
    ASSERT_EQ(3, intersection->origin.x);
    ASSERT_EQ(3, intersection->origin.y);
    ASSERT_EQ(4, intersection->size.width);
    ASSERT_EQ(3, intersection->size.height);
    
    // Ensure that this works for the opposite case too, where the first
    // rectangle encloses the second.
    intersection = munin::intersection(rhs, lhs);
    ASSERT_TRUE(intersection.is_initialized());
    
    ASSERT_EQ(3, intersection->origin.x);
    ASSERT_EQ(3, intersection->origin.y);
    ASSERT_EQ(4, intersection->size.width);
    ASSERT_EQ(3, intersection->size.height);
}

TEST(munin_algorithm, test_rectangle_intersection_enclose_bottom)
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
    
    terminalpp::rectangle lhs;
    lhs.origin.x    = 0;
    lhs.origin.y    = 3;
    lhs.size.width  = 10;
    lhs.size.height = 3;
    
    terminalpp::rectangle rhs;
    rhs.origin.x    = 2;
    rhs.origin.y    = 1;
    rhs.size.width  = 5;
    rhs.size.height = 4;
    
    auto intersection = munin::intersection(lhs, rhs);
    ASSERT_TRUE(intersection.is_initialized());
    
    ASSERT_EQ(2, intersection->origin.x);
    ASSERT_EQ(3, intersection->origin.y);
    ASSERT_EQ(5, intersection->size.width);
    ASSERT_EQ(2, intersection->size.height);
    
    // Ensure that this works for the opposite case too, where the first
    // rectangle encloses the second.
    intersection = munin::intersection(rhs, lhs);
    ASSERT_TRUE(intersection.is_initialized());
    
    ASSERT_EQ(2, intersection->origin.x);
    ASSERT_EQ(3, intersection->origin.y);
    ASSERT_EQ(5, intersection->size.width);
    ASSERT_EQ(2, intersection->size.height);
}

TEST(munin_algorithm, test_rectangle_intersection_enclose_right)
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
    
    terminalpp::rectangle lhs;
    lhs.origin.x    = 5;
    lhs.origin.y    = 1;
    lhs.size.width  = 5;
    lhs.size.height = 6;
    
    terminalpp::rectangle rhs;
    rhs.origin.x    = 2;
    rhs.origin.y    = 2;
    rhs.size.width  = 6;
    rhs.size.height = 3;
    
    auto intersection = munin::intersection(lhs, rhs);
    ASSERT_TRUE(intersection.is_initialized());
    
    ASSERT_EQ(5, intersection->origin.x);
    ASSERT_EQ(2, intersection->origin.y);
    ASSERT_EQ(3, intersection->size.width);
    ASSERT_EQ(3, intersection->size.height);
    
    // Ensure that this works for the opposite case too, where the first
    // rectangle encloses the second.
    intersection = munin::intersection(rhs, lhs);
    ASSERT_TRUE(intersection.is_initialized());
    
    ASSERT_EQ(5, intersection->origin.x);
    ASSERT_EQ(2, intersection->origin.y);
    ASSERT_EQ(3, intersection->size.width);
    ASSERT_EQ(3, intersection->size.height);
}

TEST(munin_algorithm, test_rectangle_intersection_contain)
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
    
    terminalpp::rectangle lhs;
    lhs.origin.x    = 3;
    lhs.origin.y    = 2;
    lhs.size.width  = 4;
    lhs.size.height = 3;
    
    terminalpp::rectangle rhs;
    rhs.origin.x    = 1;
    rhs.origin.y    = 1;
    rhs.size.width  = 8;
    rhs.size.height = 5;
    
    auto intersection = munin::intersection(lhs, rhs);
    ASSERT_TRUE(intersection.is_initialized());
    
    ASSERT_EQ(3, intersection->origin.x);
    ASSERT_EQ(2, intersection->origin.y);
    ASSERT_EQ(4, intersection->size.width);
    ASSERT_EQ(3, intersection->size.height);
    
    // Ensure that this works for the opposite case too, where the first
    // rectangle encloses the second.
    intersection = munin::intersection(rhs, lhs);
    ASSERT_TRUE(intersection.is_initialized());
    
    ASSERT_EQ(3, intersection->origin.x);
    ASSERT_EQ(2, intersection->origin.y);
    ASSERT_EQ(4, intersection->size.width);
    ASSERT_EQ(3, intersection->size.height);
}

TEST(munin_algorithm, test_rectangular_slice)
{
    using terminalpp::point;
    using terminalpp::extent;
    using terminalpp::rectangle;
    
    {
        // Test the identity case.  The following should be returned unchanged.
        std::vector<terminalpp::rectangle> rectangles;
        rectangles.push_back(rectangle(point(0, 0), extent(1, 1)));
        rectangles.push_back(rectangle(point(1, 1), extent(1, 1)));
        rectangles.push_back(rectangle(point(2, 2), extent(1, 1)));
        
        std::vector<terminalpp::rectangle> result = munin::rectangular_slice(rectangles);
            
        ASSERT_EQ(rectangles.size(), result.size());
        
        for (size_t index = 0; index < rectangles.size(); ++index)
        {
            ASSERT_EQ(rectangles[index], result[index]);
        } 
    }
    
    {
        // Test the simple case.  One rectangle of two lines should be sliced
        // into two rectangles of one line.
        std::vector<terminalpp::rectangle> rectangles;
        rectangles.push_back(rectangle(point(0,0), extent(1,2)));
        
        std::vector<terminalpp::rectangle> expected_rectangles;
        expected_rectangles.push_back(rectangle(point(0,0), extent(1,1)));
        expected_rectangles.push_back(rectangle(point(0,1), extent(1,1)));

        std::vector<terminalpp::rectangle> result = munin::rectangular_slice(rectangles);
        
        ASSERT_EQ(expected_rectangles.size(), result.size());
        
        for (size_t index = 0; index < expected_rectangles.size(); ++index)
        {
            ASSERT_EQ(expected_rectangles[index], result[index]);
        } 
    }
    
    {
        // Test the adjacent case.
        std::vector<terminalpp::rectangle> rectangles;
        rectangles.push_back(rectangle(point(0,0), extent(1,1)));
        rectangles.push_back(rectangle(point(1,0), extent(1,1)));

        std::vector<terminalpp::rectangle> expected_rectangles;
        expected_rectangles.push_back(rectangle(point(0,0), extent(2,1)));

        std::vector<terminalpp::rectangle> result = munin::rectangular_slice(rectangles);
        
        ASSERT_EQ(expected_rectangles.size(), result.size());
        
        for (size_t index = 0; index < expected_rectangles.size(); ++index)
        {
            ASSERT_EQ(expected_rectangles[index], result[index]);
        } 
    }

    {
        // Test the overlap case.  After being sliced, rectangles should be 
        // merged if they occupy common space.
        
        //
        //  +---+         +---+               
        //  |   |         |   |          
        //  | +-+-+       +---+-+            
        //  | |   |   ->  |     |               
        //  +-+   |       +-+---+          
        //    |   |         |   |               
        //    +---+         +---+
        
        std::vector<terminalpp::rectangle> rectangles;
        rectangles.push_back(rectangle(point(0,0), extent(2,2)));
        rectangles.push_back(rectangle(point(1,1), extent(2,2)));
        
        std::vector<terminalpp::rectangle> expected_rectangles;
        expected_rectangles.push_back(rectangle(point(0,0), extent(2,1)));
        expected_rectangles.push_back(rectangle(point(0,1), extent(3,1)));
        expected_rectangles.push_back(rectangle(point(1,2), extent(2,1)));
        
        std::vector<terminalpp::rectangle> result = munin::rectangular_slice(rectangles);
        
        ASSERT_EQ(expected_rectangles.size(), result.size());
        
        for (size_t index = 0; index < expected_rectangles.size(); ++index)
        {
            ASSERT_EQ(expected_rectangles[index], result[index]);
        } 
    }
    
    {
        // Test the unsorted case.  This is the same as before, except the
        // order of the rectangles is in reverse.  The result should be the
        // same.

        std::vector<terminalpp::rectangle> rectangles;
        rectangles.push_back(rectangle(point(1,1), extent(2,2)));
        rectangles.push_back(rectangle(point(0,0), extent(2,2)));
        
        std::vector<terminalpp::rectangle> expected_rectangles;
        expected_rectangles.push_back(rectangle(point(0,0), extent(2,1)));
        expected_rectangles.push_back(rectangle(point(0,1), extent(3,1)));
        expected_rectangles.push_back(rectangle(point(1,2), extent(2,1)));
        
        std::vector<terminalpp::rectangle> result = munin::rectangular_slice(rectangles);
        
        ASSERT_EQ(expected_rectangles.size(), result.size());
        
        for (size_t index = 0; index < expected_rectangles.size(); ++index)
        {
            ASSERT_EQ(expected_rectangles[index], result[index]);
        } 
    }
}



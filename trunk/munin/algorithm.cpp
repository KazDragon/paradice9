// ==========================================================================
// Munin Algorithm.
//
// Copyright (C) 2010 Matthew Chaplain, All Rights Reserved.
//
// Permission to reproduce, distribute, perform, display, and to prepare
// derivitive works from this file under the following conditions:
//
// 1. Any copy, reproduction or derivitive work of any part of this file 
//    contains this copyright notice and licence in its entirety.
//
// 2. The rights granted to you under this license automatically terminate
//    should you attempt to assert any patent claims against the licensor 
//    or contributors, which in any way restrict the ability of any party 
//    from using this software or portions thereof in any form under the
//    terms of this license.
//
// Disclaimer: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
//             KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
//             WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
//             PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
//             OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
//             OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
//             OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//             SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
// ==========================================================================
#include "munin/algorithm.hpp"
#include "munin/canvas.hpp"
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>
#include <algorithm>

using namespace odin;
using namespace boost;
using namespace std;

namespace munin {
    
// ==========================================================================
// INTERSECTION
// ==========================================================================
optional<rectangle> intersection(rectangle const &lhs, rectangle const &rhs)
{
    // Check to see if the rectangles overlap.
    
    // Calculate the rectangle with the leftmost origin, and its counterpart.
    // Note: the rectangle that is not the leftmost is not necessarily the
    // rightmost, since the leftmost rectangle may extend further than its
    // counterpart.
    rectangle const &leftmost     = lhs.origin.x < rhs.origin.x ? lhs : rhs;
    rectangle const &not_leftmost = lhs.origin.x < rhs.origin.x ? rhs : lhs;
    
    // Calculate the rectangle with the topmost origin.
    rectangle const &topmost        = lhs.origin.y < rhs.origin.y ? lhs : rhs;
    rectangle const &not_topmost    = lhs.origin.y < rhs.origin.y ? rhs : lhs;
    
    bool overlaps = 
        not_leftmost.origin.x >= leftmost.origin.x
     && not_leftmost.origin.x <  leftmost.origin.x + leftmost.size.width
     && not_topmost.origin.y  >= topmost.origin.y
     && not_topmost.origin.y  <  topmost.origin.y + topmost.size.height;
     
    if (!overlaps)
    {
        // There is no overlapping rectangle.
        return optional<rectangle>();
    }

    // The overlapping rectangle has an origin that starts at the same x-
    // co-ordinate as the not-leftmost rectangle, and at the same y-co-ordinate
    // as the not-topmost rectangle.
    rectangle overlap;
    overlap.origin.x = not_leftmost.origin.x;
    overlap.origin.y = not_topmost.origin.y;
    
    // If the leftmost rectangle completely contains the other rectangle,
    // then the width is simply that of the enclosed rectangle.
    if (leftmost.origin.x + leftmost.size.width
      > not_leftmost.origin.x + not_leftmost.size.width)
    {
        overlap.size.width = not_leftmost.size.width;
    }
    // Otherwise, the calculated width is the width of the leftmost rectangle
    // minus the difference between the origins of the two rectangles.
    else
    {
        overlap.size.width =
            leftmost.size.width - (not_leftmost.origin.x - leftmost.origin.x); 
    }
    
    // Same as above, but with height instead of width.
    if (topmost.origin.y + topmost.size.height
      > not_topmost.origin.y + not_topmost.size.height)
    {
        overlap.size.height = not_topmost.size.height;
    }
    else
    {
        overlap.size.height =
            topmost.size.height - (not_topmost.origin.y - topmost.origin.y);
    }
    
    return overlap;
}

// ==========================================================================
// CUT_SLICES
// ==========================================================================
static vector<rectangle> cut_slices(vector<rectangle> const &rectangles)
{
    vector<rectangle> slices;
    
    BOOST_FOREACH(rectangle const &current_rectangle, rectangles)
    {
        for (odin::s32 row = 0; row < current_rectangle.size.height; ++row)
        {
            slices.push_back(
                rectangle(point(current_rectangle.origin.x
                              , current_rectangle.origin.y + row)
                        , extent(current_rectangle.size.width
                               , 1))); 
        }
    }
    
    return slices;
}

// ==========================================================================
// MERGE_OVERLAPPING_SLICES
// ==========================================================================
static vector<rectangle> merge_overlapping_slices(vector<rectangle> rectangles)
{
    BOOST_AUTO(first_slice, rectangles.begin());
    
    while (first_slice != rectangles.end())
    {
        BOOST_AUTO(second_slice, first_slice + 1);
        
        while (second_slice != rectangles.end())
        {
            if (first_slice->origin.y == second_slice->origin.y)
            {
                rectangle const &leftmost = 
                    first_slice->origin.x < second_slice->origin.x
                  ? *first_slice
                  : *second_slice;
                  
                rectangle const &not_leftmost =
                    first_slice->origin.x < second_slice->origin.x
                  ? *second_slice
                  : *first_slice;
                  
                if (leftmost.origin.x + leftmost.size.width
                   >= not_leftmost.origin.x)
                {
                    odin::s32 left = 
                        (min)(first_slice->origin.x, second_slice->origin.x);
                    odin::s32 right =
                        (max)(first_slice->origin.x + first_slice->size.width
                            , second_slice->origin.x + second_slice->size.width);
                        
                    first_slice->origin.x   = left;
                    first_slice->size.width = right - left;
    
                    second_slice = rectangles.erase(second_slice);
                }
                else
                {
                    ++second_slice;
                }
            }
            else
            {
                ++second_slice;
            }
        }
        
        ++first_slice;
    }
    
    return rectangles;
}

// ==========================================================================
// COMPARE_SLICES
// ==========================================================================
static bool compare_slices(rectangle const &lhs, rectangle const &rhs)
{
    if (lhs.origin.y == rhs.origin.y)
    {
        return lhs.origin.x < rhs.origin.x;
    }
    else
    {
        return lhs.origin.y < rhs.origin.y;
    }
}


// ==========================================================================
// RECTANGULAR_SLICE
// ==========================================================================
vector<rectangle> rectangular_slice(vector<rectangle> const &rectangles)
{
    vector<rectangle> slices = merge_overlapping_slices(cut_slices(rectangles));
    sort(slices.begin(), slices.end(), &compare_slices);
    
    return slices;
}

// ==========================================================================
// CLIP_REGION
// ==========================================================================
static munin::rectangle clip_region(
    munin::rectangle region, munin::extent size)
{
    if (region.origin.x < 0)
    {
        size.width += region.origin.x;
        region.origin.x = 0;
    }

    if (region.origin.y < 0)
    {
        size.height += region.origin.y;
        region.origin.y = 0;
    }
   
    if (region.origin.x >= size.width)
    {
        region.size.width = 0;
    }
    
    if (region.origin.y >= size.height)
    {
        region.size.height = 0;
    }

    if (region.origin.x < size.width
     && region.origin.y < size.height)
    {
        if (region.origin.x + region.size.width >= size.width)
        {
            region.size.width = size.width - region.origin.x;
        }

        if (region.origin.y + region.size.height >= size.height)
        {
            region.size.height = size.height - region.origin.y;
        }
    }

    return region;
}

// ==========================================================================
// HAS_ZERO_DIMENSION
// ==========================================================================
static bool has_zero_dimension(munin::rectangle const &region)
{
    return region.size.width == 0 || region.size.height == 0;
}

// ==========================================================================
// CLIP_REGIONS
// ==========================================================================
vector<rectangle> clip_regions(vector<rectangle> regions, extent size)
{
    // Returns a vector of rectangles that is identical to the regions
    // passed in, except that their extends are clipped to those of the
    // content's.
    transform(
        regions.begin()
      , regions.end()
      , regions.begin()
      , bind(clip_region, _1, size));

    return regions;
}

// ==========================================================================
// PRUNE_REGIONS
// ==========================================================================
vector<rectangle> prune_regions(vector<rectangle> regions)
{
    regions.erase(
        remove_if(
            regions.begin()
          , regions.end()
          , bind(has_zero_dimension, _1))
      , regions.end());
    
    return regions;
}

// ==========================================================================
// COPY_REGION
// ==========================================================================
void copy_region(
    rectangle const &region
  , canvas const    &source
  , canvas          &destination)
{
    for (s32 y_coord = region.origin.y; 
         y_coord < region.origin.y + region.size.height;
         ++y_coord)
    {
        for (s32 x_coord = region.origin.x;
             x_coord < region.origin.x + region.size.width;
             ++x_coord)
        {
            destination[x_coord][y_coord] = 
                source[x_coord][y_coord];
        }
    }
}

}

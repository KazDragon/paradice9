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
#include "munin/detail/string_to_elements_parser.hpp"
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
// HAS_EMPTY_HEIGHT
// ==========================================================================
static bool has_empty_height(rectangle const &rect)
{
    return rect.size.height == 0;
}

// ==========================================================================
// MERGE_OVERLAPPING_SLICES
// ==========================================================================
static vector<rectangle> merge_overlapping_slices(vector<rectangle> rectangles)
{
    sort(rectangles.begin(), rectangles.end(), compare_slices);

    BOOST_AUTO(first_slice, rectangles.begin());
    
    // Iterate through adjacent slices, merging any that overlap.
    for (; first_slice != rectangles.end(); ++first_slice)
    {
        // Skip over any slices that have been merged.
        if (first_slice->size.height == 0)
        {
            continue;
        }

        BOOST_AUTO(second_slice, first_slice + 1);

        // Iterate through all adjacent slices that share a y-coordinate
        // until we either run out of slices, or cannot merge a slice.
        for (; 
             second_slice != rectangles.end()
          && first_slice->origin.y == second_slice->origin.y
          && first_slice->origin.x + first_slice->size.width >= second_slice->origin.x;
             ++second_slice)
        {
            // Set the width of the first slice to be equivalent to the
            // rightmost point of the two rectangles.
            first_slice->size.width = (std::max)(
                first_slice->origin.x + first_slice->size.width
              , second_slice->origin.x + second_slice->size.width)
              - first_slice->origin.x;

            // Mark the second slice as having been merged.
            second_slice->size.height = 0;
        }
    }

    // Snip out any rectangles that have been merged (have 0 height).
    rectangles.erase(remove_if(
            rectangles.begin()
          , rectangles.end()
          , has_empty_height)
      , rectangles.end());
    
    return rectangles;
}

// ==========================================================================
// RECTANGULAR_SLICE
// ==========================================================================
vector<rectangle> rectangular_slice(vector<rectangle> const &rectangles)
{
    return merge_overlapping_slices(cut_slices(rectangles));
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
            destination[x_coord][y_coord] = source[x_coord][y_coord];
        }
    }
}

// ==========================================================================
// DIRECTIVE_VISITOR
// ==========================================================================
class directive_visitor
    : public static_visitor<>
{
public :
    // ======================================================================
    // GET_ELEMENTS
    // ======================================================================
    vector<element_type> get_elements()
    {
        return elements_;
    }
    
    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(char ch)
    {
        current_glyph_.character_ = ch;
        elements_.push_back(
            element_type(current_glyph_, current_attribute_));
    }

    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(munin::detail::default_directive const &directive)
    {
        current_glyph_     = glyph();
        current_attribute_ = attribute();
    }

    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(munin::detail::character_set_directive const &directive)
    {
        current_glyph_.character_set_ = directive.character_set_;
    }

    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(munin::detail::locale_directive const &directive)
    {
        current_glyph_.locale_ = directive.locale_;
    }

    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(munin::detail::intensity_directive const &directive)
    {
        current_attribute_.intensity_ = directive.intensity_;
    }

    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(munin::detail::polarity_directive const &directive)
    {
        current_attribute_.polarity_ = directive.polarity_;
    }

    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(munin::detail::underlining_directive const &directive)
    {
        current_attribute_.underlining_ = directive.underlining_;
    }

    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(munin::detail::foreground_colour_directive const &directive)
    {
        current_attribute_.foreground_colour_ = directive.foreground_colour_;
    }

    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(munin::detail::background_colour_directive const &directive)
    {
        current_attribute_.background_colour_ = directive.background_colour_;
    }

private :
    vector<element_type> elements_;
    glyph                current_glyph_;
    attribute            current_attribute_;
};

// ==========================================================================
// STRING_TO_ELEMENTS
// ==========================================================================
vector<element_type> string_to_elements(string const &str)
{
    static munin::detail::string_to_elements_parser
    <
        string::const_iterator
    > parser;

    vector<munin::detail::string_to_elements_directive> parsed_result;
    BOOST_AUTO(parse_begin, str.begin());
    BOOST_AUTO(parse_end, str.end());
    boost::spirit::qi::parse(parse_begin, parse_end, parser, parsed_result);

    directive_visitor visitor;
    BOOST_FOREACH(
        munin::detail::string_to_elements_directive const &directive
      , parsed_result)
    {
        apply_visitor(visitor, directive);
    }
    
    return visitor.get_elements();
}

// ==========================================================================
// STRINGS_TO_ELEMENTS
// ==========================================================================
vector< vector<element_type> > strings_to_elements(
    vector<string> const &strings)
{
    vector< vector<element_type> > result;
    BOOST_FOREACH(string const &str, strings)
    {
        result.push_back(string_to_elements(str));
    }
    
    return result;
}

}


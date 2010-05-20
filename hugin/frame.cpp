// ==========================================================================
// Hugin Frame.
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
#include "hugin/frame.hpp"
#include "munin/ansi/ansi_canvas.hpp"
#include "munin/algorithm.hpp"
#include <boost/typeof/typeof.hpp>

using namespace munin;
using namespace odin;

namespace hugin { 

// ==========================================================================
// FRAME IMPLEMENTATION STRUCTURE
// ==========================================================================
struct frame::impl
{
public :
    impl(frame &self)
        : self_(self)
    {
    }
    
    // ======================================================================
    // DRAW
    // ======================================================================
    void draw(
        canvas<element_type> &cvs
      , point const          &offset
      , rectangle const      &region)
    {
        draw_corners(cvs, offset, region);
        draw_horizontal_borders(cvs, offset, region);
        draw_vertical_borders(cvs, offset, region);
    }
    
    // ======================================================================
    // DRAW_CORNERS
    // ======================================================================
    void draw_corners(
        canvas<element_type> &cvs
      , point const          &offset
      , rectangle const      &region)
    {
        point  position = self_.get_position();
        extent size     = self_.get_size();
        
        // Define the co-ordinates of the corners.
        rectangle topleft_rectangle(
            point(0, 0)
          , extent(1, 1));
        rectangle topright_rectangle(
            point(size.width - 1, 0)
          , extent(1, 1));
        rectangle bottomleft_rectangle(
            point(0, size.height - 1)
          , extent(1, 1));
        rectangle bottomright_rectangle(
            point(size.width - 1, size.height - 1)
          , extent(1, 1));
        
        // Find out if they intersect with the draw region.
        BOOST_AUTO(
            topleft, munin::intersection(topleft_rectangle, region));
        BOOST_AUTO(
            topright, munin::intersection(topright_rectangle, region));
        BOOST_AUTO(
            bottomleft, munin::intersection(bottomleft_rectangle, region));
        BOOST_AUTO(
            bottomright, munin::intersection(bottomright_rectangle, region));
        
        // If so, paint them.
        if (topleft)
        {
            cvs[position.x + topleft->origin.x + offset.x]
               [position.y + topleft->origin.y + offset.y] = 
                munin::ansi::element_type(
                    '+'
                  , munin::ansi::attribute());
        }
        
        if (topright)
        {
            cvs[position.x + topright->origin.x + offset.x]
               [position.y + topright->origin.y + offset.y] = 
                munin::ansi::element_type(
                    '+'
                  , munin::ansi::attribute());
        }

        if (bottomleft)
        {
            cvs[position.x + bottomleft->origin.x + offset.x]
               [position.y + bottomleft->origin.y + offset.y] = 
                munin::ansi::element_type(
                    '+'
                  , munin::ansi::attribute());
        }
        
        if (bottomright)
        {
            cvs[position.x + bottomright->origin.x + offset.x]
               [position.y + bottomright->origin.y + offset.y] = 
                munin::ansi::element_type(
                    '+'
                  , munin::ansi::attribute());
        }
    }

    // ======================================================================
    // DRAW_HORIZONTAL_BORDERS
    // ======================================================================
    void draw_horizontal_borders(
        canvas<element_type> &cvs
      , point const          &offset
      , rectangle const      &region)
    {
        point  position = self_.get_position();
        extent size     = self_.get_size();
        
        // Define a rectangle that stretches across the top border.
        rectangle top_border_rectangle(
            point(1,0)
          , extent(size.width - 2, 1));
        rectangle bottom_border_rectangle(
            point(1, size.height - 1)
          , extent(size.width - 2, 1));
        
        // Find out if this intersects the draw region.
        BOOST_AUTO(
            top_border, munin::intersection(top_border_rectangle, region));
        BOOST_AUTO(
            bottom_border
          , munin::intersection(bottom_border_rectangle, region));
        
        // If so, paint it.
        if (top_border)
        {
            rectangle &rect = top_border.get();
            
            for (s32 column = 0; column < rect.size.width; ++column)
            {
                cvs[position.x + column + rect.origin.x + offset.x]
                   [position.y +          rect.origin.y + offset.y] =
                    munin::ansi::element_type(
                        '-'
                      , munin::ansi::attribute());
            }
        }
        
        if (bottom_border)
        {
            rectangle &rect = bottom_border.get();
            
            for (s32 column = 0; column < rect.size.width; ++column)
            {
                cvs[position.x + column + rect.origin.x + offset.x]
                   [position.y +          rect.origin.y + offset.y] =
                    munin::ansi::element_type(
                        '-'
                      , munin::ansi::attribute());
            }
        }
    }

    // ======================================================================
    // DRAW_VERTICAL_BORDERS
    // ======================================================================
    void draw_vertical_borders(
        canvas<element_type> &cvs
      , point const          &offset
      , rectangle const      &region)
    {
        point  position = self_.get_position();
        extent size     = self_.get_size();
        
        // Define rectangles that stretches across the left and right borders.
        rectangle left_border_rectangle(
            point(0, 1)
          , extent(1, size.height - 2));
        rectangle right_border_rectangle(
            point(size.width - 1, 1)
          , extent(1, size.height - 2));
        
        // Find out if they intersect the draw region.
        BOOST_AUTO(
            left_border, munin::intersection(left_border_rectangle, region));
        BOOST_AUTO(
            right_border, munin::intersection(right_border_rectangle, region));
        
        // If so, paint them.
        if (left_border)
        {
            rectangle &rect = left_border.get();
            
            for (s32 row = 0; row < rect.size.height; ++row)
            {
                cvs[position.x +       rect.origin.x + offset.x]
                   [position.y + row + rect.origin.y + offset.y] =
                    munin::ansi::element_type(
                        '|'
                      , munin::ansi::attribute());
            }
        }

        if (right_border)
        {
            rectangle &rect = right_border.get();
            
            for (s32 row = 0; row < rect.size.height; ++row)
            {
                cvs[position.x +       rect.origin.x + offset.x]
                   [position.y + row + rect.origin.y + offset.y] =
                    munin::ansi::element_type(
                        '|'
                      , munin::ansi::attribute());
            }
        }
    }
    
private :
    frame &self_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
frame::frame()
{
    pimpl_.reset(new impl(*this));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
frame::~frame()
{
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
extent frame::do_get_preferred_size() const
{
    return extent(0,0);
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void frame::do_draw(
    canvas<element_type> &cvs
  , point const          &offset
  , rectangle const      &region)
{
    pimpl_->draw(cvs, offset, region);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void frame::do_event(boost::any const &event)
{
}

}



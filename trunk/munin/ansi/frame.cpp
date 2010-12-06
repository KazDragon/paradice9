// ==========================================================================
// Munin Frame.
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
#include "munin/ansi/frame.hpp"
#include "munin/ansi/ansi_canvas.hpp"
#include "munin/algorithm.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/typeof/typeof.hpp>

using namespace munin;
using namespace odin;
using namespace boost;
using namespace std;

namespace munin { namespace ansi {

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
        BOOST_AUTO(position, self_.get_position());
        BOOST_AUTO(size,     self_.get_size());
        
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
        
        BOOST_AUTO(pen, pen_);
        pen.character_set = odin::ansi::character_set::CHARACTER_SET_G0;
        pen.locale        = odin::ansi::character_set::LOCALE_SCO;
        
        // If so, paint them.
        if (topleft)
        {
            cvs[position.x + topleft->origin.x + offset.x]
               [position.y + topleft->origin.y + offset.y] = 
                munin::ansi::element_type(char(201), pen);
        }
        
        if (topright)
        {
            cvs[position.x + topright->origin.x + offset.x]
               [position.y + topright->origin.y + offset.y] = 
                munin::ansi::element_type(char(187), pen);
        }

        if (bottomleft)
        {
            cvs[position.x + bottomleft->origin.x + offset.x]
               [position.y + bottomleft->origin.y + offset.y] = 
                munin::ansi::element_type(char(200), pen);
        }
        
        if (bottomright)
        {
            cvs[position.x + bottomright->origin.x + offset.x]
               [position.y + bottomright->origin.y + offset.y] = 
                munin::ansi::element_type(char(188), pen);
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
        BOOST_AUTO(position, self_.get_position());
        BOOST_AUTO(size,     self_.get_size());
        
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
        
        BOOST_AUTO(pen, pen_);
        pen.character_set = odin::ansi::character_set::CHARACTER_SET_G0;
        pen.locale        = odin::ansi::character_set::LOCALE_SCO;
        
        // If so, paint it.
        if (top_border)
        {
            BOOST_AUTO(rect, top_border.get());
            
            for (s32 column = 0; column < rect.size.width; ++column)
            {
                cvs[position.x + column + rect.origin.x + offset.x]
                   [position.y +          rect.origin.y + offset.y] =
                    munin::ansi::element_type(char(205), pen);
            }
        }
        
        if (bottom_border)
        {
            BOOST_AUTO(rect, bottom_border.get());
            
            for (s32 column = 0; column < rect.size.width; ++column)
            {
                cvs[position.x + column + rect.origin.x + offset.x]
                   [position.y +          rect.origin.y + offset.y] =
                    munin::ansi::element_type(char(205), pen);
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
        BOOST_AUTO(position, self_.get_position());
        BOOST_AUTO(size,     self_.get_size());
        
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
        
        BOOST_AUTO(pen, pen_);
        pen.character_set = odin::ansi::character_set::CHARACTER_SET_G0;
        pen.locale        = odin::ansi::character_set::LOCALE_SCO;
        
        // If so, paint them.
        if (left_border)
        {
            BOOST_AUTO(rect, left_border.get());
            
            for (s32 row = 0; row < rect.size.height; ++row)
            {
                cvs[position.x +       rect.origin.x + offset.x]
                   [position.y + row + rect.origin.y + offset.y] =
                    munin::ansi::element_type(char(186), pen);
            }
        }

        if (right_border)
        {
            BOOST_AUTO(rect, right_border.get());
            
            for (s32 row = 0; row < rect.size.height; ++row)
            {
                cvs[position.x +       rect.origin.x + offset.x]
                   [position.y + row + rect.origin.y + offset.y] =
                    munin::ansi::element_type(char(186), pen);
            }
        }
    }
    
    void set_pen(attribute const &pen)
    {
        pen_ = pen;
    }
    
private :
    frame     &self_;
    attribute  pen_;
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
// DO_GET_FRAME_WIDTH
// ==========================================================================
u32 frame::do_get_frame_width() const
{
    return 1;
}

// ==========================================================================
// DO_GET_FRAME_HEIGHT
// ==========================================================================
u32 frame::do_get_frame_height() const
{
    return 1;
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
extent frame::do_get_preferred_size() const
{
    return extent(
        get_frame_width() * 2
      , get_frame_height() * 2);
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

// ==========================================================================
// DO_SET_ATTRIBUTE
// ==========================================================================
void frame::do_set_attribute(string const &name, any const &attr)
{
    munin::frame<munin::ansi::element_type>::do_set_attribute(name, attr);
    
    if (name == ATTRIBUTE_PEN)
    {
        attribute const *pen = any_cast<attribute>(&attr);
        
        if (pen != NULL)
        {
            pimpl_->set_pen(*pen);
            
            BOOST_AUTO(size, get_size());

            vector<rectangle> regions;

            // Upper border
            regions.push_back(rectangle(
                point()
              , extent(size.width, get_frame_height())));

            // Lower border
            regions.push_back(rectangle(
                point(0, size.height - get_frame_height())
              , extent(size.width, get_frame_height())));

            // Left border
            regions.push_back(rectangle(
                point()
              , extent(get_frame_width(), size.height)));

            // Right border
            regions.push_back(rectangle(
                point(size.width - get_frame_width(), 0)
              , extent(get_frame_width(), size.height)));

            on_redraw(regions);
        }
    }
}

}}


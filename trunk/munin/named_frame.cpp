// ==========================================================================
// Munin Named Frame.
//
// Copyright (C) 2011 Matthew Chaplain, All Rights Reserved.
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
#include "munin/named_frame.hpp"
#include "munin/canvas.hpp"
#include "munin/algorithm.hpp"
#include "munin/ansi/protocol.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/typeof/typeof.hpp>

using namespace munin::ansi;
using namespace odin;
using namespace boost;
using namespace std;

namespace munin {

// ==========================================================================
// FRAME IMPLEMENTATION STRUCTURE
// ==========================================================================
struct named_frame::impl
{
public :
    impl(named_frame &self)
        : self_(self)
    {
    }
    
    // ======================================================================
    // SET_NAME
    // ======================================================================
    void set_name(string const &name)
    {
        name_ = elements_from_string(name);
    }
    
    // ======================================================================
    // DRAW
    // ======================================================================
    void draw(
        canvas          &cvs
      , rectangle const &region)
    {
        draw_corners(cvs, region);
        draw_horizontal_borders(cvs, region);
        draw_vertical_borders(cvs, region);
        draw_name(cvs, region);
    }
    
    // ======================================================================
    // DRAW_NAME
    // ======================================================================
    void draw_name(
        canvas          &cvs
      , rectangle const &region)
    {
        // In order for there to be a name drawn, there must be a certain
        // amount of width available.  That is:
        //     border width * 2     
        //   + 2 (for a small amount of top border either side)
        //   + 2 (for a small amount of blank space either side)
        BOOST_AUTO(width, self_.get_size().width);
        BOOST_AUTO(frame_width, self_.get_frame_width());
        BOOST_AUTO(
            minimum_width
          , (frame_width * 2) + 2 + 2);
        
        if (width <= s32(minimum_width))
        {
            return;
        }
        
        // Find out if the redraw region overlaps the name. 
        BOOST_AUTO(
            opt_overlap
          , intersection(
                region
              , rectangle(point(0, 0), extent(width, 1))));
        
        if (opt_overlap == NULL)
        {
            return;
        }
        
        BOOST_AUTO(overlap, opt_overlap.get());

        if (overlap.size.width == 0 || overlap.size.height == 0)
        {
            return;
        }
        
        // Find out how much text space we have.
        BOOST_AUTO(text_space, width - minimum_width);
        
        // Find out how much text we're actually going to draw
        BOOST_AUTO(
            letters
          , (min)(text_space, u32(name_.size())));
        
        for (u32 letter = 0; letter < letters; ++letter)
        {
            s32 x_coord = frame_width + 2 + letter;
                        
            if (x_coord >= (overlap.origin.x + overlap.size.width))
            {
                break;
            }
            
            cvs[x_coord][0] = name_[letter];
        }
        
        // Finally, if there was a name, then draw the blank spaces to either
        // side.
        if (letters != 0)
        {
            BOOST_AUTO(left_blank_pos, s32(frame_width + 1));
            
            if (overlap.origin.x <= left_blank_pos
             && overlap.origin.x + overlap.size.width >= left_blank_pos)
            {
                cvs[left_blank_pos][0] = element_type(' ', attribute());
            }
            
            BOOST_AUTO(right_blank_pos, s32(frame_width + 1 + letters + 1));
            
            if (overlap.origin.x <= right_blank_pos
             && overlap.origin.x + overlap.size.width >= right_blank_pos)
            {
                cvs[right_blank_pos][0] = element_type(' ', attribute());
            }
        }
    }
    
    // ======================================================================
    // DRAW_CORNERS
    // ======================================================================
    void draw_corners(
        canvas          &cvs
      , rectangle const &region)
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
            cvs[position.x + topleft->origin.x]
               [position.y + topleft->origin.y] = 
                munin::element_type(char(201), pen);
        }
        
        if (topright)
        {
            cvs[position.x + topright->origin.x]
               [position.y + topright->origin.y] = 
                munin::element_type(char(187), pen);
        }

        if (bottomleft)
        {
            cvs[position.x + bottomleft->origin.x]
               [position.y + bottomleft->origin.y] = 
                munin::element_type(char(200), pen);
        }
        
        if (bottomright)
        {
            cvs[position.x + bottomright->origin.x]
               [position.y + bottomright->origin.y] = 
                munin::element_type(char(188), pen);
        }
    }

    // ======================================================================
    // DRAW_HORIZONTAL_BORDERS
    // ======================================================================
    void draw_horizontal_borders(
        canvas          &cvs
      , rectangle const &region)
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
                cvs[position.x + column + rect.origin.x]
                   [position.y +          rect.origin.y] =
                    munin::element_type(char(205), pen);
            }
        }
        
        if (bottom_border)
        {
            BOOST_AUTO(rect, bottom_border.get());
            
            for (s32 column = 0; column < rect.size.width; ++column)
            {
                cvs[position.x + column + rect.origin.x]
                   [position.y +          rect.origin.y] =
                    munin::element_type(char(205), pen);
            }
        }
    }

    // ======================================================================
    // DRAW_VERTICAL_BORDERS
    // ======================================================================
    void draw_vertical_borders(
        canvas          &cvs
      , rectangle const &region)
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
                cvs[position.x +       rect.origin.x]
                   [position.y + row + rect.origin.y] =
                    munin::element_type(char(186), pen);
            }
        }

        if (right_border)
        {
            BOOST_AUTO(rect, right_border.get());
            
            for (s32 row = 0; row < rect.size.height; ++row)
            {
                cvs[position.x +       rect.origin.x]
                   [position.y + row + rect.origin.y] =
                    munin::element_type(char(186), pen);
            }
        }
    }
    
    void set_pen(attribute const &pen)
    {
        pen_ = pen;
    }
    
private :
    named_frame                 &self_;
    runtime_array<munin::element_type>  name_;
    attribute                    pen_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
named_frame::named_frame()
{
    pimpl_.reset(new impl(*this));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
named_frame::~named_frame()
{
}

// ==========================================================================
// SET_NAME
// ==========================================================================
void named_frame::set_name(string const &name)
{
    pimpl_->set_name(name);
    
    vector<rectangle> redraw_regions;
    redraw_regions.push_back(rectangle(
        point(0, 0)
      , extent(get_size().width, 1)));
    on_redraw(redraw_regions);
}

// ==========================================================================
// DO_GET_FRAME_WIDTH
// ==========================================================================
u32 named_frame::do_get_frame_width() const
{
    return 1;
}

// ==========================================================================
// DO_GET_FRAME_HEIGHT
// ==========================================================================
u32 named_frame::do_get_frame_height() const
{
    return 1;
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
extent named_frame::do_get_preferred_size() const
{
    return extent(
        get_frame_width() * 2
      , get_frame_height() * 2);
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void named_frame::do_draw(
    canvas          &cvs
  , rectangle const &region)
{
    pimpl_->draw(cvs, region);
}

// ==========================================================================
// DO_SET_ATTRIBUTE
// ==========================================================================
void named_frame::do_set_attribute(string const &name, any const &attr)
{
    munin::frame::do_set_attribute(name, attr);
    
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

}

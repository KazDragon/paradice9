// ==========================================================================
// Munin Text Area.
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
#include "munin/text_area.hpp"
#include "munin/canvas.hpp"
#include "munin/ansi/ansi_types.hpp"
#include <string>
#include <vector>

using namespace std;
using namespace munin;
using namespace odin;
using namespace boost;

namespace munin { 

// ==========================================================================
// TEXT_AREA IMPLEMENTATION STRUCTURE
// ==========================================================================
struct text_area::impl
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(text_area &self)
        : self_(self)
        , cursor_position_(0, 0)
    {
        content_.push_back("");
    }

    // ======================================================================
    // INSERT_TEXT
    // ======================================================================
    void insert_text(string const &text)
    {
        // If this would be added at the end of a line, we will need to
        // move the cursor to the next row.
        content_[cursor_position_.y] += text;
        ++cursor_position_.x;
        
        if (cursor_position_.x == self_.get_size().width)
        {
            cursor_position_.x = 0;
            ++cursor_position_.y;
            content_.push_back("");
        }
    }
    
    // ======================================================================
    // GET_CURSOR_POSITION
    // ======================================================================
    point get_cursor_position() const
    {
        return cursor_position_;
    }
    
    // ======================================================================
    // DRAW
    // ======================================================================
    void draw(
        canvas<element_type> &cvs
      , point const          &offset
      , rectangle const      &region)
    {
        point position = self_.get_position();
        
        for (s32 row = 0; row < region.size.height; ++row)
        {
            for (s32 column = 0; column < region.size.width; ++column)
            {
                // If the position being examined is at a valid content
                // location, then use that character.  Otherwise, use the
                // default space character.
                char element =
                    row    < s32(content_.size())
                 && column < s32(content_[row].size())
                  ? content_[row][column]
                  : ' ';
                
                cvs[position.x + column + region.origin.x + offset.x]
                   [position.y + row    + region.origin.y + offset.y] =
                   munin::ansi::element_type(element, munin::ansi::attribute());
            }
        }
    }
    
private :
    // A reference to the text_area that we are implementing.
    text_area    &self_;
    
    // The text is modeled as a collection of strings.  Each string represents
    // a line in the control.
    typedef vector<string> content_type;
    content_type  content_;
    
    // The point at which the cursor lies.
    point         cursor_position_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
text_area::text_area()
{
    pimpl_.reset(new impl(*this));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
text_area::~text_area()
{
}

// ==========================================================================
// INSERT_TEXT
// ==========================================================================
void text_area::insert_text(string const &text)
{
    pimpl_->insert_text(text);
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
extent text_area::do_get_preferred_size() const
{
    return extent(0,0);
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void text_area::do_draw(
    canvas<element_type> &cvs
  , point const          &offset
  , rectangle const      &region)
{
    pimpl_->draw(cvs, offset, region);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void text_area::do_event(boost::any const &event)
{
    char const* pch = any_cast<char>(&event);

    if (pch != NULL)
    {
        insert_text(string(1, *pch));
    }
}

// ==========================================================================
// DO_GET_CURSOR_STATE
// ==========================================================================
bool text_area::do_get_cursor_state() const
{
    // An input area always has a cursor.
    return true;
}

// ==========================================================================
// DO_GET_CURSOR_POSITION
// ==========================================================================
point text_area::do_get_cursor_position() const
{
    return pimpl_->get_cursor_position();
}

}



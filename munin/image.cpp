// ==========================================================================
// Munin Image Component.
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
#include "munin/image.hpp"
#include "munin/canvas.hpp"
#include "munin/context.hpp"
#include "munin/ansi/protocol.hpp"
#include "odin/ansi/protocol.hpp"
#include "odin/ascii/protocol.hpp"
#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>
#include <algorithm>

using namespace odin;
using namespace boost;
using namespace boost::assign;
using namespace std;

namespace munin {

// ==========================================================================
// IMAGE_FROM_TEXT
// ==========================================================================
vector< vector<element_type> > image_from_text(vector<string> const &text)
{
    vector< vector<element_type> > elements(text.size());

    for (u32 index = 0; index < elements.size(); ++index)
    {
        elements[index] = munin::ansi::elements_from_string(text[index]);
    }
    
    return elements;
}

// ==========================================================================
// IMAGE::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct image::impl : boost::noncopyable
{
    vector< vector<element_type> > elements_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
image::image(vector< vector<element_type> > elements)
    : pimpl_(make_shared<impl>())
{
    pimpl_->elements_  = elements;
    set_can_focus(false);
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
image::image(vector<element_type> elements)
    : pimpl_(make_shared<impl>())
{
    pimpl_->elements_ = list_of(elements);
    set_can_focus(false);
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
image::~image()
{
}

// ==========================================================================
// SET_IMAGE
// ==========================================================================
void image::set_image(vector< vector<element_type> > elements)
{
    pimpl_->elements_ = elements;
    on_preferred_size_changed();
    on_redraw(list_of(rectangle(point(), get_size())));
}

// ==========================================================================
// SET_IMAGE
// ==========================================================================
void image::set_image(vector<element_type> elements)
{
    vector< vector<element_type> > elements_lines;
    elements_lines.push_back(elements);
    set_image(elements_lines);
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
extent image::do_get_preferred_size() const
{
    extent preferred_size;
    preferred_size.height = pimpl_->elements_.size();
    
    BOOST_FOREACH(vector<element_type> const &row, pimpl_->elements_)
    {
        preferred_size.width = (max)(
            u32(preferred_size.width)
          , u32(row.size()));
    }
    
    return preferred_size;
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void image::do_draw(
    context         &ctx
  , rectangle const &region)
{
    static element_type const default_element(' ');
    canvas &cvs = ctx.get_canvas();

    for (u32 row = region.origin.y; 
         row < u32(region.origin.y + region.size.height);
         ++row)
    {
        for (u32 column = region.origin.x;
             column < u32(region.origin.x + region.size.width);
             ++column)
        {
            if (row < pimpl_->elements_.size()
             && column < pimpl_->elements_[row].size())
            {
                cvs[column][row] = pimpl_->elements_[row][column];
            }
            else
            {
                cvs[column][row] = default_element;
            }
        }
    }
}

}


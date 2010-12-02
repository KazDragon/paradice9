// ==========================================================================
// Munin ANSI Image Component.
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
#include "munin/ansi/image.hpp"
#include "munin/ansi/protocol.hpp"
#include "munin/canvas.hpp"
#include "odin/ansi/protocol.hpp"
#include "odin/ascii/protocol.hpp"
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>
#include <algorithm>

using namespace odin;
using namespace boost;
using namespace std;

namespace munin { namespace ansi {

// ==========================================================================
// IMAGE_FROM_TEXT
// ==========================================================================
runtime_array< runtime_array<element_type> >
    image_from_text(runtime_array<string> const &text)
{
    runtime_array< runtime_array<element_type> > elements(text.size());

    for (u32 index = 0; index < elements.size(); ++index)
    {
        elements[index] = elements_from_string(text[index]);
    }
    
    return elements;
}

// ==========================================================================
// IMAGE::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct image::impl
{
    runtime_array< runtime_array<munin::ansi::element_type> > elements_;    
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
image::image(
    runtime_array< runtime_array<munin::ansi::element_type> > elements)
    : pimpl_(new impl)
{
    pimpl_->elements_ = elements;
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
image::~image()
{
}

// ==========================================================================
// DO_CAN_FOCUS
// ==========================================================================
bool image::do_can_focus() const
{
    return false;
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
munin::extent image::do_get_preferred_size() const
{
    munin::extent preferred_size;
    preferred_size.height = pimpl_->elements_.size();
    
    BOOST_FOREACH(
        runtime_array<munin::ansi::element_type> row, pimpl_->elements_)
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
    canvas<element_type> &cvs
  , point const          &offset
  , rectangle const      &region)
{
    BOOST_AUTO(position, get_position());
    
    for (u32 row = region.origin.y; 
         row < u32(region.origin.y + region.size.height);
         ++row)
    {
        for (u32 column = region.origin.x;
             column < u32(region.origin.x + region.size.width);
             ++column)
        {
            if (row < pimpl_->elements_.size())
            {
                if (column < pimpl_->elements_[row].size())
                {
                    cvs[position.x + column + offset.x]
                       [position.y + row    + offset.y]
                        = pimpl_->elements_[row][column];
                }
                else
                {
                    cvs[position.x + column + offset.x]
                       [position.y + row    + offset.y]
                        = element_type(' ', attribute());
                }
            }
            else
            {
                cvs[position.x + column + offset.x]
                   [position.y + row    + offset.y]
                    = element_type(' ', attribute());
            }
        }
    }
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void image::do_event(any const &event)
{
}

}}


// ==========================================================================
// Guibuilder Sized Block Component.
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
#include "guibuilder/sized_block.hpp"
#include "munin/canvas.hpp"
#include "munin/ansi/protocol.hpp"
#include "odin/ansi/protocol.hpp"
#include "odin/ascii/protocol.hpp"
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>
#include <algorithm>

using namespace munin;
using namespace odin;
using namespace boost;
using namespace std;

namespace guibuilder {

// ==========================================================================
// SIZED_BLOCK::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct sized_block::impl : boost::noncopyable
{
    munin::extent size_;
    element_type  element_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
sized_block::sized_block(element_type const &element, munin::extent size)
    : pimpl_(make_shared<impl>())
{
    pimpl_->size_    = size;
    pimpl_->element_ = element;
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
sized_block::~sized_block()
{
}

// ==========================================================================
// DO_SET_SIZE
// ==========================================================================
void sized_block::do_set_size(munin::extent const &size)
{
    // Do nothing
}

// ==========================================================================
// DO_GET_SIZE
// ==========================================================================
munin::extent sized_block::do_get_size() const
{
    return pimpl_->size_;
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
munin::extent sized_block::do_get_preferred_size() const
{
    return pimpl_->size_;
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void sized_block::do_draw(
    canvas          &cvs
  , rectangle const &region)
{
    for (u32 row = region.origin.y; 
         row < u32(region.origin.y + region.size.height);
         ++row)
    {
        for (u32 column = region.origin.x;
             column < u32(region.origin.x + region.size.width);
             ++column)
        {
            element_type element = pimpl_->element_;
            element.attribute_.foreground_colour_ = ((column + row) % 8);
            cvs[column][row] = element;
        }
    }
}

}


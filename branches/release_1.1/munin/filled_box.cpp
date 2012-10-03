// ==========================================================================
// Munin Filled Box Component.
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
#include "munin/filled_box.hpp"
#include "munin/canvas.hpp"
#include <boost/assign/list_of.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace odin;
using namespace boost;
using namespace boost::assign;
using namespace std;

namespace munin {

// ==========================================================================
// FILLED_BOX::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct filled_box::impl
{
    element_type element_;
    extent       preferred_size_;
    bool         locked_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
filled_box::filled_box(element_type const &element)
    : pimpl_(make_shared<impl>())
{
    pimpl_->element_        = element;
    pimpl_->preferred_size_ = extent(1, 1);
    pimpl_->locked_         = false;
    set_can_focus(false);
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
filled_box::~filled_box()
{
}

// ==========================================================================
// SET_PREFERRED_SIZE
// ==========================================================================
void filled_box::set_preferred_size(extent preferred_size)
{
    pimpl_->preferred_size_ = preferred_size;
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
extent filled_box::do_get_preferred_size() const
{
    return pimpl_->preferred_size_;
}

// ==========================================================================
// DO_SET_ATTRIBUTE
// ==========================================================================
void filled_box::do_set_attribute(string const &name, any const &attr)
{
    bool attribute_changed = false;

    if (name == ATTRIBUTE_LOCK)
    {
        BOOST_AUTO(lock, any_cast<bool>(&attr));

        if (lock != NULL)
        {
            pimpl_->locked_ = *lock;
        }
    }

    if (pimpl_->locked_)
    {
        return;
    }

    if (name == ATTRIBUTE_GLYPH)
    {
        BOOST_AUTO(gly, any_cast<glyph>(&attr));

        if (gly != NULL)
        {
            pimpl_->element_.glyph_ = *gly;
            attribute_changed = true;
        }
    }

    if (name == ATTRIBUTE_PEN)
    {
        BOOST_AUTO(pen, any_cast<attribute>(&attr));

        if (pen != NULL)
        {
            pimpl_->element_.attribute_ = *pen;
            attribute_changed = true;
        }
    }

    if (attribute_changed)
    {
        on_redraw(list_of(rectangle(point(), get_size())));
    }
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void filled_box::do_draw(
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
            cvs[column][row] = pimpl_->element_;
        }
    }
}

}


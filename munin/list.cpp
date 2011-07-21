// ==========================================================================
// Munin List Component.
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
#include "munin/list.hpp"
#include "munin/ansi/protocol.hpp"
#include "munin/canvas.hpp"
#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace odin;
using namespace boost;
using namespace boost::assign;
using namespace std;

namespace munin {

// ==========================================================================
// LIST::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct list::impl
{
    runtime_array< runtime_array<element_type> > items_;
    s32                                          item_index_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
list::list()
    : pimpl_(make_shared<impl>())
{
    pimpl_->item_index_ = -1;
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
list::~list()
{
}

// ==========================================================================
// SET_ITEMS
// ==========================================================================
void list::set_items(runtime_array< runtime_array<element_type> > const &items)
{
    BOOST_AUTO(old_items_size, pimpl_->items_.size());
    BOOST_AUTO(size, get_size());

    pimpl_->items_ = items;

    // Ensure that the currently selected item is not a non-existent item.
    set_item_index((min)(old_items_size - 1, items.size() - 1));

    // We will probably require redrawing the entire component.
    on_redraw(list_of(rectangle(point(), size)));

    // This may well change the preferred size of this component.
    on_preferred_size_changed();
}

// ==========================================================================
// SET_ITEM_INDEX
// ==========================================================================
void list::set_item_index(s32 index)
{
    BOOST_AUTO(old_index, pimpl_->item_index_);
    pimpl_->item_index_ = index;

    // We will need to redraw the item both at the old index and the new
    // index.
    BOOST_AUTO(size, get_size());

    if (old_index >= 0)
    {
        on_redraw(list_of(rectangle(
            point(0, old_index)
          , extent(size.width, 1))));
    }

    if (index >= 0)
    {
        on_redraw(list_of(rectangle(
            point(0, index)
          , extent(size.width, 1))));
    }
}

// ==========================================================================
// GET_ITEM_INDEX
// ==========================================================================
s32 list::get_item_index() const
{
    return pimpl_->item_index_;
}

// ==========================================================================
// GET_ITEM
// ==========================================================================
runtime_array<element_type> list::get_item() const
{
    return pimpl_->item_index_ < 0
         ? runtime_array<element_type>()
         : pimpl_->items_[pimpl_->item_index_];
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
extent list::do_get_preferred_size() const
{
    // The preferred size of this component is the widest item wide, and
    // the number of components high.
    u32 max_width = 0;

    BOOST_FOREACH(runtime_array<element_type> const &item, pimpl_->items_)
    {
        max_width = (max)(max_width, u32(item.size()));
    }

    return extent(max_width, pimpl_->items_.size());
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void list::do_draw(
    canvas          &cvs
  , rectangle const &region)
{
    for (s32 y_coord = region.origin.y;
        y_coord < region.origin.y + region.size.height;
        ++y_coord)
    {
        if (y_coord < s32(pimpl_->items_.size()))
        {
            bool is_selected_item = 
                y_coord >= 0
             && u32(y_coord) == pimpl_->item_index_;

            runtime_array<element_type> const &item = pimpl_->items_[y_coord];

            for (s32 x_coord = region.origin.x;
                 x_coord < region.origin.x + region.size.width;
                 ++x_coord)
            {
                element_type element = x_coord < s32(item.size())
                                     ? item[x_coord]
                                     : element_type(' ');

                if (is_selected_item)
                {
                    element.attribute_.polarity_ = 
                        element.attribute_.polarity_ == odin::ansi::graphics::POLARITY_NEGATIVE
                                                      ? odin::ansi::graphics::POLARITY_POSITIVE
                                                      : odin::ansi::graphics::POLARITY_NEGATIVE;
                }

                cvs[x_coord][y_coord] = element;
            }
        }
        else
        {
            for (s32 x_coord = region.origin.x;
                 x_coord < region.origin.x + region.size.width;
                 ++x_coord)
            {
                cvs[x_coord][y_coord] = element_type(' ');
            }
        }
    }
}

// ==========================================================================
// LIST_FROM_TEXT
// ==========================================================================
runtime_array< runtime_array<element_type> >
    list_from_text(runtime_array<string> const &text)
{
    runtime_array< runtime_array<element_type> > elements(text.size());

    for (u32 index = 0; index < elements.size(); ++index)
    {
        elements[index] = munin::ansi::elements_from_string(text[index]);
    }
    
    return elements;
}

}

// ==========================================================================
// Munin Grid Layout.
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
#include "munin/grid_layout.hpp"
#include "munin/container.hpp"
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace odin;
using namespace boost;
using namespace std;

namespace munin {

// ==========================================================================
// GRID_LAYOUT::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct grid_layout::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl()
        : rows_(0)
        , columns_(0)
    {
    }

    u32 rows_;
    u32 columns_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
grid_layout::grid_layout(u32 rows, u32 columns)
    : pimpl_(make_shared<impl>())
{
    pimpl_->rows_    = rows;
    pimpl_->columns_ = columns;
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
grid_layout::~grid_layout()
{
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
extent grid_layout::do_get_preferred_size(
    vector< shared_ptr<component> > const &components
  , vector< any >                   const &hints) const
{
    // The preferred size of the whole component is the maximum preferred
    // width and the maximum preferred height of all components, 
    // multiplied appropriately by the rows and columns
    extent maximum_preferred_size(0, 0);

    BOOST_FOREACH(shared_ptr<component> comp, components)
    {
        BOOST_AUTO(preferred_size, comp->get_preferred_size());

        maximum_preferred_size.width = (std::max)(
            maximum_preferred_size.width
          , preferred_size.width);

        maximum_preferred_size.height = (std::max)(
            maximum_preferred_size.height
          , preferred_size.height);
    }
    
    maximum_preferred_size.width  *= pimpl_->columns_;
    maximum_preferred_size.height *= pimpl_->rows_;

    return maximum_preferred_size;
}

// ==========================================================================
// DO_LAYOUT
// ==========================================================================
void grid_layout::do_layout(
    vector< shared_ptr<component> > const &components
  , vector< any >                   const &hints
  , extent                                 size)
{
    for (u32 index = 0; index < components.size(); ++index)
    {
        BOOST_AUTO(comp, components[index]);

        // Work out the row/column of the current component.
        u32 row    = index / pimpl_->columns_;
        u32 column = index % pimpl_->columns_;

        // Naive: will have missing pixels and off-by-one errors
        comp->set_position(
            munin::point(
                (size.width / pimpl_->columns_) * column
              , (size.height / pimpl_->rows_) * row));

        comp->set_size(
            munin::extent(
                size.width  / pimpl_->columns_
              , size.height / pimpl_->rows_));
    }
}

}


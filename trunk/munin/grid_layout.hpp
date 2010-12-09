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
#ifndef MUNIN_GRID_LAYOUT_HPP_
#define MUNIN_GRID_LAYOUT_HPP_

#include "munin/layout.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/typeof/typeof.hpp>
#include <algorithm>
#include <vector>

namespace munin {

//* =========================================================================
/// \brief A class that knows how to lay components out in a container in
/// a grid-like manner.  Each cell of the grid has an identical size.
/// Components added to the grid will be displayed left-to-right, top-to-
/// bottom.
//* =========================================================================
template <class ElementType>
class grid_layout : public layout<ElementType>
{
public :
    typedef layout<ElementType> parent_type;
    typedef typename parent_type::component_type component_type;
    typedef typename parent_type::container_type container_type;

    //* =====================================================================
    /// \brief Constructor
    /// \param rows The number of rows in this grid.
    /// \param columns The number of columns in this grid.
    //* =====================================================================
    grid_layout(odin::u32 rows, odin::u32 columns)
        : rows_(rows)
        , columns_(columns)
    {
    }

protected :
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to retrieve the preferred size of the layout
    /// in a custom manner.
    //* =====================================================================
    virtual extent do_get_preferred_size(
        boost::shared_ptr< munin::container<ElementType> const> const &cont) const
    {
        // The preferred size of the whole component is the maximum preferred
        // width and the maximum preferred height of all components, 
        // multiplied appropriately by the rows and columns
        extent maximum_preferred_size(0, 0);

        for (odin::u32 index = 0;
             index < cont->get_number_of_components();
             ++index)
        {
            BOOST_AUTO(comp,           cont->get_component(index));
            BOOST_AUTO(preferred_size, comp->get_preferred_size());

            maximum_preferred_size.width = (std::max)(
                maximum_preferred_size.width
              , preferred_size.width);

            maximum_preferred_size.height = (std::max)(
                maximum_preferred_size.height
              , preferred_size.height);
        }
        
        maximum_preferred_size.width  *= columns_;
        maximum_preferred_size.height *= rows_;

        return maximum_preferred_size;
    }
    
    //* =====================================================================
    /// \brief Called by operator().  Derived classes must override this
    /// function in order to lay a container's components out in a custom
    /// manner.
    //* =====================================================================
    virtual void do_layout(
        boost::shared_ptr<container_type> const &cont)
    {
        munin::extent size = cont->get_size();

        for (odin::u32 index = 0;
             index < cont->get_number_of_components();
             ++index)
        {
            BOOST_AUTO(comp, cont->get_component(index));

            // Work out the row/column of the current component.
            odin::u32 row    = index / columns_;
            odin::u32 column = index % columns_;

            // Naive: will have missing pixels and off-by-one errors
            comp->set_position(
                munin::point(
                    (size.width / columns_) * column
                  , (size.height / rows_) * row));

            comp->set_size(
                munin::extent(
                    size.width  / columns_
                  , size.height / rows_));
        }
    }

private :    
    odin::u32 rows_;
    odin::u32 columns_;
};
    
}

#endif

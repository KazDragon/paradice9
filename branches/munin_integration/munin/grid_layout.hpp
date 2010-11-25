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
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
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

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~grid_layout()
    {
    }
    
protected :
    //* =====================================================================
    /// \brief Called by get_number_of_components().  Derived classes must
    /// override this function in order to get the number of components in
    /// a custom manner.
    //* =====================================================================
    virtual odin::u32 do_get_number_of_components() const
    {
        return odin::u32(components_.size());
    }
    
    //* =====================================================================
    /// \brief Called by add_component().  Derived classes must override this
    /// function in order to add a component in a custom manner.
    //* =====================================================================
    virtual void do_add_component(
        boost::shared_ptr<component_type> const &comp
      , boost::any                               hint)
    {
        components_.push_back(comp);
    }
    
    //* =====================================================================
    /// \brief Called by remove_component().  Derived classes must override 
    /// this function in order to remove a component in a custom manner.
    //* =====================================================================
    virtual void do_remove_component(
        boost::shared_ptr<component_type> const &comp)
    {
        components_.erase(
            std::remove(
                components_.begin()
              , components_.end()
              , comp)
          , components_.end());
    }
    
    //* =====================================================================
    /// \brief Called by get_component().  Derived classes must override this
    /// function in order to retrieve a component in a custom manner.
    //* =====================================================================
    virtual boost::shared_ptr<component_type> 
        do_get_component(odin::u32 index) const
    {
        return boost::shared_ptr<component_type>();
    }
    
    //* =====================================================================
    /// \brief Called by get_hint().  Derived classes must override this
    /// function in order to retrieve a component's hint in a custom manner.
    //* =====================================================================
    virtual boost::any do_get_hint(odin::u32 index) const
    {
        return boost::any();
    }

    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to retrieve the preferred size of the layout
    /// in a custom manner.
    //* =====================================================================
    virtual extent do_get_preferred_size() const
    {
        // The preferred size of the whole component is the maximum preferred
        // width and the maximum preferred height of all components.
        extent maximum_preferred_size(0, 0);

        BOOST_FOREACH(boost::shared_ptr<component_type> comp, components_)
        {
            extent preferred_size = comp->get_preferred_size();

            maximum_preferred_size.width = (std::max)(
                maximum_preferred_size.width
              , preferred_size.width);

            maximum_preferred_size.height = (std::max)(
                maximum_preferred_size.height
              , preferred_size.height);
        }

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

        for (size_t index = 0; index < components_.size(); ++index)
        {
            // Work out the row/column of the current component.
            odin::u32 row    = index / columns_;
            odin::u32 column = index % columns_;

            // Naive: will have missing pixels and off-by-one errors
            components_[index]->set_position(
                munin::point(
                    (size.width / columns_) * column
                  , (size.height / rows_) * row));

            components_[index]->set_size(
                munin::extent(
                    size.width  / columns_
                  , size.height / rows_));

            printf("GRID LAYOUT: \n"
                   "    Setting component %d to \n"
                   "        position = (%d,%d)\n"
                   "        size     = (%d,%d)\n",
                   index,
                   components_[index]->get_position().x,
                   components_[index]->get_position().y,
                   components_[index]->get_size().width,
                   components_[index]->get_size().height);
        }
    }

private :    
    std::vector< boost::shared_ptr<component_type> > components_;
    odin::u32                                        rows_;
    odin::u32                                        columns_;
};
    
}

#endif

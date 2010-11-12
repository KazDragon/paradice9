// ==========================================================================
// Munin Compass Layout.
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
#ifndef MUNIN_COMPASS_LAYOUT_HPP_
#define MUNIN_COMPASS_LAYOUT_HPP_

#include "munin/layout.hpp"
#include <utility>
#include <vector>

namespace munin {

//* =========================================================================
/// \brief A class that can lay out its components according to basic compass
/// directions.  Components can be laid out to the north, east, south, west,
/// and centre.  Components to the north and south are given their
/// preferred height while having the width of the containing component, and
/// components to the west and east are given their preferred width, while
/// having the height of the containing component.
//* =========================================================================
template <class ElementType>
class compass_layout : public layout<ElementType>
{
public :
    BOOST_STATIC_CONSTANT(odin::u32, CENTRE = 0);
    BOOST_STATIC_CONSTANT(odin::u32, NORTH  = 1);
    BOOST_STATIC_CONSTANT(odin::u32, EAST   = 2);
    BOOST_STATIC_CONSTANT(odin::u32, SOUTH  = 3);
    BOOST_STATIC_CONSTANT(odin::u32, WEST   = 4);

private :
    //* =====================================================================
    /// \brief Called by get_number_of_components().  Derived classes must
    /// override this function in order to get the number of components in
    /// a custom manner.
    //* =====================================================================
    virtual odin::u32 do_get_number_of_components() const
    {
        return component_pairs_.size();
    }
    
    //* =====================================================================
    /// \brief Called by add_component().  Derived classes must override this
    /// function in order to add a component in a custom manner.
    //* =====================================================================
    virtual void do_add_component(
        boost::shared_ptr<component_type> const &comp
      , boost::any                               hint)
    {
        component_pairs_.push_back(std::make_pair(comp, hint));
    }
    
    //* =====================================================================
    /// \brief Called by remove_component().  Derived classes must override 
    /// this function in order to remove a component in a custom manner.
    //* =====================================================================
    virtual void do_remove_component(
        boost::shared_ptr<component_type> const &comp)
    {
    }
    
    //* =====================================================================
    /// \brief Called by get_component().  Derived classes must override this
    /// function in order to retrieve a component in a custom manner.
    //* =====================================================================
    virtual boost::shared_ptr<component_type> 
        do_get_component(odin::u32 index) const
    {
        return component_pairs_[index].first;
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
        return extent();
    }
    
    //* =====================================================================
    /// \brief Called by operator().  Derived classes must override this
    /// function in order to lay a container's components out in a custom
    /// manner.
    //* =====================================================================
    virtual void do_layout(
        boost::shared_ptr<container_type> const &cont)
    {
    }

    typedef std::pair<
        boost::shared_ptr<component_type>
      , boost::any
    > component_pair_type;

    std::vector<component_pair_type> component_pairs_;
};
    
}

#endif

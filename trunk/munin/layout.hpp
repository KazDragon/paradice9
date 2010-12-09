// ==========================================================================
// Munin Layout.
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
#ifndef MUNIN_LAYOUT_HPP_
#define MUNIN_LAYOUT_HPP_

#include "odin/types.hpp"
#include "munin/types.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>

namespace munin {

template <class ElementType>
class component;

template <class ElementType>
class container;

//* =========================================================================
/// \brief A class that knows how to lay components out in a container in
/// a specified manner.
//* =========================================================================
template <class ElementType>
class layout
{
public :
    typedef component<ElementType> component_type;
    typedef container<ElementType> container_type;
    
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~layout()
    {
    }

    //* =====================================================================
    /// \brief Returns the preferred size of this layout.
    /// \par
    /// The preferred size of the layout is the size that the layout would be
    /// if all components in it were at their preferred sizes.
    //* =====================================================================
    extent get_preferred_size(
        boost::shared_ptr<container_type const> const &cont) const
    {
        return do_get_preferred_size(cont);
    }
    
    //* =====================================================================
    /// \brief Performs the laying out of the components that were added to
    /// this layout, within the context of the specified container.
    //* =====================================================================
    void operator()(boost::shared_ptr<container_type> const &cont)
    {
        do_layout(cont);
    }
    
protected :
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to retrieve the preferred size of the layout
    /// in a custom manner.
    //* =====================================================================
    virtual extent do_get_preferred_size(
        boost::shared_ptr<container_type const> const &cont) const = 0;
    
    //* =====================================================================
    /// \brief Called by operator().  Derived classes must override this
    /// function in order to lay a container's components out in a custom
    /// manner.
    //* =====================================================================
    virtual void do_layout(
        boost::shared_ptr<container_type> const &cont) = 0;
};
    
}

#endif

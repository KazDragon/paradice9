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
    /// \brief Retrieves the number of components that this layout is in
    /// charge of laying out.
    //* =====================================================================
    odin::u32 get_number_of_components() const
    {
        return do_get_number_of_components();
    }
    
    //* =====================================================================
    /// \brief Adds a component to this layout, with a specified hint whose
    /// purpose is defined by the derived layout type.
    //* =====================================================================
    void add_component(
        boost::shared_ptr<component_type> const &comp
      , boost::any                               hint)
    {
        do_add_component(comp, hint);
    }
    
    //* =====================================================================
    /// \brief Removes a component from this layout.
    //* =====================================================================
    void remove_component(boost::shared_ptr<component_type> const &comp)
    {
        do_remove_component(comp);
    }
    
    //* =====================================================================
    /// \brief Retrieves the indexth component
    //* =====================================================================
    boost::shared_ptr<component_type> get_component(odin::u32 index) const
    {
        return do_get_component(index);
    }
        
    //* =====================================================================
    /// \brief Retrieves the indexth component's hint.
    //* =====================================================================
    boost::any get_hint(odin::u32 index) const
    {
        return do_get_hint(index);
    }

    //* =====================================================================
    /// \brief Returns the preferred size of this layout.
    /// \par
    /// The preferred size of the layout is the size that the layout would be
    /// if all components in it were at their preferred sizes.
    //* =====================================================================
    extent get_preferred_size() const
    {
        return do_get_preferred_size();
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
    /// \brief Called by get_number_of_components().  Derived classes must
    /// override this function in order to get the number of components in
    /// a custom manner.
    //* =====================================================================
    virtual odin::u32 do_get_number_of_components() const = 0;
    
    //* =====================================================================
    /// \brief Called by add_component().  Derived classes must override this
    /// function in order to add a component in a custom manner.
    //* =====================================================================
    virtual void do_add_component(
        boost::shared_ptr<component_type> const &comp
      , boost::any                               hint) = 0;
    
    //* =====================================================================
    /// \brief Called by remove_component().  Derived classes must override 
    /// this function in order to remove a component in a custom manner.
    //* =====================================================================
    virtual void do_remove_component(
        boost::shared_ptr<component_type> const &comp) = 0;
    
    //* =====================================================================
    /// \brief Called by get_component().  Derived classes must override this
    /// function in order to retrieve a component in a custom manner.
    //* =====================================================================
    virtual boost::shared_ptr<component_type> 
        do_get_component(odin::u32 index) const = 0;
    
    //* =====================================================================
    /// \brief Called by get_hint().  Derived classes must override this
    /// function in order to retrieve a component's hint in a custom manner.
    //* =====================================================================
    virtual boost::any do_get_hint(odin::u32 index) const = 0;

    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to retrieve the preferred size of the layout
    /// in a custom manner.
    //* =====================================================================
    virtual extent do_get_preferred_size() const = 0;
    
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

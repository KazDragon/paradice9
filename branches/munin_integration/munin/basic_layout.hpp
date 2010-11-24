// ==========================================================================
// Munin Basic_layout.
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
#ifndef MUNIN_BASIC_LAYOUT_HPP_
#define MUNIN_BASIC_LAYOUT_HPP_

#include "munin/layout.hpp"
#include <utility>
#include <vector>

namespace munin {

template <class ElementType>
class component;

//* =========================================================================
/// \brief A default implementation of a layout.  Only do_layout() and
/// do_get_preferred_size() remain unimplemented.
//* =========================================================================
template <class ElementType>
class basic_layout : public layout<ElementType>
{
public :
    typedef component<ElementType> component_type;
    
protected :    
    //* =====================================================================
    /// \brief Called by get_number_of_components().  Derived classes must
    /// override this function in order to get the number of components in
    /// a custom manner.
    //* =====================================================================
    virtual odin::u32 do_get_number_of_components() const
    {
        return components_.size();
    }
    
    //* =====================================================================
    /// \brief Called by add_component().  Derived classes must override this
    /// function in order to add a component in a custom manner.
    //* =====================================================================
    virtual void do_add_component(
        boost::shared_ptr<component_type> const &comp
      , boost::any                               hint)
    {
        components_.push_back(std::make_pair(comp, hint));
    }
    
    //* =====================================================================
    /// \brief Called by remove_component().  Derived classes must override 
    /// this function in order to remove a component in a custom manner.
    //* =====================================================================
    virtual void do_remove_component(
        boost::shared_ptr<component_type> const &comp)
    {
        typedef typename component_hint_vector::iterator component_iterator;
        
        for (component_iterator current_component = components_.begin();
             current_component != components_.end();
             // Nothing
            )
        {
            if (current_component->first == comp)
            {
                current_component = components_.erase(current_component);
            }
            else
            {
                ++current_component;
            }
        }
    }
    
    //* =====================================================================
    /// \brief Called by get_component().  Derived classes must override this
    /// function in order to retrieve a component in a custom manner.
    //* =====================================================================
    virtual boost::shared_ptr<component_type> 
        do_get_component(odin::u32 index) const
    {
        return components_[index].first;
    }
    
    //* =====================================================================
    /// \brief Called by get_hint().  Derived classes must override this
    /// function in order to retrieve a component's hint in a custom manner.
    //* =====================================================================
    virtual boost::any do_get_hint(odin::u32 index) const
    {
        return components_[index].second;
    }

private :
    typedef typename std::vector<
        std::pair<
            boost::shared_ptr<component_type>
          , boost::any
        >
    > component_hint_vector;
        
    component_hint_vector components_;
};
    
}

#endif

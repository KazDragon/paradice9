// ==========================================================================
// Munin Component.
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
#ifndef MUNIN_BASIC_COMPONENT_HPP_
#define MUNIN_BASIC_COMPONENT_HPP_

#include "munin/component.hpp"
#include <boost/enable_shared_from_this.hpp>
#include <boost/weak_ptr.hpp>

namespace munin {

//* =========================================================================
/// \brief A default implementation of a component.  Only do_draw()
/// do_get_preferred_size() remain unimplemented.
//* =========================================================================
template <class ElementType>
class basic_component 
    : public component<ElementType>
    , public boost::enable_shared_from_this< basic_component<ElementType> >
{
public :
    typedef ElementType                    element_type;
    typedef munin::component<element_type> component_type;
    
    
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    basic_component()
        : has_focus_(false)
    {
        bounds_.origin.x    = 0;
        bounds_.origin.y    = 0;
        bounds_.size.width  = 0;
        bounds_.size.height = 0;
    }
    
private :
    rectangle                                  bounds_;
    boost::weak_ptr< container<element_type> > parent_;
    bool                                       has_focus_;
    
    //* =====================================================================
    /// \brief Called by set_position().  Derived classes must override this
    /// function in order to set the position of the component in a custom
    /// manner.
    //* =====================================================================
    virtual void do_set_position(point const &position)
    {
        bounds_.origin = position;
    }
    
    //* =====================================================================
    /// \brief Called by get_position().  Derived classes must override this
    /// function in order to get the position of the component in a custom
    /// manner.
    //* =====================================================================
    virtual point do_get_position() const
    {
        return bounds_.origin;
    }
    
    //* =====================================================================
    /// \brief Called by set_size().  Derived classes must override this 
    /// function in order to set the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual void do_set_size(extent const &size)
    {
        bounds_.size = size;
    }

    //* =====================================================================
    /// \brief Called by get_size().  Derived classes must override this
    /// function in order to get the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual extent do_get_size() const
    {
        return bounds_.size;
    }

    //* =====================================================================
    /// \brief Called by set_parent().  Derived classes must override this
    /// function in order to set the parent of the component in a custom
    /// manner.
    //* =====================================================================
    virtual void do_set_parent(
        boost::shared_ptr< container<element_type> > const &parent)
    {
        parent_ = parent;
    }
    
    //* =====================================================================
    /// \brief Called by get_parent().  Derived classes must override this
    /// function in order to get the parent of the component in a custom
    /// manner.
    //* =====================================================================
    virtual boost::shared_ptr< container<element_type> > do_get_parent() const
    {
        return parent_.lock();
    }

    //* =====================================================================
    /// \brief Called by has_focus().  Derived classes must override this
    /// function in order to return whether this component has focus in a
    /// custom manner.
    //* =====================================================================
    virtual bool do_has_focus() const
    {
        return has_focus_;
    }
    
    //* =====================================================================
    /// \brief Called by can_focus().  Derived classes must override this
    /// function in order to return whether this component can be focused in
    /// a custom manner.
    //* =====================================================================
    virtual bool do_can_focus() const
    {
        return true;
    }
    
    //* =====================================================================
    /// \brief Called by set_focus().  Derived classes must override this
    /// function in order to set the focus to this component in a custom
    /// manner.
    //* =====================================================================
    virtual void do_set_focus()
    {
        if (this->can_focus())
        {
            has_focus_ = true;
            this->on_focus_set();
        }
    }
    
    //* =====================================================================
    /// \brief Called by lose_focus().  Derived classes must override this
    /// function in order to lose the focus from this component in a
    /// custom manner.
    //* =====================================================================
    virtual void do_lose_focus()
    {
        has_focus_ = false;
        this->on_focus_lost();
    }

    // ======================================================================
    // TOGGLE_FOCUS
    // ======================================================================
    void toggle_focus()
    {
        if (has_focus_)
        {
            has_focus_ = false;
            this->on_focus_lost();
        }
        else
        {
            has_focus_ = true;
            this->on_focus_set();
        }
    }

    //* =====================================================================
    /// \brief Called by focus_next().  Derived classes must override this
    /// function in order to move the focus in a custom manner.
    //* =====================================================================
    virtual void do_focus_next()
    {
        toggle_focus();
    }
    
    //* =====================================================================
    /// \brief Called by focus_previous().  Derived classes must override 
    /// this function in order to move the focus in a custom manner.
    //* =====================================================================
    virtual void do_focus_previous()
    {
        toggle_focus();
    }

    //* =====================================================================
    /// \brief Called by get_focussed_component().  Derived classes must
    /// override this function in order to return the focussed component
    /// in a custom manner.
    //* =====================================================================
    virtual boost::shared_ptr<component_type> do_get_focussed_component()
    {
        return has_focus_
             ? boost::shared_ptr<component_type>(this->shared_from_this())
             : boost::shared_ptr<component_type>();
    }
};
    
}

#endif

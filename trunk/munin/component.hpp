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
#ifndef MUNIN_COMPONENT_HPP_
#define MUNIN_COMPONENT_HPP_

#include "munin/types.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>
#include <vector>

namespace munin {
    
template <class ElementType>
class container;

template <class ElementType>
class graphics_context;

//* =========================================================================
/// \brief An object capable of being drawn on a graphics context.
/// A component is a fundamental piece of the munin architecture and is the
/// base class for anything that is capable of being drawn in some way.  It
/// is closely attached to its graphics context, with which it shares an
/// ElementType.  This type is the fundamental graphical unit, which could 
/// be a coloured pixel, or an attributed character, or something else. 
//* =========================================================================
template <class ElementType>
class component
{
public :
    typedef ElementType element_type;

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~component()
    {
    }
    
    //* =====================================================================
    /// \brief Sets the position of this component.  This does not cause a
    /// redraw, on the basis that the entity performing the move (usually
    /// a layout manager) knows about it, and is better informed about all
    /// regions redrawn.
    //* =====================================================================
    void set_position(point const &position)
    {
        do_set_position(position);
    }
    
    //* =====================================================================
    /// \brief Retrieve the position of this component.
    //* =====================================================================
    point get_position() const
    {
        return do_get_position();
    }
    
    //* =====================================================================
    /// \brief Sets the size of this component.  This does not cause a
    /// redraw, on the basis that the entity performing the resize (usually
    /// a layout manager) knows about it, and is better informed about all
    /// regions redrawn.
    //* =====================================================================
    void set_size(extent const &size)
    {
        do_set_size(size);
    }

    //* =====================================================================
    /// \brief Retreives the size of this component.
    //* =====================================================================
    extent get_size() const
    {
        return do_get_size();
    }
    
    //* =====================================================================
    /// \brief Retrieves the preferred size of this component.
    /// \par
    /// The preferred size of a component is the size at which all elements
    /// can be displayed with clarity.  For example, the preferred size of
    /// a static text control with the following text:
    /// \code
    /// This is a
    /// Static Text Box
    /// \endcode
    /// would be (15, 2).
    //* =====================================================================
    extent get_preferred_size() const
    {
        return do_get_preferred_size();
    }
    
    //* =====================================================================
    /// \brief Sets the parent of the component
    //* =====================================================================
    void set_parent(boost::shared_ptr< container<element_type> > const &parent)
    {
        do_set_parent(parent);
    }
    
    //* =====================================================================
    /// \brief Retrieves the parent of this component.
    //* =====================================================================
    boost::shared_ptr< container<element_type> > get_parent() const
    {
        return do_get_parent();
    }
    
    //* =====================================================================
    /// \brief Draws the component.
    ///
    /// \param context the context in which the component should draw itself.
    /// \param offset the position of the parent component (if there is one)
    ///        relative to the context.
    /// \param region the region relative to this component's origin that
    /// should be drawn.
    //* =====================================================================
    void draw(
        graphics_context<element_type> &context
      , point const                    &offset
      , rectangle const                &region)
    {
        do_draw(context, offset, region);
    }
    
    //* =====================================================================
    /// \fn on_redraw
    /// \param regions The regions of the component that requires redrawing.
    /// \brief Connect to this signal in order to receive notifications about
    /// when the component should be redrawn.
    //* =====================================================================
    boost::signal
    <
        void (std::vector<rectangle> regions)
    > on_redraw;
    
private :
    //* =====================================================================
    /// \brief Called by set_position().  Derived classes must override this
    /// function in order to set the position of the component in a custom
    /// manner.
    //* =====================================================================
    virtual void do_set_position(point const &position) = 0;
    
    //* =====================================================================
    /// \brief Called by get_position().  Derived classes must override this
    /// function in order to get the position of the component in a custom
    /// manner.
    //* =====================================================================
    virtual point do_get_position() const = 0;
    
    //* =====================================================================
    /// \brief Called by set_size().  Derived classes must override this 
    /// function in order to set the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual void do_set_size(extent const &size) = 0;

    //* =====================================================================
    /// \brief Called by get_size().  Derived classes must override this
    /// function in order to get the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual extent do_get_size() const = 0;

    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to get the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual extent do_get_preferred_size() const = 0;
    
    //* =====================================================================
    /// \brief Called by set_parent().  Derived classes must override this
    /// function in order to set the parent of the component in a custom
    /// manner.
    //* =====================================================================
    virtual void do_set_parent(
        boost::shared_ptr< container<element_type> > const &parent) = 0;
    
    //* =====================================================================
    /// \brief Called by get_parent().  Derived classes must override this
    /// function in order to get the parent of the component in a custom
    /// manner.
    //* =====================================================================
    virtual boost::shared_ptr< 
        container<element_type> 
    > do_get_parent() const = 0;
    
    //* =====================================================================
    /// \brief Called by draw().  Derived classes must override this function
    /// in order to draw onto the passed graphics context.  A component must
    /// only draw the part of itself specified by the region.
    ///
    /// \param context the context in which the component should draw itself.
    /// \param offset the position of the parent component (if there is one)
    ///        relative to the context.  That is, (0,0) to this component
    ///        is actually (offset.x, offset.y) in the context.
    /// \param region the region relative to this component's origin that
    /// should be drawn.
    //* =====================================================================
    virtual void do_draw(
        graphics_context<element_type> &context
      , point const                    &offset
      , rectangle const                &region) = 0;
};
    
}

#endif

// ==========================================================================
// Munin ANSI Framed Component.
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
#include "munin/ansi/framed_component.hpp"
#include "munin/ansi/basic_container.hpp"
#include "munin/ansi/frame.hpp"
#include "munin/basic_layout.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/typeof/typeof.hpp>
#include <boost/make_shared.hpp>

using namespace odin;
using namespace boost;

namespace munin { namespace ansi {
    
namespace {
        
//* =========================================================================
/// \brief A specialised layout for frames that place the interior component
/// inside the frame's borders.
//* =========================================================================
class framed_component_layout
    : public munin::basic_layout<munin::ansi::element_type>
{
public :
    enum hint_type
    {
        hint_type_border
      , hint_type_interior
    };

private :
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to retrieve the preferred size of the layout
    /// in a custom manner.
    //* =====================================================================
    virtual extent do_get_preferred_size() const
    {
        extent preferred_size;
        
        for (u32 index = 0; index < get_number_of_components(); ++index)
        {
            BOOST_AUTO(comp, get_component(index));
            
            preferred_size += comp->get_preferred_size();
        }
        
        return preferred_size;
    }

    //* =====================================================================
    /// \brief Called by operator().  Derived classes must override this
    /// function in order to lay a container's components out in a custom
    /// manner.
    //* =====================================================================
    virtual void do_layout(shared_ptr<container_type> const &cont)
    {
        // This layout expects only two subcomponents: a frame, and a central
        // component.  It lays them out in the expected format.  In addition,
        BOOST_AUTO(size, cont->get_size());

        for (u32 index = 0; index < get_number_of_components(); ++index)
        {
            BOOST_AUTO(comp, get_component(index));
            BOOST_AUTO(any_hint, get_hint(index));

            BOOST_AUTO(hint, any_cast<hint_type>(any_hint));

            if (hint == hint_type_border)
            {
                comp->set_position(munin::point(0, 0));
                comp->set_size(size);
            }
            else if (hint == hint_type_interior)
            {
                comp->set_position(munin::point(1, 1));
                comp->set_size(
                    munin::extent(size.width - 2, size.height - 2));
            }
        }
    }
};

}

// ==========================================================================
// FRAMED_COMPONENT::IMPLEMENTATION_STRUCTURE
// ==========================================================================
struct framed_component::impl
{
    void on_focus_change(
        weak_ptr< munin::frame<munin::ansi::element_type> >     weak_border
      , weak_ptr< munin::component<munin::ansi::element_type> > weak_interior)
    {
        BOOST_AUTO(border,   weak_border.lock());
        BOOST_AUTO(interior, weak_interior.lock());
        
        if (border != NULL && interior != NULL)
        {
            BOOST_AUTO(focussed, interior->has_focus());
            
            attribute attr;
            attr.foreground_colour = focussed
                                   ? odin::ansi::graphics::COLOUR_CYAN
                                   : odin::ansi::graphics::COLOUR_DEFAULT;
            
            attr.intensity = focussed
                           ? odin::ansi::graphics::INTENSITY_BOLD
                           : odin::ansi::graphics::INTENSITY_NORMAL;
                           
            border->set_attribute(ATTRIBUTE_PEN, attr);
        }
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
framed_component::framed_component(
    shared_ptr< munin::frame<munin::ansi::element_type> >     border
  , shared_ptr< munin::component<munin::ansi::element_type> > interior)
    : munin::composite_component<element_type>(make_shared<basic_container>())
    , pimpl_(new impl)
{
    BOOST_AUTO(container, get_container());
    
    container->set_layout(make_shared<framed_component_layout>());
    container->add_component(
        border, framed_component_layout::hint_type_border);
    container->add_component(
        interior, framed_component_layout::hint_type_interior);
    
    interior->on_focus_set.connect(
        bind(&impl::on_focus_change
           , pimpl_.get()
           , weak_ptr< munin::frame<munin::ansi::element_type> >(border)
           , weak_ptr< munin::component<munin::ansi::element_type> >(interior)
        ));
             
    interior->on_focus_lost.connect(
        bind(&impl::on_focus_change
           , pimpl_.get()
           , weak_ptr< munin::frame<munin::ansi::element_type> >(border)
           , weak_ptr< munin::component<munin::ansi::element_type> >(interior)
        ));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
framed_component::~framed_component()
{
}

}}


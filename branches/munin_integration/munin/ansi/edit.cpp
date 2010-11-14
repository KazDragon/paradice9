// ==========================================================================
// Munin ANSI Edit Component.
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
#include "munin/ansi/edit.hpp"
#include "munin/ansi/basic_container.hpp"
#include "munin/ansi/frame.hpp"
#include "munin/ansi/text/default_singleline_document.hpp"
#include "munin/basic_layout.hpp"
#include "munin/canvas.hpp"
#include <boost/make_shared.hpp>

using namespace odin;
using namespace boost;

namespace munin { namespace ansi {

namespace detail {

class edit_renderer 
    : public munin::basic_component<munin::ansi::element_type>
{
public :
    //* =====================================================================
    /// \brief Constructor
    /// \param doc - the document that this renderer will be drawing
    //* =====================================================================
    edit_renderer(
        shared_ptr< munin::text::document<munin::ansi::element_type> > doc)
      : document_(doc)
      , cursor_state_(false)
    {
    }

    //* =====================================================================
    /// \brief Sets the cursor state.
    //* =====================================================================
    void set_cursor_state(bool state)
    {
        do_set_cursor_state(state);
    }

private :
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to get the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual extent do_get_preferred_size() const
    {
        return get_size();
    }

    //* =====================================================================
    /// \brief Called by draw().  Derived classes must override this function
    /// in order to draw onto the passed canvas.  A component must only draw 
    /// the part of itself specified by the region.
    ///
    /// \param cvs the canvas in which the component should draw itself.
    /// \param offset the position of the parent component (if there is one)
    ///        relative to the canvas.  That is, (0,0) to this component
    ///        is actually (offset.x, offset.y) in the canvas.
    /// \param region the region relative to this component's origin that
    /// should be drawn.
    //* =====================================================================
    virtual void do_draw(
        canvas<element_type> &cvs
      , point const          &offset
      , rectangle const      &region)
    {
        munin::point position = get_position();

        // DUMMY:
        for (s32 index = 0; index < region.size.width; ++index)
        {
            cvs[position.x + index + offset.x]
               [position.y + 0     + offset.y] = 
                   munin::ansi::element_type(
                       char('a' + (index % 26))
                     , munin::ansi::attribute());
        }
    }

    //* =====================================================================
    /// \brief Called by event().  Derived classes must override this 
    /// function in order to handle events in a custom manner.
    //* =====================================================================
    virtual void do_event(boost::any const &event)
    {
    }

    //* =====================================================================
    /// \brief Called by get_cursor_state().  Derived classes must override
    /// this function in order to return the cursor state in a custom manner.
    //* =====================================================================
    virtual bool do_get_cursor_state() const
    {
        return cursor_state_;
    }

    //* =====================================================================
    //* =====================================================================
    virtual void do_set_cursor_state(bool state)
    {
        cursor_state_ = state;
        on_cursor_state_changed(cursor_state_);
    }

    shared_ptr< munin::text::document<element_type> > document_;
    munin::point                                      cursor_position_;
    bool                                              cursor_state_;
    munin::extent                                     last_size_;
};

class edit_layout
    : public munin::basic_layout<munin::ansi::element_type>
{
public :
    enum hint_type
    {
        hint_type_frame
      , hint_type_renderer
    };

private :
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to retrieve the preferred size of the layout
    /// in a custom manner.
    //* =====================================================================
    virtual extent do_get_preferred_size() const
    {
        return extent(0, 0);
    }

    //* =====================================================================
    /// \brief Called by operator().  Derived classes must override this
    /// function in order to lay a container's components out in a custom
    /// manner.
    //* =====================================================================
    virtual void do_layout(
        boost::shared_ptr<container_type> const &cont)
    {
        // This layout expects only two subcomponents: a frame, and a central
        // component.  It lays them out in the expected format.  In addition,
        BOOST_AUTO(size, cont->get_size());

        for (u32 index = 0; index < get_number_of_components(); ++index)
        {
            BOOST_AUTO(comp, get_component(index));
            BOOST_AUTO(any_hint, get_hint(index));

            BOOST_AUTO(hint, any_cast<hint_type>(any_hint));

            if (hint == hint_type_frame)
            {
                comp->set_position(munin::point(0, 0));
                comp->set_size(munin::extent(size.width, 3));
            }
            else if (hint == hint_type_renderer)
            {
                comp->set_position(munin::point(1, 1));
                comp->set_size(munin::extent(size.width - 2, 1));
            }
        }
    }
};

}

struct edit::impl
{
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
edit::edit()
  : munin::composite_component<element_type>(make_shared<basic_container>())
  , pimpl_(new impl)
{
    get_container()->set_layout(
        make_shared<detail::edit_layout>());

    BOOST_AUTO(border, make_shared<frame>());
    get_container()->add_component(
        border
      , detail::edit_layout::hint_type_frame);

    BOOST_AUTO(renderer, make_shared<detail::edit_renderer>(
        make_shared<munin::ansi::text::default_singleline_document>()));

    get_container()->add_component(
        renderer
      , detail::edit_layout::hint_type_renderer);

    renderer->set_cursor_state(true);
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
edit::~edit()
{
}

}}
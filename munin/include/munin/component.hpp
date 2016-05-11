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

#include "munin/export.hpp"
#include "munin/rectangle.hpp"
#include <terminalpp/point.hpp>
#include <terminalpp/extent.hpp>
#include <boost/any.hpp>
#include <boost/signal.hpp>
#include <memory>
#include <vector>

namespace munin {

class context;

// TODO: Remove these somehow.
static const std::string ATTRIBUTE_GLYPH = "GLYPH";
static const std::string ATTRIBUTE_PEN = "PEN";

//* =========================================================================
/// \brief An object capable of being drawn on a canvas.
/// \par
/// A component is a fundamental piece of the munin architecture and is the
/// base class for anything that is capable of being drawn in some way.
//* =========================================================================
class MUNIN_EXPORT component
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    component();

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~component();

    //* =====================================================================
    /// \brief Sets the position of this component.  This does not cause a
    /// redraw, on the basis that the entity performing the move (usually
    /// a layout manager) knows about it, and is better informed about all
    /// regions redrawn.
    //* =====================================================================
    void set_position(terminalpp::point const &position);

    //* =====================================================================
    /// \brief Retrieve the position of this component.
    //* =====================================================================
    terminalpp::point get_position() const;

    //* =====================================================================
    /// \brief Sets the size of this component.  This does not cause a
    /// redraw, on the basis that the entity performing the resize (usually
    /// a layout manager) knows about it, and is better informed about all
    /// regions redrawn.  It does, however, inform an active layout to lay
    /// the components out.
    //* =====================================================================
    void set_size(terminalpp::extent const &size);

    //* =====================================================================
    /// \brief Retreives the size of this component.
    //* =====================================================================
    terminalpp::extent get_size() const;

    //* =====================================================================
    /// \brief Sets the parent of this component.
    /// \note The parent should be stored as a weak pointer so that there
    /// are no cyclic lifetimes.
    //* =====================================================================
    void set_parent(std::shared_ptr<component> const &parent);

    //* =====================================================================
    /// \brief Retrieves the parent of this component.  The pointer will be
    /// empty if this component has no parent.
    //* =====================================================================
    std::shared_ptr<component> get_parent() const;

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
    terminalpp::extent get_preferred_size() const;

    //* =====================================================================
    /// \brief Returns whether this component currently has focus.
    //* =====================================================================
    bool has_focus() const;

    //* =====================================================================
    /// \brief Sets whether this component can take focus.
    //* =====================================================================
    void set_can_focus(bool focus);

    //* =====================================================================
    /// \brief Returns whether this component can be focused.
    //* =====================================================================
    bool can_focus() const;

    //* =====================================================================
    /// \brief Sets this component to have the focus.
    //* =====================================================================
    void set_focus();

    //* =====================================================================
    /// \brief Causes this component to lose its focus.
    //* =====================================================================
    void lose_focus();

    //* =====================================================================
    /// \brief If this component has subcomponents, causes the component to
    /// move the focus from the currently focused subcomponent to the next
    /// subcomponent.  If it is a leaf component, then it toggles its focus,
    /// simulating a container with one element.
    //* =====================================================================
    void focus_next();

    //* =====================================================================
    /// \brief If this component has subcomponents, causes the component to
    /// move the focus from the currently focused subcomponent to the
    /// previous subcomponent.  If it is a leaf component, then it toggles
    /// its focus, simulating a container with one element.
    //* =====================================================================
    void focus_previous();

    //* =====================================================================
    /// \brief Returns component with focus at the deepest part of the
    /// component heirarchy, or an empty shared_ptr<> if no component has
    /// focus.
    //* =====================================================================
    std::shared_ptr<component> get_focussed_component();

    //* =====================================================================
    /// \brief Enables the component.
    //* =====================================================================
    void enable();

    //* =====================================================================
    /// \brief Disables the component.
    //* =====================================================================
    void disable();

    //* =====================================================================
    /// \brief Returns whether the component is enabled or not.
    //* =====================================================================
    bool is_enabled() const;

    //* =====================================================================
    /// \brief Returns true if this component has a visible cursor, false
    /// otherwise.
    //* =====================================================================
    bool get_cursor_state() const;

    //* =====================================================================
    /// \brief Returns the cursor's current position within the component.
    //* =====================================================================
    terminalpp::point get_cursor_position() const;

    //* =====================================================================
    /// \brief Sets the cursor's current position within the component.
    //* =====================================================================
    void set_cursor_position(terminalpp::point const &position);

    //* =====================================================================
    /// \brief Sets an implementation-specific attribute of the component.
    //* =====================================================================
    void set_attribute(std::string const &name, boost::any const &attr);

    //* =====================================================================
    /// \brief Tells the component to lay itself out.
    //* =====================================================================
    void layout();

    //* =====================================================================
    /// \brief Draws the component.
    ///
    /// \param cvs the context in which the component should draw itself.
    /// \param region the region relative to this component's origin that
    /// should be drawn.
    //* =====================================================================
    void draw(
        context         &ctx
      , rectangle const &region);

    //* =====================================================================
    /// \brief Send an event to the component.  This may be of any type.
    /// A component must specify the types of messages it may receive and
    /// what it will do with it.
    //* =====================================================================
    void event(boost::any const &event);

    //* =====================================================================
    /// \fn on_redraw
    /// \param regions The regions of the component that requires redrawing.
    /// \brief Connect to this signal in order to receive notifications about
    /// when the component should be redrawn.
    //* =====================================================================
    boost::signal
    <
        void (std::vector<rectangle> const &regions)
    > on_redraw;

    //* =====================================================================
    /// \fn on_layout_change
    /// \brief Certain component, such as containers, make requests that we
    /// update the overall layout should be done.  Connect to this signal
    /// in order to receive notifications about this.
    //* =====================================================================
    boost::signal
    <
        void ()
    > on_layout_change;

    //* =====================================================================
    /// \fn on_preferred_size_changed
    /// \brief Certain components sizes want to change during their lifetime,
    /// such as text controls that grow with the text within them.  Connect
    /// to this signal in order to receive notifications about this.
    //* =====================================================================
    boost::signal
    <
        void ()
    > on_preferred_size_changed;

    //* =====================================================================
    /// \fn on_size_changed
    /// \brief Certain components sizes change during their lifetime.
    /// Connect to this signal in order to receive notifications about this.
    //* =====================================================================
    boost::signal
    <
        void ()
    > on_size_changed;

    //* =====================================================================
    /// \fn on_position_changed
    /// \param from The position from which the component moved.
    /// \param to   The position to which the component moved.
    /// \brief Connect to this signal in order to receive notification about
    /// when the component changes position.
    //* =====================================================================
    boost::signal
    <
        void (terminalpp::point from, terminalpp::point to)
    > on_position_changed;

    //* =====================================================================
    /// \fn on_focus_set
    /// \brief Connect to this signal in order to receive notifications about
    /// when the component has gained focus.
    //* =====================================================================
    boost::signal<
        void ()
    > on_focus_set;

    //* =====================================================================
    /// \fn on_focus_lost
    /// \brief Connect to this signal in order to receive notifications about
    /// when the component has lost focus.
    //* =====================================================================
    boost::signal<
        void ()
    > on_focus_lost;

    //* =====================================================================
    /// \fn on_cursor_state_changed
    /// \brief Connect to this signal in order to receive notifications about
    /// when the component's cursor state changes.
    //* =====================================================================
    boost::signal<
        void (bool)
    > on_cursor_state_changed;

    //* =====================================================================
    /// \fn on_cursor_position_changed
    /// \brief Connect to this signal in order to receive notifications about
    /// when the component's cursor position changes.
    //* =====================================================================
    boost::signal<
        void (terminalpp::point)
    > on_cursor_position_changed;

protected :
    //* =====================================================================
    /// \brief Called by set_position().  Derived classes must override this
    /// function in order to set the position of the component in a custom
    /// manner.
    //* =====================================================================
    virtual void do_set_position(terminalpp::point const &position) = 0;

    //* =====================================================================
    /// \brief Called by get_position().  Derived classes must override this
    /// function in order to get the position of the component in a custom
    /// manner.
    //* =====================================================================
    virtual terminalpp::point do_get_position() const = 0;

    //* =====================================================================
    /// \brief Called by set_size().  Derived classes must override this
    /// function in order to set the size of the component in a custom
    /// manner.
    //* =====================================================================
    virtual void do_set_size(terminalpp::extent const &size) = 0;

    //* =====================================================================
    /// \brief Called by get_size().  Derived classes must override this
    /// function in order to get the size of the component in a custom
    /// manner.
    //* =====================================================================
    virtual terminalpp::extent do_get_size() const = 0;

    //* =====================================================================
    /// \brief Called by set_parent().  Derived classes must override this
    /// function in order to set the parent of the component in a custom
    /// manner.
    //* =====================================================================
    virtual void do_set_parent(std::shared_ptr<component> const &parent) = 0;

    //* =====================================================================
    /// \brief Called by get_parent().  Derived classes must override this
    /// function in order to get the parent of the component in a custom
    /// manner.
    //* =====================================================================
    virtual std::shared_ptr<component> do_get_parent() const = 0;

    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to get the size of the component in a custom
    /// manner.
    //* =====================================================================
    virtual terminalpp::extent do_get_preferred_size() const = 0;

    //* =====================================================================
    /// \brief Called by has_focus().  Derived classes must override this
    /// function in order to return whether this component has focus in a
    /// custom manner.
    //* =====================================================================
    virtual bool do_has_focus() const = 0;

    //* =====================================================================
    /// \brief Called by set_can_focus().  Derived classes must override this
    /// function in order to set whether this component can be focussed in
    /// a custom manner.
    //* =====================================================================
    virtual void do_set_can_focus(bool focus) = 0;

    //* =====================================================================
    /// \brief Called by can_focus().  Derived classes must override this
    /// function in order to return whether this component can be focused in
    /// a custom manner.
    //* =====================================================================
    virtual bool do_can_focus() const = 0;

    //* =====================================================================
    /// \brief Called by set_focus().  Derived classes must override this
    /// function in order to set the focus to this component in a custom
    /// manner.
    //* =====================================================================
    virtual void do_set_focus() = 0;

    //* =====================================================================
    /// \brief Called by lose_focus().  Derived classes must override this
    /// function in order to lose the focus from this component in a
    /// custom manner.
    //* =====================================================================
    virtual void do_lose_focus() = 0;

    //* =====================================================================
    /// \brief Called by focus_next().  Derived classes must override this
    /// function in order to move the focus in a custom manner.
    //* =====================================================================
    virtual void do_focus_next() = 0;

    //* =====================================================================
    /// \brief Called by focus_previous().  Derived classes must override
    /// this function in order to move the focus in a custom manner.
    //* =====================================================================
    virtual void do_focus_previous() = 0;

    //* =====================================================================
    /// \brief Called by get_focussed_component().  Derived classes must
    /// override this function in order to return the focussed component
    /// in a custom manner.
    //* =====================================================================
    virtual std::shared_ptr<component> do_get_focussed_component() = 0;

    //* =====================================================================
    /// \brief Called by enable().  Derived classes must override this
    /// function in order to disable the component in a custom manner.
    //* =====================================================================
    virtual void do_enable() = 0;

    //* =====================================================================
    /// \brief Called by disable().  Derived classes must override this
    /// function in order to disable the component in a custom manner.
    //* =====================================================================
    virtual void do_disable() = 0;

    //* =====================================================================
    /// \brief Called by is_enabled().  Derived classes must override this
    /// function in order to return whether the component is disabled or not
    /// in a custom manner.
    //* =====================================================================
    virtual bool do_is_enabled() const = 0;

    //* =====================================================================
    /// \brief Called by get_cursor_state().  Derived classes must override
    /// this function in order to return the cursor state in a custom manner.
    //* =====================================================================
    virtual bool do_get_cursor_state() const = 0;

    //* =====================================================================
    /// \brief Called by get_cursor_position().  Derived classes must
    /// override this function in order to return the cursor position in
    /// a custom manner.
    //* =====================================================================
    virtual terminalpp::point do_get_cursor_position() const = 0;

    //* =====================================================================
    /// \brief Called by set_cursor_position().  Derived classes must
    /// override this function in order to set the cursor position in
    /// a custom manner.
    //* =====================================================================
    virtual void do_set_cursor_position(terminalpp::point const &position) = 0;

    //* =====================================================================
    /// \brief Called by set_attribute().  Derived classes must override this
    /// function in order to set an attribute in a custom manner.
    //* =====================================================================
    virtual void do_set_attribute(
        std::string const &name, boost::any const &attr) = 0;

    //* =====================================================================
    /// \brief Called by layout().  Derived classes must override this
    /// function in order to lay the component out.  If the component
    /// contains subcomponents, these must also be laid out.
    //* =====================================================================
    virtual void do_layout() = 0;

    //* =====================================================================
    /// \brief Called by draw().  Derived classes must override this function
    /// in order to draw onto the passed context.  A component must only draw
    /// the part of itself specified by the region.
    ///
    /// \param ctx the context in which the component should draw itself.
    /// \param region the region relative to this component's origin that
    /// should be drawn.
    //* =====================================================================
    virtual void do_draw(
        context         &ctx
      , rectangle const &region) = 0;

    //* =====================================================================
    /// \brief Called by event().  Derived classes must override this
    /// function in order to handle events in a custom manner.
    //* =====================================================================
    virtual void do_event(boost::any const &event) = 0;
};

}

#endif


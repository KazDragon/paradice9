// ==========================================================================
// Munin Vertical Scroll Bar Component.
//
// Copyright (C) 2011 Matthew Chaplain, All Rights Reserved.
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
#ifndef MUNIN_VERTICAL_SCROLL_BAR_HPP_
#define MUNIN_VERTICAL_SCROLL_BAR_HPP_

#include "munin/basic_component.hpp"
#include "odin/core.hpp"
#include <boost/optional.hpp>

namespace munin {

//* =========================================================================
/// \brief A class that models a vertical scroll bar.
//* =========================================================================
class MUNIN_EXPORT vertical_scroll_bar : public basic_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    /// \param elements - a multidimentional array with each element
    /// representing one vertical line of the vertical_scroll_bar.
    //* =====================================================================
    vertical_scroll_bar();

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~vertical_scroll_bar();

    //* =====================================================================
    /// \fn on_page_up
    /// \brief Connect to this signal in order to be informed of a control
    /// that causes the scrollbar to want to be paged up.
    //* =====================================================================
    boost::signal<
        void()
    > on_page_up;

    //* =====================================================================
    /// \fn on_page_down
    /// \brief Connect to this signal in order to be informed of a control
    /// that causes the scrollbar to want to be paged down.
    //* =====================================================================
    boost::signal<
        void()
    > on_page_down;

    //* =====================================================================
    /// \brief Gets the position that the slider is in.  This is expressed
    /// as an optional percentage where 0 is the absolute left, 100 is the
    /// absolute right, and 1..99 is somewhere inbetween.  No percentage
    /// (the default) indicates that the slider is not be present.
    //* =====================================================================
    boost::optional<odin::u8> get_slider_position() const;

    //* =====================================================================
    /// \brief Sets the position that the slider is in.  This is expressed
    /// as an optional percentage where 0 is the absolute left, 100 is the
    /// absolute right, and 1..99 is somewhere inbetween.  No percentage
    /// (the default) indicates that the slider should not be present.
    //* =====================================================================
    void set_slider_position(boost::optional<odin::u8> percentage);

protected :
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to get the size of the component in a custom
    /// manner.
    //* =====================================================================
    virtual terminalpp::extent do_get_preferred_size() const override;

    //* =====================================================================
    /// \brief Called by set_size().  Derived classes must override this
    /// function in order to set the size of the component in a custom
    /// manner.
    //* =====================================================================
    virtual void do_set_size(terminalpp::extent const &size) override;

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
      , rectangle const &region) override;

    //* =====================================================================
    /// \brief Called by event().  Derived classes must override this
    /// function in order to handle events in a custom manner.
    //* =====================================================================
    virtual void do_event(boost::any const &event) override;

    //* =====================================================================
    /// \brief Called by set_attribute().  Derived classes must override this
    /// function in order to set an attribute in a custom manner.
    //* =====================================================================
    virtual void do_set_attribute(
        std::string const &name, boost::any const &attr) override;

private :
    struct impl;
    std::shared_ptr<impl> pimpl_;
};

//* =========================================================================
/// \brief Returns a newly created vertical scroll bar
//* =========================================================================
MUNIN_EXPORT
std::shared_ptr<vertical_scroll_bar> make_vertical_scroll_bar();

}

#endif


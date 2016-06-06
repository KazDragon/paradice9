// ==========================================================================
// Munin Filled Box Component.
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
#ifndef MUNIN_FILLED_BOX_HPP_
#define MUNIN_FILLED_BOX_HPP_

#include "munin/basic_component.hpp"
#include <terminalpp/element.hpp>

namespace munin {

// Changing this boolean attribute to true will stop other attributes from
// changing until the attribute is once again false.
static const std::string ATTRIBUTE_LOCK = "Lock";

//* =========================================================================
/// \brief A class that models a box that is always completely filled with
/// a given element.  It allows changing the attribute and glyph of the
/// element independently.
//* =========================================================================
class MUNIN_EXPORT filled_box : public munin::basic_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    filled_box(terminalpp::element const &element);

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~filled_box();

    //* =====================================================================
    /// \brief Sets the preferred size of this box.  The default is (1,1).
    //* =====================================================================
    void set_preferred_size(terminalpp::extent preferred_size);

    //* =====================================================================
    /// \brief Sets the element with which the box is filled.
    //* =====================================================================
    void set_fill(terminalpp::element const &element);

    //* =====================================================================
    /// \brief Gets the element with which the box is filled.
    //* =====================================================================
    terminalpp::element get_fill() const;

protected :
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to get the size of the component in a custom
    /// manner.
    //* =====================================================================
    virtual terminalpp::extent do_get_preferred_size() const;

    //* =====================================================================
    /// \brief Called by set_attribute().  Derived classes must override this
    /// function in order to set an attribute in a custom manner.
    //* =====================================================================
    virtual void do_set_attribute(
        std::string const &name, boost::any const &attr);

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
      , rectangle const &region);

private :
    struct impl;
    std::shared_ptr<impl> pimpl_;
};

//* =========================================================================
/// \brief Returns a newly created filled box.
//* =========================================================================
MUNIN_EXPORT
std::shared_ptr<filled_box> make_fill(terminalpp::element const &fill);

}

#endif


// ==========================================================================
// Munin Framed Component.
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
#ifndef MUNIN_FRAMED_COMPONENT_HPP_
#define MUNIN_FRAMED_COMPONENT_HPP_

#include "munin/composite_component.hpp"
#include "munin/frame.hpp"

namespace munin {

BOOST_STATIC_CONSTANT(std::string,
    FOCUSSED_BORDER_PEN = "Focussed.Border.Pen");
BOOST_STATIC_CONSTANT(std::string,
    UNFOCUSSED_BORDER_PEN = "Unfocussed.Border.Pen");

//* =========================================================================
/// \brief A class that models a multi-line text control with a frame
/// bordering it.
//* =========================================================================
class MUNIN_EXPORT framed_component
  : public munin::composite_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    framed_component(
        std::shared_ptr<frame>     const &border
      , std::shared_ptr<component> const &interior);

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~framed_component();

protected :
    //* =====================================================================
    /// \brief Called by set_attribute().  Derived classes must override this
    /// function in order to set an attribute in a custom manner.
    //* =====================================================================
    virtual void do_set_attribute(
        std::string const &name, boost::any const &attr);

    //* =====================================================================
    /// \brief Called by event().  Derived classes must override this
    /// function in order to handle events in a custom manner.
    //* =====================================================================
    virtual void do_event(boost::any const &event);

    struct impl;
    std::shared_ptr<impl> pimpl_;
};

//* =========================================================================
/// \brief Returns a newly created framed component
//* =========================================================================
MUNIN_EXPORT
std::shared_ptr<component> make_framed_component(
    std::shared_ptr<frame>     const &border,
    std::shared_ptr<component> const &interior);

}

#endif

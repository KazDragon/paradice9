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

#include "munin/export.hpp"
#include "odin/core.hpp"
#include <terminalpp/extent.hpp>
#include <boost/any.hpp>
#include <boost/optional.hpp>
#include <memory>
#include <vector>

namespace munin {

class component;

//* =========================================================================
/// \brief A class that knows how to lay components out in a container in
/// a specified manner.
//* =========================================================================
class MUNIN_EXPORT layout
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    layout();

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~layout();

    //* =====================================================================
    /// \brief Returns the preferred size of this layout.
    /// \par
    /// The preferred size of the layout is the size that the layout would be
    /// if all components in it were at their preferred sizes.
    //* =====================================================================
    terminalpp::extent get_preferred_size(
        std::vector<std::shared_ptr<component>> const &components,
        std::vector<boost::any>                 const &hints) const;

    //* =====================================================================
    /// \brief Performs a layout of the specified components within the
    /// specified bounds.
    //* =====================================================================
    void operator()(
        std::vector<std::shared_ptr<component>> const &components,
        std::vector<boost::any>                 const &hints,
        terminalpp::extent                             size);

protected :
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to retrieve the preferred size of the layout
    /// in a custom manner.
    //* =====================================================================
    virtual terminalpp::extent do_get_preferred_size(
        std::vector<std::shared_ptr<component>> const &components,
        std::vector<boost::any>                 const &hints) const = 0;

    //* =====================================================================
    /// \brief Called by operator().  Derived classes must override this
    /// function in order to lay a container's components out in a custom
    /// manner.
    //* =====================================================================
    virtual void do_layout(
        std::vector<std::shared_ptr<component> > const &components,
        std::vector<boost::any>                  const &hints,
        terminalpp::extent                              size) = 0;
};

}

#endif


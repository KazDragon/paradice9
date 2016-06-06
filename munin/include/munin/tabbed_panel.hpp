// ==========================================================================
// Munin Tabbed Panel.
//
// Copyright (C) 2013 Matthew Chaplain, All Rights Reserved.
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
#ifndef MUNIN_TABBED_PANEL_HPP_
#define MUNIN_TABBED_PANEL_HPP_

#include "munin/composite_component.hpp"
#include "odin/core.hpp"
#include <boost/optional.hpp>
#include <string>

namespace munin {

//* =========================================================================
/// \brief A class that constructs a tabbed panel.
//* =========================================================================
class MUNIN_EXPORT tabbed_panel : public munin::composite_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    tabbed_panel();

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~tabbed_panel();

    //* =====================================================================
    /// \brief Inserts a tab into the frame.
    /// \par text The text of the tab.  This may be specified using the ANSI
    /// markup notation.  It will be the identifier of the tab when called
    /// back via the on_tab_selected() signal.
    /// \par comp A component to display on this tab.
    /// \par index Optionally, the index at which the tab should be inserted.
    /// If this is not specified, then it is inserted as the right-most tab.
    //* =====================================================================
    void insert_tab(
        std::string                       const &text,
        std::shared_ptr<munin::component> const &comp,
        boost::optional<odin::u32>               index = {});

    //* =====================================================================
    /// \brief Removes a tab from the frame.
    /// \par index The index at which the tab should be removed.
    //* =====================================================================
    void remove_tab(odin::u32 index);

private :
    struct impl;
    std::shared_ptr<impl> pimpl_;
};

//* =========================================================================
/// \brief Returns a newly created tabbed panel
//* =========================================================================
MUNIN_EXPORT
std::shared_ptr<tabbed_panel> make_tabbed_panel();

}

#endif

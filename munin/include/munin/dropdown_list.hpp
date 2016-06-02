// ==========================================================================
// Munin Drop-Down List Component.
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
#ifndef MUNIN_DROPDOWN_LIST_HPP_
#define MUNIN_DROPDOWN_LIST_HPP_

#include "munin/export.hpp"
#include "munin/composite_component.hpp"
#include "odin/core.hpp"

namespace terminalpp {
    class string;
}

namespace munin {

//* =========================================================================
/// \brief A class that models a dropdown_list of items.
//* =========================================================================
class MUNIN_EXPORT dropdown_list : public munin::composite_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    dropdown_list();

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~dropdown_list();

    //* =====================================================================
    /// \brief Sets the items in the drop-down list.
    //* =====================================================================
    void set_items(std::vector<terminalpp::string> const &items);

    //* =====================================================================
    /// \brief Selects an item with the given index.
    /// \param selected_item the item to select, or -1 for no item.
    //* =====================================================================
    void set_item_index(odin::s32 index);

    //* =====================================================================
    /// \brief Gets the index of the currently selected item, or -1 if no
    /// item is selected.
    //* =====================================================================
    odin::s32 get_item_index() const;

    //* =====================================================================
    /// \brief Gets the value of the currently selected item.
    //* =====================================================================
    terminalpp::string get_item() const;

    //* =====================================================================
    /// \fn on_item_changed
    /// \brief Connect to this signal to receive updates about when the
    /// selected item changes.
    //* =====================================================================
    boost::signal<
        void (odin::s32)
    > on_item_changed;

protected :
    //* =====================================================================
    /// \brief Called by event().  Derived classes must override this
    /// function in order to handle events in a custom manner.
    //* =====================================================================
    virtual void do_event(boost::any const &event);

private :
    struct impl;
    std::shared_ptr<impl> pimpl_;
};

//* =========================================================================
/// \brief Returns a newly created dropdown list
//* =========================================================================
MUNIN_EXPORT
std::shared_ptr<dropdown_list> make_dropdown_list();

}

#endif


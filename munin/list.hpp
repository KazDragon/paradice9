// ==========================================================================
// Munin List Component.
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
#ifndef MUNIN_LIST_HPP_
#define MUNIN_LIST_HPP_

#include "munin/basic_component.hpp"
#include <boost/shared_ptr.hpp>

namespace munin {

//* =========================================================================
/// \brief A class that models a list of items.
//* =========================================================================
class list : public munin::basic_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    list();

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~list();

    //* =====================================================================
    /// \brief Sets the items in the list.
    //* =====================================================================
    void set_items(std::vector< std::vector<element_type> > items);

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
    std::vector<element_type> get_item() const;

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
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to get the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual extent do_get_preferred_size() const;
    
    //* =====================================================================
    /// \brief Called by get_cursor_position().  Derived classes must
    /// override this function in order to return the cursor position in
    /// a custom manner.
    //* =====================================================================
    virtual point do_get_cursor_position() const;

    //* =====================================================================
    /// \brief Called by set_cursor_position().  Derived classes must
    /// override this function in order to set the cursor position in
    /// a custom manner.
    //* =====================================================================
    virtual void do_set_cursor_position(point const &position);

    //* =====================================================================
    /// \brief Called by draw().  Derived classes must override this function
    /// in order to draw onto the passed canvas.  A component must only draw 
    /// the part of itself specified by the region.
    ///
    /// \param cvs the canvas in which the component should draw itself.
    /// \param region the region relative to this component's origin that
    /// should be drawn.
    //* =====================================================================
    virtual void do_draw(
        canvas          &cvs
      , rectangle const &region);
    
    //* =====================================================================
    /// \brief Called by event().  Derived classes must override this 
    /// function in order to handle events in a custom manner.
    //* =====================================================================
    virtual void do_event(boost::any const &event);
    
private :
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}

#endif


// ==========================================================================
// Hugin Encounters Page
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
#ifndef HUGIN_ENCOUNTERS_PAGE_HPP_
#define HUGIN_ENCOUNTERS_PAGE_HPP_

#include "hugin/model/encounter.hpp"
#include "munin/composite_component.hpp"
#include <boost/optional.hpp>
#include <vector>

namespace hugin {

//* =========================================================================
/// \brief The page for the encounters tab in the encounter wizard.
//* =========================================================================
class encounters_page : public munin::composite_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    encounters_page();
    
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    ~encounters_page();
    
    //* =====================================================================
    /// \brief Sets the encounters visible on this page
    //* =====================================================================
    void set_encounters(
        std::vector<hugin::model::encounter> const &encounters);

    //* =====================================================================
    /// \brief Retrieves the encounters visible on this page
    //* =====================================================================
    std::vector<hugin::model::encounter> get_encounters() const;

    //* =====================================================================
    /// \brief Retrieves the currently selected encounter.
    //* =====================================================================
    boost::optional<hugin::model::encounter> get_selected_encounter() const;

    //* =====================================================================
    /// \fn on_new
    /// \brief Called when the 'new' button is pressed.
    //* =====================================================================
    boost::signal<void ()> on_new;

    //* =====================================================================
    /// \fn on_clone
    /// \brief Called when the 'clone' button is pressed.
    //* =====================================================================
    boost::signal<void (hugin::model::encounter const &)> on_clone;

    //* =====================================================================
    /// \fn on_edit
    /// \brief Called when the 'edit' button is pressed.
    //* =====================================================================
    boost::signal<void (hugin::model::encounter const &)> on_edit;

    //* =====================================================================
    /// \fn on_fight
    /// \brief Called when the 'fight!' button is pressed.
    //* =====================================================================
    boost::signal<void (hugin::model::encounter const &)> on_fight;

    //* =====================================================================
    /// \fn on_delete
    /// \brief Called when the 'delete' button is pressed.
    //* =====================================================================
    boost::signal<void (hugin::model::encounter const &)> on_delete;

private :
    struct impl;
    std::shared_ptr<impl> pimpl_;
};

}

#endif

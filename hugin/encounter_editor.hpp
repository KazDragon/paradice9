// ==========================================================================
// Hugin Encounter Editor
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
#ifndef HUGIN_ENCOUNTER_EDITOR_HPP_
#define HUGIN_ENCOUNTER_EDITOR_HPP_

#include "munin/composite_component.hpp"

namespace paradice {
    class beast;
}

namespace hugin {

//* =========================================================================
/// \brief The page for editing encounters in the encounter wizard.
//* =========================================================================
class encounter_editor : public munin::composite_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    encounter_editor();
    
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    ~encounter_editor();

    //* =====================================================================
    /// \brief Sets the name of the encounter that the editor is working with.
    //* =====================================================================
    void set_encounter_name(std::string const &name);

    //* =====================================================================
    /// \brief Retrieves the name of the encounter that the editor is working 
    /// with.
    //* =====================================================================
    std::string get_encounter_name() const;

    //* =====================================================================
    /// \brief Sets the bestiary that the editor is working with.
    //* =====================================================================
    void set_bestiary(
        std::vector<std::shared_ptr<paradice::beast>> const &beasts);

    //* =====================================================================
    /// \brief Sets the beasts of the encounter that the editor is working
    /// with.
    //* =====================================================================
    void set_encounter_beasts(
        std::vector<std::shared_ptr<paradice::beast>> const &beasts);

    //* =====================================================================
    /// \brief Retrieves the beasts of the encounter that the editor is
    /// working with.
    //* =====================================================================
    std::vector<std::shared_ptr<paradice::beast>> get_encounter_beasts() const;

    //* =====================================================================
    /// \fn on_revert
    /// \brief Called when the revert button is pressed.
    //* =====================================================================
    boost::signal<void ()> on_revert;

    //* =====================================================================
    /// \fn on_save
    /// \brief Called when the save button is pressed.
    //* =====================================================================
    boost::signal<void ()> on_save;

private :
    struct impl;
    std::shared_ptr<impl> pimpl_;
};

}

#endif

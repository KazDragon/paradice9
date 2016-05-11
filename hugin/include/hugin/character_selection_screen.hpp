// ==========================================================================
// Hugin Character Selection Screen
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
#ifndef HUGIN_CHARACTER_SELECTION_SCREEN_HPP_
#define HUGIN_CHARACTER_SELECTION_SCREEN_HPP_

#include "munin/composite_component.hpp"
#include <string>

namespace hugin {

//* =========================================================================
/// \brief An abstraction of the primary user interface for the Paradice
/// application.
//* =========================================================================
class character_selection_screen
    : public munin::composite_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    character_selection_screen();
    
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~character_selection_screen();
    
    //* =====================================================================
    /// \brief Clears the screen of all data.
    //* =====================================================================
    void clear();
    
    //* =====================================================================
    /// \brief Sets the character names belonging to this account.
    /// \param names a list of pairs of strings, where the first element
    /// is the given name of the character, and the second is the full mode
    /// of address (e.g. ["Dave", "Dave the Black Ork"] 
    //* =====================================================================
    void set_character_names(
        std::vector<
            std::pair<std::string, std::string> 
        > const &names);

    //* =====================================================================
    /// \brief Provide a function to be called if the user opts to create
    /// a new character.
    //* =====================================================================
    boost::signal<void ()> on_new_character;
    
    //* =====================================================================
    /// \brief Provide a function to be called if the user opts to use an
    /// existing character.
    //* =====================================================================
    boost::signal<void (std::string const &name)> on_character_selected;
    
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

}

#endif

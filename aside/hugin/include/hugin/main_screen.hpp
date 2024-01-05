// ==========================================================================
// Hugin Main Screen
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
#ifndef HUGIN_MAIN_SCREEN_HPP_
#define HUGIN_MAIN_SCREEN_HPP_

#include "munin/composite_component.hpp"
#include "paradice/active_encounter.hpp"
#include <string>

namespace terminalpp {
    class string;
}

namespace hugin {

//* =========================================================================
/// \brief An abstraction of the primary user interface for the Paradice
/// application.
//* =========================================================================
class main_screen
    : public munin::composite_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    main_screen();
    
    //* =====================================================================
    /// \brief Clears the screen of all data.
    //* =====================================================================
    void clear();
    
    //* =====================================================================
    /// \brief Set a function to be called when the user inputs a command
    /// on the main screen.
    //* =====================================================================
    boost::signals2::signal<void (std::string const &input)> on_input_entered;
    
    //* =====================================================================
    /// \brief Adds output to the output text area on the main screen.
    //* =====================================================================
    void add_output_text(terminalpp::string const &text);
    
    //* =====================================================================
    /// \brief Updates the who list on the main screen.
    //* =====================================================================
    void update_wholist(std::vector<std::string> const &names);
    
    //* =====================================================================
    /// \brief Adds a line of command history to the input pane.
    //* =====================================================================
    void add_command_history(std::string const &history);

    //* =====================================================================
    /// \brief Shows the Help window.
    //* =====================================================================
    void show_help_window();
    
    //* =====================================================================
    /// \brief Hides the Help window.
    //* =====================================================================
    void hide_help_window();
    
    //* =====================================================================
    /// \brief Shows the active encounter window.
    //* =====================================================================
    void show_active_encounter_window();

    //* =====================================================================
    /// \brief Hides the active encounter window.
    //* =====================================================================
    void hide_active_encounter_window();

    //* =====================================================================
    /// \brief Sets the Active Encounter.
    //* =====================================================================
    void set_active_encounter(
        std::shared_ptr<paradice::active_encounter> const &active_encounter);

    //* =====================================================================
    /// \brief Sets the text contained in the Help window.
    //* =====================================================================
    void set_help_window_text(
        terminalpp::string const &text);

    //* =====================================================================
    /// \brief Register a callback for when the close button on the help
    /// screen is called.
    //* =====================================================================
    boost::signals2::signal<void ()> on_help_closed;

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


// ==========================================================================
// Hugin User Interface
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
#ifndef HUGIN_USER_INTERFACE_HPP_
#define HUGIN_USER_INTERFACE_HPP_

#include "hugin/export.hpp"
#include "munin/composite_component.hpp"
#include "paradice/active_encounter.hpp"
#include <boost/asio/io_context.hpp>

namespace terminalpp {
    class string;
}

namespace paradice {
    class beast;
    class encounter;
}

namespace hugin {

BOOST_STATIC_CONSTANT(std::string, FACE_INTRO            = "Intro");
BOOST_STATIC_CONSTANT(std::string, FACE_ACCOUNT_CREATION = "AcctCreate");
BOOST_STATIC_CONSTANT(std::string, FACE_CHAR_SELECTION   = "CharSelect");
BOOST_STATIC_CONSTANT(std::string, FACE_CHAR_CREATION    = "CharCreate");
BOOST_STATIC_CONSTANT(std::string, FACE_PASSWORD_CHANGE  = "PwdChange");
BOOST_STATIC_CONSTANT(std::string, FACE_MAIN             = "Main");
BOOST_STATIC_CONSTANT(std::string, FACE_GM_TOOLS         = "GM Tools");

//* =========================================================================
/// \brief An abstraction of the primary user interface for the Paradice
/// application.
//* =========================================================================
class HUGIN_EXPORT user_interface
    : public munin::composite_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    user_interface(boost::asio::io_context::strand &strand);

    //* =====================================================================
    /// \brief Clears the intro screen.
    //* =====================================================================
    void clear_intro_screen();

    //* =====================================================================
    /// \brief Clears the account creation screen.
    //* =====================================================================
    void clear_account_creation_screen();

    //* =====================================================================
    /// \brief Clears the character selection screen.
    //* =====================================================================
    void clear_character_selection_screen();

    //* =====================================================================
    /// \brief Clears the character creation screen.
    //* =====================================================================
    void clear_character_creation_screen();

    //* =====================================================================
    /// \brief Clears the main screen.
    //* =====================================================================
    void clear_main_screen();

    //* =====================================================================
    /// \brief Clears the password change screen.
    //* =====================================================================
    void clear_password_change_screen();

    //* =====================================================================
    /// \brief Set a function to be called when the user inputs the details
    /// for the change of a password.
    //* =====================================================================
    boost::signals2::signal<
        void (std::string const &old_password,
              std::string const &new_password,
              std::string const &new_password_verify)> on_password_changed;

    //* =====================================================================
    /// \brief Set a function to be called when the user cancels the change
    /// of a password.
    //* =====================================================================
    boost::signals2::signal<void ()> on_password_change_cancelled;

    //* =====================================================================
    /// \brief Set a function to be called when the user inputs a name
    /// and password on the intro screen.
    //* =====================================================================
    boost::signals2::signal<
        void (std::string const &username,
              std::string const &hashed_password)> on_login;

    //* =====================================================================
    /// \brief Set a function to be called when the user wants to create
    /// a new account.
    //* =====================================================================
    boost::signals2::signal<void ()> on_new_account;

    //* =====================================================================
    /// \brief Set a function to be called when the user inputs the details
    /// for the creation of an account.
    //* =====================================================================
    boost::signals2::signal<
        void (std::string const &account_name,
              std::string const &password,
              std::string const &password_verify)> on_account_created;
              
    //* =====================================================================
    /// \brief Set a function to be called when the user cancels the creation
    /// of an account.
    //* =====================================================================
    boost::signals2::signal<void ()> on_account_creation_cancelled;

    //* =====================================================================
    /// \brief Set a function to be called when the user inputs a command
    /// on the main screen.
    //* =====================================================================
    boost::signals2::signal<void (std::string const &input)> on_input_entered;

    //* =====================================================================
    /// \brief Provide a function to be called if the user opts to create
    /// a new character.o
    //* =====================================================================
    boost::signals2::signal<void ()> on_new_character;
    
    //* =====================================================================
    /// \brief Provide a function to be called if the user opts to use an
    /// existing character.
    //* =====================================================================
    boost::signals2::signal<void (std::string const &name)> on_character_selected;

    //* =====================================================================
    /// \brief Provide a function to be called if the user creates a new
    /// character.
    //* =====================================================================
    boost::signals2::signal<
        void (std::string const &name, bool is_gm)> on_character_created;

    //* =====================================================================
    /// \brief Provide a function to be called if the user decides to cancel
    /// the creation of a character.
    //* =====================================================================
    boost::signals2::signal<void ()> on_character_creation_cancelled;

    //* =====================================================================
    /// \brief Provide a function to be called if the user hits the 'back'
    /// button on the GM Tools screen
    //* =====================================================================
    boost::signals2::signal<void ()> on_gm_tools_back;

    //* =====================================================================
    /// \brief Provide a function to be called if the user inserts a
    /// beast into the current encounter.
    //* =====================================================================
    boost::signals2::signal<
        void (std::shared_ptr<paradice::beast> const &)> on_gm_fight_beast;

    //* =====================================================================
    /// \brief Provide a function to be called if the user inserts an
    /// encounter into the current encounter.
    //* =====================================================================
    boost::signals2::signal<
        void (std::shared_ptr<paradice::encounter> const &)
    > on_gm_fight_encounter;

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
    /// \brief Sets the character names belonging to this account.
    //* =====================================================================
    void set_character_names(
        std::vector<
            std::pair<std::string, std::string>
        > const &names);

    //* =====================================================================
    /// \brief Sets the beasts to be used by the user interface
    //* =====================================================================
    void set_beasts(
        std::vector<std::shared_ptr<paradice::beast>> const &beasts);

    //* =====================================================================
    /// \brief Returns the beasts used by the user interface
    //* =====================================================================
    std::vector<std::shared_ptr<paradice::beast>> get_beasts() const;

    //* =====================================================================
    /// \brief Sets the encounters to be used by the user interface
    //* =====================================================================
    void set_encounters(
        std::vector<std::shared_ptr<paradice::encounter>> const &encounters);

    //* =====================================================================
    /// \brief Returns the encounters used by the user interface
    //* =====================================================================
    std::vector<std::shared_ptr<paradice::encounter>> get_encounters() const;

    //* =====================================================================
    /// \brief Select a user interface screen to be shown.
    //* =====================================================================
    void select_face(std::string const &face_name);

    //* =====================================================================
    /// \brief Adds output to the output text area on the main screen.
    //* =====================================================================
    void add_output_text(terminalpp::string const &text);

    //* =====================================================================
    /// \brief Sets the content of the status bar on the intro screen.
    //* =====================================================================
    void set_statusbar_text(terminalpp::string const &text);

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
    /// \brief Set up a callback for when the close icon on the help
    /// window is clicked.
    //* =====================================================================
    boost::signals2::signal<void ()> on_help_closed;

    //* =====================================================================
    /// \brief Sets the text contained in the Help window.
    //* =====================================================================
    void set_help_window_text(terminalpp::string const &text);

private :
    struct impl;
    std::shared_ptr<impl> pimpl_;
};

}

#endif

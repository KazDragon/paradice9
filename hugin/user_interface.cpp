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
#include "user_interface.hpp"
#include "hugin/account_creation_screen.hpp"
#include "hugin/character_creation_screen.hpp"
#include "hugin/character_selection_screen.hpp"
#include "hugin/intro_screen.hpp"
#include "hugin/main_screen.hpp"
#include "hugin/password_change_screen.hpp"
#include "munin/basic_container.hpp"
#include "munin/card.hpp"
#include "munin/grid_layout.hpp"
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace munin;
using namespace odin;
using namespace boost;
using namespace std;

namespace hugin {

// ==========================================================================
// USER_INTERFACE::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct user_interface::impl
{
    shared_ptr<card>                       active_screen_;
    string                                 active_face_;                  
    
    shared_ptr<intro_screen>               intro_screen_;   
    shared_ptr<account_creation_screen>    account_creation_screen_;
    shared_ptr<character_selection_screen> character_selection_screen_;
    shared_ptr<character_creation_screen>  character_creation_screen_;
    shared_ptr<main_screen>                main_screen_;
    shared_ptr<password_change_screen>     password_change_screen_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
user_interface::user_interface()
    : composite_component(make_shared<basic_container>())
    , pimpl_(make_shared<impl>())
{
    pimpl_->active_screen_              = make_shared<card>();
    pimpl_->intro_screen_               = make_shared<intro_screen>();
    pimpl_->account_creation_screen_    = make_shared<account_creation_screen>();
    pimpl_->character_creation_screen_  = make_shared<character_creation_screen>();
    pimpl_->character_selection_screen_ = make_shared<character_selection_screen>();
    pimpl_->main_screen_                = make_shared<main_screen>();
    pimpl_->password_change_screen_     = make_shared<password_change_screen>();
    
    pimpl_->active_screen_->add_face(
        pimpl_->intro_screen_, hugin::FACE_INTRO);
    pimpl_->active_screen_->add_face(
        pimpl_->account_creation_screen_, hugin::FACE_ACCOUNT_CREATION);
    pimpl_->active_screen_->add_face(
        pimpl_->character_selection_screen_, hugin::FACE_CHAR_SELECTION);
    pimpl_->active_screen_->add_face(
        pimpl_->character_creation_screen_, hugin::FACE_CHAR_CREATION);
    pimpl_->active_screen_->add_face(
        pimpl_->main_screen_, hugin::FACE_MAIN);
    pimpl_->active_screen_->add_face(
        pimpl_->password_change_screen_, hugin::FACE_PASSWORD_CHANGE);
    
    pimpl_->active_screen_->select_face(hugin::FACE_INTRO);
    pimpl_->active_face_ = hugin::FACE_INTRO;

    BOOST_AUTO(container, get_container());
    container->set_layout(make_shared<grid_layout>(1, 1));
    container->add_component(pimpl_->active_screen_);
}

// ==========================================================================
// CLEAR_INTRO_SCREEN
// ==========================================================================
void user_interface::clear_intro_screen()
{
    pimpl_->intro_screen_->clear();
}

// ==========================================================================
// CLEAR_ACCOUNT_CREATION_SCREEN
// ==========================================================================
void user_interface::clear_account_creation_screen()
{
    pimpl_->account_creation_screen_->clear();
}

// ==========================================================================
// CLEAR_CHARACTER_SELECTION_SCREEN
// ==========================================================================
void user_interface::clear_character_selection_screen()
{
}

// ==========================================================================
// CLEAR_CHARACTER_CREATION_SCREEN
// ==========================================================================
void user_interface::clear_character_creation_screen()
{
    pimpl_->character_creation_screen_->clear();
}

// ==========================================================================
// CLEAR_MAIN_SCREEN
// ==========================================================================
void user_interface::clear_main_screen()
{
    pimpl_->main_screen_->clear();
}

// ==========================================================================
// CLEAR_PASSWORD_CHANGE_SCREEN
// ==========================================================================
void user_interface::clear_password_change_screen()
{
    pimpl_->password_change_screen_->clear();
}

// ==========================================================================
// ON_PASSWORD_CHANGED
// ==========================================================================
void user_interface::on_password_changed(
    function<void (string, string, string)> callback)
{
    pimpl_->password_change_screen_->on_password_changed(callback);
}

// ==========================================================================
// ON_PASSWORD_CHANGE_CANCELLED
// ==========================================================================
void user_interface::on_password_change_cancelled(function<void ()> callback)
{
    pimpl_->password_change_screen_->on_password_change_cancelled(callback);
}

// ==========================================================================
// ON_ACCOUNT_DETAILS_ENTERED
// ==========================================================================
void user_interface::on_account_details_entered(
    function<void (string, string)> callback)
{
    pimpl_->intro_screen_->on_account_details_entered(callback);
}

// ==========================================================================
// ON_ACCOUNT_CREATED
// ==========================================================================
void user_interface::on_account_created(
    function<void (string, string, string)> callback)
{
    pimpl_->account_creation_screen_->on_account_created(callback);
}

// ==========================================================================
// ON_ACCOUNT_CREATION_CANCELLED
// ==========================================================================
void user_interface::on_account_creation_cancelled(function<void ()> callback)
{
    pimpl_->account_creation_screen_->on_account_creation_cancelled(callback);
}

// ==========================================================================
// ON_INPUT_ENTERED
// ==========================================================================
void user_interface::on_input_entered(function<void (string)> callback)
{
    pimpl_->main_screen_->on_input_entered(callback);
}

// ==========================================================================
// ON_NEW_CHARACTER
// ==========================================================================
void user_interface::on_new_character(function<void ()> callback)
{
    pimpl_->character_selection_screen_->on_new_character(callback);
}

// ==========================================================================
// ON_CHARACTER_SELECTED
// ==========================================================================
void user_interface::on_character_selected(function<void (string)> callback)
{
    pimpl_->character_selection_screen_->on_character_selected(callback);
}

// ==========================================================================
// ON_CHARACTER_CREATED
// ==========================================================================
void user_interface::on_character_created(function<void (string)> callback)
{
    pimpl_->character_creation_screen_->on_character_created(callback);
}

// ==========================================================================
// ON_CHARACTER_CREATION_CANCELLED
// ==========================================================================
void user_interface::on_character_creation_cancelled(
    function<void ()> callback)
{
    pimpl_->character_creation_screen_->on_character_creation_cancelled(
        callback);
}

// ==========================================================================
// SET_CHARACTER_NAMES
// ==========================================================================
void user_interface::set_character_names(        
    runtime_array< pair<string, string> > const &names)
{
    pimpl_->character_selection_screen_->set_character_names(names);
}
    
// ==========================================================================
// SELECT_FACE
// ==========================================================================
void user_interface::select_face(string const &face_name)
{
    pimpl_->active_screen_->select_face(face_name);
    pimpl_->active_screen_->set_focus();
    pimpl_->active_face_ = face_name;
}

// ==========================================================================
// ADD_OUTPUT_TEXT
// ==========================================================================
void user_interface::add_output_text(
    runtime_array<element_type> const &text)
{
    pimpl_->main_screen_->add_output_text(text);
}

// ==========================================================================
// SET_STATUSBAR_TEXT
// ==========================================================================
void user_interface::set_statusbar_text(
    runtime_array<element_type> const &text)
{
    if (pimpl_->active_face_ == hugin::FACE_INTRO)
    {
        pimpl_->intro_screen_->set_statusbar_text(text);
    }
    else if (pimpl_->active_face_ == hugin::FACE_ACCOUNT_CREATION)
    {
        pimpl_->account_creation_screen_->set_statusbar_text(text);
    }
    else if (pimpl_->active_face_ == hugin::FACE_CHAR_CREATION)
    {
        pimpl_->character_creation_screen_->set_statusbar_text(text);
    }
    else if (pimpl_->active_face_ == hugin::FACE_PASSWORD_CHANGE)
    {
        pimpl_->password_change_screen_->set_statusbar_text(text);
    }
}

// ==========================================================================
// UPDATE_WHOLIST
// ==========================================================================
void user_interface::update_wholist(runtime_array<string> const &names)
{
    pimpl_->main_screen_->update_wholist(names);
}

// ==========================================================================
// ADD_COMMAND_HISTORY
// ==========================================================================
void user_interface::add_command_history(string const &history)
{
    pimpl_->main_screen_->add_command_history(history);
}

// ==========================================================================
// SHOW_HELP_WINDOW
// ==========================================================================
void user_interface::show_help_window()
{
    pimpl_->main_screen_->show_help_window();
}

// ==========================================================================
// HIDE_HELP_WINDOW
// ==========================================================================
void user_interface::hide_help_window()
{
    pimpl_->main_screen_->hide_help_window();
}

// ==========================================================================
// ON_HELP_CLOSED
// ==========================================================================
void user_interface::on_help_closed(function<void ()> callback)
{
    pimpl_->main_screen_->on_help_closed(callback);
}

// ==========================================================================
// SET_HELP_WINDOW_TEXT
// ==========================================================================
void user_interface::set_help_window_text(
    odin::runtime_array<element_type> const &text)
{
    pimpl_->main_screen_->set_help_window_text(text);
}

}


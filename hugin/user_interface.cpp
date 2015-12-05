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
#include "hugin/gm_tools_screen.hpp"
#include "hugin/intro_screen.hpp"
#include "hugin/main_screen.hpp"
#include "hugin/password_change_screen.hpp"
#include "munin/basic_container.hpp"
#include "munin/card.hpp"
#include "munin/clock.hpp"
#include "munin/compass_layout.hpp"
#include "munin/grid_layout.hpp"
#include "munin/image.hpp"
#include "munin/status_bar.hpp"
#include <terminalpp/string.hpp>
#include <deque>
#include <mutex>
#include <thread>

namespace hugin {

// ==========================================================================
// USER_INTERFACE::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct user_interface::impl
    : public std::enable_shared_from_this<impl>
{
    impl(boost::asio::strand &strand)
        : strand_(strand)
    {
    }
    
    boost::asio::strand                        &strand_;
    std::shared_ptr<munin::card>                active_screen_;
    std::string                                 active_face_;                  
    
    std::shared_ptr<intro_screen>               intro_screen_;   
    std::shared_ptr<account_creation_screen>    account_creation_screen_;
    std::shared_ptr<character_selection_screen> character_selection_screen_;
    std::shared_ptr<character_creation_screen>  character_creation_screen_;
    std::shared_ptr<main_screen>                main_screen_;
    std::shared_ptr<password_change_screen>     password_change_screen_;
    std::shared_ptr<gm_tools_screen>            gm_tools_screen_;

    std::shared_ptr<munin::status_bar>          status_bar_;

    std::mutex                                  dispatch_queue_mutex_;
    std::deque<std::function<void ()>>          dispatch_queue_;
    
    // ======================================================================
    // SELECT_FACE
    // ======================================================================
    void select_face(std::string const &face_name)
    {
        active_screen_->select_face(face_name);
        active_screen_->set_focus();
        active_face_ = face_name;
    }
    
    // ======================================================================
    // SET_STATUSBAR_TEXT
    // ======================================================================
    void set_statusbar_text(terminalpp::string const &text)
    {
        status_bar_->set_message(text);
    }
    
    // ======================================================================
    // ASYNC
    // ======================================================================
    template <typename F>
    void async(F const &fn)
    {
        {
            std::unique_lock<std::mutex> lock(dispatch_queue_mutex_);
            dispatch_queue_.push_back(fn);
        }
        
        strand_.post([pthis=shared_from_this()]{pthis->dispatch_queue();});
    }

private:
    // ======================================================================
    // DISPATCH_QUEUE
    // ======================================================================
    void dispatch_queue()
    {
        std::function<void ()> fn;

        std::unique_lock<std::mutex> lock(dispatch_queue_mutex_);
        
        while (!dispatch_queue_.empty())
        {
            fn = dispatch_queue_.front();
            dispatch_queue_.pop_front();
            lock.unlock();

            fn();
            
            lock.lock();
        }
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
user_interface::user_interface(boost::asio::strand &strand)
  : pimpl_(std::make_shared<impl>(std::ref(strand)))
{
    using namespace terminalpp::literals;

    pimpl_->active_screen_              = std::make_shared<munin::card>();
    pimpl_->intro_screen_               = std::make_shared<intro_screen>();
    pimpl_->account_creation_screen_    = std::make_shared<account_creation_screen>();
    pimpl_->character_creation_screen_  = std::make_shared<character_creation_screen>();
    pimpl_->character_selection_screen_ = std::make_shared<character_selection_screen>();
    pimpl_->main_screen_                = std::make_shared<main_screen>();
    pimpl_->password_change_screen_     = std::make_shared<password_change_screen>();
    pimpl_->gm_tools_screen_            = std::make_shared<gm_tools_screen>();
    pimpl_->status_bar_                 = std::make_shared<munin::status_bar>();

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
    pimpl_->active_screen_->add_face(
        pimpl_->gm_tools_screen_, hugin::FACE_GM_TOOLS);
    
    pimpl_->active_screen_->select_face(hugin::FACE_INTRO);
    pimpl_->active_face_ = hugin::FACE_INTRO;

    // Create a container that has a single spacer element and ... A CLOCK!
    auto clock_container = std::make_shared<munin::basic_container>();
    clock_container->set_layout(std::make_shared<munin::compass_layout>());
    clock_container->add_component(
        std::make_shared<munin::image>(" "_ts)
        , munin::COMPASS_LAYOUT_WEST);
    clock_container->add_component(
        std::make_shared<munin::clock>()
        , munin::COMPASS_LAYOUT_EAST);

    auto status_bar_container = std::make_shared<munin::basic_container>();
    status_bar_container->set_layout(std::make_shared<munin::compass_layout>());
    status_bar_container->add_component(
        pimpl_->status_bar_
        , munin::COMPASS_LAYOUT_CENTRE);
    status_bar_container->add_component(
        clock_container
        , munin::COMPASS_LAYOUT_EAST);

    auto container = get_container();
    container->set_layout(std::make_shared<munin::compass_layout>());
    container->add_component(
        pimpl_->active_screen_
        , munin::COMPASS_LAYOUT_CENTRE);
    container->add_component(
        status_bar_container
        , munin::COMPASS_LAYOUT_SOUTH);
}

// ==========================================================================
// CLEAR_INTRO_SCREEN
// ==========================================================================
void user_interface::clear_intro_screen()
{
    pimpl_->async([pimpl_=pimpl_]{pimpl_->intro_screen_->clear();});
}

// ==========================================================================
// CLEAR_ACCOUNT_CREATION_SCREEN
// ==========================================================================
void user_interface::clear_account_creation_screen()
{
    pimpl_->async([pimpl_=pimpl_]{pimpl_->account_creation_screen_->clear();});
}

// ==========================================================================
// CLEAR_CHARACTER_SELECTION_SCREEN
// ==========================================================================
void user_interface::clear_character_selection_screen()
{
    pimpl_->async([pimpl_=pimpl_]{pimpl_->character_selection_screen_->clear();});
}

// ==========================================================================
// CLEAR_CHARACTER_CREATION_SCREEN
// ==========================================================================
void user_interface::clear_character_creation_screen()
{
    pimpl_->async([pimpl_=pimpl_]{pimpl_->character_creation_screen_->clear();});
}

// ==========================================================================
// CLEAR_MAIN_SCREEN
// ==========================================================================
void user_interface::clear_main_screen()
{
    pimpl_->async([pimpl_=pimpl_]{pimpl_->main_screen_->clear();});
}

// ==========================================================================
// CLEAR_PASSWORD_CHANGE_SCREEN
// ==========================================================================
void user_interface::clear_password_change_screen()
{
    pimpl_->async([pimpl_=pimpl_]{pimpl_->password_change_screen_->clear();});
}

// ==========================================================================
// ON_PASSWORD_CHANGED
// ==========================================================================
void user_interface::on_password_changed(
    std::function<
        void (
            std::string const &, 
            std::string const &, 
            std::string const &)> const &callback)
{
    pimpl_->password_change_screen_->on_password_changed(callback);
}

// ==========================================================================
// ON_PASSWORD_CHANGE_CANCELLED
// ==========================================================================
void user_interface::on_password_change_cancelled(
    std::function<void ()> const &callback)
{
    pimpl_->password_change_screen_->on_password_change_cancelled(callback);
}

// ==========================================================================
// ON_LOGIN
// ==========================================================================
void user_interface::on_login(
    std::function<void (std::string const &, std::string const &)> const &callback)
{
    pimpl_->intro_screen_->on_login.connect(callback);
}

// ==========================================================================
// ON_NEW_ACCOUNT
// ==========================================================================
void user_interface::on_new_account(std::function<void ()> const &callback)
{
    pimpl_->intro_screen_->on_new_account.connect(callback);
}

// ==========================================================================
// ON_ACCOUNT_CREATED
// ==========================================================================
void user_interface::on_account_created(
    std::function<
        void (std::string const &, 
              std::string const &, 
              std::string const &)> const &callback)
{
    pimpl_->account_creation_screen_->on_account_created(callback);
}

// ==========================================================================
// ON_ACCOUNT_CREATION_CANCELLED
// ==========================================================================
void user_interface::on_account_creation_cancelled(
    std::function<void ()> const &callback)
{
    pimpl_->account_creation_screen_->on_account_creation_cancelled(callback);
}

// ==========================================================================
// ON_INPUT_ENTERED
// ==========================================================================
void user_interface::on_input_entered(
    std::function<void (std::string const &)> const &callback)
{
    pimpl_->main_screen_->on_input_entered(callback);
}

// ==========================================================================
// ON_NEW_CHARACTER
// ==========================================================================
void user_interface::on_new_character(std::function<void ()> const &callback)
{
    pimpl_->character_selection_screen_->on_new_character(callback);
}

// ==========================================================================
// ON_CHARACTER_SELECTED
// ==========================================================================
void user_interface::on_character_selected(
    std::function<void (std::string const &)> const &callback)
{
    pimpl_->character_selection_screen_->on_character_selected(callback);
}

// ==========================================================================
// ON_CHARACTER_CREATED
// ==========================================================================
void user_interface::on_character_created(
    std::function<void (std::string const &, bool)> const &callback)
{
    pimpl_->character_creation_screen_->on_character_created(callback);
}

// ==========================================================================
// ON_CHARACTER_CREATION_CANCELLED
// ==========================================================================
void user_interface::on_character_creation_cancelled(
    std::function<void ()> const &callback)
{
    pimpl_->character_creation_screen_->on_character_creation_cancelled(
        callback);
}

// ==========================================================================
// ON_GM_TOOLS_BACK
// ==========================================================================
void user_interface::on_gm_tools_back(std::function<void ()> const &callback)
{
    pimpl_->gm_tools_screen_->on_back.connect(callback);
}

// ==========================================================================
// ON_GM_FIGHT_BEAST
// ==========================================================================
void user_interface::on_gm_fight_beast(
    std::function<void (std::shared_ptr<paradice::beast> const &)> const &callback)
{
    pimpl_->gm_tools_screen_->on_fight_beast.connect(callback);
}

// ==========================================================================
// ON_GM_FIGHT_ENCOUNTER
// ==========================================================================
void user_interface::on_gm_fight_encounter(
    std::function<void (std::shared_ptr<paradice::encounter> const &)> const &callback)
{
    pimpl_->gm_tools_screen_->on_fight_encounter.connect(callback);
}

// ==========================================================================
// SHOW_ACTIVE_ENCOUNTER_WINDOW
// ==========================================================================
void user_interface::show_active_encounter_window()
{
    pimpl_->main_screen_->show_active_encounter_window();
}

// ==========================================================================
// HIDE_ACTIVE_ENCOUNTER_WINDOW
// ==========================================================================
void user_interface::hide_active_encounter_window()
{
    pimpl_->main_screen_->hide_active_encounter_window();
}

// ==========================================================================
// SET_ACTIVE_ENCOUNTER
// ==========================================================================
void user_interface::set_active_encounter(
    std::shared_ptr<paradice::active_encounter> const &enc)
{
    pimpl_->main_screen_->set_active_encounter(enc);
}

// ==========================================================================
// SET_CHARACTER_NAMES
// ==========================================================================
void user_interface::set_character_names(        
    std::vector<std::pair<std::string, std::string>> const &names)
{
    pimpl_->async([pimpl_=pimpl_, names]{
        pimpl_->character_selection_screen_->set_character_names(names);
    });
}

// ==========================================================================
// SET_BEASTS
// ==========================================================================
void user_interface::set_beasts(
    std::vector<std::shared_ptr<paradice::beast>> const &beasts)
{
    pimpl_->gm_tools_screen_->set_beasts(beasts);
}

// ==========================================================================
// GET_BEASTS
// ==========================================================================
std::vector<std::shared_ptr<paradice::beast>> user_interface::get_beasts() const
{
    return pimpl_->gm_tools_screen_->get_beasts();
}

// ==========================================================================
// SET_ENCOUNTERS
// ==========================================================================
void user_interface::set_encounters(
    std::vector<std::shared_ptr<paradice::encounter>> const &encounters)
{
    pimpl_->gm_tools_screen_->set_encounters(encounters);
}

// ==========================================================================
// GET_ENCOUNTERS
// ==========================================================================
std::vector<std::shared_ptr<paradice::encounter>> 
user_interface::get_encounters() const
{
    return pimpl_->gm_tools_screen_->get_encounters();
}

// ==========================================================================
// SELECT_FACE
// ==========================================================================
void user_interface::select_face(std::string const &face_name)
{
    pimpl_->async([pimpl_=pimpl_, face_name]{pimpl_->select_face(face_name);});
}

// ==========================================================================
// ADD_OUTPUT_TEXT
// ==========================================================================
void user_interface::add_output_text(terminalpp::string const &text)
{
    pimpl_->async([pimpl_=pimpl_, text]{
        pimpl_->main_screen_->add_output_text(text);
    });
}

// ==========================================================================
// SET_STATUSBAR_TEXT
// ==========================================================================
void user_interface::set_statusbar_text(terminalpp::string const &text)
{
    pimpl_->async([pimpl_=pimpl_, text]{pimpl_->set_statusbar_text(text);});
}

// ==========================================================================
// UPDATE_WHOLIST
// ==========================================================================
void user_interface::update_wholist(std::vector<std::string> const &names)
{
    pimpl_->async([pimpl_=pimpl_, names]{pimpl_->main_screen_->update_wholist(names);});
}

// ==========================================================================
// ADD_COMMAND_HISTORY
// ==========================================================================
void user_interface::add_command_history(std::string const &history)
{
    pimpl_->async([pimpl_=pimpl_, history]{
        pimpl_->main_screen_->add_command_history(history);
    });
}

// ==========================================================================
// SHOW_HELP_WINDOW
// ==========================================================================
void user_interface::show_help_window()
{
    pimpl_->async([pimpl_=pimpl_]{pimpl_->main_screen_->show_help_window();});
}

// ==========================================================================
// HIDE_HELP_WINDOW
// ==========================================================================
void user_interface::hide_help_window()
{
    pimpl_->async([pimpl_=pimpl_]{pimpl_->main_screen_->hide_help_window();});
}

// ==========================================================================
// ON_HELP_CLOSED
// ==========================================================================
void user_interface::on_help_closed(std::function<void ()> const &callback)
{
    pimpl_->main_screen_->on_help_closed(callback);
}

// ==========================================================================
// SET_HELP_WINDOW_TEXT
// ==========================================================================
void user_interface::set_help_window_text(terminalpp::string const &text)
{
    pimpl_->main_screen_->set_help_window_text(text);
}

}


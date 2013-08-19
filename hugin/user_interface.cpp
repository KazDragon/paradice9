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
#include "munin/ansi/protocol.hpp"
#include "munin/basic_container.hpp"
#include "munin/card.hpp"
#include "munin/clock.hpp"
#include "munin/compass_layout.hpp"
#include "munin/grid_layout.hpp"
#include "munin/image.hpp"
#include "munin/status_bar.hpp"
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>
#include <boost/typeof/typeof.hpp>
#include <deque>

using namespace paradice;
using namespace munin;
using namespace munin::ansi;
using namespace odin;
using namespace boost;
using namespace std;

namespace hugin {

// ==========================================================================
// USER_INTERFACE::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct user_interface::impl
    : public enable_shared_from_this<impl>
{
    impl(boost::asio::strand &strand)
        : strand_(strand)
    {
    }
    
    boost::asio::strand                   &strand_;
    shared_ptr<card>                       active_screen_;
    string                                 active_face_;                  
    
    shared_ptr<intro_screen>               intro_screen_;   
    shared_ptr<account_creation_screen>    account_creation_screen_;
    shared_ptr<character_selection_screen> character_selection_screen_;
    shared_ptr<character_creation_screen>  character_creation_screen_;
    shared_ptr<main_screen>                main_screen_;
    shared_ptr<password_change_screen>     password_change_screen_;
    shared_ptr<gm_tools_screen>            gm_tools_screen_;

    shared_ptr<status_bar>                 status_bar_;

    mutex                                  dispatch_queue_mutex_;
    deque< function<void ()> >             dispatch_queue_;
    
    // ======================================================================
    // SELECT_FACE
    // ======================================================================
    void select_face(string const &face_name)
    {
        active_screen_->select_face(face_name);
        active_screen_->set_focus();
        active_face_ = face_name;
    }
    
    // ======================================================================
    // SET_STATUSBAR_TEXT
    // ======================================================================
    void set_statusbar_text(vector<element_type> const &text)
    {
        status_bar_->set_message(text);
    }
    
    // ======================================================================
    // ASYNC
    // ======================================================================
    void async(function<void ()> fn)
    {
        {
            unique_lock<mutex> lock(dispatch_queue_mutex_);
            dispatch_queue_.push_back(fn);
        }
        
        strand_.post(bind(&impl::dispatch_queue, shared_from_this()));
    }

private:
    // ======================================================================
    // DISPATCH_QUEUE
    // ======================================================================
    void dispatch_queue()
    {
        function<void ()> fn;

        unique_lock<mutex> lock(dispatch_queue_mutex_);
        
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
    : pimpl_(make_shared<impl>(ref(strand)))
{
    pimpl_->active_screen_              = make_shared<card>();
    pimpl_->intro_screen_               = make_shared<intro_screen>();
    pimpl_->account_creation_screen_    = make_shared<account_creation_screen>();
    pimpl_->character_creation_screen_  = make_shared<character_creation_screen>();
    pimpl_->character_selection_screen_ = make_shared<character_selection_screen>();
    pimpl_->main_screen_                = make_shared<main_screen>();
    pimpl_->password_change_screen_     = make_shared<password_change_screen>();
    pimpl_->gm_tools_screen_            = make_shared<gm_tools_screen>();
    pimpl_->status_bar_                 = make_shared<status_bar>();

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
    BOOST_AUTO(clock_container, make_shared<basic_container>());
    clock_container->set_layout(make_shared<compass_layout>());
    clock_container->add_component(
        make_shared<image>(elements_from_string(" "))
      , COMPASS_LAYOUT_WEST);
    clock_container->add_component(
        make_shared<munin::clock>()
      , COMPASS_LAYOUT_EAST);

    BOOST_AUTO(status_bar_container, make_shared<basic_container>());
    status_bar_container->set_layout(make_shared<compass_layout>());
    status_bar_container->add_component(
        pimpl_->status_bar_
      , COMPASS_LAYOUT_CENTRE);
    status_bar_container->add_component(
        clock_container
      , COMPASS_LAYOUT_EAST);

    BOOST_AUTO(container, get_container());
    container->set_layout(make_shared<compass_layout>());
    container->add_component(
        pimpl_->active_screen_
      , COMPASS_LAYOUT_CENTRE);
    container->add_component(
        status_bar_container
      , COMPASS_LAYOUT_SOUTH);
}

// ==========================================================================
// CLEAR_INTRO_SCREEN
// ==========================================================================
void user_interface::clear_intro_screen()
{
    pimpl_->async(bind(&intro_screen::clear, pimpl_->intro_screen_));
}

// ==========================================================================
// CLEAR_ACCOUNT_CREATION_SCREEN
// ==========================================================================
void user_interface::clear_account_creation_screen()
{
    pimpl_->async(bind(
        &account_creation_screen::clear, pimpl_->account_creation_screen_));
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
    pimpl_->async(bind(
        &character_creation_screen::clear, pimpl_->character_creation_screen_));
}

// ==========================================================================
// CLEAR_MAIN_SCREEN
// ==========================================================================
void user_interface::clear_main_screen()
{
    pimpl_->async(bind(&main_screen::clear, pimpl_->main_screen_));
}

// ==========================================================================
// CLEAR_PASSWORD_CHANGE_SCREEN
// ==========================================================================
void user_interface::clear_password_change_screen()
{
    pimpl_->async(bind(
        &password_change_screen::clear, pimpl_->password_change_screen_));
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
void user_interface::on_character_created(
    function<void (string, bool)> callback)
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
// ON_GM_TOOLS_BACK
// ==========================================================================
void user_interface::on_gm_tools_back(function<void ()> callback)
{
    pimpl_->gm_tools_screen_->on_back.connect(callback);
}

// ==========================================================================
// SET_CHARACTER_NAMES
// ==========================================================================
void user_interface::set_character_names(        
    vector< pair<string, string> > const &names)
{
    pimpl_->async(bind(
        &character_selection_screen::set_character_names
      , pimpl_->character_selection_screen_
      , names));
}

// ==========================================================================
// SET_BEASTS
// ==========================================================================
void user_interface::set_beasts(
    vector< shared_ptr<beast> > beasts)
{
    pimpl_->gm_tools_screen_->set_beasts(beasts);
}

// ==========================================================================
// GET_BEASTS
// ==========================================================================
vector< shared_ptr<beast> > user_interface::get_beasts() const
{
    return pimpl_->gm_tools_screen_->get_beasts();
}

// ==========================================================================
// SET_ENCOUNTERS
// ==========================================================================
void user_interface::set_encounters(
    vector< shared_ptr<encounter> > encounters)
{
    pimpl_->gm_tools_screen_->set_encounters(encounters);
}

// ==========================================================================
// GET_ENCOUNTERS
// ==========================================================================
vector< shared_ptr<encounter> > user_interface::get_encounters() const
{
    return pimpl_->gm_tools_screen_->get_encounters();
}

// ==========================================================================
// SELECT_FACE
// ==========================================================================
void user_interface::select_face(string const &face_name)
{
    pimpl_->async(bind(&impl::select_face, pimpl_, face_name)); 
}

// ==========================================================================
// ADD_OUTPUT_TEXT
// ==========================================================================
void user_interface::add_output_text(vector<element_type> const &text)
{
    pimpl_->async(bind(
        &main_screen::add_output_text
      , pimpl_->main_screen_
      , text));
}

// ==========================================================================
// SET_STATUSBAR_TEXT
// ==========================================================================
void user_interface::set_statusbar_text(vector<element_type> const &text)
{
    pimpl_->async(bind(&impl::set_statusbar_text, pimpl_, text));
}

// ==========================================================================
// UPDATE_WHOLIST
// ==========================================================================
void user_interface::update_wholist(vector<string> const &names)
{
    pimpl_->async(bind(
        &main_screen::update_wholist
      , pimpl_->main_screen_
      , names));
}

// ==========================================================================
// ADD_COMMAND_HISTORY
// ==========================================================================
void user_interface::add_command_history(string const &history)
{
    pimpl_->async(bind(
        &main_screen::add_command_history
      , pimpl_->main_screen_
      , history));
}

// ==========================================================================
// SHOW_HELP_WINDOW
// ==========================================================================
void user_interface::show_help_window()
{
    pimpl_->async(bind(&main_screen::show_help_window, pimpl_->main_screen_));
}

// ==========================================================================
// HIDE_HELP_WINDOW
// ==========================================================================
void user_interface::hide_help_window()
{
    pimpl_->async(bind(&main_screen::hide_help_window, pimpl_->main_screen_));
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
void user_interface::set_help_window_text(vector<element_type> const &text)
{
    pimpl_->main_screen_->set_help_window_text(text);
}

}


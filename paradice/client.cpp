// ==========================================================================
// Paradice Client
//
// Copyright (C) 2009 Matthew Chaplain, All Rights Reserved.
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
#include "client.hpp"
#include "account.hpp"
#include "admin.hpp"
#include "character.hpp"
#include "communication.hpp"
#include "configuration.hpp"
#include "connection.hpp"
#include "context.hpp"
#include "encounters.hpp"
#include "help.hpp"
#include "rules.hpp"
#include "who.hpp"
#include "hugin/user_interface.hpp"
#include "munin/ansi/protocol.hpp"
#include "munin/algorithm.hpp"
#include "munin/grid_layout.hpp"
#include "munin/window.hpp"
#include "odin/tokenise.hpp"
#include <boost/asio/strand.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>
#include <boost/typeof/typeof.hpp>
#include <cstdio>
#include <deque>
#include <string>
#include <vector>

using namespace odin;
using namespace munin;
using namespace boost;
using namespace std;

namespace paradice {

namespace {
    #define PARADICE_CMD_ENTRY_NOP(name) \
        { name,        NULL                                  , 0, 0 }
        
    #define PARADICE_CMD_ENTRY(func) \
        { #func,       bind(&paradice::do_##func, _1, _2, _3), 0, 0 }
        
    #define PARADICE_CMD_ALIAS(func, alias) \
        { alias,       bind(&paradice::do_##func, _1, _2, _3), 0, 0 }
        
    #define PARADICE_ADMIN_ENTRY(func, level) \
        { #func,       bind(&paradice::do_##func, _1, _2, _3), (level), 0 }

    #define PARADICE_GM_ENTRY(func, level) \
        { #func,       bind(&paradice::do_##func, _1, _2, _3), 0, (level) }
        
    typedef function<void (
        shared_ptr<paradice::context> ctx
      , string                        args
      , shared_ptr<paradice::client>  player)> paradice_command;
    
    static struct command
    {
        string           command_;
        paradice_command function_;
        u32              admin_level_required_;
        u32              gm_level_required_;
    } const command_list[] =
    {
        PARADICE_CMD_ENTRY_NOP("!")
    
      , PARADICE_CMD_ENTRY(say)    
      , PARADICE_CMD_ALIAS(say, ".")
      
      , PARADICE_CMD_ENTRY(whisper)
      , PARADICE_CMD_ALIAS(whisper, ">")
      
      , PARADICE_CMD_ENTRY(emote)
      , PARADICE_CMD_ALIAS(emote, ":")
    
      , PARADICE_CMD_ENTRY(set)
      , PARADICE_CMD_ENTRY(title)
      , PARADICE_CMD_ALIAS(title, "surname")
      , PARADICE_CMD_ENTRY(prefix)
      , PARADICE_CMD_ALIAS(prefix, "honorific")
      , PARADICE_CMD_ENTRY(roll)
      , PARADICE_CMD_ENTRY(rollprivate)
      , PARADICE_CMD_ENTRY(showrolls)
      , PARADICE_CMD_ENTRY(clearrolls)
    
      , PARADICE_CMD_ENTRY(help)

      , PARADICE_CMD_ENTRY(password)      
      , PARADICE_CMD_ENTRY(quit)
      , PARADICE_CMD_ENTRY(logout)

      , PARADICE_GM_ENTRY(encounter, 100)

      , PARADICE_ADMIN_ENTRY(admin_set_password, 100)
      , PARADICE_ADMIN_ENTRY(admin_shutdown,     100)
    };

    #undef PARADICE_CMD_ENTRY_NOP
    #undef PARADICE_CMD_ENTRY
    #undef PARADICE_CMD_ALIAS

    // ======================================================================
    // CAPITALISE
    // ======================================================================
    static void capitalise(string &name)
    {
        if (!name.empty())
        {
            name[0] = toupper(name[0]);
            
            for (string::size_type index = 1; index < name.size(); ++index)
            {
                name[index] = tolower(name[index]);
            }
        }
    }
}
    
// ==========================================================================
// CLIENT IMPLEMENTATION STRUCTURE
// ==========================================================================
class client::impl
    : public enable_shared_from_this<client::impl>
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(
        client                  &self
      , boost::asio::io_service &io_service
      , shared_ptr<context>      ctx)
        : self_(self)
        , strand_(io_service)
        , context_(ctx)
        , window_(make_shared<munin::window>(ref(io_service), ref(strand_)))
        , user_interface_(make_shared<hugin::user_interface>(ref(strand_)))
    {
        window_->set_size(munin::extent(80, 24));
    }
    
    // ======================================================================
    // SET_CONNECTION
    // ======================================================================
    void set_connection(shared_ptr<connection> cnx)
    {
        connection_ = cnx;
        
        // CONNECTION CALLBACKS
        connection_->on_text(bind(
            &impl::on_text
          , this
          , _1));
    
        connection_->on_mouse_report(bind(
            &impl::on_mouse_report
          , this
          , _1));
        
        connection_->on_control_sequence(bind(
            &impl::on_control_sequence
          , this
          , _1));
    
        connection_->on_window_size_changed(bind(
            &impl::on_window_size_changed
          , this
          , _1
          , _2));
    
        // WINDOW CALLBACKS
        window_->on_repaint.connect(bind(
            &impl::on_repaint
          , this
          , _1));
        
        // USER INTERFACE CALLBACKS
        user_interface_->on_input_entered(bind(
            &impl::on_input_entered
          , this
          , _1));
        
        user_interface_->on_account_details_entered(bind(
            &impl::on_account_details_entered
          , this
          , _1
          , _2));
        
        user_interface_->on_account_created(bind(
            &impl::on_account_created
          , this
          , _1
          , _2
          , _3));
        
        user_interface_->on_account_creation_cancelled(bind(
            &impl::on_account_creation_cancelled
          , this));

        user_interface_->on_new_character(bind(
            &impl::on_new_character
          , this));

        user_interface_->on_character_selected(bind(
            &impl::on_character_selected
          , this
          , _1));
        
        user_interface_->on_character_created(bind(
            &impl::on_character_created
          , this
          , _1
          , _2));
        
        user_interface_->on_character_creation_cancelled(bind(
            &impl::on_character_creation_cancelled
          , this));

        user_interface_->on_encounters_back(bind(
            &impl::on_encounters_back
          , this));

        user_interface_->on_help_closed(bind(
            &impl::on_help_closed
          , this));
        
        user_interface_->on_password_changed(bind(
            &impl::on_password_changed
          , this
          , _1
          , _2
          , _3));
        
        user_interface_->on_password_change_cancelled(bind(
            &impl::on_password_change_cancelled
          , this));

        set_window_title("Paradice9");

        BOOST_AUTO(content, window_->get_content());
        content->set_layout(make_shared<grid_layout>(1, 1));
        content->add_component(user_interface_);
        content->set_focus();
    
        window_->enable_mouse_tracking();
        window_->on_repaint(munin::ansi::clear_screen());
        window_->on_repaint(munin::ansi::set_normal_cursor_keys());
    }

    // ======================================================================
    // SET_ACCOUNT
    // ======================================================================
    void set_account(shared_ptr<account> acc)
    {
        account_ = acc;
    }
    
    // ======================================================================
    // GET_ACCOUNT
    // ======================================================================
    shared_ptr<account> get_account()
    {
        return account_;
    }

    // ======================================================================
    // SET_CHARACTER
    // ======================================================================
    void set_character(shared_ptr<character> ch)
    {
        character_ = ch;
    }
    
    // ======================================================================
    // GET_CHARACTER
    // ======================================================================
    shared_ptr<character> get_character()
    {
        return character_;
    }
    
    // ======================================================================
    // GET_USER_INTERFACE
    // ======================================================================
    shared_ptr<hugin::user_interface> get_user_interface()
    {
        return user_interface_;
    }
    
    // ======================================================================
    // GET_WINDOW
    // ======================================================================
    shared_ptr<munin::window> get_window()
    {
        return window_;
    }

    // ======================================================================
    // SET_WINDOW_TITLE
    // ======================================================================
    void set_window_title(string const &title)
    {
        {
            unique_lock<mutex> lock(dispatch_queue_mutex_);
            dispatch_queue_.push_back(bind(
                &window::set_title, window_, title));
        }
        
        strand_.post(bind(&impl::dispatch_queue, shared_from_this()));
    }
    
    // ======================================================================
    // SET_WINDOW_SIZE
    // ======================================================================
    void set_window_size(u16 width, u16 height)
    {
        {
            unique_lock<mutex> lock(dispatch_queue_mutex_);
            dispatch_queue_.push_back(bind(
                &window::set_size, window_, munin::extent(width, height)));
        }
        
        strand_.post(bind(&impl::dispatch_queue, shared_from_this()));
    }

    // ======================================================================
    // DISCONNECT
    // ======================================================================
    void disconnect()
    {
        connection_->disconnect();
    }
    
    // ======================================================================
    // ON_CONNECTION_DEATH
    // ======================================================================
    void on_connection_death(function<void ()> callback)
    {
        connection_->on_socket_death(callback);
    }

private :
    // ======================================================================
    // ON_TEXT
    // ======================================================================
    void on_text(char text)
    {
        unique_lock<mutex> lock(dispatch_queue_mutex_);
        
        dispatch_queue_.push_back(
            bind(&window::event, window_, text));
        
        strand_.post(bind(&impl::dispatch_queue, shared_from_this()));
    }

    // ======================================================================
    // ON_MOUSE_REPORT
    // ======================================================================
    void on_mouse_report(odin::ansi::mouse_report const &report)
    {
        unique_lock<mutex> lock(dispatch_queue_mutex_);
        dispatch_queue_.push_back(bind(&window::event, window_, report));
        strand_.post(bind(&impl::dispatch_queue, shared_from_this()));
    }
    
    // ======================================================================
    // ON_CONTROL_SEQUENCE
    // ======================================================================
    void on_control_sequence(
        odin::ansi::control_sequence const &control_sequence)
    {
        unique_lock<mutex> lock(dispatch_queue_mutex_);
        dispatch_queue_.push_back(
            bind(&window::event, window_, control_sequence));
        strand_.post(bind(&impl::dispatch_queue, shared_from_this()));
    }
    
    // ======================================================================
    // ON_WINDOW_SIZE_CHANGED
    // ======================================================================
    void on_window_size_changed(u16 width, u16 height)
    {
        unique_lock<mutex> lock(dispatch_queue_mutex_);
        dispatch_queue_.push_back(
            bind(&window::set_size, window_, munin::extent(width, height)));
        strand_.post(bind(&impl::dispatch_queue, shared_from_this()));
    }
    
    // ======================================================================
    // ON_REPAINT
    // ======================================================================
    void on_repaint(string const &paint_data)
    {
        vector<u8> data(paint_data.size());
        copy(paint_data.begin(), paint_data.end(), data.begin());

        connection_->write(data);
    }
    
    // ======================================================================
    // REMOVE_DUPLICATE_ACCOUNTS
    // ======================================================================
    void remove_duplicate_accounts(shared_ptr<account> acc)
    {
        vector< shared_ptr<client> > clients_to_remove;
        BOOST_AUTO(clients, context_->get_clients());
        
        BOOST_FOREACH(shared_ptr<client> current_client, clients)
        {
            BOOST_AUTO(current_account, current_client->get_account());
            
            if (current_account != NULL && current_account != acc)
            {
                if (current_account->get_name() == acc->get_name())
                {
                    clients_to_remove.push_back(current_client);
                }
            }
        }
        
        BOOST_FOREACH(shared_ptr<client> current_client, clients_to_remove)
        {
            current_client->disconnect();
            context_->remove_client(current_client);
        }
    }

    // ======================================================================
    // UPDATE_CHARACTER_NAMES
    // ======================================================================
    void update_character_names()
    {
        BOOST_AUTO(
            number_of_characters
          , account_->get_number_of_characters());
        
        vector< pair<string, string> > characters(number_of_characters);
        
        for(size_t index = 0; index < number_of_characters; ++index)
        {
            BOOST_AUTO(name, account_->get_character_name(index));

            try
            {
                BOOST_AUTO(character, context_->load_character(name));                    
                
                if (character != NULL)
                {
                    characters[index] = 
                        make_pair(name, context_->get_moniker(character)); 
                }
            }
            catch(std::exception &ex)
            {
                printf("Error loading character %s on account %s: %s\n",
                    name.c_str(), account_->get_name().c_str(), ex.what());
            }
        }

        user_interface_->set_character_names(characters);
    }

    // ======================================================================
    // ON_ACCOUNT_DETAILS_ENTERED
    // ======================================================================
    void on_account_details_entered(
        string const &username
      , string const &password)
    {
        if (username == "" && password == "")
        {
            user_interface_->clear_account_creation_screen();
            user_interface_->select_face(hugin::FACE_ACCOUNT_CREATION);
            user_interface_->set_focus();
        }
        else
        {
            string account_name(username);
            capitalise(account_name);
    
            shared_ptr<account> account;

            try
            {
                account = context_->load_account(account_name);
            }
            catch(std::exception &ex)
            {
                // TODO: Use an actual logging library for this message.
                printf("Error loading account: %s\n", ex.what());
                user_interface_->set_statusbar_text(string_to_elements(
                    "\\[1Invalid username/password combination"));
                return;
            }
    
            if (account == NULL)
            {
                user_interface_->set_statusbar_text(string_to_elements(
                    "\\[1Invalid username/password combination"));
                return;
            }
            
            if (!account->password_match(password))
            {
                user_interface_->set_statusbar_text(string_to_elements(
                    "\\[1Invalid username/password combination"));
                return;
            }

            // First, ensure that if this account is logged in already,
            // it is booted.
            remove_duplicate_accounts(account);
            
            account_ = account;
            update_character_names();

            user_interface_->select_face(hugin::FACE_CHAR_SELECTION);
            user_interface_->set_focus();
        }
    }

    // ======================================================================
    // ON_ACCOUNT_CREATED
    // ======================================================================
    void on_account_created(
        string        account_name
      , string const &password
      , string const &password_verify)
    {
        // Check that the account name is valid.  If not, report an
        // error message and return.  This also removes cases where the
        // account name could have filesystem characters in it such as
        // * or ../, which could potentially wreck the system.
        if (!is_acceptible_name(account_name))
        {
            user_interface_->set_statusbar_text(string_to_elements(
                "\\[1Name must be alphabetic only and at least three "
                "characters long"));
            return;
        }
        
        // Check to see if the account name exists already.  If so,
        // report an error message and return.
        shared_ptr<account> test_account;

        try
        {
            test_account = context_->load_account(account_name);
        }
        catch(std::exception &ex)
        {
            // Something strange happened.  Perhaps the account file was 
            // corrupted, or it was swept out from under the process.  Either
            // way, best not do anything with it.
            // TODO: Use an actual logging library for this message.
            printf("Error loading account: %s\n", ex.what());

            user_interface_->set_statusbar_text(string_to_elements(
                "\\1[That account name is unavailable.  Please try with "
                "a different name"));
            return;
        }

        if (test_account != NULL)
        {
            user_interface_->set_statusbar_text(string_to_elements(
                "\\[1An account with that name already exists"));
            return;
        }

        // Check to see if the passwords match.  If not, report an error
        // message and return.
        if (password != password_verify)
        {
            user_interface_->set_statusbar_text(string_to_elements(
                "\\[1Passwords do not match"));
            return;
        }
        
        shared_ptr<account> acc = make_shared<account>();
        capitalise(account_name);
        acc->set_name(account_name);
        acc->set_password(password);
        
        try
        {
            context_->save_account(acc);
        }
        catch(std::exception &ex)
        {
            // TODO: Use an actual logging library for this message.
            printf("Error saving account: %s\n", ex.what());

            user_interface_->set_statusbar_text(string_to_elements(
                "\\[1Unexpected error setting saving your account.  "
                "Please try again."));
            return;
        }

        account_ = acc;
        
        user_interface_->select_face(hugin::FACE_CHAR_SELECTION);
    }

    // ======================================================================
    // ON_ACCOUNT_CREATION_CANCELLED
    // ======================================================================
    void on_account_creation_cancelled()
    {
        user_interface_->clear_intro_screen();
        user_interface_->select_face(hugin::FACE_INTRO);
        user_interface_->set_focus();
    }
    
    // ======================================================================
    // ON_NEW_CHARACTER
    // ======================================================================
    void on_new_character()
    {
        user_interface_->select_face(hugin::FACE_CHAR_CREATION);
        user_interface_->clear_character_creation_screen();
        user_interface_->set_focus();
    }

    // ======================================================================
    // ON_CHARACTER_SELECTED
    // ======================================================================
    void on_character_selected(string const &character_name)
    {
        shared_ptr<character> ch;

        try
        {
            ch = context_->load_character(character_name);
        }
        catch(std::exception &ex)
        {
            printf("Error loading character %s: %s\n",
                character_name.c_str(), ex.what());
            
            user_interface_->set_statusbar_text(string_to_elements(
                "\\[1Error loading character file."));

            return;
        }

        // If this is a GM character, then check for any other GM characters
        // online, since there can only be one.
        if (ch->get_gm_level() != 0)
        {
            BOOST_FOREACH(shared_ptr<client> cli, context_->get_clients())
            {
                BOOST_AUTO(other_ch, cli->get_character());

                if (other_ch && other_ch != ch && other_ch->get_gm_level() != 0)
                {
                    user_interface_->set_statusbar_text(string_to_elements(
                        "\\[1There is already a GM character online."));
                    return;
                }
            }
        }

        // If this is a GM character, then be sure to set the beasts and
        // encounters in the UI.
        if (ch->get_gm_level() != 0)
        {
            user_interface_->set_beasts(ch->get_beasts());
        }

        character_ = ch;
        context_->update_names();
        
        user_interface_->select_face(hugin::FACE_MAIN);
        set_window_title(character_->get_name() + " - Paradice9");

        send_to_all(
            context_
          , "#SERVER: "
          + context_->get_moniker(character_)
          + " has entered Paradice.\n");
    }

    // ======================================================================
    // ON_CHARACTER_CREATED
    // ======================================================================
    void on_character_created(string character_name, bool is_gm)
    {
        capitalise(character_name);
        
        // Check that the name is appropriate.
        if (!is_acceptible_name(character_name))
        {
            user_interface_->set_statusbar_text(string_to_elements(
                "\\[1Name must be alphabetic only and at least three "
                "characters long"));
            return;
        }
        
        // Test that the character doesn't already exist.
        shared_ptr<character> test_character;

        try
        {
            test_character = context_->load_character(character_name);
        }
        catch(std::exception &ex)
        {
            // This is an unexpected case.  load_character() tests for
            // existence with fs::exists().  If after that it fails to read
            // from the file, then there is a problem and the character 
            // probably existed.  Therefore, try again.
            printf("Error reading character %s: %s\n",
                character_name.c_str(), ex.what());

            user_interface_->set_statusbar_text(string_to_elements(
                "\\[1Error testing for that character."));
            return;
        }
        
        if (test_character != NULL)
        {
            user_interface_->set_statusbar_text(string_to_elements(
                "\\[1A character with that name already exists"));
            return;
        }

        character_ = make_shared<character>();
        character_->set_name(character_name);
        
        if (is_gm)
        {
            character_->set_gm_level(100);
        }
        
        try
        {
            context_->save_character(character_);
        }
        catch(std::exception &ex)
        {
            printf("Error saving character %s: %s\n",
                character_name.c_str(), ex.what());

            user_interface_->set_statusbar_text(string_to_elements(
                "\\[1There was an error saving the character."));
            return;
        }
        
        account_->add_character(character_name);

        try
        {
            context_->save_account(account_);
        }
        catch(std::exception &ex)
        {
            // TODO: Use an actual logging library for this message.
            printf("Error saving account: %s\n", ex.what());

            user_interface_->set_statusbar_text(string_to_elements(
                "\\[1Unexpected error saving your account.  "
                "Please try again."));

            account_->remove_character(character_name);
            return;
        }

        update_character_names();
        user_interface_->select_face(hugin::FACE_CHAR_SELECTION);
    }
    
    // ======================================================================
    // ON_CHARACTER_CREATION_CANCELLED
    // ======================================================================
    void on_character_creation_cancelled()
    {
        user_interface_->select_face(hugin::FACE_CHAR_SELECTION);
        user_interface_->set_focus();
    }
    
    // ======================================================================
    // ON_ENCOUNTERS_BACK
    // ======================================================================
    void on_encounters_back()
    {
        character_->set_beasts(user_interface_->get_beasts());
        context_->save_character(character_);

        user_interface_->select_face(hugin::FACE_MAIN);
        user_interface_->set_focus();
    }

    // ======================================================================
    // ON_HELP_CLOSED
    // ======================================================================
    void on_help_closed()
    {
        user_interface_->hide_help_window();
    }
    
    // ======================================================================
    // ON_PASSWORD_CHANGED
    // ======================================================================
    void on_password_changed(
        string const &old_password
      , string const &new_password
      , string const &new_password_verify)
    {
        if (!account_->password_match(old_password))
        {
            user_interface_->set_statusbar_text(string_to_elements(
                "\\[1Old password did not match."));
            return;
        }
        
        if (new_password != new_password_verify)
        {
            user_interface_->set_statusbar_text(string_to_elements(
                "\\[1New passwords did not match."));
            return;
        }
        
        account_->set_password(new_password);

        try
        {
            context_->save_account(account_);
        }
        catch(std::exception &ex)
        {
            // TODO: Use an actual logging library for this message.
            printf("Error saving account: %s\n", ex.what());

            user_interface_->set_statusbar_text(string_to_elements(
                "\\[1Unexpected error saving your account.  "
                "Please try again."));

            return;
        }

        user_interface_->select_face(hugin::FACE_MAIN);
    }
    
    // ======================================================================
    // ON_PASSWORD_CHANGE_CANCELLED
    // ======================================================================
    void on_password_change_cancelled()
    {
        user_interface_->select_face(hugin::FACE_MAIN);
    }
    
    // ======================================================================
    // ON_INPUT_ENTERED
    // ======================================================================
    void on_input_entered(string const &input)
    {
        shared_ptr<client> player = self_.shared_from_this();
        
        assert(player != NULL);
        
        if (input.empty())
        {
            return;
        }
        
        BOOST_AUTO(command_mode, account_->get_command_mode());
        
        if (command_mode == account::command_mode_mud)
        {
            on_command(input);
        }
        else
        {
            if (input[0] == '/')
            {
                if (input.size() >= 2 && input[1] == '!')
                {
                    on_input_entered("/" + last_command_);
                }
                else
                {
                    on_command(input.substr(1));
                }
            }
            else
            {
                INVOKE_PARADICE_COMMAND(say, context_, input, player);
            }
        }
    }
    
    // ======================================================================
    // ON_COMMAND
    // ======================================================================
    void on_command(string const &input)
    {
        shared_ptr<client> player = self_.shared_from_this();
        assert(player != NULL);
        
        user_interface_->add_command_history(input);
    
        BOOST_AUTO(arg, odin::tokenise(input));
    
        // Transform the command to lower case.
        for (string::iterator ch = arg.first.begin();
             ch != arg.first.end();
             ++ch)
        {
            *ch = tolower(*ch);
        }
    
        if (arg.first == "!")
        {
            on_input_entered(last_command_);
            return;
        }
    
        BOOST_AUTO(admin_level, account_->get_admin_level());
        
        // Search through the list for commands
        BOOST_FOREACH(command const &cur_command, command_list)
        {
            if (cur_command.command_ == arg.first)
            {
                // If the account in question has the required access rights,
                // then execute the command.  Otherwise, pretend it doesn't
                // exist.
                if (admin_level >= cur_command.admin_level_required_)
                {
                    cur_command.function_(context_, arg.second, player);
                    last_command_ = input;
                    return;
                }
            }
        }
    
        string text = 
            "\nDidn't understand that.  Available commands are:\n";
    
        BOOST_FOREACH(command const &cur_command, command_list)
        {
            if (admin_level >= cur_command.admin_level_required_)
            {
                text += cur_command.command_ + " ";
            }
        }
    
        text += "\n";
    
        send_to_player(context_, text, player);
    }

    client                            &self_;
    boost::asio::strand                strand_;
    shared_ptr<context>                context_;
    shared_ptr<account>                account_;
    shared_ptr<character>              character_;

    shared_ptr<connection>             connection_;
    shared_ptr<munin::window>          window_;
    shared_ptr<hugin::user_interface>  user_interface_;

    mutex                              dispatch_queue_mutex_;
    deque< function<void ()> >         dispatch_queue_;
    string                             last_command_;

private :
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
client::client(
    boost::asio::io_service &io_service
  , shared_ptr<context>      ctx)
{
    pimpl_ = make_shared<impl>(ref(*this), ref(io_service), ctx);
}
    
// ==========================================================================
// DESTRUCTOR
// ==========================================================================
client::~client()
{
}

// ==========================================================================
// SET_CONNECTION
// ==========================================================================
void client::set_connection(shared_ptr<connection> const &cnx)
{
    pimpl_->set_connection(cnx);
}

// ==========================================================================
// GET_USER_INTERFACE
// ==========================================================================
shared_ptr<hugin::user_interface> client::get_user_interface()
{
    return pimpl_->get_user_interface();
}

// ==========================================================================
// GET_WINDOW
// ==========================================================================
shared_ptr<munin::window> client::get_window()
{
    return pimpl_->get_window();
}

// ==========================================================================
// SET_WINDOW_TITLE
// ==========================================================================
void client::set_window_title(string const &title)
{
    pimpl_->set_window_title(title);
}

// ==========================================================================
// SET_WINDOW_SIZE
// ==========================================================================
void client::set_window_size(u16 width, u16 height)
{
    pimpl_->set_window_size(width, height);
}

// ==========================================================================
// SET_ACCOUNT
// ==========================================================================
void client::set_account(shared_ptr<account> acc)
{
    pimpl_->set_account(acc);
}

// ==========================================================================
// GET_ACCOUNT
// ==========================================================================
shared_ptr<account> client::get_account() const
{
    return pimpl_->get_account();
}

// ==========================================================================
// SET_CHARACTER
// ==========================================================================
void client::set_character(shared_ptr<character> ch)
{
    pimpl_->set_character(ch);
}

// ==========================================================================
// GET_CHARACTER
// ==========================================================================
shared_ptr<character> client::get_character() const
{
    return pimpl_->get_character();
}

// ==========================================================================
// DISCONNECT
// ==========================================================================
void client::disconnect()
{
    pimpl_->disconnect();
}

// ==========================================================================
// ON_CONNECTION_DEATH
// ==========================================================================
void client::on_connection_death(function<void ()> callback)
{
    pimpl_->on_connection_death(callback);
}

}

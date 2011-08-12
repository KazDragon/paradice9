// ==========================================================================
// Paradice9 Application
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
#include "paradice9.hpp"
#include "context_impl.hpp"
#include "paradice/account.hpp"
#include "paradice/admin.hpp"
#include "paradice/character.hpp"
#include "paradice/client.hpp"
#include "paradice/communication.hpp"
#include "paradice/configuration.hpp"
#include "paradice/connection.hpp"
#include "paradice/help.hpp"
#include "paradice/rules.hpp"
#include "paradice/server.hpp"
#include "paradice/who.hpp"
#include "hugin/user_interface.hpp"
#include "munin/container.hpp"
#include "munin/window.hpp"
#include "munin/grid_layout.hpp"
#include "odin/net/socket.hpp"
#include "odin/telnet/protocol.hpp"
#include "odin/tokenise.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace munin::ansi;
using namespace munin;
using namespace odin;
using namespace boost;
using namespace std;

namespace {
    #define PARADICE_CMD_ENTRY_NOP(name) \
        { name,        NULL                                  , 0 }
        
    #define PARADICE_CMD_ENTRY(func) \
        { #func,       bind(&paradice::do_##func, _1, _2, _3), 0 }
        
    #define PARADICE_CMD_ALIAS(func, alias) \
        { alias,       bind(&paradice::do_##func, _1, _2, _3), 0 }
        
    #define PARADICE_ADMIN_ENTRY(func, level) \
        { #func,       bind(&paradice::do_##func, _1, _2, _3), (level) }
        
    typedef function<void (
        shared_ptr<paradice::context> ctx
      , std::string                   args
      , shared_ptr<paradice::client>  player)> paradice_command;
    
    static struct command
    {
        string           command_;
        paradice_command function_;
        u32              admin_level_required_;
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
// PARADICE9::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct paradice9::impl
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(
        asio::io_service &io_service
      , unsigned int      port)
        : io_service_(io_service) 
        , server_(new paradice::server(
              io_service_
            , port
            , bind(&impl::on_accept, this, _1)))
        , context_(make_shared<context_impl>(server_))
    {
    }

private :
    // ======================================================================
    // ON_ACCEPT
    // ======================================================================
    void on_accept(shared_ptr<odin::net::socket> socket)
    {
        // Create the connection and client structures for the socket.
        BOOST_AUTO(connection, make_shared<paradice::connection>(socket));
        BOOST_AUTO(client, make_shared<paradice::client>());
        client->set_connection(connection);
        
        // SOCKET CALLBACKS
        socket->on_death(bind(
            &impl::on_death
          , this
          , weak_ptr<paradice::client>(client)));
    
        // CONNECTION CALLBACKS
        connection->on_window_size_changed(
            bind(
                &impl::on_window_size_changed
              , this
              , weak_ptr<paradice::connection>(connection)
              , _1
              , _2));

        pending_clients_.push_back(client);
        
        connection->async_get_terminal_type(
            bind(
                &impl::on_terminal_type
              , this
              , weak_ptr<odin::net::socket>(socket)
              , weak_ptr<paradice::client>(client)
              , _1));
    }

    // ======================================================================
    // ON_TERMINAL_TYPE
    // ======================================================================
    void on_terminal_type(
        weak_ptr<odin::net::socket>  weak_socket
      , weak_ptr<paradice::client>   weak_client
      , string const                &terminal_type)
    {
        printf("Terminal type is: \"%s\"\n", terminal_type.c_str());
        
        BOOST_AUTO(socket, weak_socket.lock());
        BOOST_AUTO(client, weak_client.lock());
        
        if (socket != NULL && client != NULL)
        {
            pending_clients_.erase(remove(
                pending_clients_.begin()
              , pending_clients_.end()
              , client)
                , pending_clients_.end());
              
            // Complete the setup of the connection and client.
            BOOST_AUTO(connection, client->get_connection());
    
            connection->on_text(
                bind(
                    &impl::on_text
                  , this
                  , weak_ptr<paradice::connection>(connection)
                  , _1));
        
            connection->on_mouse_report(
                bind(
                    &impl::on_mouse_report
                  , this
                  , weak_ptr<paradice::connection>(connection)
                  , _1));
            
            connection->on_control_sequence(
                bind(
                    &impl::on_control_sequence
                  , this
                  , weak_ptr<paradice::connection>(connection)
                  , _1));
            
            BOOST_AUTO(window,  connection->get_window());
            BOOST_AUTO(content, window->get_content());
        
            // WINDOW CALLBACKS
            window->on_repaint.connect(
                bind(
                    &impl::on_repaint
                  , this
                  , weak_ptr<odin::net::socket>(socket)
                  , _1));
        
            BOOST_AUTO(user_interface, make_shared<hugin::user_interface>());
            
            // USER INTERFACE CALLBACKS
            user_interface->on_account_details_entered(bind(
                &impl::on_account_details_entered
              , this
              , weak_ptr<paradice::client>(client)
              , _1
              , _2));
        
            user_interface->on_account_created(bind(
                &impl::on_account_created
              , this
              , weak_ptr<paradice::client>(client)
              , _1
              , _2
              , _3));
                
            user_interface->on_account_creation_cancelled(bind(
                &impl::on_account_creation_cancelled
              , this
              , weak_ptr<paradice::client>(client)));
            
            user_interface->on_input_entered(bind(
                &impl::on_input_entered
              , this
              , weak_ptr<paradice::client>(client)
              , _1));
    
            user_interface->on_new_character(bind(
                &impl::on_new_character
              , this
              , weak_ptr<paradice::client>(client)));
    
            user_interface->on_character_selected(bind(
                &impl::on_character_selected
              , this
              , weak_ptr<paradice::client>(client)
              , _1));
            
            user_interface->on_character_created(bind(
                &impl::on_character_created
              , this
              , weak_ptr<paradice::client>(client)
              , _1));
            
            user_interface->on_character_creation_cancelled(bind(
                &impl::on_character_creation_cancelled
              , this
              , weak_ptr<paradice::client>(client)));
            
            user_interface->on_help_closed(bind(
                &impl::on_help_closed
              , this
              , weak_ptr<paradice::client>(client)));
            
            user_interface->on_password_changed(bind(
                &impl::on_password_changed
              , this
              , weak_ptr<paradice::client>(client)
              , _1
              , _2
              , _3));
            
            user_interface->on_password_change_cancelled(bind(
                &impl::on_password_change_cancelled
              , this
              , weak_ptr<paradice::client>(client)));
            
            client->set_user_interface(user_interface);
            client->set_window_title("Paradice9");
            
            context_->add_client(client);
            context_->update_names();
            
            content->set_size(munin::extent(80, 24));
            content->set_layout(make_shared<grid_layout>(1, 1));
        
            content->add_component(user_interface);
            content->set_focus();
        
            window->enable_mouse_tracking();
            window->on_repaint(clear_screen());
            window->on_repaint(set_normal_cursor_keys());
        }
    }
    
    // ======================================================================
    // ON_DEATH
    // ======================================================================
    void on_death(weak_ptr<paradice::client> &weak_client)
    {
        BOOST_AUTO(client, weak_client.lock());
    
        if (client != NULL)
        {
            pending_clients_.erase(remove(
                pending_clients_.begin()
              , pending_clients_.end()
              , client)
                , pending_clients_.end());
            
            context_->remove_client(client);
            context_->update_names();
    
            BOOST_AUTO(character, client->get_character());
            
            if (character != NULL)
            {
                BOOST_AUTO(name, character->get_name());
            
                if (!name.empty())
                {
                    paradice::send_to_all(
                        context_
                      , "#SERVER: "
                      + context_->get_moniker(character)
                      + " has left Paradice.\n");
                }
            }
        }
    }
    
    // ======================================================================
    // ON_WINDOW_SIZE_CHANGED
    // ======================================================================
    void on_window_size_changed(
        weak_ptr<paradice::connection> weak_connection,
        u16                            width,
        u16                            height)
    {
        BOOST_AUTO(connection, weak_connection.lock());
    
        if (connection != NULL)
        {
            BOOST_AUTO(window, connection->get_window());
            
            if (window != NULL)
            {
                window->set_size(munin::extent(width, height));
            }
        }
    }
    
    // ======================================================================
    // ON_TEXT
    // ======================================================================
    void on_text(
        weak_ptr<paradice::connection>  weak_connection
      , string const                   &text)
    {
        BOOST_AUTO(connection, weak_connection.lock());
    
        if (connection != NULL)
        {
            BOOST_AUTO(window, connection->get_window());

            if (window != NULL)
            {
                BOOST_FOREACH(char ch, text)
                {
                    window->event(ch);
                }
            }
        }
    }
    
    // ======================================================================
    // ON_MOUSE_REPORT
    // ======================================================================
    void on_mouse_report(
        weak_ptr<paradice::connection>  weak_connection
      , odin::ansi::mouse_report const &report)
    {
        BOOST_AUTO(connection, weak_connection.lock());
        
        if (connection != NULL)
        {
            BOOST_AUTO(window, connection->get_window());

            if (window != NULL)
            {
                window->event(report);
            }
        }
    }
    
    // ======================================================================
    // ON_CONTROL_SEQUENCE
    // ======================================================================
    void on_control_sequence(
        weak_ptr<paradice::connection>      weak_connection
      , odin::ansi::control_sequence const &control_sequence)
    {
        BOOST_AUTO(connection, weak_connection.lock());
        
        if (connection != NULL)
        {
            BOOST_AUTO(window, connection->get_window());

            if (window != NULL)
            {
                window->event(control_sequence);
            }
        }
    }
    
    // ======================================================================
    // ON_REPAINT
    // ======================================================================
    void on_repaint(
        weak_ptr<odin::net::socket>  weak_socket
      , string const                &paint_data)
    {
        BOOST_AUTO(socket, weak_socket.lock());
    
        if (socket != NULL)
        {
            runtime_array<u8> data(paint_data.size());
            copy(paint_data.begin(), paint_data.end(), data.begin());
    
            socket->async_write(data, NULL);
        }
    }
    
    // ======================================================================
    // ON_ACCOUNT_DETAILS_ENTERED
    // ======================================================================
    void on_account_details_entered(
        weak_ptr<paradice::client>  weak_client
      , string               const &username
      , string               const &password)
    {
        BOOST_AUTO(client, weak_client.lock());

        if (client != NULL)
        {
            BOOST_AUTO(user_interface, client->get_user_interface());
            
            if (username == "" && password == "")
            {
                user_interface->clear_account_creation_screen();
                user_interface->select_face(hugin::FACE_ACCOUNT_CREATION);
                user_interface->set_focus();
            }
            else
            {
                std::string account_name(username);
                capitalise(account_name);

                BOOST_AUTO(account, context_->load_account(account_name));                

                if (account == NULL)
                {
                    attribute pen;
                    pen.foreground_colour_ = odin::ansi::graphics::COLOUR_RED;
                    
                    user_interface->set_statusbar_text(elements_from_string(
                        "Invalid username/password combination"
                      , pen));
                    return;
                }
                
                if (!account->password_match(password))
                {
                    attribute pen;
                    pen.foreground_colour_ = odin::ansi::graphics::COLOUR_RED;
                    
                    user_interface->set_statusbar_text(elements_from_string(
                        "Invalid username/password combination"
                      , pen));
                    return;
                }

                client->set_account(account);
                
                // Ensure that this is the only use of this account.
                remove_duplicate_accounts(client);
                
                BOOST_AUTO(
                    number_of_characters
                  , account->get_number_of_characters());
                
                runtime_array< pair<string, string> > characters(
                    number_of_characters);
                
                for(size_t index = 0; index < number_of_characters; ++index)
                {
                    BOOST_AUTO(name, account->get_character_name(index));
                    BOOST_AUTO(character, context_->load_character(name));                    
                    
                    if (character != NULL)
                    {
                        characters[index] = 
                            make_pair(name, context_->get_moniker(character)); 
                    }
                }
                
                user_interface->set_character_names(characters);
                user_interface->select_face(hugin::FACE_CHAR_SELECTION);
                user_interface->set_focus();
            }
        }
    }
    
    // ======================================================================
    // ON_ACCOUNT_CREATED
    // ======================================================================
    void on_account_created(
        weak_ptr<paradice::client>  weak_client
      , string const               &account_name
      , string const               &password
      , string const               &password_verify)
    {
        BOOST_AUTO(client, weak_client.lock());

        if (client != NULL)
        {
            BOOST_AUTO(user_interface, client->get_user_interface());
            
            attribute pen;
            pen.foreground_colour_ = odin::ansi::graphics::COLOUR_RED;
            
            // Check that the account name is valid.  If not, report an
            // error message and return.  This also removes cases where the
            // account name could have filesystem characters in it such as
            // * or ../, which could potentially wreck the system.
            if (!paradice::is_acceptible_name(account_name))
            {
                user_interface->set_statusbar_text(elements_from_string(
                    "Name must be alphabetic only and at least three "
                    "characters long"
                  , pen));
                return;
            }
            
            // Check to see if the account name exists already.  If so,
            // report an error message and return.
            BOOST_AUTO(test_account, context_->load_account(account_name));
            
            if (test_account != NULL)
            {
                user_interface->set_statusbar_text(elements_from_string(
                    "An account with that name already exists"
                  , pen));
                return;
            }

            // Check to see if the passwords match.  If not, report an error
            // message and return.
            if (password != password_verify)
            {
                user_interface->set_statusbar_text(elements_from_string(
                    "Passwords do not match"
                  , pen));
                return;
            }
            
            shared_ptr<paradice::account> account(new paradice::account);
            account->set_name(account_name);
            account->set_password(password);
            
            context_->save_account(account);
            client->set_account(account);
            
            user_interface->select_face(hugin::FACE_CHAR_SELECTION);
        }
    }

    // ======================================================================
    // ON_ACCOUNT_CREATION_CANCELLED
    // ======================================================================
    void on_account_creation_cancelled(weak_ptr<paradice::client> weak_client)
    {
        BOOST_AUTO(client, weak_client.lock());

        if (client != NULL)
        {
            BOOST_AUTO(user_interface, client->get_user_interface());
            
            user_interface->clear_intro_screen();
            user_interface->select_face(hugin::FACE_INTRO);
            user_interface->set_focus();
        }
    }
    
    // ======================================================================
    // ON_INPUT_ENTERED
    // ======================================================================
    void on_input_entered(
        weak_ptr<paradice::client>  weak_client
      , string const               &input)
    {
        BOOST_AUTO(client, weak_client.lock());
        
        if (client != NULL)
        {
            if (input.empty())
            {
                return;
            }
            
            BOOST_AUTO(command_mode, client->get_account()->get_command_mode());
            
            if (command_mode == paradice::account::command_mode_mud)
            {
                on_command(client, input);
            }
            else
            {
                if (input[0] == '/')
                {
                    if (input.size() >= 2 && input[1] == '!')
                    {
                        on_input_entered(
                            weak_client
                          , "/" + client->get_last_command());
                    }
                    else
                    {
                        on_command(client, input.substr(1));
                    }
                }
                else
                {
                    INVOKE_PARADICE_COMMAND(say, context_, input, client);
                }
            }
        }
    }
    
    // ======================================================================
    // ON_NEW_CHARACTER
    // ======================================================================
    void on_new_character(
        weak_ptr<paradice::client> weak_client)
    {
        BOOST_AUTO(client, weak_client.lock());
        
        if (client != NULL)
        {
            BOOST_AUTO(user_interface, client->get_user_interface());
            
            user_interface->select_face(hugin::FACE_CHAR_CREATION);
            user_interface->clear_character_creation_screen();
            user_interface->set_focus();
        }            
    }

    // ======================================================================
    // ON_CHARACTER_SELECTED
    // ======================================================================
    void on_character_selected(
        weak_ptr<paradice::client>  weak_client
      , string const               &character_name)
    {
        BOOST_AUTO(client, weak_client.lock());
        
        if (client != NULL)
        {
            BOOST_AUTO(character, context_->load_character(character_name));
            client->set_character(character);
            context_->update_names();
            client->get_user_interface()->select_face(hugin::FACE_MAIN);
            client->set_window_title(character->get_name() + " - Paradice9");

            paradice::send_to_all(
                context_
              , "#SERVER: "
              + context_->get_moniker(character)
              + " has entered Paradice.\n");
        }
    }

    // ======================================================================
    // ON_CHARACTER_CREATED
    // ======================================================================
    void on_character_created(
        weak_ptr<paradice::client> weak_client
      , string                     character_name)
    {
        BOOST_AUTO(client, weak_client.lock());
        
        if (client != NULL)
        {
            capitalise(character_name);
            
            BOOST_AUTO(user_interface, client->get_user_interface());
            
            // Check that the name is appropriate.
            if (!paradice::is_acceptible_name(character_name))
            {
                attribute pen;
                pen.foreground_colour_ = odin::ansi::graphics::COLOUR_RED;
                
                user_interface->set_statusbar_text(elements_from_string(
                    "Name must be alphabetic only and at least three "
                    "characters long"
                  , pen));
                return;
            }
            
            // Test that the character doesn't already exist.
            BOOST_AUTO(test_character, context_->load_character(character_name));
            
            if (test_character != NULL)
            {
                attribute pen;
                pen.foreground_colour_ = odin::ansi::graphics::COLOUR_RED;
                
                user_interface->set_statusbar_text(elements_from_string(
                    "A character with that name already exists"
                  , pen));
                return;
            }
    
            shared_ptr<paradice::character> character(new paradice::character);
            character->set_name(character_name);
            context_->save_character(character);
            
            BOOST_AUTO(account, client->get_account());
            account->add_character(character_name);
            context_->save_account(account);
            client->set_character(character);
            context_->update_names();

            client->set_window_title(character_name + " - Paradice9");
            
            client->get_user_interface()->select_face(hugin::FACE_MAIN);
            
            paradice::send_to_all(
                context_
              , "#SERVER: "
              + context_->get_moniker(character)
              + " has entered Paradice.\n");
        }        
    }
    
    // ======================================================================
    // ON_CHARACTER_CREATION_CANCELLED
    // ======================================================================
    void on_character_creation_cancelled(
        weak_ptr<paradice::client> weak_client)
    {
        BOOST_AUTO(client, weak_client.lock());
        
        if (client != NULL)
        {
            client->get_user_interface()->select_face(
                hugin::FACE_CHAR_SELECTION);
            client->get_user_interface()->set_focus();
        }
    }
    
    // ======================================================================
    // ON_HELP_CLOSED
    // ======================================================================
    void on_help_closed(
        weak_ptr<paradice::client> weak_client)
    {
        BOOST_AUTO(client, weak_client.lock());
        
        if (client != NULL)
        {
            client->get_user_interface()->hide_help_window();
        }
    }
    
    // ======================================================================
    // ON_PASSWORD_CHANGED
    // ======================================================================
    void on_password_changed(
        weak_ptr<paradice::client>  weak_client
      , string const               &old_password
      , string const               &new_password
      , string const               &new_password_verify)
    {
        BOOST_AUTO(client, weak_client.lock());
        
        if (client != NULL)
        {
            BOOST_AUTO(account, client->get_account());
            BOOST_AUTO(user_interface, client->get_user_interface());
            
            attribute error_pen;
            error_pen.foreground_colour_ = odin::ansi::graphics::COLOUR_RED;
                  
            if (!account->password_match(old_password))
            {
                user_interface->set_statusbar_text(elements_from_string(
                    "Old password did not match."
                  , error_pen));
                return;
            }
            
            if (new_password != new_password_verify)
            {
                user_interface->set_statusbar_text(elements_from_string(
                    "New passwords did not match."
                  , error_pen));
                return;
            }
            
            account->set_password(new_password);
            context_->save_account(account);
            user_interface->select_face(hugin::FACE_MAIN);
        }
    }
    
    // ======================================================================
    // ON_PASSWORD_CHANGE_CANCELLED
    // ======================================================================
    void on_password_change_cancelled(
        weak_ptr<paradice::client> weak_client)
    {
        BOOST_AUTO(client, weak_client.lock());
        
        if (client != NULL)
        {
            client->get_user_interface()->select_face(hugin::FACE_MAIN);
        }
    }
    
    // ======================================================================
    // ON_COMMAND
    // ======================================================================
    void on_command(
        shared_ptr<paradice::client>   client
      , string const                  &input)
    {
        client->get_user_interface()->add_command_history(input);
    
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
            on_input_entered(client, client->get_last_command());
            return;
        }
    
        BOOST_AUTO(admin_level, client->get_account()->get_admin_level());
        
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
                    cur_command.function_(context_, arg.second, client);
                    client->set_last_command(input);
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
    
        paradice::send_to_player(context_, text, client);
    }
    
    // ======================================================================
    // REMOVE_DUPLICATE_ACCOUNTS
    // ======================================================================
    void remove_duplicate_accounts(shared_ptr<paradice::client> client)
    {
        BOOST_AUTO(account, client->get_account());
        
        vector< shared_ptr<paradice::client> > clients_to_remove;
        
        BOOST_AUTO(clients, context_->get_clients());
        
        BOOST_FOREACH(shared_ptr<paradice::client> current_client, clients)
        {
            if (current_client != client)
            {
                BOOST_AUTO(current_account, current_client->get_account());
                
                if (current_account->get_name() == account->get_name())
                {
                    clients_to_remove.push_back(current_client);
                }
            }
        }
        
        BOOST_FOREACH(
            shared_ptr<paradice::client> current_client
          , clients_to_remove)
        {
            current_client->get_connection()->disconnect();
        }
    }
    
    asio::io_service              &io_service_;
    shared_ptr<paradice::server>   server_;
    shared_ptr<paradice::context>  context_;
    
    // A vector of clients whose connections are being negotiated.
    vector< shared_ptr<paradice::client> > pending_clients_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
paradice9::paradice9(
    asio::io_service &io_service
  , unsigned int      port)
    : pimpl_(new impl(io_service, port))  
{
}


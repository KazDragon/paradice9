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
#include "gm.hpp"
#include "help.hpp"
#include "rules.hpp"
#include "who.hpp"
#include "hugin/user_interface.hpp"
#include "munin/algorithm.hpp"
#include "munin/container.hpp"
#include "munin/grid_layout.hpp"
#include "munin/window.hpp"
#include "odin/tokenise.hpp"
#include "terminalpp/encoder.hpp"
#include "terminalpp/string.hpp"
#include <boost/asio/strand.hpp>
#include <boost/format.hpp>
#include <cstdio>
#include <deque>
#include <mutex>
#include <string>
#include <vector>

namespace paradice {

namespace {
    #define PARADICE_CMD_ENTRY_NOP(name) \
        { name,        NULL                                  , 0, 0 }

    #define PARADICE_CMD_ENTRY(func) \
        { #func,        [](auto ctx, auto args, auto player) \
                        {do_##func(ctx, args, player);}, \
                        0, 0 \
        }

    #define PARADICE_CMD_ALIAS(func, alias) \
        { alias,        [](auto ctx, auto args, auto player) \
                        {do_##func(ctx, args, player);}, \
                        0, 0 \
        }

    #define PARADICE_ADMIN_ENTRY(func, level) \
        { #func,        [](auto ctx, auto args, auto player) \
                        {do_##func(ctx, args, player);}, \
                        (level), 0 \
        }

    #define PARADICE_GM_ENTRY(func, level) \
        { #func,        [](auto ctx, auto args, auto player) \
                        {do_##func(ctx, args, player);}, \
                        0, (level) \
        }

    typedef std::function<void (
        std::shared_ptr<paradice::context> ctx,
        std::string                        args,
        std::shared_ptr<paradice::client>  player)> paradice_command;

    static struct command
    {
        std::string      command_;
        paradice_command function_;
        odin::u32        admin_level_required_;
        odin::u32        gm_level_required_;
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

      , PARADICE_GM_ENTRY(gm, 100)

      , PARADICE_ADMIN_ENTRY(admin_set_password, 100)
      , PARADICE_ADMIN_ENTRY(admin_shutdown,     100)
    };

    #undef PARADICE_CMD_ENTRY_NOP
    #undef PARADICE_CMD_ENTRY
    #undef PARADICE_CMD_ALIAS

    // ======================================================================
    // CAPITALISE
    // ======================================================================
    static void capitalise(std::string &name)
    {
        if (!name.empty())
        {
            name[0] = toupper(name[0]);

            for (std::string::size_type index = 1; index < name.size(); ++index)
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
    : public std::enable_shared_from_this<client::impl>
{
    static terminalpp::terminal::behaviour create_behaviour()
    {
        terminalpp::terminal::behaviour behaviour;
        behaviour.can_use_eight_bit_control_codes = true;
        behaviour.supports_basic_mouse_tracking = true;
        behaviour.supports_window_title_bel = true;
        
        return behaviour;
    }
    
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(
        client                  &self,
        boost::asio::io_service &io_service,
        std::shared_ptr<context>      ctx)
      : self_(self),
        strand_(io_service),
        context_(ctx),
        window_(std::make_shared<munin::window>(
            std::ref(strand_), create_behaviour())),
        user_interface_(std::make_shared<hugin::user_interface>(std::ref(strand_)))
    {
        window_->set_size(terminalpp::extent(80, 24));
    }

    // ======================================================================
    // SET_CONNECTION
    // ======================================================================
    void set_connection(std::shared_ptr<connection> cnx)
    {
        connection_ = cnx;

        // CONNECTION CALLBACKS
        connection_->on_data_read(
            [this](std::string const &data)
            {
                std::unique_lock<std::mutex> lock(dispatch_queue_mutex_);
                dispatch_queue_.push_back(
                    bind(&munin::window::data, window_, data));
                strand_.post(bind(&impl::dispatch_queue, shared_from_this()));
            });

        connection_->on_window_size_changed(
            [this](auto const &width, auto const &height)
            {
                std::cout << "WINDOW SIZE = " << width << ", " << height << "\n";
                on_window_size_changed(width, height);
            });

        // WINDOW CALLBACKS
        window_->on_repaint.connect(
            [this](auto const &regions)
            {
                this->on_repaint(regions);
            });

        // USER INTERFACE CALLBACKS
        user_interface_->on_input_entered(
            [this](auto const &input)
            {
                this->on_input_entered(input);
            });

        user_interface_->on_login.connect(
            [this](auto const &name, auto const &pwd)
            {
                this->on_login(name, pwd);
            });

        user_interface_->on_new_account.connect(
            [this]
            {
                this->on_new_account();
            });

        user_interface_->on_account_created(
            [this](auto const &name, auto const &pwd, auto const &pwd_verify)
            {
                this->on_account_created(name, pwd, pwd_verify);
            });

        user_interface_->on_account_creation_cancelled(
            [this]
            {
                this->on_account_creation_cancelled();
            });

        user_interface_->on_new_character(
            [this]
            {
                this->on_new_character();
            });

        user_interface_->on_character_selected(
            [this](auto const &idx)
            {
                this->on_character_selected(idx);
            });

        user_interface_->on_character_created(
            [this](auto const &name, auto const &is_gm)
            {
                this->on_character_created(name, is_gm);
            });

        user_interface_->on_character_creation_cancelled(
            [this]
            {
                this->on_character_creation_cancelled();
            });

        user_interface_->on_gm_tools_back(
            [this]
            {
                this->on_gm_tools_back();
            });

        user_interface_->on_gm_fight_beast(
            [this](auto const &beast)
            {
                this->on_gm_fight_beast(beast);
            });

        user_interface_->on_gm_fight_encounter(
            [this](auto const &encounter)
            {
                this->on_gm_fight_encounter(encounter);
            });

        user_interface_->on_help_closed(
            [this]
            {
                this->on_help_closed();
            });

        user_interface_->on_password_changed.connect(
            [this](
                auto const &old_pwd,
                auto const &new_pwd,
                auto const &new_pwd_verify)
            {
                this->on_password_changed(old_pwd, new_pwd, new_pwd_verify);
            });

        user_interface_->on_password_change_cancelled.connect(
            [this]
            {
                this->on_password_change_cancelled();
            });

        set_window_title("Paradice9");

        auto content = window_->get_content();
        content->set_layout(std::make_shared<munin::grid_layout>(1, 1));
        content->add_component(user_interface_);
        content->set_focus();

        window_->enable_mouse_tracking();
    }

    // ======================================================================
    // SET_ACCOUNT
    // ======================================================================
    void set_account(std::shared_ptr<account> acc)
    {
        account_ = acc;
    }

    // ======================================================================
    // GET_ACCOUNT
    // ======================================================================
    std::shared_ptr<account> get_account()
    {
        return account_;
    }

    // ======================================================================
    // SET_CHARACTER
    // ======================================================================
    void set_character(std::shared_ptr<character> ch)
    {
        character_ = ch;
    }

    // ======================================================================
    // GET_CHARACTER
    // ======================================================================
    std::shared_ptr<character> get_character()
    {
        return character_;
    }

    // ======================================================================
    // GET_USER_INTERFACE
    // ======================================================================
    std::shared_ptr<hugin::user_interface> get_user_interface()
    {
        return user_interface_;
    }

    // ======================================================================
    // GET_WINDOW
    // ======================================================================
    std::shared_ptr<munin::window> get_window()
    {
        return window_;
    }

    // ======================================================================
    // SET_WINDOW_TITLE
    // ======================================================================
    void set_window_title(std::string const &title)
    {
        {
            std::unique_lock<std::mutex> lock(dispatch_queue_mutex_);
            dispatch_queue_.push_back(bind(
                &munin::window::set_title, window_, title));
        }

        strand_.post(bind(&impl::dispatch_queue, shared_from_this()));
    }

    // ======================================================================
    // SET_WINDOW_SIZE
    // ======================================================================
    void set_window_size(odin::u16 width, odin::u16 height)
    {
        {
            std::unique_lock<std::mutex> lock(dispatch_queue_mutex_);
            dispatch_queue_.push_back(bind(
                &munin::window::set_size, window_, terminalpp::extent(width, height)));
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
    void on_connection_death(std::function<void ()> const &callback)
    {
        connection_->on_socket_death(callback);
    }

private :
    // ======================================================================
    // ON_WINDOW_SIZE_CHANGED
    // ======================================================================
    void on_window_size_changed(odin::u16 width, odin::u16 height)
    {
        std::unique_lock<std::mutex> lock(dispatch_queue_mutex_);
        dispatch_queue_.push_back(
            bind(&munin::window::set_size, window_, terminalpp::extent(width, height)));
        strand_.post(bind(&impl::dispatch_queue, shared_from_this()));
    }

    // ======================================================================
    // ON_REPAINT
    // ======================================================================
    void on_repaint(std::string const &paint_data)
    {
        std::vector<odin::u8> data(paint_data.size());
        std::copy(paint_data.begin(), paint_data.end(), data.begin());

        connection_->write(data);
    }

    // ======================================================================
    // REMOVE_DUPLICATE_ACCOUNTS
    // ======================================================================
    void remove_duplicate_accounts(std::shared_ptr<account> acc)
    {
        std::vector<std::shared_ptr<client>> clients_to_remove;
        auto clients = context_->get_clients();

        for (auto current_client : clients)
        {
            auto current_account = current_client->get_account();

            if (current_account != NULL && current_account != acc)
            {
                if (current_account->get_name() == acc->get_name())
                {
                    clients_to_remove.push_back(current_client);
                }
            }
        }

        for (auto current_client : clients_to_remove)
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
        auto number_of_characters = account_->get_number_of_characters();

        std::vector<std::pair<std::string, std::string>> characters(
            number_of_characters);

        for(size_t index = 0; index < number_of_characters; ++index)
        {
            auto name = account_->get_character_name(index);

            try
            {
                auto character = context_->load_character(name);

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
    // ON_LOGIN
    // ======================================================================
    void on_login(
        std::string const &username,
        std::string const &password)
    {
        using namespace terminalpp::literals;
        
        std::string account_name(username);
        capitalise(account_name);

        std::shared_ptr<account> account;

        try
        {
            account = context_->load_account(account_name);
        }
        catch(std::exception &ex)
        {
            // TODO: Use an actual logging library for this message.
            printf("Error loading account: %s\n", ex.what());
            user_interface_->set_statusbar_text(
                "\\[1Invalid username/password combination"_ets);
            return;
        }

        if (account == NULL)
        {
            user_interface_->set_statusbar_text(
                "\\[1Invalid username/password combination"_ets);
            return;
        }

        if (!account->password_match(password))
        {
            user_interface_->set_statusbar_text(
                "\\[1Invalid username/password combination"_ets);
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

    // ======================================================================
    // ON_NEW_ACCOUNT
    // ======================================================================
    void on_new_account()
    {
        user_interface_->clear_account_creation_screen();
        user_interface_->select_face(hugin::FACE_ACCOUNT_CREATION);
        user_interface_->set_focus();
    }

    // ======================================================================
    // ON_ACCOUNT_CREATED
    // ======================================================================
    void on_account_created(
        std::string        account_name,
        std::string const &password,
        std::string const &password_verify)
    {
        using namespace terminalpp::literals;

        // Check that the account name is valid.  If not, report an
        // error message and return.  This also removes cases where the
        // account name could have filesystem characters in it such as
        // * or ../, which could potentially wreck the system.
        if (!is_acceptible_name(account_name))
        {
            user_interface_->set_statusbar_text(
                "\\[1Name must be alphabetic only and at least three "
                "characters long"_ets);
            return;
        }

        // Check to see if the account name exists already.  If so,
        // report an error message and return.
        std::shared_ptr<account> test_account;

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

            user_interface_->set_statusbar_text(
                "\\1[That account name is unavailable.  Please try with "
                "a different name"_ets);
            return;
        }

        if (test_account != NULL)
        {
            user_interface_->set_statusbar_text(
                "\\[1An account with that name already exists"_ets);
            return;
        }

        // Check to see if the passwords match.  If not, report an error
        // message and return.
        if (password != password_verify)
        {
            user_interface_->set_statusbar_text(
                "\\[1Passwords do not match"_ets);
            return;
        }

        auto acc = std::make_shared<account>();
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

            user_interface_->set_statusbar_text(
                "\\[1Unexpected error setting saving your account.  "
                "Please try again."_ets);
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
    void on_character_selected(std::string const &character_name)
    {
        using namespace terminalpp::literals;

        std::shared_ptr<character> ch;

        try
        {
            ch = context_->load_character(character_name);
        }
        catch(std::exception &ex)
        {
            printf("Error loading character %s: %s\n",
                character_name.c_str(), ex.what());

            user_interface_->set_statusbar_text(
                "\\[1Error loading character file."_ets);

            return;
        }

        // If this is a GM character, then check for any other GM characters
        // online, since there can only be one.
        if (ch->get_gm_level() != 0)
        {
            for (auto cli : context_->get_clients())
            {
                if (cli.get() != &self_)
                {
                    auto other_ch = cli->get_character();

                    if (other_ch && other_ch->get_gm_level() != 0)
                    {
                        user_interface_->set_statusbar_text(
                            "\\[1There is already a GM character online."_ets);
                        return;
                    }
                }
            }
        }

        // If this is a GM character, then be sure to set the beasts and
        // encounters in the UI.
        if (ch->get_gm_level() != 0)
        {
            user_interface_->set_beasts(ch->get_beasts());
            user_interface_->set_encounters(ch->get_encounters());
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
    void on_character_created(std::string character_name, bool is_gm)
    {
        using namespace terminalpp::literals;

        capitalise(character_name);

        // Check that the name is appropriate.
        if (!is_acceptible_name(character_name))
        {
            user_interface_->set_statusbar_text(
                "\\[1Name must be alphabetic only and at least three "
                "characters long"_ets);
            return;
        }

        // Test that the character doesn't already exist.
        std::shared_ptr<character> test_character;

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

            user_interface_->set_statusbar_text(
                "\\[1Error testing for that character."_ets);
            return;
        }

        if (test_character != NULL)
        {
            user_interface_->set_statusbar_text(
                "\\[1A character with that name already exists"_ets);
            return;
        }

        character_ = std::make_shared<character>();
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

            user_interface_->set_statusbar_text(
                "\\[1There was an error saving the character."_ets);
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

            user_interface_->set_statusbar_text(
                "\\[1Unexpected error saving your account.  "
                "Please try again."_ets);

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
    // ON_GM_TOOLS_BACK
    // ======================================================================
    void on_gm_tools_back()
    {
        character_->set_beasts(user_interface_->get_beasts());
        character_->set_encounters(user_interface_->get_encounters());
        context_->save_character(character_);

        user_interface_->select_face(hugin::FACE_MAIN);
        user_interface_->set_focus();
    }

    // ======================================================================
    // ON_GM_FIGHT_BEAST
    // ======================================================================
    void on_gm_fight_beast(std::shared_ptr<paradice::beast> beast)
    {
        add_beast(context_->get_active_encounter(), beast);
        context_->update_active_encounter();

        user_interface_->set_statusbar_text(terminalpp::encode(
            boost::str(boost::format("\\[3Added \\x%s\\x\\[3 to active encounter")
                % beast->get_name())));
    }

    // ======================================================================
    // ON_GM_FIGHT_ENCOUNTER
    // ======================================================================
    void on_gm_fight_encounter(std::shared_ptr<paradice::encounter> encounter)
    {
        auto enc = context_->get_active_encounter();

        for (auto beast : encounter->get_beasts())
        {
            add_beast(enc, beast);
        }

        context_->update_active_encounter();

        user_interface_->set_statusbar_text(terminalpp::encode(
            boost::str(boost::format("\\[3Added \\x%s\\x\\[3 to active encounter!")
                % encounter->get_name())));
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
        std::string const &old_password,
        std::string const &new_password,
        std::string const &new_password_verify)
    {
        using namespace terminalpp::literals;

        if (!account_->password_match(old_password))
        {
            user_interface_->set_statusbar_text(
                "\\[1Old password did not match."_ets);
            return;
        }

        if (new_password != new_password_verify)
        {
            user_interface_->set_statusbar_text(
                "\\[1New passwords did not match."_ets);
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

            user_interface_->set_statusbar_text(
                "\\[1Unexpected error saving your account.  "
                "Please try again."_ets);

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
    void on_input_entered(std::string const &input)
    {
        std::shared_ptr<client> player = self_.shared_from_this();

        assert(player != NULL);

        if (input.empty())
        {
            return;
        }

        auto command_mode = account_->get_command_mode();

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
    void on_command(std::string const &input)
    {
        std::shared_ptr<client> player = self_.shared_from_this();
        assert(player != NULL);

        user_interface_->add_command_history(input);

        auto arg = odin::tokenise(input);

        // Transform the command to lower case.
        for (auto ch = arg.first.begin(); ch != arg.first.end(); ++ch)
        {
            *ch = tolower(*ch);
        }

        if (arg.first == "!")
        {
            on_input_entered(last_command_);
            return;
        }

        auto admin_level = account_->get_admin_level();
        auto gm_level = player->get_character()->get_gm_level();

        // Search through the list for commands
        for (auto const &cur_command : command_list)
        {
            if (cur_command.command_ == arg.first)
            {
                // If the account in question has the required access rights,
                // then execute the command.  Otherwise, pretend it doesn't
                // exist.
                if (admin_level >= cur_command.admin_level_required_
                 && gm_level >= cur_command.gm_level_required_)
                {
                    cur_command.function_(context_, arg.second, player);
                    last_command_ = input;
                    return;
                }
            }
        }

        std::string text =
            "\nDidn't understand that.  Available commands are:\n";

        for (auto const &cur_command : command_list)
        {
            if (admin_level >= cur_command.admin_level_required_
             && gm_level >= cur_command.gm_level_required_)
            {
                text += cur_command.command_ + " ";
            }
        }

        text += "\n";

        send_to_player(context_, text, player);
    }

    client                                 &self_;
    boost::asio::strand                     strand_;
    std::shared_ptr<context>                context_;
    std::shared_ptr<account>                account_;
    std::shared_ptr<character>              character_;

    std::shared_ptr<connection>             connection_;
    std::shared_ptr<munin::window>          window_;
    std::shared_ptr<hugin::user_interface>  user_interface_;

    std::mutex                              dispatch_queue_mutex_;
    std::deque<std::function<void ()>>      dispatch_queue_;
    std::string                             last_command_;

private :
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
client::client(
    boost::asio::io_service &io_service
  , std::shared_ptr<context> ctx)
{
    pimpl_ = std::make_shared<impl>(
        std::ref(*this), std::ref(io_service), ctx);
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
void client::set_connection(std::shared_ptr<connection> const &cnx)
{
    pimpl_->set_connection(cnx);
}

// ==========================================================================
// GET_USER_INTERFACE
// ==========================================================================
std::shared_ptr<hugin::user_interface> client::get_user_interface()
{
    return pimpl_->get_user_interface();
}

// ==========================================================================
// GET_WINDOW
// ==========================================================================
std::shared_ptr<munin::window> client::get_window()
{
    return pimpl_->get_window();
}

// ==========================================================================
// SET_WINDOW_TITLE
// ==========================================================================
void client::set_window_title(std::string const &title)
{
    pimpl_->set_window_title(title);
}

// ==========================================================================
// SET_WINDOW_SIZE
// ==========================================================================
void client::set_window_size(odin::u16 width, odin::u16 height)
{
    pimpl_->set_window_size(width, height);
}

// ==========================================================================
// SET_ACCOUNT
// ==========================================================================
void client::set_account(std::shared_ptr<account> const &acc)
{
    pimpl_->set_account(acc);
}

// ==========================================================================
// GET_ACCOUNT
// ==========================================================================
std::shared_ptr<account> client::get_account() const
{
    return pimpl_->get_account();
}

// ==========================================================================
// SET_CHARACTER
// ==========================================================================
void client::set_character(std::shared_ptr<character> const &ch)
{
    pimpl_->set_character(ch);
}

// ==========================================================================
// GET_CHARACTER
// ==========================================================================
std::shared_ptr<character> client::get_character() const
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
void client::on_connection_death(std::function<void ()> const &callback)
{
    pimpl_->on_connection_death(callback);
}

}

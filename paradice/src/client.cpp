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
#include "paradice/client.hpp"
#include "paradice/account.hpp"
#include "paradice/admin.hpp"
#include "paradice/character.hpp"
#include "paradice/communication.hpp"
#include "paradice/configuration.hpp"
#include "paradice/connection.hpp"
#include "paradice/context.hpp"
//#include "paradice/gm.hpp"
#include "paradice/help.hpp"
#include "paradice/rules.hpp"
#include "paradice/tokenise.hpp"
#include "paradice/who.hpp"
#include "paradice/ui/user_interface.hpp"
#include <munin/container.hpp>
#include <munin/brush.hpp>
#include <munin/grid_layout.hpp>
#include <munin/window.hpp>
#include <terminalpp/behaviour.hpp>
#include <terminalpp/canvas.hpp>
#include <terminalpp/encoder.hpp>
#include <terminalpp/string.hpp>
#include <terminalpp/terminal.hpp>
#include <terminalpp/detail/lambda_visitor.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/format.hpp>
#include <cstdio>
#include <deque>
#include <mutex>
#include <string>
#include <vector>

using namespace terminalpp::literals;

namespace paradice {

namespace {

    constexpr terminalpp::extent default_window_size{80, 24};

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

    using paradice_command = std::function<void (
        std::shared_ptr<paradice::context> ctx,
        std::string                        args,
        std::shared_ptr<paradice::client>  player)>;

    static struct command
    {
        std::string      command_;
        paradice_command function_;
        std::uint32_t    admin_level_required_;
        std::uint32_t    gm_level_required_;
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

    //   , PARADICE_GM_ENTRY(gm, 100)

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
    static terminalpp::behaviour create_behaviour()
    {
        terminalpp::behaviour behaviour;
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
        boost::asio::io_context &io_context,
        std::shared_ptr<context> ctx)
      : self_{self},
        strand_{io_context},
        context_{ctx},
        terminal_{create_behaviour()},
        canvas_{default_window_size},
        user_interface_{std::make_shared<ui::user_interface>()},
        window_{user_interface_},
        repaint_requested_{false},
        cursor_state_changed_{true},
        cursor_position_changed_{true}
    {
    }

    // ======================================================================
    // SCHEDULE_NEXT_READ
    // ======================================================================
    void schedule_next_read()
    {
        assert(connection_);

        connection_->async_read(
            [this](serverpp::bytes data)
            {
                auto const tokens = terminal_.read(std::string(
                    reinterpret_cast<char const *>(data.data()),
                    data.size()));

                auto const &send_event_to_window = [this](auto const &token)
                {
                    boost::apply_visitor(
                        terminalpp::detail::make_lambda_visitor(
                            [this](terminalpp::virtual_key const &vk)
                            {
                                if (vk.key == terminalpp::vk::uppercase_q)
                                {
                                    context_->shutdown();
                                }
                                else
                                {
                                    this->run_on_ui_strand([this, vk]{ window_.event(vk); });
                                }
                            },
                            [this](auto &&event)
                            {
                                this->run_on_ui_strand([this, event] { window_.event(event); });
                            }),
                            token);
                };

                boost::for_each(tokens, send_event_to_window);
            },
            [this]
            {
                if (connection_->is_alive())
                {
                    schedule_next_read();
                }
                else
                {
                    on_connection_death_();
                }
            });
    }

    // ======================================================================
    // SET_CONNECTION
    // ======================================================================
    void set_connection(std::shared_ptr<connection> cnx)
    {
        connection_ = cnx;

        // CONNECTION CALLBACKS
        connection_->on_window_size_changed(
            [this](auto const &width, auto const &height)
            {
                this->on_window_size_changed(width, height);
            });

        // WINDOW CALLBACKS
        window_.on_repaint_request.connect(
            [this]()
            {
                this->on_repaint();
            });

        // USER INTERFACE CALLBACKS
        user_interface_->on_cursor_state_changed.connect(
            [this]()
            {
                cursor_state_changed_ = true;
                this->on_repaint();
            });

        user_interface_->on_cursor_position_changed.connect(
            [this]()
            {
                cursor_position_changed_ = true;
                this->on_repaint();
            });

        // user_interface_->on_input_entered.connect(
        //     [this](auto const &input)
        //     {
        //         this->on_input_entered(input);
        //     });

        // user_interface_->on_login.connect(
        //     [this](auto const &name, auto const &pwd)
        //     {
        //         this->on_login(name, pwd);
        //     });

        user_interface_->on_new_account.connect(
            [this](std::string const &name, encrypted_string const &password)
            {
                return this->on_new_account(name, password);
            });

        // user_interface_->on_account_created.connect(
        //     [this](auto const &name, auto const &pwd, auto const &pwd_verify)
        //     {
        //         this->on_account_created(name, pwd, pwd_verify);
        //     });

        // user_interface_->on_account_creation_cancelled.connect(
        //     [this]
        //     {
        //         this->on_account_creation_cancelled();
        //     });

        // user_interface_->on_new_character.connect(
        //     [this]
        //     {
        //         this->on_new_character();
        //     });

        // user_interface_->on_character_selected.connect(
        //     [this](auto const &idx)
        //     {
        //         this->on_character_selected(idx);
        //     });

        // user_interface_->on_character_created.connect(
        //     [this](auto const &name, auto const &is_gm)
        //     {
        //         this->on_character_created(name, is_gm);
        //     });

        // user_interface_->on_character_creation_cancelled.connect(
        //     [this]
        //     {
        //         this->on_character_creation_cancelled();
        //     });

        // user_interface_->on_gm_tools_back.connect(
        //     [this]
        //     {
        //         this->on_gm_tools_back();
        //     });

        // user_interface_->on_gm_fight_beast.connect(
        //     [this](auto const &beast)
        //     {
        //         this->on_gm_fight_beast(beast);
        //     });

        // user_interface_->on_gm_fight_encounter.connect(
        //     [this](auto const &encounter)
        //     {
        //         this->on_gm_fight_encounter(encounter);
        //     });

        // user_interface_->on_help_closed.connect(
        //     [this]
        //     {
        //         this->on_help_closed();
        //     });

        // user_interface_->on_password_changed.connect(
        //     [this](
        //         auto const &old_pwd,
        //         auto const &new_pwd,
        //         auto const &new_pwd_verify)
        //     {
        //         this->on_password_changed(old_pwd, new_pwd, new_pwd_verify);
        //     });

        // user_interface_->on_password_change_cancelled.connect(
        //     [this]
        //     {
        //         this->on_password_change_cancelled();
        //     });

        // set_window_title("Paradice9");

        user_interface_->set_focus();

        write_to_connection(terminal_.enable_mouse());

        // window_->use_alternate_screen_buffer();

        schedule_next_read();
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
    /*
    std::shared_ptr<hugin::user_interface> get_user_interface()
    {
        return user_interface_;
    }
    */

    // ======================================================================
    // SET_WINDOW_TITLE
    // ======================================================================
    void set_window_title(std::string const &title)
    {
        // {
        //     std::unique_lock<std::mutex> lock(dispatch_queue_mutex_);
        //     dispatch_queue_.push_back(bind(
        //         &munin::window::set_title, window_, title));
        // }

        // strand_.post(bind(&impl::dispatch_queue, shared_from_this()));
    }

    // ======================================================================
    // SET_WINDOW_SIZE
    // ======================================================================
    void set_window_size(std::uint16_t width, std::uint16_t height)
    {
        canvas_.resize({width, height});
        cursor_position_changed_ = true;
        on_repaint();
    }

    // ======================================================================
    // DISCONNECT
    // ======================================================================
    void disconnect()
    {
        connection_->close();
    }

    // ======================================================================
    // ON_CONNECTION_DEATH
    // ======================================================================
    void on_connection_death(std::function<void ()> const &callback)
    {
        on_connection_death_ = callback;
    }

private :
    // ======================================================================
    // RUN_ON_UI_STRAND
    // ======================================================================
    template <class Function>
    void run_on_ui_strand(Function &&function)
    {
        // Here we capture a weak pointer to the impl.  Since all the
        // deferred functions required that this object still exists, locking
        // the weak pointer ensures that this is the case.
        auto const exec = 
            [wp = std::weak_ptr<impl>(shared_from_this()),
             function = std::forward<Function>(function)]
            {
                auto const pthis = wp.lock();

                if (pthis)
                {
                    function();
                }
            };

        strand_.post(exec);
    }

    // ======================================================================
    // WRITE_TO_CONNECTION
    // ======================================================================
    void write_to_connection(std::string const &text)
    {
        serverpp::bytes bytes(
            reinterpret_cast<serverpp::byte const *>(text.data()),
            text.size());

        connection_->write(bytes);
    }

    // ======================================================================
    // ON_WINDOW_SIZE_CHANGED
    // ======================================================================
    void on_window_size_changed(std::uint16_t width, std::uint16_t height)
    {
        set_window_size(width, height);
    }

    // ======================================================================
    // ON_REPAINT
    // ======================================================================
    void on_repaint()
    {
        // Set up a repaint event only if another repaint hasn't already been
        // requested.
        if (!repaint_requested_.exchange(true))
        {
            run_on_ui_strand([this]{ do_repaint(); });
        }
    }

    // ======================================================================
    // DO_REPAINT
    // ======================================================================
    void do_repaint()
    {
        repaint_requested_ = false;

        auto repaint_string = window_.repaint(canvas_, terminal_);
        auto const cursor_state = user_interface_->get_cursor_state();
            
        if (cursor_state_changed_.exchange(false))
        {
            repaint_string += cursor_state
              ? terminal_.show_cursor()
              : terminal_.hide_cursor();
        }

        if (cursor_state)
        {
            repaint_string += terminal_.move_cursor(
                user_interface_->get_cursor_position());
        }

        write_to_connection(repaint_string);
    }

    // ======================================================================
    // REMOVE_DUPLICATE_ACCOUNTS
    // ======================================================================
    void remove_duplicate_accounts(std::shared_ptr<account> acc)
    {
        /*
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
        */
    }

    // ======================================================================
    // UPDATE_CHARACTER_NAMES
    // ======================================================================
    void update_character_names()
    {
        /*
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

        //user_interface_->set_character_names(characters);
        */
    }

    // ======================================================================
    // ON_LOGIN
    // ======================================================================
    void on_login(
        std::string const &username,
        std::string const &password)
    {
        /*
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
            // user_interface_->set_statusbar_text(
            //     "\\[1Invalid username/password combination"_ets);
            return;
        }

        if (account == NULL)
        {
            // user_interface_->set_statusbar_text(
            //     "\\[1Invalid username/password combination"_ets);
            return;
        }

        if (!account->password_match(password))
        {
            // user_interface_->set_statusbar_text(
            //     "\\[1Invalid username/password combination"_ets);
            return;
        }

        // First, ensure that if this account is logged in already,
        // it is booted.
        remove_duplicate_accounts(account);

        account_ = account;
        update_character_names();

        // user_interface_->select_face(hugin::FACE_CHAR_SELECTION);
        // user_interface_->set_focus();
        */
    }

    // ======================================================================
    // ON_NEW_ACCOUNT
    // ======================================================================
    model::account on_new_account(
        std::string const &name, 
        encrypted_string const &password)
    {
        return context_->new_account(name, password);
    }

    // ======================================================================
    // ON_ACCOUNT_CREATED
    // ======================================================================
    void on_account_created(
        std::string        account_name,
        std::string const &password,
        std::string const &password_verify)
    {
        /*
        using namespace terminalpp::literals;

        // Check that the account name is valid.  If not, report an
        // error message and return.  This also removes cases where the
        // account name could have filesystem characters in it such as
        // * or ../, which could potentially wreck the system.
        if (!is_acceptible_name(account_name))
        {
            // user_interface_->set_statusbar_text(
            //     "\\[1Name must be alphabetic only and at least three "
            //     "characters long"_ets);
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

            // user_interface_->set_statusbar_text(
            //     "\\1[That account name is unavailable.  Please try with "
            //     "a different name"_ets);
            return;
        }

        if (test_account != NULL)
        {
            // user_interface_->set_statusbar_text(
            //     "\\[1An account with that name already exists"_ets);
            return;
        }

        // Check to see if the passwords match.  If not, report an error
        // message and return.
        if (password != password_verify)
        {
            // user_interface_->set_statusbar_text(
            //     "\\[1Passwords do not match"_ets);
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

            // user_interface_->set_statusbar_text(
            //     "\\[1Unexpected error setting saving your account.  "
            //     "Please try again."_ets);
            return;
        }

        account_ = acc;

        // user_interface_->select_face(hugin::FACE_CHAR_SELECTION);
        */
    }

    // ======================================================================
    // ON_ACCOUNT_CREATION_CANCELLED
    // ======================================================================
    void on_account_creation_cancelled()
    {
        // user_interface_->clear_intro_screen();
        // user_interface_->select_face(hugin::FACE_INTRO);
        // user_interface_->set_focus();
    }

    // ======================================================================
    // ON_NEW_CHARACTER
    // ======================================================================
    void on_new_character()
    {
        // user_interface_->select_face(hugin::FACE_CHAR_CREATION);
        // user_interface_->clear_character_creation_screen();
        // user_interface_->set_focus();
    }

    // ======================================================================
    // ON_CHARACTER_SELECTED
    // ======================================================================
    void on_character_selected(std::string const &character_name)
    {
        /*
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

            // user_interface_->set_statusbar_text(
            //     "\\[1Error loading character file."_ets);

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
                        // user_interface_->set_statusbar_text(
                        //     "\\[1There is already a GM character online."_ets);
                        return;
                    }
                }
            }
        }

        // If this is a GM character, then be sure to set the beasts and
        // encounters in the UI.
        if (ch->get_gm_level() != 0)
        {
            // user_interface_->set_beasts(ch->get_beasts());
            // user_interface_->set_encounters(ch->get_encounters());
        }

        character_ = ch;
        context_->update_names();

        // user_interface_->select_face(hugin::FACE_MAIN);
        set_window_title(character_->get_name() + " - Paradice9");

        send_to_all(
            context_
          , "#SERVER: "
          + context_->get_moniker(character_)
          + " has entered Paradice.\n");
        */
    }

    // ======================================================================
    // ON_CHARACTER_CREATED
    // ======================================================================
    void on_character_created(std::string character_name, bool is_gm)
    {
        /*
        using namespace terminalpp::literals;

        capitalise(character_name);

        // Check that the name is appropriate.
        if (!is_acceptible_name(character_name))
        {
            // user_interface_->set_statusbar_text(
            //     "\\[1Name must be alphabetic only and at least three "
            //     "characters long"_ets);
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

            // user_interface_->set_statusbar_text(
            //     "\\[1Error testing for that character."_ets);
            return;
        }

        if (test_character != NULL)
        {
            // user_interface_->set_statusbar_text(
            //     "\\[1A character with that name already exists"_ets);
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

            // user_interface_->set_statusbar_text(
            //     "\\[1There was an error saving the character."_ets);
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

            // user_interface_->set_statusbar_text(
            //     "\\[1Unexpected error saving your account.  "
            //     "Please try again."_ets);

            account_->remove_character(character_name);
            return;
        }

        update_character_names();
        // user_interface_->select_face(hugin::FACE_CHAR_SELECTION);
        */
    }

    // ======================================================================
    // ON_CHARACTER_CREATION_CANCELLED
    // ======================================================================
    void on_character_creation_cancelled()
    {
        // user_interface_->select_face(hugin::FACE_CHAR_SELECTION);
        // user_interface_->set_focus();
    }

    // ======================================================================
    // ON_GM_TOOLS_BACK
    // ======================================================================
    void on_gm_tools_back()
    {
        // character_->set_beasts(user_interface_->get_beasts());
        // character_->set_encounters(user_interface_->get_encounters());
        // context_->save_character(character_);

        // user_interface_->select_face(hugin::FACE_MAIN);
        // user_interface_->set_focus();
    }

    // ======================================================================
    // ON_GM_FIGHT_BEAST
    // ======================================================================
    void on_gm_fight_beast(std::shared_ptr<paradice::beast> beast)
    {
        // add_beast(context_->get_active_encounter(), beast);
        // context_->update_active_encounter();

        // user_interface_->set_statusbar_text(terminalpp::encode(
        //     boost::str(boost::format("\\[3Added \\x%s\\x\\[3 to active encounter")
        //         % beast->get_name())));
    }

    // ======================================================================
    // ON_GM_FIGHT_ENCOUNTER
    // ======================================================================
    void on_gm_fight_encounter(std::shared_ptr<paradice::encounter> encounter)
    {
        // auto enc = context_->get_active_encounter();

        // for (auto beast : encounter->get_beasts())
        // {
        //     add_beast(enc, beast);
        // }

        // context_->update_active_encounter();

        // user_interface_->set_statusbar_text(terminalpp::encode(
        //     boost::str(boost::format("\\[3Added \\x%s\\x\\[3 to active encounter!")
        //         % encounter->get_name())));
    }

    // ======================================================================
    // ON_HELP_CLOSED
    // ======================================================================
    void on_help_closed()
    {
        // user_interface_->hide_help_window();
    }

    // ======================================================================
    // ON_PASSWORD_CHANGED
    // ======================================================================
    void on_password_changed(
        std::string const &old_password,
        std::string const &new_password,
        std::string const &new_password_verify)
    {
        /*
        using namespace terminalpp::literals;

        if (!account_->password_match(old_password))
        {
            // user_interface_->set_statusbar_text(
            //     "\\[1Old password did not match."_ets);
            return;
        }

        if (new_password != new_password_verify)
        {
            // user_interface_->set_statusbar_text(
            //     "\\[1New passwords did not match."_ets);
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

            // user_interface_->set_statusbar_text(
            //     "\\[1Unexpected error saving your account.  "
            //     "Please try again."_ets);

            return;
        }

        // user_interface_->select_face(hugin::FACE_MAIN);
        */
    }

    // ======================================================================
    // ON_PASSWORD_CHANGE_CANCELLED
    // ======================================================================
    void on_password_change_cancelled()
    {
        // user_interface_->select_face(hugin::FACE_MAIN);
    }

    // ======================================================================
    // ON_INPUT_ENTERED
    // ======================================================================
    void on_input_entered(std::string const &input)
    {
        /*
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
        */
    }

    // ======================================================================
    // ON_COMMAND
    // ======================================================================
    void on_command(std::string const &input)
    {
        /*
        std::shared_ptr<client> player = self_.shared_from_this();
        assert(player != NULL);

        // user_interface_->add_command_history(input);

        auto arg = paradice::tokenise(input);

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
        */
    }

    client                                 &self_;
    boost::asio::io_context::strand         strand_;

    std::shared_ptr<context>                context_;
    std::shared_ptr<account>                account_;
    std::shared_ptr<character>              character_;

    std::shared_ptr<connection>             connection_;

    terminalpp::canvas                      canvas_;
    terminalpp::terminal                    terminal_;

    std::shared_ptr<ui::user_interface>     user_interface_;
    munin::window                           window_;

    std::function<void ()>                  on_connection_death_;

    std::string                             last_command_;
    std::atomic_bool                        repaint_requested_;
    std::atomic_bool                        cursor_state_changed_;
    std::atomic_bool                        cursor_position_changed_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client(
    boost::asio::io_context &io_context, std::shared_ptr<context> ctx)
  : pimpl_(std::make_shared<impl>(*this, io_context, std::move(ctx)))
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
client::~client() = default;

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
// std::shared_ptr<hugin::user_interface> client::get_user_interface()
// {
//     return pimpl_->get_user_interface();
// }

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
void client::set_window_size(std::uint16_t width, std::uint16_t height)
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
    // pimpl_->get_window()->use_normal_screen_buffer();
    // pimpl_->get_window()->disable_mouse_tracking();
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

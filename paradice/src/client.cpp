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

#include "paradice/admin_commands.hpp"
#include "paradice/command_catalog.hpp"
#include "paradice/connection.hpp"
#include "paradice/context.hpp"
#include "paradice/dice_roll_parser.hpp"
#include "paradice/permissions.hpp"
#include "paradice/room_lifecycle.hpp"
#include "paradice/ui/message.hpp"
#include "paradice/ui/shell/user_interface.hpp"

#include <munin/background_animator.hpp>
#include <munin/brush.hpp>
#include <munin/container.hpp>
#include <munin/grid_layout.hpp>
#include <munin/window.hpp>
#include <terminalpp/behaviour.hpp>
#include <terminalpp/canvas.hpp>
#include <terminalpp/string.hpp>
#include <terminalpp/terminal.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/asio/post.hpp>
#include <boost/range/algorithm/for_each.hpp>

#include <array>
#include <format>
#include <random>
#include <string>
#include <string_view>
#include <cstdio>

using namespace terminalpp::literals;  // NOLINT

namespace paradice {

namespace {

constexpr terminalpp::extent default_window_size{80, 24};
constexpr auto roll_usage_message =
    "\n Usage:   roll [n*]<dice>d<sides>[<bonuses...>] [<category>]"
    "\n Example: roll 2d6+3-20"
    "\n Example: roll 20*2d6"
    "\n Example: roll 1d10+4 initiative"
    "\n";
constexpr auto admin_usage_message =
    "USAGE: /admin shutdown|list_accounts|list_characters <account>|"
    "set_password <account> <password>|"
    "set_permission <account> <permission>|"
    "clear_permission <account> <permission>";

}  // namespace

// ==========================================================================
// CLIENT IMPLEMENTATION STRUCTURE
// ==========================================================================
class client::impl : public std::enable_shared_from_this<client::impl>
{
public:
    impl(impl const &) = delete;
    impl(impl &&) = delete;
    impl &operator=(impl const &) = delete;
    impl &operator=(impl &&) = delete;

    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(
        client &self,
        boost::asio::io_context &io_context,
        context &ctx,
        connection &&cnx,
        terminalpp::behaviour beh,
        std::function<std::int32_t(std::uint32_t)> roller)
      : self_{self},
        strand_{io_context},
        context_{ctx},
        canvas_{default_window_size},
        connection_{std::move(cnx)},
        roller_{std::move(roller)},
        terminal_{connection_, beh},
        animator_(strand_),
        user_interface_{std::make_shared<ui::user_interface>(animator_)},
        window_{terminal_, user_interface_},
        repaint_requested_{false},
        cursor_state_changed_{true}
    {
        // CONNECTION CALLBACKS
        connection_.on_window_size_changed(
            [this](auto const &width, auto const &height) {
                this->on_window_size_changed(width, height);
            });

        // WINDOW CALLBACKS
        window_.on_repaint_request.connect([this]() { this->on_repaint(); });

        // USER INTERFACE CALLBACKS
        user_interface_->on_cursor_state_changed.connect([this]() {
            cursor_state_changed_ = true;
            this->on_repaint();
        });

        user_interface_->on_cursor_position_changed.connect(
            [this]() { this->on_repaint(); });

        user_interface_->on_command.connect(
            [this](auto const &input) { this->on_command(input); });

        user_interface_->on_login.connect(
            [this](auto const &name, auto const &pwd) {
                return this->on_login(name, pwd);
            });

        user_interface_->on_new_account.connect(
            [this](std::string const &name, std::string const &password) {
                return this->on_new_account(name, password);
            });

        user_interface_->on_character_selected.connect(
            [this](model::account &acct, int index) {
                return this->on_character_selected(acct, index);
            });

        user_interface_->on_character_created.connect(
            [this](model::account &acct, std::string const &character_name) {
                return this->on_character_created(acct, character_name);
            });

        user_interface_->on_entered_game.connect([this](model::character &chr) {
            chr.send_message = [this](terminalpp::string const &message) {
                self_.send_message(message);
            };
            character_ = chr;
            context_.register_online_character(*character_);

            paradice::model::room &main_room = context_.get_main_room();
            character_->in_room = &main_room;
            main_room.characters.push_back(&chr);
            update_roster_from_current_room();

            context_.send_message(chr, "You have entered Paradice!");
            context_.send_message(
                main_room, chr, chr.name + " has entered Paradice!");
        });

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

        user_interface_->set_focus();

        terminal_ << terminalpp::set_window_title("Paradice9")
                  << terminalpp::enable_mouse()
                  << terminalpp::use_alternate_screen_buffer();

        schedule_next_read();
    }

    ~impl()
    {
        disconnect_character_if_present();
    }

    // ======================================================================
    // WRITE
    // ======================================================================
    void write(terminalpp::bytes data)
    {
        do
        {
            auto const amount =
                std::min(buffer_.size() - buffer_top_, data.size());

            std::copy_n(data.begin(), amount, buffer_.begin() + buffer_top_);
            buffer_top_ += amount;
            data = data.subspan(amount);

            if (buffer_top_ == buffer_.size())
            {
                flush_immediately();
            }
        } while (!data.empty());

        flush();
    }

    // ======================================================================
    // FLUSH_IMMEDIATELY
    // ======================================================================
    void flush_immediately()
    {
        boost::asio::dispatch(strand_, [this]() {
            flush_requested_ = false;
            terminal_.write({buffer_.begin(), buffer_top_});
            buffer_top_ = 0;
        });
    }

    // ======================================================================
    // FLUSH
    // ======================================================================
    void flush()
    {
        if (!std::atomic_exchange(&flush_requested_, true))
        {
            boost::asio::post(strand_, [this]() { flush_immediately(); });
        }
    }

    // ======================================================================
    // ON_TOKENS_READ
    // ======================================================================
    void on_tokens_read(terminalpp::tokens const &tokens)
    {
        for (auto const &token : tokens)
        {
            std::visit(
                [this](auto const &event) {
                    this->run_on_ui_strand(
                        [this, event] { window_.event(event); });
                },
                token);
        }
    }

    // ======================================================================
    // SCHEDULE_NEXT_READ
    // ======================================================================
    void schedule_next_read()
    {
        terminal_.async_read([this](terminalpp::tokens data) {
            on_tokens_read(data);

            if (terminal_.is_alive())
            {
                schedule_next_read();
            }
            else
            {
                disconnect_character_if_present();
                on_connection_death_();
            }
        });
    }

    // ======================================================================
    // SET_WINDOW_TITLE
    // ======================================================================
    void set_window_title(std::string const &title)
    {
        terminal_ << terminalpp::set_window_title(title);
    }

    // ======================================================================
    // SET_WINDOW_SIZE
    // ======================================================================
    void set_window_size(std::uint16_t width, std::uint16_t height)
    {
        canvas_.resize({width, height});
        on_repaint();
    }

    // ======================================================================
    // DISCONNECT
    // ======================================================================
    void disconnect()
    {
        disconnect_character_if_present();
        terminal_ << terminalpp::disable_mouse() << terminalpp::show_cursor()
                  << terminalpp::use_normal_screen_buffer();

        connection_.close();
    }

    // ======================================================================
    // ON_CONNECTION_DEATH
    // ======================================================================
    void on_connection_death(std::function<void()> const &callback)
    {
        on_connection_death_ = callback;
    }

    // ======================================================================
    // SEND_MESSAGE
    // ======================================================================
    void send_message(terminalpp::string const &message)
    {
        run_on_ui_strand([this, message] {
            update_roster_from_current_room();
            user_interface_->event(ui::message{message});
        });
    }

private:
    [[nodiscard]] static auto as_titled_list(
        std::string const &title, std::vector<std::string> const &items)
    {
        auto message = title + ":";

        for (auto const &item : items)
        {
            message += std::format("\n{}", item);
        }

        return message;
    }

    [[nodiscard]] static auto split_two_arguments(std::string const &arguments)
        -> std::optional<std::pair<std::string, std::string>>
    {
        auto const split = arguments.find(' ');

        if (split == std::string::npos)
        {
            return std::nullopt;
        }

        return std::pair{
            arguments.substr(0, split), arguments.substr(split + 1)};
    }

    [[nodiscard]] auto granted_admin_permissions() const
        -> std::vector<std::string_view>
    {
        constexpr auto optional_admin_permissions = std::array{
            permissions::admin_shutdown,
            permissions::admin_set_password,
            permissions::admin_set_permission};

        auto granted_permissions = std::vector<std::string_view>{};

        for (auto const permission : optional_admin_permissions)
        {
            if (context_.has_permission(*active_account_, std::string{permission}))
            {
                granted_permissions.push_back(permission);
            }
        }

        return granted_permissions;
    }

    [[nodiscard]] auto has_active_account_permission(
        std::string_view permission) const -> bool
    {
        return active_account_
               && context_.has_permission(*active_account_, std::string{permission});
    }

    [[nodiscard]] auto admin_help_message() const
    {
        return as_titled_list(
            "Admin commands",
            visible_admin_commands(granted_admin_permissions()));
    }

    [[nodiscard]] auto help_message() const
    {
        return as_titled_list(
            "Commands",
            visible_top_level_commands(
                has_active_account_permission(permissions::admin_access)));
    }

    void emit_tell_messages(
        model::character &recipient, std::string const &message)
    {
        context_.send_message(
            *character_,
            std::format("you tell {}, \"{}\"", recipient.name, message));
        context_.send_message(
            recipient,
            std::format("{} tells you, \"{}\"", character_->name, message));
    }

    void emit_tell_target_not_found(std::string const &recipient_name)
    {
        context_.send_message(
            *character_,
            std::format(
                "A player with name {} could not be found.", recipient_name));
    }

    bool try_handle_tell_command(std::string const &input)
    {
        if (!input.starts_with("/tell "))
        {
            return false;
        }

        auto const tell_arguments = input.substr(6);
        auto const split = tell_arguments.find(' ');

        if (split == std::string::npos)
        {
            return true;
        }

        auto const recipient_name = tell_arguments.substr(0, split);
        auto const message = tell_arguments.substr(split + 1);
        auto *recipient =
            context_.find_online_character_by_name(recipient_name);

        if (recipient != nullptr)
        {
            emit_tell_messages(*recipient, message);
        }
        else
        {
            emit_tell_target_not_found(recipient_name);
        }

        return true;
    }

    void emit_say_messages(std::string const &spoken_text)
    {
        context_.send_message(
            *character_, std::format("you say, \"{}\"", spoken_text));
        context_.send_message(
            context_.get_main_room(),
            *character_,
            std::format("{} says, \"{}\"", character_->name, spoken_text));
    }

    bool try_handle_roll_command(std::string const &input)
    {
        auto const shared_roll_prefix = std::string{"/roll "};
        auto const private_roll_prefix = std::string{"/rollprivate "};
        auto const is_shared_roll = input.starts_with(shared_roll_prefix);
        auto const is_private_roll = input.starts_with(private_roll_prefix);

        if (!is_shared_roll && !is_private_roll)
        {
            return false;
        }

        auto const roll_text = is_shared_roll
                                 ? input.substr(shared_roll_prefix.size())
                                 : input.substr(private_roll_prefix.size());
        auto begin = roll_text.cbegin();
        auto const end = roll_text.cend();

        auto const parsed_roll = parse_dice_roll(begin, end);

        if (!parsed_roll)
        {
            context_.send_message(*character_, roll_usage_message);
            return true;
        }

        if (parsed_roll->sides_ == 0)
        {
            context_.send_message(
                *character_,
                "You fumble your roll and spill all your zero-sided dice on "
                "the floor.\n");

            if (is_shared_roll)
            {
                context_.send_message(
                    context_.get_main_room(),
                    *character_,
                    std::format(
                        "{} fumbles their roll and spills a pile of zero-sided "
                        "dice on the floor.\n",
                        character_->name));
            }

            return true;
        }

        auto const faces = roll_faces(
            *parsed_roll,
            [this](std::uint32_t sides) { return roll_die(sides); });
        auto const result_text = describe_roll_result(*parsed_roll, faces);

        context_.send_message(
            *character_,
            std::format("you roll {} and score {}", roll_text, result_text));

        if (is_shared_roll)
        {
            context_.send_message(
                context_.get_main_room(),
                *character_,
                std::format(
                    "{} rolls {} and scores {}",
                    character_->name,
                    roll_text,
                    result_text));
        }
        return true;
    }

    bool try_handle_admin_command(std::string const &input)
    {
        if (!input.starts_with("/admin") || !active_account_
            || !has_active_account_permission(permissions::admin_access))
        {
            return false;
        }

        if (paradice::try_handle_admin_command(
                context_, *active_account_, *character_, input))
        {
            return true;
        }

        if (input == "/admin shutdown")
        {
            if (!has_active_account_permission(permissions::admin_shutdown))
            {
                context_.send_message(
                    *character_,
                    "You do not have permission to use /admin shutdown");
                return true;
            }

            context_.shutdown();
            return true;
        }

        if (input.starts_with("/admin list_characters"))
        {
            auto account_name =
                input.substr(std::string{"/admin list_characters"}.size());

            if (!account_name.empty() && account_name.front() == ' ')
            {
                account_name.erase(0, 1);
            }

            context_.send_message(
                *character_,
                as_titled_list(
                    "Characters", context_.list_characters(account_name)));
            return true;
        }

        if (input.starts_with("/admin set_password "))
        {
            if (!has_active_account_permission(permissions::admin_set_password))
            {
                context_.send_message(
                    *character_,
                    "You do not have permission to use /admin set_password");
                return true;
            }

            auto arguments =
                input.substr(std::string{"/admin set_password "}.size());
            auto const split = split_two_arguments(arguments);

            if (!split)
            {
                context_.send_message(*character_, admin_usage_message);
                return true;
            }

            auto const &[account_name, password] = *split;

            context_.set_password(account_name, password);
            context_.send_message(*character_, "Password changed.");
            return true;
        }

        if (input.starts_with("/admin set_permission "))
        {
            if (!has_active_account_permission(
                    permissions::admin_set_permission))
            {
                context_.send_message(
                    *character_,
                    "You do not have permission to use /admin set_permission");
                return true;
            }

            auto arguments =
                input.substr(std::string{"/admin set_permission "}.size());
            auto const split = split_two_arguments(arguments);

            if (!split)
            {
                context_.send_message(*character_, admin_usage_message);
                return true;
            }

            auto const &[account_name, permission] = *split;

            context_.set_permission(account_name, permission);
            context_.send_message(*character_, "Permission granted.");
            return true;
        }

        if (input.starts_with("/admin clear_permission "))
        {
            if (!has_active_account_permission(
                    permissions::admin_set_permission))
            {
                context_.send_message(
                    *character_,
                    "You do not have permission to use /admin "
                    "clear_permission");
                return true;
            }

            auto arguments =
                input.substr(std::string{"/admin clear_permission "}.size());
            auto const split = split_two_arguments(arguments);

            if (!split)
            {
                context_.send_message(*character_, admin_usage_message);
                return true;
            }

            auto const &[account_name, permission] = *split;
            auto const target_account = model::account{.name = account_name};

            if (context_.has_permission(
                    target_account,
                    std::string{permissions::admin_set_permission}))
            {
                context_.send_message(
                    *character_,
                    "You cannot clear permissions from accounts with /admin "
                    "set_permission.");
                return true;
            }

            context_.clear_permission(account_name, permission);
            context_.send_message(*character_, "Permission cleared.");
            return true;
        }

        context_.send_message(*character_, admin_usage_message);
        return true;
    }

    bool try_handle_help_command(std::string const &input)
    {
        if (input == "/help admin")
        {
            if (!has_active_account_permission(permissions::admin_access))
            {
                return false;
            }

            context_.send_message(*character_, admin_help_message());
            return true;
        }

        if (input != "/help")
        {
            return false;
        }

        context_.send_message(*character_, help_message());
        return true;
    }

    std::int32_t roll_die(std::uint32_t sides)
    {
        if (roller_)
        {
            return roller_(sides);
        }

        auto random_source = std::random_device{};
        auto generator = std::mt19937{random_source()};
        auto distribution =
            std::uniform_int_distribution<std::int32_t>(1, sides);
        return distribution(generator);
    }

    void update_roster_from_current_room()
    {
        if (!character_ || character_->in_room == nullptr)
        {
            return;
        }

        std::vector<terminalpp::string> names;

        for (auto const *occupant : character_->in_room->characters)
        {
            names.emplace_back(occupant->name);
        }

        user_interface_->set_player_characters(std::move(names));
    }

    void disconnect_character_if_present()
    {
        if (character_)
        {
            context_.unregister_online_character(*character_);
            paradice::disconnect_character(context_, *character_);
        }
    }

    // ======================================================================
    // RUN_ON_UI_STRAND
    // ======================================================================
    template <class Function>
    void run_on_ui_strand(Function &&function)
    {
        // Here we capture a weak pointer to the impl.  Since all the
        // deferred functions required that this object still exists, locking
        // the weak pointer ensures that this is the case.
        auto const exec = [wp = std::weak_ptr<impl>(shared_from_this()),
                           function = std::forward<Function>(function)] {
            auto const pthis = wp.lock();

            if (pthis)
            {
                function();
            }
        };

        boost::asio::post(strand_, exec);
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
            run_on_ui_strand([this] { do_repaint(); });
        }
    }

    // ======================================================================
    // DO_REPAINT
    // ======================================================================
    void do_repaint()
    {
        repaint_requested_ = false;
        window_.repaint(canvas_);

        auto const cursor_state = user_interface_->get_cursor_state();

        if (cursor_state_changed_.exchange(false))
        {
            if (cursor_state)
            {
                terminal_ << terminalpp::show_cursor();
            }
            else
            {
                terminal_ << terminalpp::hide_cursor();
            }
        }

        if (cursor_state)
        {
            terminal_ << terminalpp::move_cursor(
                user_interface_->get_cursor_position());
        }
    }

    // ======================================================================
    // ON_LOGIN
    // ======================================================================
    model::account on_login(
        std::string const &username, std::string const &password)
    {
        active_account_ = context_.load_account(username, password);
        return *active_account_;
    }

    // ======================================================================
    // ON_NEW_ACCOUNT
    // ======================================================================
    model::account on_new_account(
        std::string const &name, std::string const &password)
    {
        active_account_ = context_.new_account(name, password);
        return *active_account_;
    }

    // ======================================================================
    // ON_CHARACTER_SELECTED
    // ======================================================================
    model::character on_character_selected(model::account &acct, int index)
    {
        return context_.load_character(acct, index);
    }

    // ======================================================================
    // ON_CHARACTER_CREATED
    // ======================================================================
    model::character on_character_created(
        model::account &acct, std::string const &character_name)
    {
        return context_.new_character(acct, character_name);
    }

    // ======================================================================
    // ON_COMMAND
    // ======================================================================
    void on_command(std::string const &input)
    {
        if (try_handle_admin_command(input))
        {
            return;
        }

        if (try_handle_help_command(input))
        {
            return;
        }

        if (try_handle_tell_command(input))
        {
            return;
        }

        if (try_handle_roll_command(input))
        {
            return;
        }

        if (input.starts_with("/say "))
        {
            emit_say_messages(input.substr(5));
            return;
        }

        if (input.starts_with('/'))
        {
            context_.send_message(
                *character_, std::format("Unknown command: {}", input));
            return;
        }

        emit_say_messages(input);
    }

    client &self_;
    boost::asio::io_context::strand strand_;

    context &context_;
    connection connection_;
    std::function<std::int32_t(std::uint32_t)> roller_;
    boost::optional<model::account> active_account_;

    std::array<terminalpp::byte, 4096> buffer_;
    std::array<terminalpp::byte, 4096>::size_type buffer_top_{0};
    std::atomic<bool> flush_requested_{false};

    boost::optional<model::character &> character_;

    terminalpp::canvas canvas_;
    terminalpp::terminal terminal_;

    munin::background_animator animator_;
    std::shared_ptr<ui::user_interface> user_interface_;
    munin::window window_;

    std::function<void()> on_connection_death_;

    std::string last_command_;
    std::atomic_bool repaint_requested_;
    std::atomic_bool cursor_state_changed_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client(
    boost::asio::io_context &io_context,
    context &ctx,
    connection &&cnx,
    terminalpp::behaviour beh,
    std::function<std::int32_t(std::uint32_t)> roller)
  : pimpl_(
        std::make_shared<impl>(
            *this, io_context, ctx, std::move(cnx), beh, std::move(roller)))
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
client::~client() = default;

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
// DISCONNECT
// ==========================================================================
void client::disconnect()
{
    pimpl_->disconnect();
}

// ==========================================================================
// ON_CONNECTION_DEATH
// ==========================================================================
void client::on_connection_death(std::function<void()> const &callback)
{
    pimpl_->on_connection_death(callback);
}

// ==========================================================================
// SEND_MESSAGE
// ==========================================================================
void client::send_message(terminalpp::string const &message)
{
    pimpl_->send_message(message);
}

}  // namespace paradice

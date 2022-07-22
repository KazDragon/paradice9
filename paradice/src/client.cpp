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
#include "paradice/connection.hpp"
#include "paradice/context.hpp"
#include "paradice/ui/message.hpp"
#include "paradice/ui/user_interface.hpp"
#include <munin/container.hpp>
#include <munin/background_animator.hpp>
#include <munin/brush.hpp>
#include <munin/grid_layout.hpp>
#include <munin/window.hpp>
#include <terminalpp/behaviour.hpp>
#include <terminalpp/canvas.hpp>
#include <terminalpp/encoder.hpp>
#include <terminalpp/string.hpp>
#include <terminalpp/terminal.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <fmt/format.h>
#include <cstdio>
#include <array>
#include <deque>
#include <mutex>
#include <string>
#include <vector>

using namespace terminalpp::literals;

namespace paradice {

namespace {

constexpr terminalpp::extent default_window_size{80, 24};

template <typename... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;

}

// ==========================================================================
// CLIENT IMPLEMENTATION STRUCTURE
// ==========================================================================
class client::impl
    : public std::enable_shared_from_this<client::impl>
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(
        client &self,
        boost::asio::io_context &io_context,
        context &ctx,
        connection &&cnx,
        terminalpp::behaviour beh)
      : self_{self},
        strand_{io_context},
        context_{ctx},
        canvas_{default_window_size},
        connection_{std::move(cnx)},
        terminal_{connection_, beh},
        animator_(strand_),
        user_interface_{std::make_shared<ui::user_interface>(animator_)},
        window_{terminal_, user_interface_},
        repaint_requested_{false},
        cursor_state_changed_{true}
    {
        // CONNECTION CALLBACKS
        connection_.on_window_size_changed(
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
                this->on_repaint();
            });

        user_interface_->on_command.connect(
            [this](auto const &input)
            {
                this->on_command(input);
            });

        user_interface_->on_login.connect(
            [this](auto const &name, auto const &pwd)
            {
                return this->on_login(name, pwd);
            });

        user_interface_->on_new_account.connect(
            [this](std::string const &name, std::string const &password)
            {
                return this->on_new_account(name, password);
            });

        user_interface_->on_character_selected.connect(
            [this](model::account &acct, int index)
            {
                return this->on_character_selected(acct, index);
            });

        user_interface_->on_character_created.connect(
            [this](model::account &acct, std::string character_name)
            {
                return this->on_character_created(acct, character_name);
            });

        user_interface_->on_entered_game.connect(
            [this](model::character &chr)
            {
                chr.send_message = [this](terminalpp::string const &message) {
                    self_.send_message(message);
                };
                character_ = chr;

                paradice::model::room &main_room = context_.get_main_room();
                character_->in_room = &main_room;
                main_room.characters_.push_back(&chr);

                context_.send_message(chr, "You have entered Paradice!");
                context_.send_message(main_room, chr, chr.name + " has entered Paradice!");
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

        terminal_
            << terminalpp::set_window_title("Paradice9")
            << terminalpp::enable_mouse()
            << terminalpp::use_alternate_screen_buffer();

        schedule_next_read();
    }

    ~impl()
    {
        if (character_ && character_->in_room)
        {
            context_.send_message(
                *character_->in_room,
                *character_, 
                character_->name + " has left Paradice");
        }
    }

    // ======================================================================
    // WRITE
    // ======================================================================
    void write(terminalpp::bytes data)
    {
        do
        {
            auto const amount = 
                std::min(buffer_.size() - buffer_top_,
                data.size());

            std::copy_n(data.begin(), amount, buffer_.begin() + buffer_top_);
            buffer_top_ += amount;
            data = data.subspan(amount);

            if (buffer_top_ == data.size())
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
        strand_.dispatch(
            [this]()
            {
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
            strand_.post(
                [this]()
                {
                    flush_immediately();
                });
        }
    }

    // ======================================================================
    // ON_TOKENS_READ
    // ======================================================================
    void on_tokens_read(terminalpp::tokens const &tokens)
    {
        const auto &apply_token = 
            [this](terminalpp::token const &token)
            {
                std::visit(
                    overloaded{
                        [this](terminalpp::virtual_key const &vk)
                        {
                            if (vk.key == terminalpp::vk::uppercase_q)
                            {
                                context_.shutdown();
                            }
                            else
                            {
                                this->run_on_ui_strand([this, vk]{ window_.event(vk); });
                            }
                        },
                        [this](terminalpp::mouse::event const &ev)
                        {
                            this->run_on_ui_strand([this, ev] { window_.event(ev); });
                        },
                        [this](terminalpp::control_sequence const &cs)
                        {
                            this->run_on_ui_strand([this, cs] { window_.event(cs); });
                        }
                    },
                    token);
            };
    
        boost::for_each(tokens, apply_token);
    }

    // ======================================================================
    // SCHEDULE_NEXT_READ
    // ======================================================================
    void schedule_next_read()
    {
        terminal_.async_read(
            [this](terminalpp::tokens data)
            {
                on_tokens_read(data);

                if (terminal_.is_alive())
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
        terminal_
            << terminalpp::disable_mouse()
            << terminalpp::show_cursor()
            << terminalpp::use_normal_screen_buffer();

        connection_.close();
    }

    // ======================================================================
    // ON_CONNECTION_DEATH
    // ======================================================================
    void on_connection_death(std::function<void ()> const &callback)
    {
        on_connection_death_ = callback;
    }

    // ======================================================================
    // SEND_MESSAGE
    // ======================================================================
    void send_message(terminalpp::string const &message)
    {
        run_on_ui_strand([this, message]{ 
            user_interface_->event(ui::message{message});
        });
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
            terminal_ 
                 << terminalpp::move_cursor(user_interface_->get_cursor_position());
        }
    }

    // ======================================================================
    // ON_LOGIN
    // ======================================================================
    model::account on_login(
        std::string const &username,
        std::string const &password)
    {
        return context_.load_account(username, password);
    }

    // ======================================================================
    // ON_NEW_ACCOUNT
    // ======================================================================
    model::account on_new_account(
        std::string const &name,
        std::string const &password)
    {
        return context_.new_account(name, password);
    }

    // ======================================================================
    // ON_CHARACTER_SELECTED
    // ======================================================================
    model::character on_character_selected(
        model::account &acct,
        int index)
    {
        return context_.load_character(acct, index);
    }

    // ======================================================================
    // ON_CHARACTER_CREATED
    // ======================================================================
    model::character on_character_created(
        model::account &acct,
        std::string const &character_name)
    {
        return context_.new_character(acct, character_name);
    }

    // ======================================================================
    // ON_COMMAND
    // ======================================================================
    void on_command(std::string const &input)
    {
        context_.send_message(
            *character_,
            fmt::format("you say, \"{}\"", input));
        context_.send_message(
            context_.get_main_room(),
            *character_,
            fmt::format("{} says, \"{}\"", character_->name, input));
    }

    client                                 &self_;
    boost::asio::io_context::strand         strand_;

    context                                &context_;
    connection                              connection_;

    std::array<terminalpp::byte, 4096>      buffer_;
    std::array<terminalpp::byte, 4096>::size_type buffer_top_{0};
    std::atomic<bool>                       flush_requested_{false};

    boost::optional<model::character &>     character_;

    terminalpp::canvas                      canvas_;
    terminalpp::terminal                    terminal_;

    munin::background_animator              animator_;
    std::shared_ptr<ui::user_interface>     user_interface_;
    munin::window                           window_;

    std::function<void ()>                  on_connection_death_;

    std::string                             last_command_;
    std::atomic_bool                        repaint_requested_;
    std::atomic_bool                        cursor_state_changed_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client(
    boost::asio::io_context &io_context, 
    context &ctx,
    connection &&cnx,
    terminalpp::behaviour beh)
  : pimpl_(std::make_shared<impl>(*this, io_context, ctx, std::move(cnx), beh))
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
void client::on_connection_death(std::function<void ()> const &callback)
{
    pimpl_->on_connection_death(callback);
}

// ==========================================================================
// SEND_MESSAGE
// ==========================================================================
void client::send_message(terminalpp::string const &msg)
{
    pimpl_->send_message(msg);
}

}

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
#include <terminalpp/detail/lambda_visitor.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/format.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <cstdio>
#include <deque>
#include <mutex>
#include <string>
#include <vector>

using namespace terminalpp::literals;

namespace paradice {

namespace {

constexpr terminalpp::extent default_window_size{80, 24};

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
        canvas_{default_window_size},
        terminal_{create_behaviour()},
        animator_(strand_),
        user_interface_{std::make_shared<ui::user_interface>(animator_)},
        window_{user_interface_},
        repaint_requested_{false},
        cursor_state_changed_{true},
        cursor_position_changed_{true}
    {
    }

    // ======================================================================
    // ON_TOKENS_READ
    // ======================================================================
    void on_tokens_read(terminalpp::tokens const &tokens)
    {
        const auto &apply_token = 
            [this](terminalpp::token const &token)
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
    
        boost::for_each(tokens, apply_token);
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
                auto const &read_tokens =
                    [this](terminalpp::tokens const &tokens)
                    {
                        this->on_tokens_read(tokens);
                    };

                terminal_.read(read_tokens) >> data;
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

        user_interface_->on_login.connect(
            [this](auto const &name, auto const &pwd)
            {
                return this->on_login(name, pwd);
            });

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

        user_interface_->on_character_created.connect(
            [this](model::account &acct, std::string character_name)
            {
                return this->on_character_created(acct, character_name);
            });

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

        user_interface_->set_focus();

        terminal_.write(write_to_connection) 
            << terminalpp::set_window_title("Paradice9")
            << terminalpp::enable_mouse()
            << terminalpp::use_alternate_screen_buffer();

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
    // SET_WINDOW_TITLE
    // ======================================================================
    void set_window_title(std::string const &title)
    {
        terminal_.write(write_to_connection) 
            << terminalpp::set_window_title(title);
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
        terminal_.write(write_to_connection) 
            << terminalpp::disable_mouse()
            << terminalpp::show_cursor()
            << terminalpp::use_normal_screen_buffer();

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

        serverpp::byte_storage paint_data;
        auto const &append_to_paint_data =
            [&paint_data](terminalpp::bytes data)
            {
                paint_data.append(data.cbegin(), data.cend());
            };

        window_.repaint(canvas_, terminal_, append_to_paint_data);
        auto const cursor_state = user_interface_->get_cursor_state();
            
        if (cursor_state_changed_.exchange(false))
        {
            if (cursor_state)
            {
                terminal_.write(append_to_paint_data) 
                    << terminalpp::show_cursor();
            }
            else
            {
                terminal_.write(append_to_paint_data)
                    << terminalpp::hide_cursor();
            }
        }

        if (cursor_state)
        {
            terminal_.write(append_to_paint_data) << 
                terminalpp::move_cursor(user_interface_->get_cursor_position());
        }

        connection_->write(paint_data);
    }

    // ======================================================================
    // ON_LOGIN
    // ======================================================================
    model::account on_login(
        std::string const &username,
        paradice::encrypted_string const &password)
    {
        return context_->load_account(username, password);
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
    // ON_CHARACTER_CREATED
    // ======================================================================
    model::character on_character_created(
        model::account &acct,
        std::string const &character_name)
    {
        return context_->new_character(acct, character_name);
    }

    client                                 &self_;
    boost::asio::io_context::strand         strand_;

    std::shared_ptr<context>                context_;
    std::shared_ptr<account>                account_;
    std::shared_ptr<character>              character_;

    std::shared_ptr<connection>             connection_;

    std::function<void (terminalpp::bytes)> write_to_connection{
        [this](terminalpp::bytes data)
        { 
            connection_->write(data); 
        }};

    terminalpp::canvas                      canvas_;
    terminalpp::terminal                    terminal_;

    munin::background_animator              animator_;
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

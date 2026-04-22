#include <gtest/gtest.h>

#include "../paradice9/include/paradice9/context_impl.hpp"

#include <munin/background_animator.hpp>
#include <munin/render_surface.hpp>
#include <paradice/ui/shell/user_interface.hpp>
#include <terminalpp/canvas.hpp>
#include <terminalpp/string.hpp>
#include <terminalpp/virtual_key.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/filesystem.hpp>

#include <algorithm>
#include <string>
#include <vector>

namespace {

std::vector<std::string> render_lines(
    munin::component &component, terminalpp::extent const size)
{
    component.set_size(size);

    terminalpp::canvas canvas{size};
    munin::render_surface surface{canvas};
    component.draw(surface, {{}, size});

    auto result = std::vector<std::string>{};
    result.reserve(size.height_);

    for (terminalpp::coordinate_type row = 0; row < size.height_; ++row)
    {
        auto line = std::string{};
        line.reserve(size.width_);

        for (terminalpp::coordinate_type column = 0; column < size.width_;
             ++column)
        {
            line.push_back(
                static_cast<char>(canvas[column][row].glyph_.character_));
        }

        result.push_back(std::move(line));
    }

    return result;
}

void send_key(munin::component &component, terminalpp::vk const key)
{
    component.event(terminalpp::virtual_key{key});
}

void type_text(munin::component &component, std::string const &text)
{
    for (auto const ch : text)
    {
        component.event(
            terminalpp::virtual_key{static_cast<terminalpp::vk>(ch)});
    }
}

class a_paradice9_system_fixture : public ::testing::Test
{
protected:
    a_paradice9_system_fixture()
      : strand(io_context),
        animator(strand),
        user_interface(animator),
        database_path(
            boost::filesystem::temp_directory_path() /
            boost::filesystem::unique_path("paradice-%%%%-%%%%-%%%%.db3")),
        context(io_context, database_path, [&] { ++shutdown_calls; })
    {
        user_interface.on_login.connect(
            [this](auto const &name, auto const &password) {
                return context.load_account(name, password);
            });
        user_interface.on_character_selected.connect(
            [this](auto &account, int index) {
                return context.load_character(account, index);
            });
    }

    ~a_paradice9_system_fixture() override
    {
        boost::filesystem::remove(database_path);
    }

    void enter_game_with_password(std::string const &password)
    {
        type_text(user_interface, "account");
        send_key(user_interface, terminalpp::vk::ht);
        type_text(user_interface, password);
        send_key(user_interface, terminalpp::vk::ht);
        send_key(user_interface, terminalpp::vk::ht);
        send_key(user_interface, terminalpp::vk::enter);

        send_key(user_interface, terminalpp::vk::cursor_down);
        send_key(user_interface, terminalpp::vk::ht);
        send_key(user_interface, terminalpp::vk::ht);
        send_key(user_interface, terminalpp::vk::enter);
    }

    boost::asio::io_context io_context;
    boost::asio::io_context::strand strand;
    munin::background_animator animator;
    paradice::ui::user_interface user_interface;
    boost::filesystem::path database_path;
    std::size_t shutdown_calls{0};
    paradice9::context_impl context;
};

}  // namespace

TEST_F(
    a_paradice9_system_fixture,
    enters_the_game_with_a_password_updated_via_set_password)
{
    auto account = context.new_account("account", "password");
    context.new_character(account, "Mallory");
    context.set_password("account", "secret");

    enter_game_with_password("secret");

    auto const lines = render_lines(user_interface, {80, 24});

    ASSERT_TRUE(std::any_of(
        lines.begin(),
        lines.end(),
        [](std::string const &line) {
            return line.find("Mallory") != std::string::npos;
        }));
}

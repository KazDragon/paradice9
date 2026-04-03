#include <gtest/gtest.h>

#include <munin/background_animator.hpp>
#include <munin/render_surface.hpp>
#include <paradice/ui/components/who_list.hpp>
#include <paradice/ui/pages/main_page.hpp>
#include <paradice/ui/shell/user_interface.hpp>
#include <terminalpp/canvas.hpp>
#include <terminalpp/string.hpp>
#include <terminalpp/virtual_key.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/io_context_strand.hpp>

#include <string>
#include <vector>

using namespace terminalpp::literals;  // NOLINT

namespace {

std::vector<std::string> render_lines(
    munin::component &component, terminalpp::extent const size)
{
    component.set_size(size);

    terminalpp::canvas canvas{size};
    munin::render_surface surface{canvas};
    component.draw(surface, {{}, size});

    std::vector<std::string> result;
    result.reserve(size.height_);

    for (terminalpp::coordinate_type row = 0; row < size.height_; ++row)
    {
        std::string line;
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

std::vector<std::string> render_lines_on_prefilled_canvas(
    munin::component &component,
    terminalpp::extent const size,
    char const fill_character)
{
    component.set_size(size);

    terminalpp::canvas canvas{size};

    for (terminalpp::coordinate_type row = 0; row < size.height_; ++row)
    {
        for (terminalpp::coordinate_type column = 0; column < size.width_;
             ++column)
        {
            canvas[column][row] = fill_character;
        }
    }

    munin::render_surface surface{canvas};
    component.draw(surface, {{}, size});

    std::vector<std::string> result;
    result.reserve(size.height_);

    for (terminalpp::coordinate_type row = 0; row < size.height_; ++row)
    {
        std::string line;
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

bool contains(
    std::vector<terminalpp::rectangle> const &regions,
    terminalpp::point const point)
{
    return std::any_of(
        regions.begin(),
        regions.end(),
        [point](auto const &region) {
            auto const x_in_range =
                point.x_ >= region.origin_.x_
                && point.x_ < region.origin_.x_ + region.size_.width_;
            auto const y_in_range =
                point.y_ >= region.origin_.y_
                && point.y_ < region.origin_.y_ + region.size_.height_;

            return x_in_range && y_in_range;
        });
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

}  // namespace

TEST(a_who_list, prefers_a_height_of_four)
{
    auto who_list = paradice::ui::make_who_list();

    ASSERT_EQ(terminalpp::extent(0, 4), who_list->get_preferred_size());
}

TEST(a_who_list, draws_the_first_player_character_on_the_left_with_a_margin)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_player_characters({"You"_ts});

    auto const lines = render_lines(*who_list, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You                ",
             "                    ",
             "                    ",
             "                    "}),
        lines);
}

TEST(a_who_list, blanks_every_unoccupied_cell_when_drawing_the_first_name)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_player_characters({"You"_ts});

    auto const lines = render_lines_on_prefilled_canvas(*who_list, {20, 4}, 'x');

    ASSERT_EQ(
        std::vector<std::string>(
            {" You                ",
             "                    ",
             "                    ",
             "                    "}),
        lines);
}

TEST(a_who_list, draws_the_second_player_character_in_the_right_column)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_player_characters({"You"_ts, "Bob"_ts});

    auto const lines = render_lines(*who_list, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Bob      ",
             "                    ",
             "                    ",
             "                    "}),
        lines);
}

TEST(a_who_list, draws_the_third_player_character_on_the_left_of_the_second_row)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_player_characters({"You"_ts, "Bob"_ts, "Alice"_ts});

    auto const lines = render_lines(*who_list, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Bob      ",
             " Alice              ",
             "                    ",
             "                    "}),
        lines);
}

TEST(a_who_list, draws_the_fourth_player_character_in_the_right_column_of_the_second_row)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_player_characters(
        {"You"_ts, "Bob"_ts, "Alice"_ts, "Eve"_ts});

    auto const lines = render_lines(*who_list, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Bob      ",
             " Alice     Eve      ",
             "                    ",
             "                    "}),
        lines);
}

TEST(a_who_list, draws_the_fifth_player_character_on_the_left_of_the_third_row)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_player_characters(
        {"You"_ts, "Bob"_ts, "Alice"_ts, "Eve"_ts, "Mallory"_ts});

    auto const lines = render_lines(*who_list, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Bob      ",
             " Alice     Eve      ",
             " Mallory            ",
             "                    "}),
        lines);
}

TEST(a_who_list, does_not_draw_page_information_when_six_player_characters_fit)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_player_characters(
        {"You"_ts,
         "Bob"_ts,
         "Alice"_ts,
         "Eve"_ts,
         "Mallory"_ts,
         "Trent"_ts});

    auto const lines = render_lines(*who_list, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Bob      ",
             " Alice     Eve      ",
             " Mallory   Trent    ",
             "                    "}),
        lines);
}

TEST(a_who_list, draws_page_information_when_a_seventh_player_character_exists)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_player_characters(
        {"You"_ts,
         "Bob"_ts,
         "Alice"_ts,
         "Eve"_ts,
         "Mallory"_ts,
         "Trent"_ts,
         "Peggy"_ts});

    auto const lines = render_lines(*who_list, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Bob      ",
             " Alice     Eve      ",
             " Mallory   Trent    ",
             "             1 / 2  "}),
        lines);
}

TEST(a_who_list, draws_the_current_page_indicator_for_an_overflowing_second_page)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_player_characters(
        {"You"_ts,
         "Bob"_ts,
         "Alice"_ts,
         "Eve"_ts,
         "Mallory"_ts,
         "Trent"_ts,
         "Peggy"_ts});
    who_list->set_current_page(1);

    auto const lines = render_lines(*who_list, {20, 4});

    ASSERT_EQ("             2 / 2  ", lines[3]);
}

TEST(a_who_list, displays_second_page_entries_when_the_current_page_advances)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_player_characters(
        {"You"_ts,
         "Bob"_ts,
         "Alice"_ts,
         "Eve"_ts,
         "Mallory"_ts,
         "Trent"_ts,
         "Peggy"_ts});
    who_list->set_current_page(1);

    auto const lines = render_lines(*who_list, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" Peggy              ",
             "                    ",
             "                    ",
             "             2 / 2  "}),
        lines);
}

TEST(a_who_list, truncates_an_overlong_left_column_name_with_an_ellipsis)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_player_characters({"Alexandria"_ts});

    auto const lines = render_lines(*who_list, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" Alexan...          ",
             "                    ",
             "                    ",
             "                    "}),
        lines);
}

TEST(a_who_list, truncates_an_overlong_right_column_name_with_an_ellipsis)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_player_characters({"You"_ts, "Alexandria"_ts});

    auto const lines = render_lines(*who_list, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Alexa... ",
             "                    ",
             "                    ",
             "                    "}),
        lines);
}

TEST(a_who_list, preserves_a_blank_middle_column_between_left_and_right_entries)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_player_characters({"Alexandria"_ts, "Benedicta"_ts});

    auto const lines = render_lines(*who_list, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" Alexan... Bened... ",
             "                    ",
             "                    ",
             "                    "}),
        lines);
}

TEST(a_who_list, requests_a_redraw_when_the_displayed_player_characters_change)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_size({20, 4});

    std::vector<terminalpp::rectangle> redraw_regions;
    who_list->on_redraw.connect(
        [&redraw_regions](auto const &regions) {
            redraw_regions.insert(
                redraw_regions.end(), regions.begin(), regions.end());
        });

    who_list->set_player_characters({"You"_ts});

    ASSERT_EQ(
        std::vector<terminalpp::rectangle>({terminalpp::rectangle{{0, 0}, {20, 4}}}),
        redraw_regions);
}

TEST(a_who_list, requests_a_redraw_when_the_current_page_changes)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_size({20, 4});
    who_list->set_player_characters(
        {"You"_ts,
         "Bob"_ts,
         "Alice"_ts,
         "Eve"_ts,
         "Mallory"_ts,
         "Trent"_ts,
         "Peggy"_ts});

    std::vector<terminalpp::rectangle> redraw_regions;
    who_list->on_redraw.connect(
        [&redraw_regions](auto const &regions) {
            redraw_regions.insert(
                redraw_regions.end(), regions.begin(), regions.end());
        });

    who_list->set_current_page(1);

    ASSERT_EQ(
        std::vector<terminalpp::rectangle>({terminalpp::rectangle{{0, 0}, {20, 4}}}),
        redraw_regions);
}

TEST(a_who_list, page_change_redraw_covers_the_changed_roster_rows)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_size({20, 4});
    who_list->set_player_characters(
        {"You"_ts,
         "Bob"_ts,
         "Alice"_ts,
         "Eve"_ts,
         "Mallory"_ts,
         "Trent"_ts,
         "Peggy"_ts});

    std::vector<terminalpp::rectangle> redraw_regions;
    who_list->on_redraw.connect(
        [&redraw_regions](auto const &regions) {
            redraw_regions.insert(
                redraw_regions.end(), regions.begin(), regions.end());
        });

    who_list->set_current_page(1);

    ASSERT_TRUE(contains(redraw_regions, {0, 0}));
    ASSERT_TRUE(contains(redraw_regions, {19, 0}));
    ASSERT_TRUE(contains(redraw_regions, {0, 2}));
    ASSERT_TRUE(contains(redraw_regions, {19, 2}));
}

TEST(a_who_list, player_character_change_redraw_covers_the_page_information_row)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_size({20, 4});
    who_list->set_player_characters(
        {"You"_ts,
         "Bob"_ts,
         "Alice"_ts,
         "Eve"_ts,
         "Mallory"_ts,
         "Trent"_ts});

    std::vector<terminalpp::rectangle> redraw_regions;
    who_list->on_redraw.connect(
        [&redraw_regions](auto const &regions) {
            redraw_regions.insert(
                redraw_regions.end(), regions.begin(), regions.end());
        });

    who_list->set_player_characters(
        {"You"_ts,
         "Bob"_ts,
         "Alice"_ts,
         "Eve"_ts,
         "Mallory"_ts,
         "Trent"_ts,
         "Peggy"_ts});

    ASSERT_TRUE(contains(redraw_regions, {0, 3}));
    ASSERT_TRUE(contains(redraw_regions, {19, 3}));
}

TEST(a_who_list, can_receive_focus)
{
    auto who_list = paradice::ui::make_who_list();

    who_list->set_focus();

    ASSERT_TRUE(who_list->has_focus());
}

TEST(a_who_list, advances_to_the_next_page_on_right_arrow_input_when_focused)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_player_characters(
        {"You"_ts,
         "Bob"_ts,
         "Alice"_ts,
         "Eve"_ts,
         "Mallory"_ts,
         "Trent"_ts,
         "Peggy"_ts});
    who_list->set_focus();

    who_list->event(terminalpp::virtual_key{terminalpp::vk::cursor_right});

    auto const lines = render_lines(*who_list, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" Peggy              ",
             "                    ",
             "                    ",
             "             2 / 2  "}),
        lines);
}

TEST(a_who_list, cycles_from_the_last_page_to_the_first_on_right_arrow_input_when_focused)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_player_characters(
        {"You"_ts,
         "Bob"_ts,
         "Alice"_ts,
         "Eve"_ts,
         "Mallory"_ts,
         "Trent"_ts,
         "Peggy"_ts});
    who_list->set_current_page(1);
    who_list->set_focus();

    who_list->event(terminalpp::virtual_key{terminalpp::vk::cursor_right});

    auto const lines = render_lines(*who_list, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Bob      ",
             " Alice     Eve      ",
             " Mallory   Trent    ",
             "             1 / 2  "}),
        lines);
}

TEST(a_who_list, cycles_from_the_first_page_to_the_last_on_left_arrow_input_when_focused)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_player_characters(
        {"You"_ts,
         "Bob"_ts,
         "Alice"_ts,
         "Eve"_ts,
         "Mallory"_ts,
         "Trent"_ts,
         "Peggy"_ts});
    who_list->set_focus();

    who_list->event(terminalpp::virtual_key{terminalpp::vk::cursor_left});

    auto const lines = render_lines(*who_list, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" Peggy              ",
             "                    ",
             "                    ",
             "             2 / 2  "}),
        lines);
}

TEST(a_who_list, returns_to_the_previous_page_on_left_arrow_input_when_focused)
{
    auto who_list = paradice::ui::make_who_list();
    who_list->set_player_characters(
        {"You"_ts,
         "Bob"_ts,
         "Alice"_ts,
         "Eve"_ts,
         "Mallory"_ts,
         "Trent"_ts,
         "Peggy"_ts});
    who_list->set_current_page(1);
    who_list->set_focus();

    who_list->event(terminalpp::virtual_key{terminalpp::vk::cursor_left});

    auto const lines = render_lines(*who_list, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Bob      ",
             " Alice     Eve      ",
             " Mallory   Trent    ",
             "             1 / 2  "}),
        lines);
}

TEST(a_main_page, does_not_render_the_old_you_placeholder_in_the_who_list_area)
{
    paradice::ui::main_page page;

    auto const lines = render_lines(page, {40, 12});

    ASSERT_TRUE(std::none_of(
        lines.begin(),
        lines.end(),
        [](std::string const &line) { return line.find("You") != std::string::npos; }));
}

TEST(a_main_page, displays_player_character_names_in_the_hosted_who_list)
{
    paradice::ui::main_page page;
    page.set_player_characters({"Mallory"_ts, "Peggy"_ts});

    auto const lines = render_lines(page, {40, 12});

    ASSERT_TRUE(std::any_of(
        lines.begin(),
        lines.end(),
        [](std::string const &line) {
            return line.find("Mallory") != std::string::npos;
        }));
    ASSERT_TRUE(std::any_of(
        lines.begin(),
        lines.end(),
        [](std::string const &line) {
            return line.find("Peggy") != std::string::npos;
        }));
}

TEST(a_user_interface, displays_the_entered_character_in_the_active_main_page_who_list)
{
    boost::asio::io_context io_context;
    boost::asio::io_context::strand strand(io_context);
    munin::background_animator animator(strand);

    paradice::ui::user_interface user_interface(animator);
    user_interface.on_login.connect([](auto const &, auto const &) {
        return paradice::model::account{
            .name = "account",
            .character_names = {"Mallory"}};
    });
    user_interface.on_character_selected.connect([](auto &, int) {
        return paradice::model::character{
            .name = "Mallory",
            .prefix = "",
            .suffix = "",
            .send_message = {},
            .in_room = nullptr};
    });

    type_text(user_interface, "account");
    send_key(user_interface, terminalpp::vk::ht);
    type_text(user_interface, "password");
    send_key(user_interface, terminalpp::vk::ht);
    send_key(user_interface, terminalpp::vk::ht);
    send_key(user_interface, terminalpp::vk::enter);

    send_key(user_interface, terminalpp::vk::cursor_down);
    send_key(user_interface, terminalpp::vk::ht);
    send_key(user_interface, terminalpp::vk::ht);
    send_key(user_interface, terminalpp::vk::enter);

    auto const lines = render_lines(user_interface, {80, 24});

    ASSERT_TRUE(std::any_of(
        lines.begin(),
        lines.end(),
        [](std::string const &line) {
            return line.find("Mallory") != std::string::npos;
        }));
}

#include <gtest/gtest.h>

#include <munin/background_animator.hpp>
#include <munin/render_surface.hpp>
#include <paradice/ui/components/roster.hpp>
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

std::vector<terminalpp::string> six_player_roster()
{
    return {
        "You"_ts,
        "Bob"_ts,
        "Alice"_ts,
        "Eve"_ts,
        "Mallory"_ts,
        "Trent"_ts};
}

std::vector<terminalpp::string> seven_player_roster()
{
    auto roster = six_player_roster();
    roster.push_back("Peggy"_ts);
    return roster;
}

std::shared_ptr<paradice::ui::roster> make_roster_with_players(
    std::vector<terminalpp::string> names)
{
    auto roster = paradice::ui::make_roster();
    roster->set_player_characters(std::move(names));
    return roster;
}

class a_user_interface_fixture : public ::testing::Test
{
protected:
    a_user_interface_fixture()
      : strand(io_context), animator(strand), user_interface(animator)
    {
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
    }

    void enter_game()
    {
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
    }

    boost::asio::io_context io_context;
    boost::asio::io_context::strand strand;
    munin::background_animator animator;
    paradice::ui::user_interface user_interface;
};

class a_paginated_roster_fixture : public ::testing::Test
{
protected:
    a_paginated_roster_fixture()
      : roster(paradice::ui::make_roster())
    {
        roster->set_player_characters(seven_player_roster());
        roster->set_focus();
    }

    std::shared_ptr<paradice::ui::roster> roster;
};

}  // namespace

TEST(a_roster, prefers_a_height_of_four)
{
    auto roster = paradice::ui::make_roster();

    ASSERT_EQ(terminalpp::extent(0, 4), roster->get_preferred_size());
}

TEST(a_roster, prefers_a_width_that_fits_a_single_left_column_name_without_truncation)
{
    auto roster = paradice::ui::make_roster();
    roster->set_player_characters({"You"_ts});

    ASSERT_EQ(terminalpp::extent(5, 4), roster->get_preferred_size());
}

TEST(a_roster, prefers_a_width_that_fits_both_visible_columns_without_truncation)
{
    auto roster = paradice::ui::make_roster();
    roster->set_player_characters({"You"_ts, "Bob"_ts});

    ASSERT_EQ(terminalpp::extent(9, 4), roster->get_preferred_size());
}

TEST(a_roster, prefers_a_width_that_fits_the_widest_left_column_name_without_truncation)
{
    auto roster = paradice::ui::make_roster();
    roster->set_player_characters({"You"_ts, "Bob"_ts, "Mallory"_ts});

    ASSERT_EQ(terminalpp::extent(13, 4), roster->get_preferred_size());
}

TEST(a_roster, prefers_a_width_that_fits_the_widest_right_column_name_without_truncation)
{
    auto roster = paradice::ui::make_roster();
    roster->set_player_characters(
        {"You"_ts, "Bob"_ts, "Alice"_ts, "Mallory"_ts});

    ASSERT_EQ(terminalpp::extent(15, 4), roster->get_preferred_size());
}

TEST(a_roster, prefers_a_width_that_fits_page_information_without_truncation)
{
    auto roster = paradice::ui::make_roster();
    roster->set_player_characters(
        {"A"_ts, "B"_ts, "C"_ts, "D"_ts, "E"_ts, "F"_ts, "G"_ts});

    ASSERT_EQ(terminalpp::extent(7, 4), roster->get_preferred_size());
}

TEST(a_roster, announces_a_preferred_size_change_when_player_characters_change_its_width)
{
    auto roster = paradice::ui::make_roster();
    roster->set_player_characters({"You"_ts});

    std::optional<terminalpp::extent> preferred_size;
    roster->on_preferred_size_changed.connect([&preferred_size, &roster]() {
        preferred_size = roster->get_preferred_size();
    });

    roster->set_player_characters({"You"_ts, "Mallory"_ts});

    ASSERT_TRUE(preferred_size.has_value());
    ASSERT_EQ(terminalpp::extent(13, 4), *preferred_size);
}

TEST(a_roster, draws_the_first_player_character_on_the_left_with_a_margin)
{
    auto roster = paradice::ui::make_roster();
    roster->set_player_characters({"You"_ts});

    auto const lines = render_lines(*roster, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You                ",
             "                    ",
             "                    ",
             "                    "}),
        lines);
}

TEST(a_roster, blanks_every_unoccupied_cell_when_drawing_the_first_name)
{
    auto roster = paradice::ui::make_roster();
    roster->set_player_characters({"You"_ts});

    auto const lines = render_lines_on_prefilled_canvas(*roster, {20, 4}, 'x');

    ASSERT_EQ(
        std::vector<std::string>(
            {" You                ",
             "                    ",
             "                    ",
             "                    "}),
        lines);
}

TEST(a_roster, draws_the_second_player_character_in_the_right_column)
{
    auto roster = paradice::ui::make_roster();
    roster->set_player_characters({"You"_ts, "Bob"_ts});

    auto const lines = render_lines(*roster, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Bob      ",
             "                    ",
             "                    ",
             "                    "}),
        lines);
}

TEST(a_roster, draws_the_third_player_character_on_the_left_of_the_second_row)
{
    auto roster = paradice::ui::make_roster();
    roster->set_player_characters({"You"_ts, "Bob"_ts, "Alice"_ts});

    auto const lines = render_lines(*roster, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Bob      ",
             " Alice              ",
             "                    ",
             "                    "}),
        lines);
}

TEST(a_roster, draws_the_fourth_player_character_in_the_right_column_of_the_second_row)
{
    auto roster = paradice::ui::make_roster();
    roster->set_player_characters(
        {"You"_ts, "Bob"_ts, "Alice"_ts, "Eve"_ts});

    auto const lines = render_lines(*roster, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Bob      ",
             " Alice     Eve      ",
             "                    ",
             "                    "}),
        lines);
}

TEST(a_roster, draws_the_fifth_player_character_on_the_left_of_the_third_row)
{
    auto roster = paradice::ui::make_roster();
    roster->set_player_characters(
        {"You"_ts, "Bob"_ts, "Alice"_ts, "Eve"_ts, "Mallory"_ts});

    auto const lines = render_lines(*roster, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Bob      ",
             " Alice     Eve      ",
             " Mallory            ",
             "                    "}),
        lines);
}

TEST(a_roster, does_not_draw_page_information_when_six_player_characters_fit)
{
    auto roster = make_roster_with_players(six_player_roster());

    auto const lines = render_lines(*roster, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Bob      ",
             " Alice     Eve      ",
             " Mallory   Trent    ",
             "                    "}),
        lines);
}

TEST(a_roster, draws_page_information_when_a_seventh_player_character_exists)
{
    auto roster = make_roster_with_players(seven_player_roster());

    auto const lines = render_lines(*roster, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Bob      ",
             " Alice     Eve      ",
             " Mallory   Trent    ",
             "             1 / 2  "}),
        lines);
}

TEST(a_roster, clips_away_the_footer_row_when_drawn_at_height_three)
{
    auto roster = make_roster_with_players(seven_player_roster());

    auto const lines = render_lines(*roster, {20, 3});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Bob      ",
             " Alice     Eve      ",
             " Mallory   Trent    "}),
        lines);
}

TEST(a_roster, clips_away_the_third_name_row_when_drawn_at_height_two)
{
    auto roster = make_roster_with_players(seven_player_roster());

    auto const lines = render_lines(*roster, {20, 2});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Bob      ",
             " Alice     Eve      "}),
        lines);
}

TEST(a_roster, clips_away_all_but_the_first_name_row_when_drawn_at_height_one)
{
    auto roster = make_roster_with_players(seven_player_roster());

    auto const lines = render_lines(*roster, {20, 1});

    ASSERT_EQ(std::vector<std::string>({" You       Bob      "}), lines);
}

TEST(a_roster, draws_nothing_when_drawn_at_height_zero)
{
    auto roster = make_roster_with_players(seven_player_roster());

    auto const lines = render_lines(*roster, {20, 0});

    ASSERT_TRUE(lines.empty());
}

TEST_F(
    a_paginated_roster_fixture,
    draws_the_current_page_indicator_for_an_overflowing_second_page)
{
    send_key(*roster, terminalpp::vk::cursor_right);

    auto const lines = render_lines(*roster, {20, 4});

    ASSERT_EQ("             2 / 2  ", lines[3]);
}

TEST_F(
    a_paginated_roster_fixture,
    displays_second_page_entries_when_the_current_page_advances)
{
    send_key(*roster, terminalpp::vk::cursor_right);

    auto const lines = render_lines(*roster, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" Peggy              ",
             "                    ",
             "                    ",
             "             2 / 2  "}),
        lines);
}

TEST_F(
    a_paginated_roster_fixture,
    returns_to_the_first_page_when_player_characters_shrink_below_the_current_page)
{
    send_key(*roster, terminalpp::vk::cursor_right);

    roster->set_player_characters(six_player_roster());

    auto const lines = render_lines(*roster, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Bob      ",
             " Alice     Eve      ",
             " Mallory   Trent    ",
             "                    "}),
        lines);
}

TEST(a_roster, truncates_an_overlong_left_column_name_with_an_ellipsis)
{
    auto roster = make_roster_with_players({"Alexandria"_ts});

    auto const lines = render_lines(*roster, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" Alexan...          ",
             "                    ",
             "                    ",
             "                    "}),
        lines);
}

TEST(a_roster, truncates_an_overlong_right_column_name_with_an_ellipsis)
{
    auto roster =
        make_roster_with_players({"You"_ts, "Alexandria"_ts});

    auto const lines = render_lines(*roster, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Alexa... ",
             "                    ",
             "                    ",
             "                    "}),
        lines);
}

TEST(a_roster, preserves_a_blank_middle_column_between_left_and_right_entries)
{
    auto roster =
        make_roster_with_players({"Alexandria"_ts, "Benedicta"_ts});

    auto const lines = render_lines(*roster, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" Alexan... Bened... ",
             "                    ",
             "                    ",
             "                    "}),
        lines);
}

TEST(a_roster, preserves_two_truncated_columns_when_drawn_at_width_nine)
{
    auto roster =
        make_roster_with_players({"Alexandria"_ts, "Benedicta"_ts});

    auto const lines = render_lines(*roster, {9, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" ... ... ",
             "         ",
             "         ",
             "         "}),
        lines);
}

TEST(a_roster, shrinks_two_truncated_columns_to_single_dots_when_drawn_at_width_five)
{
    auto roster =
        make_roster_with_players({"Alexandria"_ts, "Benedicta"_ts});

    auto const lines = render_lines(*roster, {5, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" . . ",
             "     ",
             "     ",
             "     "}),
        lines);
}

TEST(a_roster, draws_blank_space_when_drawn_narrower_than_the_two_dot_skeleton)
{
    auto roster =
        make_roster_with_players({"Alexandria"_ts, "Benedicta"_ts});

    auto const lines = render_lines(*roster, {4, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {"    ",
             "    ",
             "    ",
             "    "}),
        lines);
}

TEST(a_roster, requests_a_redraw_when_the_displayed_player_characters_change)
{
    auto roster = paradice::ui::make_roster();
    roster->set_size({20, 4});

    std::vector<terminalpp::rectangle> redraw_regions;
    roster->on_redraw.connect(
        [&redraw_regions](auto const &regions) {
            redraw_regions.insert(
                redraw_regions.end(), regions.begin(), regions.end());
        });

    roster->set_player_characters({"You"_ts});

    ASSERT_EQ(
        std::vector<terminalpp::rectangle>({terminalpp::rectangle{{0, 0}, {20, 4}}}),
        redraw_regions);
}

TEST_F(
    a_paginated_roster_fixture,
    requests_a_redraw_when_the_current_page_changes)
{
    roster->set_size({20, 4});

    std::vector<terminalpp::rectangle> redraw_regions;
    roster->on_redraw.connect(
        [&redraw_regions](auto const &regions) {
            redraw_regions.insert(
                redraw_regions.end(), regions.begin(), regions.end());
        });

    roster->set_focus();
    send_key(*roster, terminalpp::vk::cursor_right);

    ASSERT_EQ(
        std::vector<terminalpp::rectangle>({terminalpp::rectangle{{0, 0}, {20, 4}}}),
        redraw_regions);
}

TEST_F(a_paginated_roster_fixture, page_change_redraw_covers_the_changed_roster_rows)
{
    roster->set_size({20, 4});

    std::vector<terminalpp::rectangle> redraw_regions;
    roster->on_redraw.connect(
        [&redraw_regions](auto const &regions) {
            redraw_regions.insert(
                redraw_regions.end(), regions.begin(), regions.end());
        });

    roster->set_focus();
    send_key(*roster, terminalpp::vk::cursor_right);

    ASSERT_TRUE(contains(redraw_regions, {0, 0}));
    ASSERT_TRUE(contains(redraw_regions, {19, 0}));
    ASSERT_TRUE(contains(redraw_regions, {0, 2}));
    ASSERT_TRUE(contains(redraw_regions, {19, 2}));
}

TEST(a_roster, player_character_change_redraw_covers_the_page_information_row)
{
    auto roster = paradice::ui::make_roster();
    roster->set_size({20, 4});
    roster->set_player_characters(six_player_roster());

    std::vector<terminalpp::rectangle> redraw_regions;
    roster->on_redraw.connect(
        [&redraw_regions](auto const &regions) {
            redraw_regions.insert(
                redraw_regions.end(), regions.begin(), regions.end());
        });

    roster->set_player_characters(seven_player_roster());

    ASSERT_TRUE(contains(redraw_regions, {0, 3}));
    ASSERT_TRUE(contains(redraw_regions, {19, 3}));
}

TEST(a_roster, can_receive_focus)
{
    auto roster = paradice::ui::make_roster();

    roster->set_focus();

    ASSERT_TRUE(roster->has_focus());
}

TEST_F(a_paginated_roster_fixture, advances_to_the_next_page_on_right_arrow_input_when_focused)
{
    send_key(*roster, terminalpp::vk::cursor_right);

    auto const lines = render_lines(*roster, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" Peggy              ",
             "                    ",
             "                    ",
             "             2 / 2  "}),
        lines);
}

TEST_F(
    a_paginated_roster_fixture,
    cycles_from_the_last_page_to_the_first_on_right_arrow_input_when_focused)
{
    send_key(*roster, terminalpp::vk::cursor_right);

    send_key(*roster, terminalpp::vk::cursor_right);

    auto const lines = render_lines(*roster, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Bob      ",
             " Alice     Eve      ",
             " Mallory   Trent    ",
             "             1 / 2  "}),
        lines);
}

TEST_F(
    a_paginated_roster_fixture,
    cycles_from_the_first_page_to_the_last_on_left_arrow_input_when_focused)
{
    send_key(*roster, terminalpp::vk::cursor_left);

    auto const lines = render_lines(*roster, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" Peggy              ",
             "                    ",
             "                    ",
             "             2 / 2  "}),
        lines);
}

TEST_F(
    a_paginated_roster_fixture,
    returns_to_the_previous_page_on_left_arrow_input_when_focused)
{
    send_key(*roster, terminalpp::vk::cursor_right);

    send_key(*roster, terminalpp::vk::cursor_left);

    auto const lines = render_lines(*roster, {20, 4});

    ASSERT_EQ(
        std::vector<std::string>(
            {" You       Bob      ",
             " Alice     Eve      ",
             " Mallory   Trent    ",
             "             1 / 2  "}),
        lines);
}

TEST(a_main_page, does_not_render_the_old_you_placeholder_in_the_roster_area)
{
    paradice::ui::main_page page;

    auto const lines = render_lines(page, {40, 12});

    ASSERT_TRUE(std::none_of(
        lines.begin(),
        lines.end(),
        [](std::string const &line) { return line.find("You") != std::string::npos; }));
}

TEST(a_main_page, displays_player_character_names_in_the_hosted_roster)
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

TEST_F(
    a_user_interface_fixture,
    displays_the_entered_character_in_the_active_main_page_roster)
{
    enter_game();

    auto const lines = render_lines(user_interface, {80, 24});

    ASSERT_TRUE(std::any_of(
        lines.begin(),
        lines.end(),
        [](std::string const &line) {
            return line.find("Mallory") != std::string::npos;
        }));
}

TEST_F(
    a_user_interface_fixture,
    updates_the_active_main_page_roster_from_explicit_player_character_input)
{
    enter_game();

    user_interface.set_player_characters({"Mallory"_ts, "Peggy"_ts});

    auto const lines = render_lines(user_interface, {80, 24});

    ASSERT_TRUE(std::any_of(
        lines.begin(),
        lines.end(),
        [](std::string const &line) {
            return line.find("Peggy") != std::string::npos;
        }));
}

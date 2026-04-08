#include <gtest/gtest.h>

#include <paradice/client.hpp>
#include <paradice/connection.hpp>
#include <paradice/context.hpp>
#include <paradice/model/account.hpp>
#include <paradice/model/character.hpp>
#include <paradice/model/room.hpp>
#include <paradice/model/room_membership.hpp>

#include <boost/asio/io_context.hpp>

#include <algorithm>
#include <functional>
#include <memory>
#include <string>

using namespace terminalpp::literals;  // NOLINT

namespace {

struct fake_channel
{
    void async_read(std::function<void(paradice::bytes)> const &callback)
    {
        read_callback_ = callback;
    }

    void write(paradice::bytes data)
    {
        written_.append(data.begin(), data.end());
    }

    [[nodiscard]] bool is_alive() const
    {
        return alive_;
    }

    void close()
    {
        alive_ = false;

        if (read_callback_)
        {
            read_callback_({});
        }
    }

    void receive(paradice::byte_storage const &data)
    {
        std::function<void(paradice::bytes)> callback;
        std::swap(read_callback_, callback);

        if (callback)
        {
            callback({data.data(), data.size()});
        }
    }

    std::function<void(paradice::bytes)> read_callback_;
    paradice::byte_storage written_;
    bool alive_{true};
};

struct fake_context : paradice::context
{
    void add_client(std::shared_ptr<paradice::client> const &) override {}
    void remove_client(std::shared_ptr<paradice::client> const &) override {}

    paradice::model::account new_account(
        std::string const &, std::string const &) override
    {
        return {};
    }

    paradice::model::account load_account(
        std::string const &, std::string const &) override
    {
        return {
            .name = "account",
            .character_names = {"Mallory"}};
    }

    paradice::model::character load_character(
        paradice::model::account &, int) override
    {
        return {
            .name = "Mallory",
            .prefix = "",
            .suffix = "",
            .send_message = {},
            .in_room = nullptr};
    }

    paradice::model::character new_character(
        paradice::model::account &, std::string const &) override
    {
        return {};
    }

    void shutdown() override {}

    void register_online_character(paradice::model::character &character) override
    {
        online_characters.push_back(&character);
    }

    void unregister_online_character(
        paradice::model::character &character) override
    {
        auto const it = std::remove(
            online_characters.begin(), online_characters.end(), &character);
        online_characters.erase(it, online_characters.end());
    }

    paradice::model::character *find_online_character_by_name(
        std::string const &name) override
    {
        auto const it = std::find_if(
            online_characters.begin(),
            online_characters.end(),
            [&name](auto *character) { return character->name == name; });

        return it == online_characters.end() ? nullptr : *it;
    }

    void send_message(
        paradice::model::character &character,
        terminalpp::string const &message) override
    {
        direct_messages.push_back(message);
        if (character.send_message)
        {
            character.send_message(message);
        }
    }

    void send_message(
        paradice::model::room &, terminalpp::string const &) override
    {
    }

    void send_message(
        paradice::model::room &,
        paradice::model::character &,
        terminalpp::string const &message) override
    {
        room_messages.push_back(message);
    }

    paradice::model::room &get_main_room() override
    {
        return main_room;
    }

    paradice::model::room main_room;
    std::vector<terminalpp::string> direct_messages;
    std::vector<terminalpp::string> room_messages;
    std::vector<paradice::model::character *> online_characters;
};

void drain(boost::asio::io_context &io_context)
{
    io_context.restart();

    while (io_context.poll() != 0)
    {
    }
}

paradice::byte_storage bytes(std::string const &text)
{
    return {text.begin(), text.end()};
}

std::string to_string(paradice::byte_storage const &data)
{
    return {data.begin(), data.end()};
}

paradice::model::character *find_character(
    paradice::model::room &room, std::string const &name)
{
    auto const it = std::find_if(
        room.characters.begin(),
        room.characters.end(),
        [&name](auto const *character) { return character->name == name; });

    return it == room.characters.end() ? nullptr : *it;
}

void enter_game(
    boost::asio::io_context &io_context, std::shared_ptr<fake_channel> channel)
{
    channel->receive(bytes("account\tpassword\t\t\r\n"));
    drain(io_context);

    channel->receive(bytes("\x1B[B\t\t\r\n"));
    drain(io_context);
}

void enter_command_and_capture_messages(
    boost::asio::io_context &io_context,
    fake_context &context,
    std::shared_ptr<fake_channel> channel,
    std::string const &command)
{
    context.direct_messages.clear();
    context.room_messages.clear();

    channel->receive(bytes(command + "\r\n"));
    drain(io_context);
}

void assert_public_speech_messages(
    fake_context const &context, std::string const &speaker, std::string const &message)
{
    ASSERT_EQ(1u, context.direct_messages.size());
    ASSERT_EQ(1u, context.room_messages.size());
    ASSERT_EQ(
        "you say, \"" + message + "\"",
        terminalpp::to_string(context.direct_messages[0]));
    ASSERT_EQ(
        speaker + " says, \"" + message + "\"",
        terminalpp::to_string(context.room_messages[0]));
}

void assert_deterministic_roll_messages(
    std::string const &command,
    std::vector<std::int32_t> const &faces,
    terminalpp::string const &expected_direct_message,
    terminalpp::string const &expected_room_message)
{
    boost::asio::io_context io_context;
    fake_context context;
    auto channel = std::make_shared<fake_channel>();
    auto next_face = std::size_t{0};

    paradice::client client(
        io_context,
        context,
        paradice::connection(*channel),
        {},
        [&](std::uint32_t) { return faces[next_face++]; });

    drain(io_context);
    channel->written_.clear();
    enter_game(io_context, channel);
    enter_command_and_capture_messages(io_context, context, channel, command);

    ASSERT_EQ(1u, context.direct_messages.size());
    ASSERT_EQ(1u, context.room_messages.size());
    ASSERT_EQ(expected_direct_message, context.direct_messages[0]);
    ASSERT_EQ(expected_room_message, context.room_messages[0]);
}

void assert_deterministic_private_roll_message(
    std::string const &command,
    std::vector<std::int32_t> const &faces,
    terminalpp::string const &expected_direct_message)
{
    boost::asio::io_context io_context;
    fake_context context;
    auto channel = std::make_shared<fake_channel>();
    auto next_face = std::size_t{0};

    paradice::client client(
        io_context,
        context,
        paradice::connection(*channel),
        {},
        [&](std::uint32_t) { return faces[next_face++]; });

    drain(io_context);
    channel->written_.clear();
    enter_game(io_context, channel);
    enter_command_and_capture_messages(io_context, context, channel, command);

    ASSERT_EQ(1u, context.direct_messages.size());
    ASSERT_EQ(0u, context.room_messages.size());
    ASSERT_EQ(expected_direct_message, context.direct_messages[0]);
}

}  // namespace

TEST(a_client, displays_existing_room_members_in_the_roster_when_entering_the_game)
{
    boost::asio::io_context io_context;
    fake_context context;
    auto channel = std::make_shared<fake_channel>();

    paradice::model::character peggy{
        .name = "Peggy",
        .prefix = "",
        .suffix = "",
        .send_message = {},
        .in_room = &context.main_room};
    context.main_room.characters.push_back(&peggy);

    paradice::client client(
        io_context, context, paradice::connection(*channel), {});

    drain(io_context);
    channel->written_.clear();

    channel->receive(bytes("account\tpassword\t\t\r\n"));
    drain(io_context);

    channel->receive(bytes("\x1B[B\t\t\r\n"));
    drain(io_context);

    ASSERT_NE(std::string::npos, to_string(channel->written_).find("Peggy"));
}

TEST(a_client, refreshes_the_roster_when_a_later_room_member_is_observed_after_entry)
{
    boost::asio::io_context io_context;
    fake_context context;
    auto channel = std::make_shared<fake_channel>();

    paradice::client client(
        io_context, context, paradice::connection(*channel), {});

    drain(io_context);
    channel->written_.clear();

    channel->receive(bytes("account\tpassword\t\t\r\n"));
    drain(io_context);

    channel->receive(bytes("\x1B[B\t\t\r\n"));
    drain(io_context);
    channel->written_.clear();

    paradice::model::character peggy{
        .name = "Peggy",
        .prefix = "",
        .suffix = "",
        .send_message = {},
        .in_room = &context.main_room};
    context.main_room.characters.push_back(&peggy);

    auto *mallory = find_character(context.main_room, "Mallory");
    ASSERT_NE(nullptr, mallory);

    context.send_message(*mallory, ""_ts);
    drain(io_context);

    ASSERT_NE(std::string::npos, to_string(channel->written_).find("Peggy"));
}

TEST(a_client, refreshes_the_roster_when_a_later_room_member_leaves_after_entry)
{
    boost::asio::io_context io_context;
    fake_context context;
    auto channel = std::make_shared<fake_channel>();

    paradice::client client(
        io_context, context, paradice::connection(*channel), {});

    drain(io_context);
    channel->written_.clear();

    channel->receive(bytes("account\tpassword\t\t\r\n"));
    drain(io_context);

    channel->receive(bytes("\x1B[B\t\t\r\n"));
    drain(io_context);

    paradice::model::character peggy{
        .name = "Peggy",
        .prefix = "",
        .suffix = "",
        .send_message = {},
        .in_room = &context.main_room};
    context.main_room.characters.push_back(&peggy);

    auto *mallory = find_character(context.main_room, "Mallory");
    ASSERT_NE(nullptr, mallory);

    context.send_message(*mallory, ""_ts);
    drain(io_context);
    ASSERT_NE(std::string::npos, to_string(channel->written_).find("Peggy"));

    channel->written_.clear();
    paradice::model::leave_room(peggy);

    context.send_message(*mallory, ""_ts);
    drain(io_context);

    ASSERT_EQ(std::string::npos, to_string(channel->written_).find("Peggy"));
}

TEST(a_client, emits_expected_speech_text_when_a_command_is_entered_in_game)
{
    boost::asio::io_context io_context;
    fake_context context;
    auto channel = std::make_shared<fake_channel>();

    paradice::client client(
        io_context, context, paradice::connection(*channel), {});

    drain(io_context);
    channel->written_.clear();
    enter_game(io_context, channel);
    enter_command_and_capture_messages(io_context, context, channel, "hello");

    assert_public_speech_messages(context, "Mallory", "hello");
}

TEST(a_client, treats_say_prefixed_input_as_a_command_and_not_literal_speech)
{
    boost::asio::io_context io_context;
    fake_context context;
    auto channel = std::make_shared<fake_channel>();

    paradice::client client(
        io_context, context, paradice::connection(*channel), {});

    drain(io_context);
    channel->written_.clear();
    enter_game(io_context, channel);
    enter_command_and_capture_messages(
        io_context, context, channel, "/say hello");

    assert_public_speech_messages(context, "Mallory", "hello");
}

TEST(a_client, treats_non_slash_say_text_as_literal_public_speech)
{
    boost::asio::io_context io_context;
    fake_context context;
    auto channel = std::make_shared<fake_channel>();

    paradice::client client(
        io_context, context, paradice::connection(*channel), {});

    drain(io_context);
    channel->written_.clear();
    enter_game(io_context, channel);
    enter_command_and_capture_messages(
        io_context, context, channel, "say hello");

    assert_public_speech_messages(context, "Mallory", "say hello");
}

TEST(a_client, routes_slash_tell_prefixed_input_as_private_messaging)
{
    boost::asio::io_context io_context;
    fake_context context;
    auto channel = std::make_shared<fake_channel>();

    paradice::client client(
        io_context, context, paradice::connection(*channel), {});

    drain(io_context);
    channel->written_.clear();
    enter_game(io_context, channel);

    paradice::model::room side_room;
    paradice::model::character peggy{
        .name = "Peggy",
        .prefix = "",
        .suffix = "",
        .send_message = {},
        .in_room = &side_room};
    side_room.characters.push_back(&peggy);
    context.register_online_character(peggy);

    enter_command_and_capture_messages(
        io_context, context, channel, "/tell Peggy hello");

    ASSERT_EQ(2u, context.direct_messages.size());
    ASSERT_EQ(0u, context.room_messages.size());
    ASSERT_EQ("you tell Peggy, \"hello\""_ts, context.direct_messages[0]);
    ASSERT_EQ("Mallory tells you, \"hello\""_ts, context.direct_messages[1]);
}

TEST(a_client, reports_a_missing_player_when_slash_tell_target_cannot_be_found)
{
    boost::asio::io_context io_context;
    fake_context context;
    auto channel = std::make_shared<fake_channel>();

    paradice::client client(
        io_context, context, paradice::connection(*channel), {});

    drain(io_context);
    channel->written_.clear();
    enter_game(io_context, channel);

    enter_command_and_capture_messages(
        io_context, context, channel, "/tell Peggy hello");

    ASSERT_EQ(1u, context.direct_messages.size());
    ASSERT_EQ(0u, context.room_messages.size());
    ASSERT_EQ(
        "A player with name Peggy could not be found."_ts,
        context.direct_messages[0]);
}

TEST(a_client, treats_non_slash_tell_text_as_public_speech)
{
    boost::asio::io_context io_context;
    fake_context context;
    auto channel = std::make_shared<fake_channel>();

    paradice::client client(
        io_context, context, paradice::connection(*channel), {});

    drain(io_context);
    channel->written_.clear();
    enter_game(io_context, channel);
    enter_command_and_capture_messages(
        io_context, context, channel, "tell Peggy hello");

    assert_public_speech_messages(context, "Mallory", "tell Peggy hello");
}

TEST(a_client, reports_unknown_slash_commands_to_the_sender_without_room_broadcast)
{
    boost::asio::io_context io_context;
    fake_context context;
    auto channel = std::make_shared<fake_channel>();

    paradice::client client(
        io_context, context, paradice::connection(*channel), {});

    drain(io_context);
    channel->written_.clear();
    enter_game(io_context, channel);
    enter_command_and_capture_messages(io_context, context, channel, "/xyzzy");

    ASSERT_EQ(1u, context.direct_messages.size());
    ASSERT_EQ(0u, context.room_messages.size());
    ASSERT_EQ("Unknown command: /xyzzy"_ts, context.direct_messages[0]);
}

TEST(a_client, routes_slash_roll_prefixed_input_as_shared_dice_rolling)
{
    boost::asio::io_context io_context;
    fake_context context;
    auto channel = std::make_shared<fake_channel>();

    paradice::client client(
        io_context, context, paradice::connection(*channel), {});

    drain(io_context);
    channel->written_.clear();
    enter_game(io_context, channel);
    enter_command_and_capture_messages(io_context, context, channel, "/roll 1d6");

    ASSERT_EQ(1u, context.direct_messages.size());
    ASSERT_EQ(1u, context.room_messages.size());
}

TEST(a_client, reports_invalid_roll_usage_to_the_sender_without_room_broadcast)
{
    boost::asio::io_context io_context;
    fake_context context;
    auto channel = std::make_shared<fake_channel>();

    paradice::client client(
        io_context, context, paradice::connection(*channel), {});

    drain(io_context);
    channel->written_.clear();
    enter_game(io_context, channel);
    enter_command_and_capture_messages(io_context, context, channel, "/roll nope");

    ASSERT_EQ(1u, context.direct_messages.size());
    ASSERT_EQ(0u, context.room_messages.size());
    ASSERT_EQ(
        "\n Usage:   roll [n*]<dice>d<sides>[<bonuses...>] [<category>]"
        "\n Example: roll 2d6+3-20"
        "\n Example: roll 20*2d6"
        "\n Example: roll 1d10+4 initiative"
        "\n"_ts,
        context.direct_messages[0]);
}

TEST(a_client, includes_the_bonus_in_the_reported_roll_total)
{
    boost::asio::io_context io_context;
    fake_context context;
    auto channel = std::make_shared<fake_channel>();

    paradice::client client(
        io_context, context, paradice::connection(*channel), {});

    drain(io_context);
    channel->written_.clear();
    enter_game(io_context, channel);
    enter_command_and_capture_messages(io_context, context, channel, "/roll 2d6+3");

    ASSERT_EQ(1u, context.direct_messages.size());
    ASSERT_EQ(1u, context.room_messages.size());
    ASSERT_NE(
        std::string::npos,
        terminalpp::to_string(context.direct_messages[0]).find("and score"));
    ASSERT_NE(
        std::string::npos,
        terminalpp::to_string(context.room_messages[0]).find("and scores"));
}

TEST(a_client, reports_a_deterministic_shared_roll_with_category_faces_and_total)
{
    assert_deterministic_roll_messages(
        "/roll 2d6+3 initiative",
        {2, 4},
        "you roll 2d6+3 initiative and score 9 [2, 4]"_ts,
        "Mallory rolls 2d6+3 initiative and scores 9 [2, 4]"_ts);
}

TEST(a_client, reports_a_deterministic_shared_roll_with_multiple_dice_and_bonus_chain)
{
    assert_deterministic_roll_messages(
        "/roll 3d9+3-2 initiative",
        {1, 2, 3},
        "you roll 3d9+3-2 initiative and score 7 [1, 2, 3]"_ts,
        "Mallory rolls 3d9+3-2 initiative and scores 7 [1, 2, 3]"_ts);
}

TEST(a_client, reports_a_deterministic_private_roll_only_to_the_sender)
{
    assert_deterministic_private_roll_message(
        "/rollprivate 1d6", {4}, "you roll 1d6 and score 4 [4]"_ts);
}

TEST(a_client, reports_a_deterministic_private_roll_with_category_faces_and_total)
{
    assert_deterministic_private_roll_message(
        "/rollprivate 2d6+3 initiative",
        {2, 4},
        "you roll 2d6+3 initiative and score 9 [2, 4]"_ts);
}

TEST(a_client, reports_invalid_private_roll_usage_only_to_the_sender)
{
    boost::asio::io_context io_context;
    fake_context context;
    auto channel = std::make_shared<fake_channel>();

    paradice::client client(
        io_context, context, paradice::connection(*channel), {});

    drain(io_context);
    channel->written_.clear();
    enter_game(io_context, channel);
    enter_command_and_capture_messages(io_context, context, channel, "/rollprivate nope");

    ASSERT_EQ(1u, context.direct_messages.size());
    ASSERT_EQ(0u, context.room_messages.size());
    ASSERT_EQ(
        "\n Usage:   roll [n*]<dice>d<sides>[<bonuses...>] [<category>]"
        "\n Example: roll 2d6+3-20"
        "\n Example: roll 20*2d6"
        "\n Example: roll 1d10+4 initiative"
        "\n"_ts,
        context.direct_messages[0]);
}

TEST(a_client, reports_zero_sided_dice_fumble_publicly)
{
    boost::asio::io_context io_context;
    fake_context context;
    auto channel = std::make_shared<fake_channel>();

    paradice::client client(
        io_context, context, paradice::connection(*channel), {});

    drain(io_context);
    channel->written_.clear();
    enter_game(io_context, channel);
    enter_command_and_capture_messages(io_context, context, channel, "/roll 1d0");

    ASSERT_EQ(1u, context.direct_messages.size());
    ASSERT_EQ(1u, context.room_messages.size());
    ASSERT_EQ(
        "You fumble your roll and spill all your zero-sided dice on the floor.\n"_ts,
        context.direct_messages[0]);
    ASSERT_EQ(
        "Mallory fumbles their roll and spills a pile of zero-sided dice on the floor.\n"_ts,
        context.room_messages[0]);
}

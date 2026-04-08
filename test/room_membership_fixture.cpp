#include <gtest/gtest.h>

#include <paradice/context.hpp>
#include <paradice/model/character.hpp>
#include <paradice/model/room.hpp>
#include <paradice/model/room_membership.hpp>
#include <paradice/room_lifecycle.hpp>
#include <terminalpp/string.hpp>

using namespace terminalpp::literals;  // NOLINT

namespace {

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
        return {};
    }

    std::vector<std::string> list_accounts() override
    {
        return {};
    }

    std::vector<std::string> list_characters(std::string const &) override
    {
        return {};
    }

    bool has_permission(
        paradice::model::account const &, std::string const &) override
    {
        return false;
    }

    paradice::model::character load_character(
        paradice::model::account &, int) override
    {
        return {};
    }

    paradice::model::character new_character(
        paradice::model::account &, std::string const &) override
    {
        return {};
    }

    void shutdown() override {}

    void register_online_character(paradice::model::character &) override {}

    void unregister_online_character(paradice::model::character &) override {}

    paradice::model::character *find_online_character_by_name(
        std::string const &) override
    {
        return nullptr;
    }

    void send_message(
        paradice::model::character &, terminalpp::string const &) override
    {
    }

    void send_message(
        paradice::model::room &, terminalpp::string const &) override
    {
    }

    void send_message(
        paradice::model::room &room,
        paradice::model::character &character,
        terminalpp::string const &message) override
    {
        ++room_message_count;
        room_message_room = &room;
        room_message_excluded_character = &character;
        room_message = message;
    }

    paradice::model::room &get_main_room() override
    {
        return main_room;
    }

    paradice::model::room main_room;
    paradice::model::room *room_message_room = nullptr;
    paradice::model::character *room_message_excluded_character = nullptr;
    terminalpp::string room_message;
    int room_message_count = 0;
};

}  // namespace

TEST(a_character, can_leave_their_current_room)
{
    paradice::model::room room;
    paradice::model::character character{};

    character.in_room = &room;
    room.characters.push_back(&character);

    paradice::model::leave_room(character);

    ASSERT_EQ(nullptr, character.in_room);
    ASSERT_TRUE(room.characters.empty());
}

TEST(a_connected_character, leaves_their_room_before_their_departure_is_announced)
{
    fake_context context;
    paradice::model::room room;
    paradice::model::character character{};

    character.name = "Alice";
    character.in_room = &room;
    room.characters.push_back(&character);

    paradice::disconnect_character(context, character);

    ASSERT_EQ(nullptr, character.in_room);
    ASSERT_TRUE(room.characters.empty());
    ASSERT_EQ(&room, context.room_message_room);
    ASSERT_EQ(&character, context.room_message_excluded_character);
    ASSERT_EQ("Alice has left Paradice"_ts, context.room_message);
}

TEST(an_already_disconnected_character, does_nothing_when_disconnected)
{
    fake_context context;
    paradice::model::character character{};

    character.name = "Alice";
    character.in_room = nullptr;

    paradice::disconnect_character(context, character);

    ASSERT_EQ(nullptr, character.in_room);
    ASSERT_EQ(nullptr, context.room_message_room);
    ASSERT_EQ(nullptr, context.room_message_excluded_character);
    ASSERT_TRUE(context.room_message.empty());
}

TEST(a_disconnected_character, is_not_reannounced_when_disconnected_again)
{
    fake_context context;
    paradice::model::room room;
    paradice::model::character character{};

    character.name = "Alice";
    character.in_room = &room;
    room.characters.push_back(&character);

    paradice::disconnect_character(context, character);
    paradice::disconnect_character(context, character);

    ASSERT_EQ(nullptr, character.in_room);
    ASSERT_TRUE(room.characters.empty());
    ASSERT_EQ(1, context.room_message_count);
    ASSERT_EQ(&room, context.room_message_room);
    ASSERT_EQ(&character, context.room_message_excluded_character);
    ASSERT_EQ("Alice has left Paradice"_ts, context.room_message);
}

#include <gtest/gtest.h>

#include <paradice/client.hpp>
#include <paradice/connection.hpp>
#include <paradice/context.hpp>
#include <paradice/model/account.hpp>
#include <paradice/model/character.hpp>
#include <paradice/model/room.hpp>

#include <boost/asio/io_context.hpp>

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

    void send_message(
        paradice::model::character &character,
        terminalpp::string const &message) override
    {
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
        terminalpp::string const &) override
    {
    }

    paradice::model::room &get_main_room() override
    {
        return main_room;
    }

    paradice::model::room main_room;
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

}  // namespace

TEST(a_client, displays_existing_room_members_in_the_who_list_when_entering_the_game)
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

TEST(a_client, refreshes_the_who_list_when_a_later_room_member_is_observed_after_entry)
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

#include <gtest/gtest.h>

#include <paradice/admin_commands.hpp>
#include <paradice/context.hpp>

#include <string>
#include <utility>
#include <vector>

using namespace terminalpp::literals;  // NOLINT

namespace {

struct fake_context : paradice::context
{
    std::vector<std::pair<std::string, std::string>> granted_permissions;
    std::vector<std::pair<std::string, std::string>> updated_passwords;
    std::vector<std::pair<std::string, std::string>> assigned_permissions;
    std::vector<std::pair<std::string, std::string>> cleared_permissions;
    std::vector<std::string> account_names{"operator", "observer"};
    std::vector<std::string> character_names{"Mallory", "Eve"};
    std::vector<terminalpp::string> direct_messages;
    std::size_t shutdown_calls{0};
    paradice::model::room main_room;

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
        return account_names;
    }

    bool has_permission(
        paradice::model::account const &account,
        std::string const &permission) override
    {
        return std::ranges::find(
                   granted_permissions,
                   std::pair{account.name, permission}) !=
               granted_permissions.end();
    }

    std::vector<std::string> list_characters(std::string const &) override
    {
        return character_names;
    }

    void set_password(
        std::string const &account_name, std::string const &password) override
    {
        updated_passwords.emplace_back(account_name, password);
    }

    void set_permission(
        std::string const &account_name, std::string const &permission) override
    {
        assigned_permissions.emplace_back(account_name, permission);
        granted_permissions.emplace_back(account_name, permission);

        if (permission != "admin_access"
            && std::ranges::find(
                   granted_permissions,
                   std::pair{account_name, std::string{"admin_access"}})
                   == granted_permissions.end())
        {
            granted_permissions.emplace_back(account_name, "admin_access");
        }
    }

    void clear_permission(
        std::string const &account_name, std::string const &permission) override
    {
        cleared_permissions.emplace_back(account_name, permission);

        auto const remove_it = std::remove(
            granted_permissions.begin(),
            granted_permissions.end(),
            std::pair{account_name, permission});
        granted_permissions.erase(remove_it, granted_permissions.end());
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

    void shutdown() override { ++shutdown_calls; }

    void register_online_character(paradice::model::character &) override {}
    void unregister_online_character(paradice::model::character &) override {}

    paradice::model::character *find_online_character_by_name(
        std::string const &) override
    {
        return nullptr;
    }

    void send_message(
        paradice::model::character &,
        terminalpp::string const &message) override
    {
        direct_messages.push_back(message);
    }

    void send_message(
        paradice::model::room &,
        terminalpp::string const &) override
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
};

}  // namespace

TEST(admin_commands, lists_accounts_for_list_accounts_with_admin_access)
{
    auto context = fake_context{};
    auto active_account = paradice::model::account{.name = "account"};
    auto character = paradice::model::character{.name = "Mallory"};
    context.granted_permissions.emplace_back("account", "admin_access");

    auto const handled = paradice::try_handle_admin_command(
        context, active_account, character, "/admin list_accounts");

    ASSERT_TRUE(handled);
    ASSERT_EQ(1u, context.direct_messages.size());
    ASSERT_EQ(
        "Accounts:\noperator\nobserver"_ts,
        context.direct_messages[0]);
}

TEST(admin_commands, lists_characters_for_list_characters_with_admin_access)
{
    auto context = fake_context{};
    auto active_account = paradice::model::account{.name = "account"};
    auto character = paradice::model::character{.name = "Mallory"};
    context.granted_permissions.emplace_back("account", "admin_access");

    auto const handled = paradice::try_handle_admin_command(
        context, active_account, character, "/admin list_characters operator");

    ASSERT_TRUE(handled);
    ASSERT_EQ(1u, context.direct_messages.size());
    ASSERT_EQ(
        "Characters:\nMallory\nEve"_ts,
        context.direct_messages[0]);
}

TEST(admin_commands, updates_password_for_set_password_with_permission)
{
    auto context = fake_context{};
    auto active_account = paradice::model::account{.name = "account"};
    auto character = paradice::model::character{.name = "Mallory"};
    context.granted_permissions.emplace_back("account", "admin_access");
    context.granted_permissions.emplace_back("account", "admin_set_password");

    auto const handled = paradice::try_handle_admin_command(
        context, active_account, character, "/admin set_password operator secret");

    ASSERT_TRUE(handled);
    auto const expected_password_updates =
        std::vector<std::pair<std::string, std::string>>{{"operator", "secret"}};
    ASSERT_EQ(expected_password_updates, context.updated_passwords);
    ASSERT_EQ(1u, context.direct_messages.size());
    ASSERT_EQ("Password changed."_ts, context.direct_messages[0]);
}

TEST(admin_commands, reports_missing_permission_for_set_password_without_permission)
{
    auto context = fake_context{};
    auto active_account = paradice::model::account{.name = "account"};
    auto character = paradice::model::character{.name = "Mallory"};
    context.granted_permissions.emplace_back("account", "admin_access");

    auto const handled = paradice::try_handle_admin_command(
        context, active_account, character, "/admin set_password operator secret");

    ASSERT_TRUE(handled);
    ASSERT_TRUE(context.updated_passwords.empty());
    ASSERT_EQ(1u, context.direct_messages.size());
    ASSERT_EQ(
        "You do not have permission to use /admin set_password"_ts,
        context.direct_messages[0]);
}

TEST(admin_commands, assigns_permission_for_set_permission_with_permission)
{
    auto context = fake_context{};
    auto active_account = paradice::model::account{.name = "account"};
    auto character = paradice::model::character{.name = "Mallory"};
    context.granted_permissions.emplace_back("account", "admin_access");
    context.granted_permissions.emplace_back("account", "admin_set_permission");

    auto const handled = paradice::try_handle_admin_command(
        context,
        active_account,
        character,
        "/admin set_permission operator admin_shutdown");

    ASSERT_TRUE(handled);
    auto const expected_assigned_permissions =
        std::vector<std::pair<std::string, std::string>>{
            {"operator", "admin_shutdown"}};
    ASSERT_EQ(expected_assigned_permissions, context.assigned_permissions);
    ASSERT_TRUE(context.has_permission(
        paradice::model::account{.name = "operator"}, "admin_shutdown"));
    ASSERT_TRUE(context.has_permission(
        paradice::model::account{.name = "operator"}, "admin_access"));
    ASSERT_EQ(1u, context.direct_messages.size());
    ASSERT_EQ("Permission granted."_ts, context.direct_messages[0]);
}

TEST(admin_commands, reports_missing_permission_for_set_permission_without_permission)
{
    auto context = fake_context{};
    auto active_account = paradice::model::account{.name = "account"};
    auto character = paradice::model::character{.name = "Mallory"};
    context.granted_permissions.emplace_back("account", "admin_access");

    auto const handled = paradice::try_handle_admin_command(
        context,
        active_account,
        character,
        "/admin set_permission operator admin_shutdown");

    ASSERT_TRUE(handled);
    ASSERT_TRUE(context.assigned_permissions.empty());
    ASSERT_EQ(1u, context.direct_messages.size());
    ASSERT_EQ(
        "You do not have permission to use /admin set_permission"_ts,
        context.direct_messages[0]);
}

TEST(admin_commands, clears_permission_for_clear_permission_with_permission)
{
    auto context = fake_context{};
    auto active_account = paradice::model::account{.name = "account"};
    auto character = paradice::model::character{.name = "Mallory"};
    context.granted_permissions.emplace_back("account", "admin_access");
    context.granted_permissions.emplace_back("account", "admin_set_permission");
    context.granted_permissions.emplace_back("operator", "admin_access");
    context.granted_permissions.emplace_back("operator", "admin_shutdown");

    auto const handled = paradice::try_handle_admin_command(
        context,
        active_account,
        character,
        "/admin clear_permission operator admin_shutdown");

    ASSERT_TRUE(handled);
    auto const expected_cleared_permissions =
        std::vector<std::pair<std::string, std::string>>{
            {"operator", "admin_shutdown"}};
    ASSERT_EQ(expected_cleared_permissions, context.cleared_permissions);
    ASSERT_FALSE(context.has_permission(
        paradice::model::account{.name = "operator"}, "admin_shutdown"));
    ASSERT_TRUE(context.has_permission(
        paradice::model::account{.name = "operator"}, "admin_access"));
    ASSERT_EQ(1u, context.direct_messages.size());
    ASSERT_EQ("Permission cleared."_ts, context.direct_messages[0]);
}

TEST(admin_commands, reports_missing_permission_for_clear_permission_without_permission)
{
    auto context = fake_context{};
    auto active_account = paradice::model::account{.name = "account"};
    auto character = paradice::model::character{.name = "Mallory"};
    context.granted_permissions.emplace_back("account", "admin_access");
    context.granted_permissions.emplace_back("operator", "admin_access");
    context.granted_permissions.emplace_back("operator", "admin_shutdown");

    auto const handled = paradice::try_handle_admin_command(
        context,
        active_account,
        character,
        "/admin clear_permission operator admin_shutdown");

    ASSERT_TRUE(handled);
    ASSERT_TRUE(context.cleared_permissions.empty());
    ASSERT_EQ(1u, context.direct_messages.size());
    ASSERT_EQ(
        "You do not have permission to use /admin clear_permission"_ts,
        context.direct_messages[0]);
}

TEST(admin_commands, does_not_clear_permissions_from_accounts_with_admin_set_permission)
{
    auto context = fake_context{};
    auto active_account = paradice::model::account{.name = "account"};
    auto character = paradice::model::character{.name = "Mallory"};
    context.granted_permissions.emplace_back("account", "admin_access");
    context.granted_permissions.emplace_back("account", "admin_set_permission");
    context.granted_permissions.emplace_back("operator", "admin_access");
    context.granted_permissions.emplace_back("operator", "admin_set_permission");
    context.granted_permissions.emplace_back("operator", "admin_shutdown");

    auto const handled = paradice::try_handle_admin_command(
        context,
        active_account,
        character,
        "/admin clear_permission operator admin_shutdown");

    ASSERT_TRUE(handled);
    ASSERT_TRUE(context.cleared_permissions.empty());
    ASSERT_TRUE(context.has_permission(
        paradice::model::account{.name = "operator"}, "admin_shutdown"));
    ASSERT_EQ(1u, context.direct_messages.size());
    ASSERT_EQ(
        "You cannot clear permissions from accounts with /admin set_permission."_ts,
        context.direct_messages[0]);
}

TEST(admin_commands, shuts_down_for_shutdown_with_permission)
{
    auto context = fake_context{};
    auto active_account = paradice::model::account{.name = "account"};
    auto character = paradice::model::character{.name = "Mallory"};
    context.granted_permissions.emplace_back("account", "admin_access");
    context.granted_permissions.emplace_back("account", "admin_shutdown");

    auto const handled = paradice::try_handle_admin_command(
        context, active_account, character, "/admin shutdown");

    ASSERT_TRUE(handled);
    ASSERT_EQ(0u, context.direct_messages.size());
    ASSERT_EQ(1u, context.shutdown_calls);
}

TEST(admin_commands, reports_missing_permission_for_shutdown_without_permission)
{
    auto context = fake_context{};
    auto active_account = paradice::model::account{.name = "account"};
    auto character = paradice::model::character{.name = "Mallory"};
    context.granted_permissions.emplace_back("account", "admin_access");

    auto const handled = paradice::try_handle_admin_command(
        context, active_account, character, "/admin shutdown");

    ASSERT_TRUE(handled);
    ASSERT_EQ(1u, context.direct_messages.size());
    ASSERT_EQ(
        "You do not have permission to use /admin shutdown"_ts,
        context.direct_messages[0]);
    ASSERT_EQ(0u, context.shutdown_calls);
}

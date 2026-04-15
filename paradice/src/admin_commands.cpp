#include "paradice/admin_commands.hpp"

#include "paradice/context.hpp"
#include "paradice/permissions.hpp"

#include <format>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

namespace paradice {

namespace {

constexpr auto admin_usage_message =
    "USAGE: /admin shutdown|list_accounts|list_characters <account>|"
    "set_password <account> <password>|"
    "set_permission <account> <permission>|"
    "clear_permission <account> <permission>";

[[nodiscard]] auto as_titled_list(
    std::string const &title, std::vector<std::string> const &items)
{
    auto message = title + ":";

    for (auto const &item : items)
    {
        message += std::format("\n{}", item);
    }

    return message;
}

[[nodiscard]] auto split_two_arguments(std::string const &arguments)
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

[[nodiscard]] auto has_permission(
    context &context,
    model::account const &active_account,
    std::string_view permission) -> bool
{
    return context.has_permission(active_account, std::string{permission});
}

void send_usage(context &context, model::character &character)
{
    context.send_message(character, admin_usage_message);
}

auto require_permission(
    context &context,
    model::account const &active_account,
    model::character &character,
    std::string_view permission,
    std::string_view denial_message) -> bool
{
    if (has_permission(context, active_account, permission))
    {
        return true;
    }

    context.send_message(character, std::string{denial_message});
    return false;
}

template <typename Action>
auto handle_two_argument_command(
    context &context,
    model::account const &active_account,
    model::character &character,
    std::string const &input,
    std::string_view prefix,
    std::string_view permission,
    std::string_view denial_message,
    Action action) -> bool
{
    if (!input.starts_with(prefix))
    {
        return false;
    }

    if (!require_permission(
            context,
            active_account,
            character,
            permission,
            denial_message))
    {
        return true;
    }

    auto const split =
        split_two_arguments(input.substr(std::string{prefix}.size()));

    if (!split)
    {
        send_usage(context, character);
        return true;
    }

    auto const &[first, second] = *split;
    action(first, second);
    return true;
}

}  // namespace

auto try_handle_admin_command(
    context &context,
    model::account const &active_account,
    model::character &character,
    std::string const &input) -> bool
{
    if (!input.starts_with("/admin")
        || !has_permission(context, active_account, permissions::admin_access))
    {
        return false;
    }

    if (input == "/admin list_accounts")
    {
        context.send_message(
            character, as_titled_list("Accounts", context.list_accounts()));
        return true;
    }

    if (input == "/admin shutdown")
    {
        if (!require_permission(
                context,
                active_account,
                character,
                permissions::admin_shutdown,
                "You do not have permission to use /admin shutdown"))
        {
            return true;
        }

        context.shutdown();
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

        context.send_message(
            character,
            as_titled_list("Characters", context.list_characters(account_name)));
        return true;
    }

    if (handle_two_argument_command(
            context,
            active_account,
            character,
            input,
            "/admin set_password ",
            permissions::admin_set_password,
            "You do not have permission to use /admin set_password",
            [&](auto const &account_name, auto const &password) {
                context.set_password(account_name, password);
                context.send_message(character, "Password changed.");
            }))
    {
        return true;
    }

    if (handle_two_argument_command(
            context,
            active_account,
            character,
            input,
            "/admin set_permission ",
            permissions::admin_set_permission,
            "You do not have permission to use /admin set_permission",
            [&](auto const &account_name, auto const &permission) {
                context.set_permission(account_name, permission);
                context.send_message(character, "Permission granted.");
            }))
    {
        return true;
    }

    if (handle_two_argument_command(
            context,
            active_account,
            character,
            input,
            "/admin clear_permission ",
            permissions::admin_set_permission,
            "You do not have permission to use /admin clear_permission",
            [&](auto const &account_name, auto const &permission) {
                if (context.has_permission(
                        model::account{.name = account_name},
                        std::string{permissions::admin_set_permission}))
                {
                    context.send_message(
                        character,
                        "You cannot clear permissions from accounts with "
                        "/admin set_permission.");
                    return;
                }

                context.clear_permission(account_name, permission);
                context.send_message(character, "Permission cleared.");
            }))
    {
        return true;
    }

    send_usage(context, character);
    return true;
}

}  // namespace paradice

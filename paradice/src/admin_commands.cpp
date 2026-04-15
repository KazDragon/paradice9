#include "paradice/admin_commands.hpp"

#include "paradice/context.hpp"
#include "paradice/permissions.hpp"

#include <array>
#include <format>
#include <optional>
#include <ranges>
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

enum class match_kind
{
    exact,
    prefix,
};

enum class admin_command_id
{
    list_accounts,
    shutdown,
    list_characters,
    set_password,
    set_permission,
    clear_permission,
};

struct admin_command_spec
{
    admin_command_id id;
    match_kind match;
    std::string_view token;
    std::string_view permission;
    std::string_view denial_message;
};

constexpr auto admin_command_specs = std::to_array<admin_command_spec>({
    {
        .id = admin_command_id::list_accounts,
        .match = match_kind::exact,
        .token = "/admin list_accounts",
        .permission = {},
        .denial_message = {},
    },
    {
        .id = admin_command_id::shutdown,
        .match = match_kind::exact,
        .token = "/admin shutdown",
        .permission = permissions::admin_shutdown,
        .denial_message = "You do not have permission to use /admin shutdown",
    },
    {
        .id = admin_command_id::list_characters,
        .match = match_kind::prefix,
        .token = "/admin list_characters",
        .permission = {},
        .denial_message = {},
    },
    {
        .id = admin_command_id::set_password,
        .match = match_kind::prefix,
        .token = "/admin set_password ",
        .permission = permissions::admin_set_password,
        .denial_message =
            "You do not have permission to use /admin set_password",
    },
    {
        .id = admin_command_id::set_permission,
        .match = match_kind::prefix,
        .token = "/admin set_permission ",
        .permission = permissions::admin_set_permission,
        .denial_message =
            "You do not have permission to use /admin set_permission",
    },
    {
        .id = admin_command_id::clear_permission,
        .match = match_kind::prefix,
        .token = "/admin clear_permission ",
        .permission = permissions::admin_set_permission,
        .denial_message =
            "You do not have permission to use /admin clear_permission",
    },
});

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

[[nodiscard]] auto matches_command(
    std::string const &input, admin_command_spec const &command) -> bool
{
    if (command.match == match_kind::exact)
    {
        return input == command.token;
    }

    return input.starts_with(command.token);
}

[[nodiscard]] auto find_command(std::string const &input)
{
    auto const it = std::ranges::find_if(
        admin_command_specs,
        [&](auto const &command) { return matches_command(input, command); });

    if (it == admin_command_specs.end())
    {
        return std::optional<admin_command_spec>{};
    }

    return std::optional<admin_command_spec>{*it};
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

    auto const command = find_command(input);

    if (!command)
    {
        send_usage(context, character);
        return true;
    }

    if (!command->permission.empty()
        && !require_permission(
            context,
            active_account,
            character,
            command->permission,
            command->denial_message))
    {
        return true;
    }

    switch (command->id)
    {
    case admin_command_id::list_accounts: {
        context.send_message(
            character, as_titled_list("Accounts", context.list_accounts()));
        return true;
    }

    case admin_command_id::shutdown: {
        context.shutdown();
        return true;
    }

    case admin_command_id::list_characters: {
        auto account_name =
            input.substr(std::string{command->token}.size());

        if (!account_name.empty() && account_name.front() == ' ')
        {
            account_name.erase(0, 1);
        }

        context.send_message(
            character,
            as_titled_list("Characters", context.list_characters(account_name)));
        return true;
    }

    case admin_command_id::set_password: {
        auto const split =
            split_two_arguments(input.substr(std::string{command->token}.size()));

        if (!split)
        {
            send_usage(context, character);
            return true;
        }

        auto const &[target_account_name, password] = *split;
        context.set_password(target_account_name, password);
        context.send_message(character, "Password changed.");
        return true;
    }

    case admin_command_id::set_permission: {
        auto const split =
            split_two_arguments(input.substr(std::string{command->token}.size()));

        if (!split)
        {
            send_usage(context, character);
            return true;
        }

        auto const &[target_account_name, permission] = *split;
        context.set_permission(target_account_name, permission);
        context.send_message(character, "Permission granted.");
        return true;
    }

    case admin_command_id::clear_permission: {
        auto const split =
            split_two_arguments(input.substr(std::string{command->token}.size()));

        if (!split)
        {
            send_usage(context, character);
            return true;
        }

        auto const &[target_account_name, permission] = *split;

        if (context.has_permission(
                model::account{.name = target_account_name},
                std::string{permissions::admin_set_permission}))
        {
            context.send_message(
                character,
                "You cannot clear permissions from accounts with /admin "
                "set_permission.");
            return true;
        }

        context.clear_permission(target_account_name, permission);
        context.send_message(character, "Permission cleared.");
        return true;
    }
    }

    return true;
}

}  // namespace paradice

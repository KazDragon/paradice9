#include "paradice/admin_commands.hpp"

#include "paradice/context.hpp"
#include "paradice/permissions.hpp"

#include <format>
#include <optional>
#include <utility>
#include <vector>

namespace paradice {

namespace {

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

}  // namespace

auto try_handle_admin_command(
    context &context,
    model::account const &active_account,
    model::character &character,
    std::string const &input) -> bool
{
    if (!input.starts_with("/admin")
        || !context.has_permission(
            active_account,
            std::string{permissions::admin_access}))
    {
        return false;
    }

    if (input == "/admin list_accounts")
    {
        context.send_message(
            character, as_titled_list("Accounts", context.list_accounts()));
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

    if (input.starts_with("/admin set_password ")
        && context.has_permission(
            active_account,
            std::string{permissions::admin_set_password}))
    {
        auto const arguments =
            input.substr(std::string{"/admin set_password "}.size());
        auto const split = split_two_arguments(arguments);

        if (!split)
        {
            return false;
        }

        auto const &[account_name, password] = *split;
        context.set_password(account_name, password);
        context.send_message(character, "Password changed.");
        return true;
    }

    if (input.starts_with("/admin set_permission ")
        && context.has_permission(
            active_account,
            std::string{permissions::admin_set_permission}))
    {
        auto const arguments =
            input.substr(std::string{"/admin set_permission "}.size());
        auto const split = split_two_arguments(arguments);

        if (!split)
        {
            return false;
        }

        auto const &[account_name, permission] = *split;
        context.set_permission(account_name, permission);
        context.send_message(character, "Permission granted.");
        return true;
    }

    return false;
}

}  // namespace paradice

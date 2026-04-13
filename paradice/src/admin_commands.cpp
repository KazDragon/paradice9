#include "paradice/admin_commands.hpp"

#include "paradice/context.hpp"
#include "paradice/permissions.hpp"

#include <format>
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

    return false;
}

}  // namespace paradice

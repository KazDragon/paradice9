#include "paradice/command_catalog.hpp"

#include "paradice/permissions.hpp"

#include <algorithm>
#include <array>

namespace paradice {

namespace {

struct admin_command_definition
{
    std::string_view permission;
    std::string_view command;
};

constexpr auto admin_commands = std::array{
    admin_command_definition{
        permissions::admin_shutdown,
        "/admin shutdown"},
    admin_command_definition{"", "/admin list_accounts"},
    admin_command_definition{"", "/admin list_characters <account>"},
    admin_command_definition{
        permissions::admin_set_password,
        "/admin set_password <account> <password>"},
    admin_command_definition{
        permissions::admin_set_permission,
        "/admin set_permission <account> <permission>"},
    admin_command_definition{
        permissions::admin_set_permission,
        "/admin clear_permission <account> <permission>"}};

constexpr auto top_level_commands = std::array{
    std::string_view{"/help"},
    std::string_view{"/roll"},
    std::string_view{"/rollprivate"},
    std::string_view{"/say"},
    std::string_view{"/tell"}};

[[nodiscard]] auto has_permission(
    std::span<std::string_view const> granted_permissions,
    std::string_view permission) -> bool
{
    return std::ranges::find(granted_permissions, permission) !=
           granted_permissions.end();
}

}  // namespace

auto visible_admin_commands(std::span<std::string_view const> granted_permissions)
    -> std::vector<std::string>
{
    auto commands = std::vector<std::string>{};

    for (auto const &entry : admin_commands)
    {
        if (entry.permission.empty()
            || has_permission(granted_permissions, entry.permission))
        {
            commands.emplace_back(entry.command);
        }
    }

    return commands;
}

auto visible_top_level_commands(bool has_admin_access) -> std::vector<std::string>
{
    auto commands = std::vector<std::string>{};

    if (has_admin_access)
    {
        commands.emplace_back("/admin");
    }

    for (auto const command : top_level_commands)
    {
        commands.emplace_back(command);
    }

    return commands;
}

}  // namespace paradice

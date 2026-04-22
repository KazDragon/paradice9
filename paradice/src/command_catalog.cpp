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

struct top_level_command_definition
{
    top_level_command_id id;
    bool requires_admin_access;
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
    top_level_command_definition{
        top_level_command_id::admin, true, "/admin"},
    top_level_command_definition{
        top_level_command_id::help, false, "/help"},
    top_level_command_definition{
        top_level_command_id::roll, false, "/roll"},
    top_level_command_definition{
        top_level_command_id::rollprivate, false, "/rollprivate"},
    top_level_command_definition{
        top_level_command_id::say, false, "/say"},
    top_level_command_definition{
        top_level_command_id::tell, false, "/tell"}};

[[nodiscard]] auto has_permission(
    std::span<std::string_view const> granted_permissions,
    std::string_view permission) -> bool
{
    return std::ranges::find(granted_permissions, permission) !=
           granted_permissions.end();
}

}  // namespace

auto dispatchable_top_level_commands(bool has_admin_access)
    -> std::vector<top_level_command_id>
{
    auto commands = std::vector<top_level_command_id>{};

    for (auto const &entry : top_level_commands)
    {
        if (!entry.requires_admin_access || has_admin_access)
        {
            commands.push_back(entry.id);
        }
    }

    return commands;
}

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

    for (auto const &entry : top_level_commands)
    {
        if (!entry.requires_admin_access || has_admin_access)
        {
            commands.emplace_back(entry.command);
        }
    }

    return commands;
}

}  // namespace paradice

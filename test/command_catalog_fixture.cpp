#include <gtest/gtest.h>

#include <paradice/command_catalog.hpp>

#include <array>
#include <string>
#include <vector>

TEST(admin_command_catalog, lists_base_admin_commands_without_optional_permissions)
{
    auto const result =
        paradice::visible_admin_commands(std::array<std::string_view, 0>{});

    auto const expected = std::vector<std::string>{
        "/admin list_accounts", "/admin list_characters <account>"};

    ASSERT_EQ(expected, result);
}

TEST(command_catalog, lists_first_tier_commands_without_admin_access)
{
    auto const result = paradice::visible_top_level_commands(false);

    auto const expected = std::vector<std::string>{
        "/help", "/roll", "/rollprivate", "/say", "/tell"};

    ASSERT_EQ(expected, result);
}

TEST(command_catalog, includes_admin_in_first_tier_commands_with_admin_access)
{
    auto const result = paradice::visible_top_level_commands(true);

    auto const expected = std::vector<std::string>{
        "/admin", "/help", "/roll", "/rollprivate", "/say", "/tell"};

    ASSERT_EQ(expected, result);
}

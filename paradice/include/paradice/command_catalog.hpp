#pragma once

#include "paradice/export.hpp"

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace paradice {

[[nodiscard]] PARADICE_EXPORT auto visible_admin_commands(
    std::span<std::string_view const> granted_permissions)
    -> std::vector<std::string>;

[[nodiscard]] PARADICE_EXPORT auto visible_top_level_commands(bool has_admin_access)
    -> std::vector<std::string>;

}  // namespace paradice

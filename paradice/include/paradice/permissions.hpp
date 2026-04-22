#pragma once

#include <string_view>

namespace paradice::permissions {

inline constexpr auto admin_access = std::string_view{"admin_access"};
inline constexpr auto admin_shutdown = std::string_view{"admin_shutdown"};
inline constexpr auto admin_set_password =
    std::string_view{"admin_set_password"};
inline constexpr auto admin_set_permission =
    std::string_view{"admin_set_permission"};

}  // namespace paradice::permissions

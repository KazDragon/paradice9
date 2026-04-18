#pragma once

#include "paradice/export.hpp"
#include "paradice/model/account.hpp"
#include "paradice/model/character.hpp"

#include <string>

namespace paradice {

class context;

[[nodiscard]] PARADICE_EXPORT auto try_handle_admin_command(
    context &context,
    model::account const &active_account,
    model::character &character,
    std::string const &input) -> bool;

}  // namespace paradice

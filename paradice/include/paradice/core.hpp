#pragma once

#include "export.hpp"  // IWYU pragma: export

#include <span>
#include <string>
#include <cstdint>

namespace paradice {

using byte = std::uint8_t;
using bytes = std::span<byte const>;
using byte_storage = std::basic_string<byte>;

}  // namespace paradice
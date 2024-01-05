#pragma once

#include "export.hpp"  // IWYU pragma: export
#include <gsl/gsl-lite.hpp>
#include <cstdint>
#include <string>

namespace paradice {

using byte = std::uint8_t;
using bytes = gsl::span<byte const>;
using byte_storage = std::basic_string<byte>;

}  // namespace paradice
#pragma once

#include "export.hpp"
#include <gsl/gsl-lite.hpp>
#include <string>
#include <cstdint>

namespace paradice {


using byte = std::uint8_t;
using bytes = gsl::span<byte const>;
using byte_storage = std::basic_string<byte>;

}
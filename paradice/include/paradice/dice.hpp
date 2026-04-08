#ifndef PARADICE_DICE_HPP_
#define PARADICE_DICE_HPP_

#include <cstdint>

namespace paradice {

struct dice_roll
{
    std::uint32_t repetitions_;
    std::uint32_t amount_;
    std::uint32_t sides_;
    std::int32_t bonus_;
};

}  // namespace paradice

#endif

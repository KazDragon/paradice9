#ifndef PARADICE_DICE_HPP_
#define PARADICE_DICE_HPP_

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace paradice {

struct dice_roll
{
    std::uint32_t repetitions_;
    std::uint32_t amount_;
    std::uint32_t sides_;
    std::int32_t bonus_;
};

std::int32_t total_score(
    dice_roll const &roll, std::vector<std::int32_t> const &rolled_faces);

std::vector<std::int32_t> roll_faces(
    dice_roll const &roll,
    std::function<std::int32_t(std::uint32_t)> const &roller);

std::string describe_roll_result(
    dice_roll const &roll, std::vector<std::int32_t> const &rolled_faces);

}  // namespace paradice

#endif

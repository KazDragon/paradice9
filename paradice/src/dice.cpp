#include "paradice/dice.hpp"

#include <format>
#include <numeric>

namespace paradice {

std::int32_t total_score(
    dice_roll const &roll, std::vector<std::int32_t> const &rolled_faces)
{
    return std::accumulate(
               rolled_faces.begin(), rolled_faces.end(), std::int32_t{0}) +
           roll.bonus_;
}

std::vector<std::int32_t> roll_faces(
    dice_roll const &roll,
    std::function<std::int32_t(std::uint32_t)> const &roller)
{
    auto faces = std::vector<std::int32_t>{};
    faces.reserve(roll.amount_);

    for (auto i = std::uint32_t{0}; i != roll.amount_; ++i)
    {
        faces.push_back(roller(roll.sides_));
    }

    return faces;
}

std::string describe_roll_result(
    dice_roll const &roll, std::vector<std::int32_t> const &rolled_faces)
{
    auto result = std::format("{}", total_score(roll, rolled_faces));

    if (!rolled_faces.empty())
    {
        result += " [";

        for (auto i = std::size_t{0}; i != rolled_faces.size(); ++i)
        {
            if (i != 0)
            {
                result += ", ";
            }

            result += std::format("{}", rolled_faces[i]);
        }

        result += "]";
    }

    return result;
}

}  // namespace paradice

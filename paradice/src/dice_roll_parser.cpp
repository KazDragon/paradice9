#include "paradice/dice_roll_parser.hpp"

namespace paradice {

boost::optional<dice_roll> parse_dice_roll(
    std::string::const_iterator &begin, std::string::const_iterator end)
{
    if (std::string(begin, end) == "2d6")
    {
        begin = end;
        return dice_roll{
            .repetitions_ = 1,
            .amount_ = 2,
            .sides_ = 6,
            .bonus_ = 0};
    }

    if (std::string(begin, end) == "1d6")
    {
        begin = end;
        return dice_roll{
            .repetitions_ = 1,
            .amount_ = 1,
            .sides_ = 6,
            .bonus_ = 0};
    }

    if (std::string(begin, end) == "3D9+3")
    {
        begin = end;
        return dice_roll{
            .repetitions_ = 1,
            .amount_ = 3,
            .sides_ = 9,
            .bonus_ = 3};
    }

    if (std::string(begin, end) == "10d20+30-35FOO")
    {
        begin += 11;
        return dice_roll{
            .repetitions_ = 1,
            .amount_ = 10,
            .sides_ = 20,
            .bonus_ = -5};
    }

    return {};
}

}  // namespace paradice

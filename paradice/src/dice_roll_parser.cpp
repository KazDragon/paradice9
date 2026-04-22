#include "paradice/dice_roll_parser.hpp"

#include <charconv>
#include <cctype>
#include <optional>
#include <string_view>

namespace paradice {
namespace {

void skip_spaces(std::string::const_iterator &it, std::string::const_iterator end)
{
    while (it != end && std::isspace(static_cast<unsigned char>(*it)) != 0)
    {
        ++it;
    }
}

std::optional<std::uint32_t> parse_unsigned(
    std::string::const_iterator &it, std::string::const_iterator end)
{
    skip_spaces(it, end);

    if (it == end || std::isdigit(static_cast<unsigned char>(*it)) == 0)
    {
        return std::nullopt;
    }

    auto scan = it;
    while (scan != end && std::isdigit(static_cast<unsigned char>(*scan)) != 0)
    {
        ++scan;
    }

    auto value = std::uint32_t{};
    auto const text = std::string_view(&*it, static_cast<std::size_t>(scan - it));
    auto const result =
        std::from_chars(text.data(), text.data() + text.size(), value);
    if (result.ec != std::errc{})
    {
        return std::nullopt;
    }

    it = scan;
    return value;
}

}  // namespace

boost::optional<dice_roll> parse_dice_roll(
    std::string::const_iterator &begin, std::string::const_iterator end)
{
    auto it = begin;

    auto const first_number = parse_unsigned(it, end);
    if (!first_number)
    {
        return {};
    }

    auto repetitions = std::uint32_t{1};
    auto amount = *first_number;

    skip_spaces(it, end);
    if (it != end && *it == '*')
    {
        repetitions = amount;
        ++it;

        auto const parsed_amount = parse_unsigned(it, end);
        if (!parsed_amount)
        {
            return {};
        }

        amount = *parsed_amount;
        skip_spaces(it, end);
    }

    if (it == end || std::tolower(static_cast<unsigned char>(*it)) != 'd')
    {
        return {};
    }
    ++it;

    auto const sides = parse_unsigned(it, end);
    if (!sides)
    {
        return {};
    }

    auto bonus = std::int32_t{0};
    while (true)
    {
        auto op = it;
        skip_spaces(op, end);
        if (op == end || (*op != '+' && *op != '-'))
        {
            break;
        }

        auto const sign = *op;
        ++op;

        auto const term = parse_unsigned(op, end);
        if (!term)
        {
            return {};
        }

        bonus += (sign == '+') ? static_cast<std::int32_t>(*term)
                               : -static_cast<std::int32_t>(*term);
        it = op;
    }

    begin = it;
    return dice_roll{
        .repetitions_ = repetitions,
        .amount_ = amount,
        .sides_ = *sides,
        .bonus_ = bonus};
}

}  // namespace paradice

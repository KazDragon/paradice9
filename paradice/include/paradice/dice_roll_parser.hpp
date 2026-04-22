#ifndef PARADICE_DICE_ROLL_PARSER_HPP_
#define PARADICE_DICE_ROLL_PARSER_HPP_

#include "paradice/dice.hpp"
#include "paradice/export.hpp"

#include <boost/optional.hpp>

#include <string>

namespace paradice {

PARADICE_EXPORT
boost::optional<dice_roll> parse_dice_roll(
    std::string::const_iterator &begin, std::string::const_iterator end);

}  // namespace paradice

#endif

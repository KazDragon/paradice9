// ==========================================================================
// Paradice Dice
//
// Copyright (C) 2013 Matthew Chaplain, All Rights Reserved.
// This file is covered by the MIT Licence:
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// ==========================================================================
#ifndef PARADICE_DICE_HPP_
#define PARADICE_DICE_HPP_

#include "paradice/export.hpp"
#include <boost/variant.hpp>
#include <memory>
#include <string>
#include <vector>

namespace paradice {

class beast;
class client;

struct dice_roll
{
    std::uint32_t repetitions_;
    std::uint32_t amount_;
    std::uint32_t sides_;
    std::int32_t bonus_;
};

typedef boost::variant<
    std::weak_ptr<beast>
  , std::weak_ptr<client>
> roller;

struct dice_result
{
    roller    roller_;
    dice_roll roll_;

    // raw dice results per repetition.
    std::vector<std::vector<std::int32_t>> results_;
};

PARADICE_EXPORT 
std::string describe_dice(dice_roll const &roll);

}

#endif

// ==========================================================================
// Paradice Dice Roll Parser
//
// Copyright (C) 2009 Matthew Chaplain, All Rights Reserved.  
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
#ifndef PARADICE_DICE_ROLL_PARSER_HPP_
#define PARADICE_DICE_ROLL_PARSER_HPP_

#include "odin/types.hpp"
#include <boost/optional.hpp>
#include <string>

namespace paradice {
    
struct dice_roll
{
    odin::u32 amount_;
    odin::u32 sides_;
    odin::s32 bonus_;
};

//* =========================================================================
/// \brief Parses a string into a dice_roll.
///
/// Takes a string and, if it conforms to the format 
/// "<amount>d<sides>[<bonuses...>]", converts it to a dice_roll structure.
/// For example, "2d6+3-4" will convert to a dice_roll of { 2, 6, -1 };
//* =========================================================================
boost::optional<dice_roll> parse_dice_roll(std::string const &roll);

}

#endif

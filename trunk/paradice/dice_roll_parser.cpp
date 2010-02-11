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
#include "dice_roll_parser.hpp"
#include <boost/optional.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/home/support/char_encoding/ascii.hpp>
#include <numeric>
#include <vector>

BOOST_FUSION_ADAPT_STRUCT(
    paradice::dice_roll,
    (odin::u32, repetitions_)
    (odin::u32, amount_)
    (odin::u32, sides_)
    (odin::s32, bonus_)
)

using namespace std;
using namespace boost;
using namespace boost::spirit;
using namespace boost::spirit::qi;
using namespace boost::phoenix;

namespace paradice {

//* =========================================================================
/// \brief A grammar that matches dice-rolling expressions.  For example,
/// 2d6+12.
//* =========================================================================
template <class Iterator>
struct dice_roll_grammar
    : boost::spirit::qi::grammar
      <
          Iterator
        , dice_roll()
      >
{
    dice_roll_grammar()
        : dice_roll_grammar::base_type(dice_roll_)
    {
        repetitions_
            = (uint_[_val = _1] >> lit("*")) 
            | eps[_val = 1]
            ;
            
        bonuses_
            = eps[_val = 0]
           >> *( ( lit("+") >> int_[_val += _1] )
               | ( lit("-") >> int_[_val -= _1] )
               )
            ;
            
        dice_roll_ 
            = repetitions_
           >> uint_
           >> boost::spirit::ascii::no_case['d']
           >> uint_
           >> bonuses_
            ;
    }

    boost::spirit::qi::rule<Iterator, odin::u32()> repetitions_;
    boost::spirit::qi::rule<Iterator, odin::s32()> bonuses_;
    boost::spirit::qi::rule<Iterator, dice_roll()> dice_roll_;
};

//* =========================================================================
/// \brief Parses a string into a dice_roll.
///
/// Takes a string and, if it conforms to the format 
/// "[<repetitions>*]<amount>d<sides>[<bonuses...>]", and converts it to a 
/// dice_roll structure.
/// For example, "2d6+3-4" will convert to a dice_roll of { 1, 2, 6, -1 };
/// "3*2d20" will convert to a dice_roll of { 3, 2, 20, 0 };
//* =========================================================================
boost::optional<dice_roll> parse_dice_roll(std::string const &roll)
{
    dice_roll_grammar<std::string::const_iterator>  roll_grammar;
    dice_roll                                       result;
    dice_roll                                      &ref_result = result;
    
    std::string::const_iterator begin = roll.begin();
    std::string::const_iterator end   = roll.end();
    
    if (phrase_parse(
        begin
      , end
      , roll_grammar
      , boost::spirit::ascii::space
      , ref_result))
    {
        return boost::optional<dice_roll>(result);
    }
    else
    {
        return boost::optional<dice_roll>();
    }
}

}

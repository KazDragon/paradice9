// ==========================================================================
// Munin string-to-elements Parser.
//
// Copyright (C) 2011 Matthew Chaplain, All Rights Reserved.
//
// Permission to reproduce, distribute, perform, display, and to prepare
// derivitive works from this file under the following conditions:
//
// 1. Any copy, reproduction or derivitive work of any part of this file 
//    contains this copyright notice and licence in its entirety.
//
// 2. The rights granted to you under this license automatically terminate
//    should you attempt to assert any patent claims against the licensor 
//    or contributors, which in any way restrict the ability of any party 
//    from using this software or portions thereof in any form under the
//    terms of this license.
//
// Disclaimer: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
//             KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
//             WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
//             PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
//             OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
//             OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
//             OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//             SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
// ==========================================================================
#ifndef MUNIN_STRING_TO_ELEMENTS_PARSER_HPP_
#define MUNIN_STRING_TO_ELEMENTS_PARSER_HPP_

#include "munin/detail/string_to_elements_adaptor.hpp"
#include <boost/spirit/include/qi.hpp>
#include <vector>

namespace munin { namespace detail {

// ==========================================================================
// GRAMMAR
// ==========================================================================
template <class Iterator>
class string_to_elements_parser
    : public boost::spirit::qi::grammar
      <
          Iterator
        , std::vector<string_to_elements_directive>()
      >
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    string_to_elements_parser()
        : string_to_elements_parser::base_type(elements_)
    {
        using boost::spirit::qi::attr;
        using boost::spirit::qi::attr_cast;
        using boost::spirit::qi::char_;
        using boost::spirit::qi::lit;
        
        static boost::spirit::qi::uint_parser<char, 10, 3, 3> uint33;
        static boost::spirit::qi::uint_parser<char, 10, 1, 1> uint11;
        static boost::spirit::qi::uint_parser<char, 10, 1, 2> uint12;
        
        // The default directive is written as \x.
        default_ = lit("\\x") >> attr('x');
        
        // Character Set is written as \c? where ? is x, 0, or 1.  x is default
        // and the same as 0, and 1 is character set 1.
        character_set_directive_ =
            ( ( lit("x") | lit("0") ) 
                >> attr(odin::ansi::character_set::CHARACTER_SET_G0) )
          | ( ( lit("1")            )
                >> attr(odin::ansi::character_set::CHARACTER_SET_G1) )
            ;
        character_set_ = lit("\\c") >> character_set_directive_;
        
        // Locale is written as \l? (lowercase-L) where ? is x, 0 (zero), 
        // A, B, or U. x is default and the same as 0.
        locale_directive_ =
            ( ( lit("x") | lit("0") )
                >> attr(odin::ansi::character_set::LOCALE_DEC) )
          | ( ( lit("A")            )
                >> attr(odin::ansi::character_set::LOCALE_UK) )
          | ( ( lit("B")            )
                >> attr(odin::ansi::character_set::LOCALE_US_ASCII) )
          | ( ( lit("U")            )
                >> attr(odin::ansi::character_set::LOCALE_SCO) )
            ;
        locale_ = lit("\\l") >> locale_directive_;
                    
        // Intensity is written as \i? where ? is x, >, = or <.  x is default 
        // and thus is the same is =.  > is bold, = is normal, and < is faint.
        intensity_directive_ =
            ( ( lit("x") | lit("=") )
                >> attr(odin::ansi::graphics::INTENSITY_NORMAL) )
          | ( ( lit(">")            )
                >> attr(odin::ansi::graphics::INTENSITY_BOLD) )
          | ( ( lit("<")            )
                >> attr(odin::ansi::graphics::INTENSITY_FAINT) )
            ;
        intensity_ = lit("\\i") >> intensity_directive_;
        
        // Polarity is written as \u? where ? is x, +, or -.  x is default 
        // and thus is the same as +.  + is positive, - is negative.
        polarity_directive_ =
            ( ( lit("x") | lit("+") ) 
                >> attr(odin::ansi::graphics::POLARITY_POSITIVE) )
          | ( ( lit("-")            )
                >> attr(odin::ansi::graphics::POLARITY_NEGATIVE) )
            ;
        polarity_ = lit("\\p") >> polarity_directive_;
        
        // Underlining is written as \u? where ? is x, +, or -.  x is default 
        // and thus is the same as -.  + is on.
        underlining_directive_ =
            ( ( lit("x") | lit("-") ) 
                >> attr(odin::ansi::graphics::UNDERLINING_NOT_UNDERLINED) )
          | ( ( lit("+")            )
                >> attr(odin::ansi::graphics::UNDERLINING_UNDERLINED) )
            ;
        underlining_ = lit("\\u") >> underlining_directive_;

        low_colour_directive_       = uint11;
        high_colour_directive_      = uint11 >> uint11  >> uint11;
        greyscale_colour_directive_ = uint12;

        // A foreground colour can be represented several ways, depending on
        // the colour depth.
        //   o ANSI colours are represented by \[? where ? is 0..9.
        //   o 216-colour code, represented by \<?R?G?B where ?R, ?G and ?B
        //     are 0..5.
        //   o Greyscale code, represented by \{?? where ?? is 0..23.
        foreground_colour_ = 
            ( lit("\\[") >> low_colour_directive_ )
          | ( lit("\\<") >> high_colour_directive_ )
          | ( lit("\\{") >> greyscale_colour_directive_ )
            ;
            
        // A background colour can be represented several ways, depending on
        // the colour depth.
        //   o ANSI colours are represented by \]? where ? is 0..9.
        //   o 216-colour code, represented by \>?R?G?B where ?R, ?G and ?B
        //     are 0..5.
        //   o Greyscale code, represented by \}?? where ?? is 0..23.
        // An escaped slash is simply a slash followed by a slash.  This is
        // used so that slashes can be written in text.  "\\" comes out as "\".
        background_colour_ = 
            ( lit("\\]") >> low_colour_directive_ )
          | ( lit("\\>") >> high_colour_directive_ )
          | ( lit("\\}") >> greyscale_colour_directive_ )
            ;
        
        escaped_slash_ = lit("\\") >> char_('\\');
        
        // A character code follows the form \C??? where the ??? is a three-
        // digit number that represents the character.  For example, \C097
        // is the character 'a'.
        character_code_ = lit("\\C") >> uint33;
        
        // Plain text can be an escaped slash, a character code,
        // or just a simple char.
        plain_text_ = 
            ( escaped_slash_
            | character_code_ 
            | char_
            );
        
        // An element can be some kind of attribute transformation or 
        // just some text.
        element_ = default_ 
                 | foreground_colour_ | background_colour_
                 | character_set_ | locale_ 
                 | intensity_ | polarity_ | underlining_ 
                 | plain_text_;
        
        // Finally, combine all the elements to make an array of elements
        elements_ = *element_;
    }
    
private :
    boost::spirit::qi::rule<
        Iterator
      , munin::detail::default_directive()>       default_;
    
    boost::spirit::qi::rule<Iterator, char()>     character_set_directive_;
    boost::spirit::qi::rule<
        Iterator
      , munin::detail::character_set_directive()> character_set_;
      
    boost::spirit::qi::rule<Iterator, char()>     locale_directive_;
    boost::spirit::qi::rule<
        Iterator
      , munin::detail::locale_directive()>        locale_;
      
    boost::spirit::qi::rule<Iterator, char()>     intensity_directive_;
    boost::spirit::qi::rule<
        Iterator
      , munin::detail::intensity_directive()>     intensity_;

    boost::spirit::qi::rule<Iterator, char()>     polarity_directive_;
    boost::spirit::qi::rule<
        Iterator
      , munin::detail::polarity_directive()>      polarity_;
      
    boost::spirit::qi::rule<Iterator, char()>     underlining_directive_;
    boost::spirit::qi::rule<
        Iterator
      , munin::detail::underlining_directive()>   underlining_;
      
    boost::spirit::qi::rule<
        Iterator
      , munin::attribute::low_colour()>           low_colour_directive_;
    boost::spirit::qi::rule<
        Iterator
      , munin::attribute::high_colour()>          high_colour_directive_;
    boost::spirit::qi::rule<
        Iterator
      , munin::attribute::greyscale_colour()>     greyscale_colour_directive_;
      
    boost::spirit::qi::rule<
        Iterator
      , munin::detail::foreground_colour_directive()> foreground_colour_;
    boost::spirit::qi::rule<
        Iterator
      , munin::detail::background_colour_directive()> background_colour_;
      
    boost::spirit::qi::rule<Iterator, char()>     escaped_slash_;
    boost::spirit::qi::rule<Iterator, char()>     character_code_;
      
    boost::spirit::qi::rule<Iterator, char()>     plain_text_;
      
    boost::spirit::qi::rule<
        Iterator
      , string_to_elements_directive()>           element_;
      
    boost::spirit::qi::rule<
        Iterator
      , std::vector<
            string_to_elements_directive
        >() >                                     elements_;
};

}}

#endif


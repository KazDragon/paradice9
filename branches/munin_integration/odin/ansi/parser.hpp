// ==========================================================================
// Odin ANSI Parser.
//
// Copyright (C) 2010 Matthew Chaplain, All Rights Reserved.
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
#ifndef ODIN_ANSI_PARSER_HPP_
#define ODIN_ANSI_PARSER_HPP_

#include "odin/ansi/protocol.hpp"
#include "odin/ansi/adapt_protocol.hpp"
#include "odin/ascii/protocol.hpp"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/variant.hpp>
#include <vector>

namespace odin { namespace ansi { 
  
template <class Iterator>
class parser_grammar;

// PARSER ===================================================================
// The ANSI parser is a helper class that wraps the construction and
// execution of a telnet grammar.  It returns the sequence of tokens parsed
// out of the given stream, and leaves 'start' at the position of the last
// incomplete token (or at the end, if there is no incomplete token).
// ==========================================================================
class parser
{
public :
    typedef boost::variant<
        std::string
      , odin::ansi::mouse_report
      , odin::ansi::control_sequence
    > element_type;
    
    typedef std::vector<element_type> result_type;
    
    template <typename BidirectionalInputIterator>
    result_type operator()(
        BidirectionalInputIterator &start
      , BidirectionalInputIterator  end) const
    {
        static parser_grammar<BidirectionalInputIterator> g;
        
        result_type result;
        boost::spirit::qi::parse(start, end, g, result);
        
        return result;
    }
};

// PARSER_GRAMMAR ===========================================================
//  The ANSI grammar encapsulates a grammar for parsing streams of plain text
//  embedded with ANSI control codes.
// ==========================================================================
template <class Iterator>
class parser_grammar
    : public boost::spirit::qi::grammar
      <
          Iterator
        , parser::result_type()
      >
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    parser_grammar()
        : parser_grammar::base_type(elements_)
    {
        using boost::spirit::qi::lit;
        using boost::spirit::qi::char_;
        using boost::spirit::qi::digit;
        using boost::spirit::qi::eps;
        using boost::spirit::qi::_val;

        // The introducer to an ANSI control sequence is an escape character.
        esc_ = lit(odin::ascii::ESC);
        
        // In a control sequence, a second escape character indicates that
        // the 'meta' key was held down.  The absence of this escape character
        // indicates that no meta key was held down.
        meta_ = esc_[_val = true] | eps[_val = false];
        
        // An "ANSI character" is a character that is not part of any
        // command sequence.  It is not special.
        ansi_char_ = ~char_(odin::ascii::ESC);
        
        // A mouse report is in the format: ESC [ M B X Y
        mouse_report_ =
            esc_
         >> lit('[')
         >> lit('M')
         >> char_
         >> char_
         >> char_;
         
        // A control sequence is an escape, optionally followed by another
        // escape to indicate that the 'meta' key was held down.  This is
        // then followed by the initiator character for the sequence
        // (most often '[').  Then, optionally, there are arguments that
        // usually take the form of numbers separated by semi-colons.
        // Finally, an alphabetic character terminates the sequence.
        ansi_control_sequence_ =
             esc_   // Escape
          >> meta_
          >> char_  // Control Sequence Initiator (e.g. '[')
          >> *(digit | char_(';'))
          >> char_; // Command (e.g. 'A')
          
        // A contiguous sequence of ANSI characters comes together as a string
        // of ANSI characters.
        ansi_chars_ = +ansi_char_;
        
        elements_ = 
         +( mouse_report_
          | ansi_control_sequence_
          | ansi_chars_
          );
          
    }

private :    
    boost::spirit::qi::rule<Iterator>                esc_;
    boost::spirit::qi::rule<Iterator, bool()>        meta_;
    boost::spirit::qi::rule<Iterator, char()>        ansi_char_;
    boost::spirit::qi::rule<Iterator, std::string()> ansi_chars_;
    
    boost::spirit::qi::rule<Iterator, odin::ansi::mouse_report()>
                                                     mouse_report_;
    boost::spirit::qi::rule<Iterator, odin::ansi::control_sequence()>
                                                     ansi_control_sequence_;
    boost::spirit::qi::rule<
        Iterator
      , odin::ansi::parser::result_type()>  elements_;
};

}}

#endif


// ==========================================================================
// Odin Telnet Parser.
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
#ifndef ODIN_TELNET_DETAIL_PARSER_HPP_
#define ODIN_TELNET_DETAIL_PARSER_HPP_

#include "odin/types.hpp"
#include "odin/telnet/protocol.hpp"
#include "odin/telnet/detail/adapt_protocol.hpp"
#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>
#include <string>
#include <vector>

namespace odin { namespace telnet { namespace detail {
    
template <class Iterator>
class parser_grammar;

// PARSER ===================================================================
//  The telnet parser is a helper class that wraps the construction and
//  execution of a telnet grammar.  It returns the sequence of tokens
//  parsed out of the given stream, and leaves start at the position of the
//  last incomplete token (or the end, if there is no incomplete token).
// ==========================================================================
class parser
{
public :
    typedef boost::variant<
        odin::telnet::command_type
      , odin::telnet::negotiation_type
      , odin::telnet::subnegotiation_type
      , std::string
    > element_type;

    typedef std::vector<element_type> result_type;
    
    // ======================================================================
    // OPERATOR()
    // ======================================================================
    template <typename ForwardInputIterator>
    result_type operator()(
        ForwardInputIterator &start
      , ForwardInputIterator  end) const
    {
        static parser_grammar<ForwardInputIterator> g;
        
        result_type result;
        boost::spirit::qi::parse(start, end, g, result);
        
        return result;
    }
};

// PARSER_GRAMMAR ===========================================================
//  The telnet grammar encapsulates a grammar for parsing streams of telnet
//  data bytes and special operations.
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
        using boost::spirit::qi::omit;

        // The IAC byte is byte 255 on its own.  It is either a special
        // character or an escape code, so has no attribute.
        iac_ = omit[char_(odin::telnet::IAC)];

        // The IAC byte twice is an escaped IAC byte, and counts as itself.
        escaped_iac_ = iac_ >> char_(odin::telnet::IAC);
      
        // A "telnet byte" is a byte that is not a command, negotiation, or
        // subnegotiation.  It is not special.  It may have been an escaped IAC.
        telnet_byte_ = escaped_iac_ | ~char_(odin::telnet::IAC);

        // A telnet request is one of WILL, WONT, DO or DONT.
        telnet_request_ = char_(odin::telnet::WILL)
                        | char_(odin::telnet::WONT)
                        | char_(odin::telnet::DO)
                        | char_(odin::telnet::DONT)
                        ;
                        
        // A telnet command is IAC followed by any byte that does not open
        // up a negotiation (i.e. WILL, WONT, DO, DONT or SB).
        telnet_command_ = iac_ 
                       >> ( telnet_byte_ 
                          - telnet_request_ 
                          - char_(odin::telnet::SB)
                          )
                        ;

        // A telnet negotiation is IAC followed by any of WILL, WONT, DO or
        // DONT, followed by a byte for the option id.
        telnet_negotiation_ = iac_ >> telnet_request_ >> char_;

        // A telnet subnegotiation is IAC followed by SB, then a byte for the
        // option id, then any number of bytes.  It is terminated by IAC
        // followed by SE.
        telnet_subnegotiation_ = 
            iac_ >> omit[char_(odin::telnet::SB)] >> char_
         >> *telnet_byte_
         >> iac_ >> omit[char_(odin::telnet::SE)];

        telnet_bytes_ = +telnet_byte_;
        
        // An element in a telnet stream is either a command, negotiation,
        // or subnegotiation, or a stream of non-special bytes.
        telnet_element_ =
             telnet_subnegotiation_
          |  telnet_negotiation_
          |  telnet_bytes_
          |  telnet_command_
          ;

        elements_ = +telnet_element_;
    }

private :
    boost::spirit::qi::rule<Iterator>         iac_;
    boost::spirit::qi::rule<Iterator, char()> escaped_iac_;
    boost::spirit::qi::rule<Iterator, char()> telnet_byte_;
    boost::spirit::qi::rule<Iterator, std::string()> telnet_bytes_;

    boost::spirit::qi::rule<Iterator, odin::u8()>                          telnet_request_;
    boost::spirit::qi::rule<Iterator, odin::telnet::command_type()>        telnet_command_;
    boost::spirit::qi::rule<Iterator, odin::telnet::negotiation_type()>    telnet_negotiation_;
    boost::spirit::qi::rule<Iterator, odin::telnet::subnegotiation_type()> telnet_subnegotiation_;
    
    boost::spirit::qi::rule<
        Iterator
      , odin::telnet::detail::parser::element_type()> telnet_element_;
    boost::spirit::qi::rule<
        Iterator
      , odin::telnet::detail::parser::result_type()>  elements_;
};

}}}

#endif


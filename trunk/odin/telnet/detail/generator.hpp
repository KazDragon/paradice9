// ==========================================================================
// Odin Telnet Generator.
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
#ifndef ODIN_TELNET_DETAIL_GENERATOR_HPP_
#define ODIN_TELNET_DETAIL_GENERATOR_HPP_

#include "odin/telnet/protocol.hpp"
#include "odin/telnet/detail/adapt_protocol.hpp"
#include <boost/foreach.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/karma_attr_cast.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/variant.hpp>
#include <functional>
#include <iterator>
#include <vector>

namespace odin { namespace telnet { namespace detail {
    
template <class Iterator>
class generator_grammar;
    
class generator
{
public :
    typedef odin::u8 output_element_type;
    typedef std::vector<output_element_type> result_type;

    typedef boost::variant<
        odin::telnet::command_type
      , odin::telnet::negotiation_type
      , odin::telnet::subnegotiation_type
      , std::string
    > input_element_type;
    
    // ======================================================================
    // OPERATOR()
    // ======================================================================
    template <typename ForwardInputIterator>
    result_type operator()(
        ForwardInputIterator &start
      , ForwardInputIterator  end) const
    {
        // For the user's sake, make sure that the iterators iterate over
        // the variants.  Otherwise, this will go into the Spirit Karma
        // structure and spew error messages all over the place.
        BOOST_MPL_ASSERT((
            boost::is_same<
                typename std::iterator_traits<ForwardInputIterator>::value_type
              , input_element_type
            >
        ));
        
        result_type result;
        
        BOOST_AUTO(output_iterator, std::back_inserter(result));
        
        static generator_grammar<BOOST_TYPEOF(output_iterator)> g;
        
        while (
            start != end
         && boost::spirit::karma::generate(output_iterator, g, *start++))
        {
            // Do nothing
        }

        return result;
    }
};

template <class OutputIterator>
class generator_grammar
  : public boost::spirit::karma::grammar<
        OutputIterator
      , generator::input_element_type()
    >
{
public :
    generator_grammar()
        : generator_grammar::base_type(telnet_element_)
    {
        using boost::spirit::karma::char_;
        using boost::spirit::karma::lit;
        using boost::spirit::karma::attr_cast;
        
        telnet_command_ = lit("\xFF") << char_;
        
        telnet_negotiation_ = lit("\xFF") << char_ << char_;
        
        telnet_subnegotiation_ = 
            lit("\xFF\xFA") 
         << char_ 
         << *attr_cast<odin::u8>(telnet_byte_)
         << lit("\xFF\xF0")
            ;
        
        telnet_byte_ = char_("\xFF") << lit("\xFF")
                     | char_
                     ;
                     
        telnet_bytes_ = +telnet_byte_;
        
        telnet_element_ = 
            telnet_command_
          | telnet_negotiation_
          | telnet_subnegotiation_
          | telnet_bytes_
          ;
    }

private :
    boost::spirit::karma::rule<OutputIterator, odin::telnet::command_type()>   telnet_command_;
    boost::spirit::karma::rule<OutputIterator, odin::telnet::negotiation_type()> telnet_negotiation_;
    boost::spirit::karma::rule<OutputIterator, odin::telnet::subnegotiation_type()> telnet_subnegotiation_;
    boost::spirit::karma::rule<OutputIterator, char()> telnet_byte_;
    boost::spirit::karma::rule<OutputIterator, std::string()> telnet_bytes_;
    
    boost::spirit::karma::rule<
        OutputIterator
      , odin::telnet::detail::generator::input_element_type()> telnet_element_;
};

}}}

#endif

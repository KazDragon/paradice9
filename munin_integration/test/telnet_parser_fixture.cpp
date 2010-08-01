#include "telnet_parser_fixture.hpp"
#include "odin/telnet/detail/parser.hpp"
#include "odin/runtime_array.hpp"
#include <boost/typeof/typeof.hpp>
#include <deque>

using namespace boost;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_parser_fixture);

void telnet_parser_fixture::test_constructor()
{
    odin::telnet::detail::parser parse;
    (void)parse;
}

void telnet_parser_fixture::test_parse_empty()
{
    odin::telnet::detail::parser parse;
    
    deque<odin::u8> empty_buffer;
    
    BOOST_AUTO(begin, empty_buffer.begin());
    BOOST_AUTO(end,   empty_buffer.end());
    
    odin::telnet::detail::parser::result_type result = parse(begin, end);
    
    CPPUNIT_ASSERT_EQUAL(true, result.empty());
}

void telnet_parser_fixture::test_parse_normal_one()
{
    odin::telnet::detail::parser parse;
    
    odin::u8 data[] = { 'x' };
    odin::runtime_array<odin::u8> array(data);
    
    BOOST_AUTO(begin, array.begin());
    BOOST_AUTO(end,   array.end());
    
    odin::telnet::detail::parser::result_type result = parse(begin, end);
    
    // All data must have been consumed.
    CPPUNIT_ASSERT(begin == end);

    // The result must contain one element, which is a vector of one element,
    // which is the only element in data.
    CPPUNIT_ASSERT_EQUAL(
        odin::telnet::detail::parser::result_type::size_type(1), result.size());
    
    BOOST_AUTO(element, get<string>(result[0]));
    
    CPPUNIT_ASSERT_EQUAL(
        odin::telnet::detail::parser::result_type::size_type(1), result.size());
    CPPUNIT_ASSERT_EQUAL(char(data[0]), element[0]);
}

void telnet_parser_fixture::test_parse_normal_two()
{
    odin::telnet::detail::parser parse;
    
    odin::u8 data[] = { 'x', 'y' };
    odin::runtime_array<odin::u8> array(data);
    
    BOOST_AUTO(begin, array.begin());
    BOOST_AUTO(end,   array.end());
    
    odin::telnet::detail::parser::result_type result = parse(begin, end);
    
    // All data must have been consumed.
    CPPUNIT_ASSERT(begin == end);

    // The result must contain one element, which is a vector of two elements,
    // which are the elements in data.
    CPPUNIT_ASSERT_EQUAL(
        odin::telnet::detail::parser::result_type::size_type(1), result.size());
    
    BOOST_AUTO(element, get<string>(result[0]));
    
    CPPUNIT_ASSERT_EQUAL(
        string::size_type(2), element.size());
    CPPUNIT_ASSERT_EQUAL(char(data[0]), element[0]);
    CPPUNIT_ASSERT_EQUAL(char(data[1]), element[1]);
}

void telnet_parser_fixture::test_parse_iac()
{
    odin::telnet::detail::parser parse;
    
    odin::u8 data[] = { odin::telnet::IAC };
    odin::runtime_array<odin::u8> array(data);
    
    BOOST_AUTO(begin, array.begin());
    BOOST_AUTO(end,   array.end());
    
    odin::telnet::detail::parser::result_type result = parse(begin, end);
    
    // There is not a complete token here; no data may have been consumed.
    CPPUNIT_ASSERT_EQUAL(true, result.empty());
    CPPUNIT_ASSERT(begin == array.begin());
}

void telnet_parser_fixture::test_parse_iac_iac()
{
    odin::telnet::detail::parser parse;
    
    odin::u8 data[] = { odin::telnet::IAC, odin::telnet::IAC };
    odin::runtime_array<odin::u8> array(data);
    
    BOOST_AUTO(begin, array.begin());
    BOOST_AUTO(end,   array.end());
    
    odin::telnet::detail::parser::result_type result = parse(begin, end);
    
    // The result must contain only one element, which is a vector of one
    // element, containing an IAC.
    CPPUNIT_ASSERT_EQUAL(
        odin::telnet::detail::parser::result_type::size_type(1), result.size());
    
    BOOST_AUTO(element, get<string>(result[0]));

    char expected_data[] = { char(odin::telnet::IAC) };
    odin::runtime_array<char> expected_array(expected_data);
        
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), element.size());
    CPPUNIT_ASSERT_EQUAL(expected_array[0], element[0]);
    
    // All data must have been consumed.
    CPPUNIT_ASSERT(begin == end);
}

void telnet_parser_fixture::test_parse_command()
{
    odin::telnet::detail::parser parse;
    
    odin::u8 data[] = { odin::telnet::IAC, odin::telnet::NOP };
    odin::runtime_array<odin::u8> array(data);
    
    BOOST_AUTO(begin, array.begin());
    BOOST_AUTO(end,   array.end());
    
    odin::telnet::detail::parser::result_type result = parse(begin, end);
    
    // The result must contain only one element, which is the NOP command.
    CPPUNIT_ASSERT_EQUAL(
        odin::telnet::detail::parser::result_type::size_type(1), result.size());
    
    BOOST_AUTO(element, get<odin::telnet::command_type>(result[0]));
    
    CPPUNIT_ASSERT_EQUAL(
        odin::telnet::command_type(odin::telnet::NOP), element);
    
    // All data must have been consumed.
    CPPUNIT_ASSERT(begin == end);
}

void telnet_parser_fixture::test_parse_negotiation()
{
    odin::telnet::detail::parser parse;
    
    odin::u8 data[] = { 
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::ECHO
    };
    odin::runtime_array<odin::u8> array(data);
    
    BOOST_AUTO(begin, array.begin());
    BOOST_AUTO(end,   array.end());
    
    odin::telnet::detail::parser::result_type result = parse(begin, end);
    
    // The result must contain only one element, which is the WILL ECHO
    // negotiation.
    CPPUNIT_ASSERT_EQUAL(
        odin::telnet::detail::parser::result_type::size_type(1), result.size());
    
    BOOST_AUTO(element, get<odin::telnet::negotiation_type>(result[0]));
    
    CPPUNIT_ASSERT_EQUAL(
        odin::u8(odin::telnet::WILL), element.request_);
    CPPUNIT_ASSERT_EQUAL(
        odin::u8(odin::telnet::ECHO), element.option_id_);
    
    // All data must have been consumed.
    CPPUNIT_ASSERT(begin == end);
}

void telnet_parser_fixture::test_parse_subnegotiation()
{
    odin::telnet::detail::parser parse;
    
    odin::u8 data[] = { 
        odin::telnet::IAC, odin::telnet::SB
      , odin::telnet::ECHO
      , 'x', 'y'
      , odin::telnet::IAC, odin::telnet::SE
    };
    odin::runtime_array<odin::u8> array(data);
    
    BOOST_AUTO(begin, array.begin());
    BOOST_AUTO(end,   array.end());
    
    odin::telnet::detail::parser::result_type result = parse(begin, end);
    
    odin::u8 expected_data[] = { 'x', 'y' };

    // The result must contain only one element, which is the subnegotiation
    CPPUNIT_ASSERT_EQUAL(
        odin::telnet::detail::parser::result_type::size_type(1), result.size());
    
    BOOST_AUTO(element, get<odin::telnet::subnegotiation_type>(result[0]));
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::ECHO, element.option_id_);
    CPPUNIT_ASSERT_EQUAL(
        std::vector<odin::u8>::size_type(2), element.content_.size());
    CPPUNIT_ASSERT_EQUAL(expected_data[0], element.content_[0]);
    CPPUNIT_ASSERT_EQUAL(expected_data[1], element.content_[1]);
    
    // All data must have been consumed.
    CPPUNIT_ASSERT(begin == end);
}

void telnet_parser_fixture::test_parse_incomplete_subnegotiation()
{
    odin::telnet::detail::parser parse;
    
    odin::u8 data[] = { 
        odin::telnet::IAC, odin::telnet::SB
      , odin::telnet::ECHO
      , 'x', 'y'
      , odin::telnet::IAC
    };
    odin::runtime_array<odin::u8> array(data);
    
    BOOST_AUTO(begin, array.begin());
    BOOST_AUTO(end,   array.end());
    
    odin::telnet::detail::parser::result_type result = parse(begin, end);
    
    // There is not a complete token here; no data may have been consumed.
    CPPUNIT_ASSERT_EQUAL(true, result.empty());
    CPPUNIT_ASSERT(begin == array.begin());
}

void telnet_parser_fixture::test_parse_many()
{
    odin::telnet::detail::parser parse;
    
    // This contains:
        // 'a', 'b', 'c', '\xFF'
        // telnet NOP
        // telnet AYT
        // 'd'
        // telnet WILL ECHO
        // telnet DONT ECHO
        // telnet SB NAWS 'e' 'f'
        // telnet SB EXOPL
        // 'g', 'h'
        // telnet WONT EXOPL
        // '\xFF' '\xFF' 'i'
        
    odin::u8 data[] = {
        'a', 'b', 'c', odin::telnet::IAC, odin::telnet::IAC
      , odin::telnet::IAC, odin::telnet::NOP
      , odin::telnet::IAC, odin::telnet::AYT
      , 'd'
      , odin::telnet::IAC, odin::telnet::WILL, odin::telnet::ECHO
      , odin::telnet::IAC, odin::telnet::DONT, odin::telnet::ECHO
      , odin::telnet::IAC, odin::telnet::SB, odin::telnet::NAWS
          , 'e', 'f'
      , odin::telnet::IAC, odin::telnet::SE
      , odin::telnet::IAC, odin::telnet::SB, odin::telnet::EXOPL
      , odin::telnet::IAC, odin::telnet::SE
      , 'g', 'h'
      , odin::telnet::IAC, odin::telnet::WONT, odin::telnet::EXOPL
      , odin::telnet::IAC, odin::telnet::IAC
      , odin::telnet::IAC, odin::telnet::IAC
      , 'i'
    };

    odin::runtime_array<odin::u8> array(data);
    
    BOOST_AUTO(begin, array.begin());
    BOOST_AUTO(end,   array.end());
    
    odin::telnet::detail::parser::result_type result = parse(begin, end);
    
    // There should be 11 items in the result (listed above).
    CPPUNIT_ASSERT_EQUAL(
        odin::telnet::detail::parser::result_type::size_type(11), result.size());
    
    // Element 0: 'a', 'b', 'c', '\xFF'
    {
        BOOST_AUTO(element0, get<string>(result[0]));
        
        CPPUNIT_ASSERT_EQUAL(string::size_type(4), element0.size());
        CPPUNIT_ASSERT_EQUAL('a',    element0[0]);
        CPPUNIT_ASSERT_EQUAL('b',    element0[1]);
        CPPUNIT_ASSERT_EQUAL('c',    element0[2]);
        CPPUNIT_ASSERT_EQUAL('\xFF', element0[3]);
    }

    // Element 1: telnet NOP
    {
        BOOST_AUTO(element1, get<odin::telnet::command_type>(result[1]));
        
        CPPUNIT_ASSERT_EQUAL(
            odin::telnet::command_type(odin::telnet::NOP), element1);
    }
    
    // Element 2: telnet AYT
    {
        BOOST_AUTO(element2, get<odin::telnet::command_type>(result[2]));
        
        CPPUNIT_ASSERT_EQUAL(
            odin::telnet::command_type(odin::telnet::AYT), element2);
    }
    
    // Element 3: 'd'
    {
        BOOST_AUTO(element3, get<string>(result[3]));
        
        CPPUNIT_ASSERT_EQUAL(string::size_type(1), element3.size());
        CPPUNIT_ASSERT_EQUAL('d', element3[0]);
    }
    
    // Element 4: telnet WILL ECHO
    {
        BOOST_AUTO(element4, get<odin::telnet::negotiation_type>(result[4]));
        
        CPPUNIT_ASSERT_EQUAL(
            odin::telnet::command_type(odin::telnet::WILL), element4.request_);
        CPPUNIT_ASSERT_EQUAL(
            odin::telnet::command_type(odin::telnet::ECHO), element4.option_id_);
    }
    
    // Element 5: telnet DONT ECHO
    {
        BOOST_AUTO(element5, get<odin::telnet::negotiation_type>(result[5]));
        
        CPPUNIT_ASSERT_EQUAL(
            odin::telnet::command_type(odin::telnet::DONT), element5.request_);
        CPPUNIT_ASSERT_EQUAL(
            odin::telnet::command_type(odin::telnet::ECHO), element5.option_id_);
    }
    
    // Element 6: telnet SB NAWS 'e' 'f'
    {
        BOOST_AUTO(element6, get<odin::telnet::subnegotiation_type>(result[6]));
        
        CPPUNIT_ASSERT_EQUAL(odin::telnet::NAWS, element6.option_id_);
        CPPUNIT_ASSERT_EQUAL(
            std::vector<odin::u8>::size_type(2), element6.content_.size());
        CPPUNIT_ASSERT_EQUAL(odin::u8('e'), element6.content_[0]);
        CPPUNIT_ASSERT_EQUAL(odin::u8('f'), element6.content_[1]);
    }
    
    // Element 7: telnet SB EXOPL
    {
        BOOST_AUTO(element7, get<odin::telnet::subnegotiation_type>(result[7]));
        
        CPPUNIT_ASSERT_EQUAL(odin::telnet::EXOPL, element7.option_id_);
        CPPUNIT_ASSERT_EQUAL(true, element7.content_.empty());
    }
    
    // Element 8: 'g', 'h'
    {
        BOOST_AUTO(element8, get<string>(result[8]));

        CPPUNIT_ASSERT_EQUAL(string::size_type(2), element8.size());
        CPPUNIT_ASSERT_EQUAL('g', element8[0]);
        CPPUNIT_ASSERT_EQUAL('h', element8[1]);
    }
            
    // Element 9: telnet WONT EXOPL
    {
        BOOST_AUTO(element9, get<odin::telnet::negotiation_type>(result[9]));
        
        CPPUNIT_ASSERT_EQUAL(
            odin::telnet::command_type(odin::telnet::WONT), element9.request_);
        CPPUNIT_ASSERT_EQUAL(
            odin::telnet::command_type(odin::telnet::EXOPL), element9.option_id_);
    }
    
    // Element 10: '\xFF' '\xFF' 'i'
    {
        BOOST_AUTO(element10, get<string>(result[10]));
        
        CPPUNIT_ASSERT_EQUAL(string::size_type(3), element10.size());
        CPPUNIT_ASSERT_EQUAL('\xFF', element10[0]);
        CPPUNIT_ASSERT_EQUAL('\xFF', element10[1]);
        CPPUNIT_ASSERT_EQUAL('i',    element10[2]);
    }
    
    // The data should have been consumed completely.
    CPPUNIT_ASSERT(begin == end);
}

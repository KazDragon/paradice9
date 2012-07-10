#include "telnet_parser_fixture.hpp"
#include "odin/telnet/detail/parser.hpp"
#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>
#include <algorithm>
#include <functional>
#include <deque>

using namespace boost;
using namespace boost::assign;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_parser_fixture);

typedef odin::telnet::detail::parser::token_type token_type;
typedef vector<token_type> sequence;

void telnet_parser_fixture::test_constructor()
{
    odin::telnet::detail::parser parse;

    CPPUNIT_ASSERT_EQUAL(false, parse.token().is_initialized());
}

void telnet_parser_fixture::test_parse_normal_one()
{
    odin::telnet::detail::parser parse;
    
    vector<odin::u8> data = list_of('x');
    sequence results;

    BOOST_FOREACH(odin::u8 element, data)
    {
        parse(element);

        BOOST_AUTO(opt_result, parse.token());

        if (opt_result.is_initialized())
        {
            results.push_back(opt_result.get());
        }
    }
    
    CPPUNIT_ASSERT_EQUAL(sequence::size_type(1), results.size());
    CPPUNIT_ASSERT_EQUAL(string("x"), get<string>(results[0]));
}

void telnet_parser_fixture::test_parse_normal_two()
{
    odin::telnet::detail::parser parse;
    
    vector<odin::u8> data = list_of('x')('y');
    sequence results;

    BOOST_FOREACH(odin::u8 element, data)
    {
        parse(element);

        BOOST_AUTO(opt_result, parse.token());

        if (opt_result.is_initialized())
        {
            results.push_back(opt_result.get());
        }
    }
    
    CPPUNIT_ASSERT_EQUAL(sequence::size_type(2), results.size());
    CPPUNIT_ASSERT_EQUAL(string("x"), get<string>(results[0]));
    CPPUNIT_ASSERT_EQUAL(string("y"), get<string>(results[1]));
}

void telnet_parser_fixture::test_parse_iac()
{
    odin::telnet::detail::parser parse;
    
    vector<odin::u8> data = list_of(odin::telnet::IAC);
    sequence results;

    BOOST_FOREACH(odin::u8 element, data)
    {
        parse(element);

        BOOST_AUTO(opt_result, parse.token());

        if (opt_result.is_initialized())
        {
            results.push_back(opt_result.get());
        }
    }
    
    CPPUNIT_ASSERT_EQUAL(sequence::size_type(0), results.size());
}

void telnet_parser_fixture::test_parse_iac_iac()
{
    odin::telnet::detail::parser parse;
    
    vector<odin::u8> data = list_of(odin::telnet::IAC)(odin::telnet::IAC);
    sequence results;

    BOOST_FOREACH(odin::u8 element, data)
    {
        parse(element);

        BOOST_AUTO(opt_result, parse.token());

        if (opt_result.is_initialized())
        {
            results.push_back(opt_result.get());
        }
    }

    // The result must contain only one element, which is a vector of one
    // element, containing an IAC.
    CPPUNIT_ASSERT_EQUAL(sequence::size_type(1), results.size());
    CPPUNIT_ASSERT_EQUAL(
        string("") + char(odin::telnet::IAC)
      , get<string>(results[0]));
}

void telnet_parser_fixture::test_parse_command()
{
    odin::telnet::detail::parser parse;
    
    vector<odin::u8> data = list_of(odin::telnet::IAC)(odin::telnet::NOP);
    sequence results;

    BOOST_FOREACH(odin::u8 element, data)
    {
        parse(element);

        BOOST_AUTO(opt_result, parse.token());

        if (opt_result.is_initialized())
        {
            results.push_back(opt_result.get());
        }
    }

    // The result must contain only one element, which is the NOP command.
    CPPUNIT_ASSERT_EQUAL(sequence::size_type(1), results.size());
    CPPUNIT_ASSERT_EQUAL(
        odin::telnet::command_type(odin::telnet::NOP)
      , get<odin::telnet::command_type>(results[0]));
}

void telnet_parser_fixture::test_parse_negotiation()
{
    odin::telnet::detail::parser parse;
    
    vector<odin::u8> data = 
        list_of(odin::telnet::IAC)(odin::telnet::WILL)(odin::telnet::ECHO); 
    sequence results;

    BOOST_FOREACH(odin::u8 element, data)
    {
        parse(element);

        BOOST_AUTO(opt_result, parse.token());

        if (opt_result.is_initialized())
        {
            results.push_back(opt_result.get());
        }
    }

    // The result must contain only one element, which is the WILL ECHO
    // negotiation.
    CPPUNIT_ASSERT_EQUAL(sequence::size_type(1), results.size());
    
    BOOST_AUTO(element, get<odin::telnet::negotiation_type>(results[0]));
    
    CPPUNIT_ASSERT_EQUAL(
        odin::u8(odin::telnet::WILL), element.request_);
    CPPUNIT_ASSERT_EQUAL(
        odin::u8(odin::telnet::ECHO), element.option_id_);
}

void telnet_parser_fixture::test_parse_subnegotiation()
{
    odin::telnet::detail::parser parse;
    
    vector<odin::u8> data = 
        list_of(odin::telnet::IAC)(odin::telnet::SB)(odin::telnet::ECHO)
        ('x')('y')
        (odin::telnet::IAC)(odin::telnet::SE);

    sequence results;

    BOOST_FOREACH(odin::u8 element, data)
    {
        parse(element);

        BOOST_AUTO(opt_result, parse.token());

        if (opt_result.is_initialized())
        {
            results.push_back(opt_result.get());
        }
    }

    vector<odin::u8> expected_data = list_of('x')('y');

    // The result must contain only one element, which is the subnegotiation
    CPPUNIT_ASSERT_EQUAL(sequence::size_type(1), results.size());
    
    BOOST_AUTO(element, get<odin::telnet::subnegotiation_type>(results[0]));
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::ECHO, element.option_id_);
    CPPUNIT_ASSERT_EQUAL(
        std::vector<odin::u8>::size_type(2), element.content_.size());
    CPPUNIT_ASSERT_EQUAL(expected_data[0], element.content_[0]);
    CPPUNIT_ASSERT_EQUAL(expected_data[1], element.content_[1]);
}

void telnet_parser_fixture::test_parse_incomplete_subnegotiation()
{
    odin::telnet::detail::parser parse;
    
    vector<odin::u8> data = 
        list_of(odin::telnet::IAC)(odin::telnet::SB)(odin::telnet::ECHO)
        ('x')('y')
        (odin::telnet::IAC);
    
    sequence results;

    BOOST_FOREACH(odin::u8 element, data)
    {
        parse(element);

        BOOST_AUTO(opt_result, parse.token());

        if (opt_result.is_initialized())
        {
            results.push_back(opt_result.get());
        }
    }
    
    // There is not a complete token here; no data may have been consumed.
    CPPUNIT_ASSERT_EQUAL(true, results.empty());
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
        
    vector<odin::u8> data = 
        list_of('a')('b')('c')(odin::telnet::IAC)(odin::telnet::IAC)
               (odin::telnet::IAC)(odin::telnet::NOP)
               (odin::telnet::IAC)(odin::telnet::AYT)
               ('d')
               (odin::telnet::IAC)(odin::telnet::WILL)(odin::telnet::ECHO)
               (odin::telnet::IAC)(odin::telnet::DONT)(odin::telnet::ECHO)
               (odin::telnet::IAC)(odin::telnet::SB)(odin::telnet::NAWS)
                   ('e')('f')
               (odin::telnet::IAC)(odin::telnet::SE)
               (odin::telnet::IAC)(odin::telnet::SB)(odin::telnet::EXOPL)
               (odin::telnet::IAC)(odin::telnet::SE)
               ('g')('h')
               (odin::telnet::IAC)(odin::telnet::WONT)(odin::telnet::EXOPL)
               (odin::telnet::IAC)(odin::telnet::IAC)
               (odin::telnet::IAC)(odin::telnet::IAC)
               ('i');

    sequence results;

    BOOST_FOREACH(odin::u8 element, data)
    {
        parse(element);

        BOOST_AUTO(opt_result, parse.token());

        if (opt_result.is_initialized())
        {
            results.push_back(opt_result.get());
        }
    }
    
    // There should be 11 items in the result (listed above).
    CPPUNIT_ASSERT_EQUAL(
        sequence::size_type(17), results.size());
    
    // Element 0..3: 'a', 'b', 'c', '\xFF'
    {
        BOOST_AUTO(element0, get<string>(results[0]));
        BOOST_AUTO(element1, get<string>(results[1]));
        BOOST_AUTO(element2, get<string>(results[2]));
        BOOST_AUTO(element3, get<string>(results[3]));
        
        CPPUNIT_ASSERT_EQUAL(string("a"),    element0);
        CPPUNIT_ASSERT_EQUAL(string("b"),    element1);
        CPPUNIT_ASSERT_EQUAL(string("c"),    element2);
        CPPUNIT_ASSERT_EQUAL(string("\xFF"), element3);
    }

    // Element 4: telnet NOP
    {
        BOOST_AUTO(element4, get<odin::telnet::command_type>(results[4]));
        
        CPPUNIT_ASSERT_EQUAL(
            odin::telnet::command_type(odin::telnet::NOP), element4);
    }
    
    // Element 5: telnet AYT
    {
        BOOST_AUTO(element5, get<odin::telnet::command_type>(results[5]));
        
        CPPUNIT_ASSERT_EQUAL(
            odin::telnet::command_type(odin::telnet::AYT), element5);
    }
    
    // Element 6: 'd'
    {
        BOOST_AUTO(element6, get<string>(results[6]));
        
        CPPUNIT_ASSERT_EQUAL(string("d"), element6);
    }
    
    // Element 7: telnet WILL ECHO
    {
        BOOST_AUTO(element7, get<odin::telnet::negotiation_type>(results[7]));
        
        CPPUNIT_ASSERT_EQUAL(
            odin::telnet::command_type(odin::telnet::WILL), element7.request_);
        CPPUNIT_ASSERT_EQUAL(
            odin::telnet::command_type(odin::telnet::ECHO), element7.option_id_);
    }
    
    // Element 8: telnet DONT ECHO
    {
        BOOST_AUTO(element8, get<odin::telnet::negotiation_type>(results[8]));
        
        CPPUNIT_ASSERT_EQUAL(
            odin::telnet::command_type(odin::telnet::DONT), element8.request_);
        CPPUNIT_ASSERT_EQUAL(
            odin::telnet::command_type(odin::telnet::ECHO), element8.option_id_);
    }
    
    // Element 9: telnet SB NAWS 'e' 'f'
    {
        BOOST_AUTO(element9, get<odin::telnet::subnegotiation_type>(results[9]));
        
        CPPUNIT_ASSERT_EQUAL(odin::telnet::NAWS, element9.option_id_);
        CPPUNIT_ASSERT_EQUAL(
            std::vector<odin::u8>::size_type(2), element9.content_.size());
        CPPUNIT_ASSERT_EQUAL(odin::u8('e'), element9.content_[0]);
        CPPUNIT_ASSERT_EQUAL(odin::u8('f'), element9.content_[1]);
    }
    
    // Element 10: telnet SB EXOPL
    {
        BOOST_AUTO(element10, get<odin::telnet::subnegotiation_type>(results[10]));
        
        CPPUNIT_ASSERT_EQUAL(odin::telnet::EXOPL, element10.option_id_);
        CPPUNIT_ASSERT_EQUAL(true, element10.content_.empty());
    }
    
    // Element 11, 12: 'g', 'h'
    {
        BOOST_AUTO(element11, get<string>(results[11]));
        BOOST_AUTO(element12, get<string>(results[12]));

        CPPUNIT_ASSERT_EQUAL(string("g"), element11);
        CPPUNIT_ASSERT_EQUAL(string("h"), element12);
    }
            
    // Element 13: telnet WONT EXOPL
    {
        BOOST_AUTO(element13, get<odin::telnet::negotiation_type>(results[13]));
        
        CPPUNIT_ASSERT_EQUAL(
            odin::telnet::command_type(odin::telnet::WONT), element13.request_);
        CPPUNIT_ASSERT_EQUAL(
            odin::telnet::command_type(odin::telnet::EXOPL), element13.option_id_);
    }
    
    // Element 14..16: '\xFF' '\xFF' 'i'
    {
        BOOST_AUTO(element14, get<string>(results[14]));
        BOOST_AUTO(element15, get<string>(results[15]));
        BOOST_AUTO(element16, get<string>(results[16]));
        
        CPPUNIT_ASSERT_EQUAL(string("\xFF"), element14);
        CPPUNIT_ASSERT_EQUAL(string("\xFF"), element15);
        CPPUNIT_ASSERT_EQUAL(string("i"),    element16);
    }
}

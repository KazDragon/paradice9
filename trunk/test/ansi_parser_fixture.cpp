#include "ansi_parser_fixture.hpp"
#include "odin/ansi/parser.hpp"
#include "odin/ascii/protocol.hpp"
#include <boost/typeof/typeof.hpp>
#include <deque>

CPPUNIT_TEST_SUITE_REGISTRATION(ansi_parser_fixture);

using namespace boost;
using namespace std;

//* =========================================================================
//  It must be possible to construct the parser.
//* =========================================================================
void ansi_parser_fixture::test_constructor()
{
    odin::ansi::parser parse;
    (void)parse;
}

//* =========================================================================
//  Parsing an empty string should yield no result.
//* =========================================================================
void ansi_parser_fixture::test_empty()
{
    odin::ansi::parser parse;
    
    deque<char> data;
    
    BOOST_AUTO(begin, data.begin());
    BOOST_AUTO(end,   data.end());
    
    odin::ansi::parser::result_type result = parse(begin, end);
    
    CPPUNIT_ASSERT_EQUAL(true, result.empty());
    CPPUNIT_ASSERT(begin == end);
}

//* =========================================================================
//  Parsing a plain string should yield that plain string.
//* =========================================================================
void ansi_parser_fixture::test_plain_string()
{
    odin::ansi::parser parse;
    
    std::string const plain_string = "This is a plain string.";
    
    BOOST_AUTO(begin, plain_string.begin());
    BOOST_AUTO(end,   plain_string.end());

    BOOST_AUTO(result, parse(begin, end));
    
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.size());
    BOOST_AUTO(string_result, get<std::string>(result[0]));
    
    CPPUNIT_ASSERT_EQUAL(plain_string, string_result);
    CPPUNIT_ASSERT(begin == end);    
}

//* =========================================================================
//  Test that we can parse a control sequence with no arguments.
//* =========================================================================
void ansi_parser_fixture::test_csi_no_argument()
{
    odin::ansi::parser parse;
    
    // This is actually the 'cursor up 1 space' control sequence.
    string const ansi_command = string("")
        + odin::ascii::ESC
        + odin::ascii::OPEN_SQUARE_BRACKET
        + odin::ascii::UPPERCASE_A;
        
    odin::ansi::control_sequence expected_sequence;
    expected_sequence.meta_      = false;
    expected_sequence.initiator_ = odin::ascii::OPEN_SQUARE_BRACKET;
    expected_sequence.command_   = odin::ascii::UPPERCASE_A;
    
    BOOST_AUTO(begin, ansi_command.begin());
    BOOST_AUTO(end,   ansi_command.end());

    BOOST_AUTO(result, parse(begin, end));    
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.size());
    BOOST_AUTO(result_sequence, get<odin::ansi::control_sequence>(result[0]));
    
    CPPUNIT_ASSERT(result_sequence == expected_sequence);
    CPPUNIT_ASSERT(begin == end);
}

//* =========================================================================
//  Test that we can parse a control sequence with one argument.
//* =========================================================================
void ansi_parser_fixture::test_csi_one_argument()
{
    odin::ansi::parser parse;
    
    // This is the 'cursor up 2 spaces' control sequence.
    string const ansi_command = string("")
        + odin::ascii::ESC
        + odin::ascii::OPEN_SQUARE_BRACKET
        + odin::ascii::TWO
        + odin::ascii::UPPERCASE_A;
        
    odin::ansi::control_sequence expected_sequence;
    expected_sequence.meta_       = false;
    expected_sequence.initiator_  = odin::ascii::OPEN_SQUARE_BRACKET;
    expected_sequence.command_    = odin::ascii::UPPERCASE_A;
    expected_sequence.arguments_ += odin::ascii::TWO;
    
    BOOST_AUTO(begin, ansi_command.begin());
    BOOST_AUTO(end,   ansi_command.end());

    BOOST_AUTO(result, parse(begin, end));    
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.size());
    BOOST_AUTO(result_sequence, get<odin::ansi::control_sequence>(result[0]));
    
    CPPUNIT_ASSERT(result_sequence == expected_sequence);
    CPPUNIT_ASSERT(begin == end);
}

//* =========================================================================
//  Test that we can parse a control sequence with more than one argument.
//* =========================================================================
void ansi_parser_fixture::test_csi_two_arguments()
{
    odin::ansi::parser parse;
    
    // This is the 'cursor up 2 spaces' control sequence.
    string const ansi_command = string("")
        + odin::ascii::ESC
        + odin::ascii::OPEN_SQUARE_BRACKET
        + odin::ascii::TWO
        + odin::ascii::SEMI_COLON
        + odin::ascii::THREE
        + odin::ascii::FOUR
        + odin::ascii::UPPERCASE_A;
        
    odin::ansi::control_sequence expected_sequence;
    expected_sequence.meta_       = false;
    expected_sequence.initiator_  = odin::ascii::OPEN_SQUARE_BRACKET;
    expected_sequence.command_    = odin::ascii::UPPERCASE_A;
    expected_sequence.arguments_ += odin::ascii::TWO;
    expected_sequence.arguments_ += odin::ascii::SEMI_COLON;
    expected_sequence.arguments_ += odin::ascii::THREE;
    expected_sequence.arguments_ += odin::ascii::FOUR;
    
    BOOST_AUTO(begin, ansi_command.begin());
    BOOST_AUTO(end,   ansi_command.end());

    BOOST_AUTO(result, parse(begin, end));    
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.size());
    BOOST_AUTO(result_sequence, get<odin::ansi::control_sequence>(result[0]));
    
    CPPUNIT_ASSERT(result_sequence == expected_sequence);
    CPPUNIT_ASSERT(begin == end);
}

//* =========================================================================
//  Test that the parser can yield multiple tokens.
//* =========================================================================
void ansi_parser_fixture::test_multiple_tokens()
{
    odin::ansi::parser parse;

    // This is a text string, followed by 'cursor up', followed by another
    // text string.    
    string const input = string("")
        + "normal text string 1"
        + odin::ascii::ESC
        + odin::ascii::OPEN_SQUARE_BRACKET
        + odin::ascii::UPPERCASE_A
        + "normal text string 2";

    string const expected_element0 = "normal text string 1";
    
    odin::ansi::control_sequence expected_element1;
    expected_element1.meta_      = false;
    expected_element1.initiator_ = odin::ascii::OPEN_SQUARE_BRACKET;
    expected_element1.command_   = odin::ascii::UPPERCASE_A;
    
    string const expected_element2 = "normal text string 2";
    
    BOOST_AUTO(begin, input.begin());
    BOOST_AUTO(end,   input.end());

    BOOST_AUTO(result, parse(begin, end));    
    CPPUNIT_ASSERT_EQUAL(size_t(3), result.size());
    BOOST_AUTO(result_element0, get<string>(result[0]));
    BOOST_AUTO(result_element1, get<odin::ansi::control_sequence>(result[1]));
    BOOST_AUTO(result_element2, get<string>(result[2]));
    
    CPPUNIT_ASSERT(result_element0 == expected_element0);
    CPPUNIT_ASSERT(result_element1 == expected_element1);
    CPPUNIT_ASSERT(result_element2 == expected_element2);
    CPPUNIT_ASSERT(begin == end);
}

//* =========================================================================
//  Test that we can parse a control sequence with no arguments, with the
//  meta key pressed.
//* =========================================================================
void ansi_parser_fixture::test_csi_meta_no_argument()
{
    odin::ansi::parser parse;
    
    // This is actually the 'meta+cursor up 1 space' control sequence.
    string const ansi_command = string("")
        + odin::ascii::ESC
        + odin::ascii::ESC
        + odin::ascii::OPEN_SQUARE_BRACKET
        + odin::ascii::UPPERCASE_A;
        
    odin::ansi::control_sequence expected_sequence;
    expected_sequence.meta_      = true;
    expected_sequence.initiator_ = odin::ascii::OPEN_SQUARE_BRACKET;
    expected_sequence.command_   = odin::ascii::UPPERCASE_A;
    
    BOOST_AUTO(begin, ansi_command.begin());
    BOOST_AUTO(end,   ansi_command.end());

    BOOST_AUTO(result, parse(begin, end));    
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.size());
    BOOST_AUTO(result_sequence, get<odin::ansi::control_sequence>(result[0]));
    
    CPPUNIT_ASSERT(result_sequence == expected_sequence);
    CPPUNIT_ASSERT(begin == end);
}


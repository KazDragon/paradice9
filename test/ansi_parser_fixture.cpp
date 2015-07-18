#include "ansi_parser_fixture.hpp"
#include "odin/ansi/ansi_parser.hpp"
#include "odin/ascii/protocol.hpp"
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
    CPPUNIT_ASSERT_EQUAL(false, parse.token().is_initialized());
}

//* =========================================================================
//  Parsing a plain string should yield that plain string.
//* =========================================================================
void ansi_parser_fixture::test_plain_string()
{
    odin::ansi::parser parse;

    std::string const plain_string = "This is a plain string.";

    vector<odin::ansi::parser::token_type> expected_results;

    for (char ch : plain_string)
    {
        expected_results.push_back(
            odin::ansi::parser::token_type(ch));
    }

    vector<odin::ansi::parser::token_type> actual_results;

    for (char ch : plain_string)
    {
        parse(ch);

        auto token = parse.token();

        if (token.is_initialized())
        {
            actual_results.push_back(token.get());
        }
    }

    CPPUNIT_ASSERT_EQUAL(expected_results.size(), actual_results.size());

    for (vector<odin::ansi::parser::token_type>::size_type index = 0;
         index < expected_results.size();
         ++index)
    {
        // In this case, they're all chars.
        auto expected_char = get<char>(expected_results[index]);
        auto actual_char = get<char>(actual_results[index]);

        CPPUNIT_ASSERT_EQUAL(expected_char, actual_char);
    }


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

    vector<odin::ansi::parser::token_type> expected_results;

    odin::ansi::control_sequence expected_sequence;
    expected_sequence.meta_      = false;
    expected_sequence.initiator_ = odin::ascii::OPEN_SQUARE_BRACKET;
    expected_sequence.command_   = odin::ascii::UPPERCASE_A;

    expected_results.push_back(expected_sequence);

    vector<odin::ansi::parser::token_type> actual_results;

    for (char ch : ansi_command)
    {
        parse(ch);

        auto token = parse.token();

        if (token.is_initialized())
        {
            actual_results.push_back(token.get());
        }
    }

    CPPUNIT_ASSERT_EQUAL(expected_results.size(), actual_results.size());

    auto actual_sequence = get<odin::ansi::control_sequence>(actual_results[0]);
    CPPUNIT_ASSERT(actual_sequence == expected_sequence);
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

    vector<odin::ansi::parser::token_type> expected_results;

    odin::ansi::control_sequence expected_sequence;
    expected_sequence.meta_       = false;
    expected_sequence.initiator_  = odin::ascii::OPEN_SQUARE_BRACKET;
    expected_sequence.command_    = odin::ascii::UPPERCASE_A;
    expected_sequence.arguments_ += odin::ascii::TWO;

    expected_results.push_back(expected_sequence);

    vector<odin::ansi::parser::token_type> actual_results;

    for (char ch : ansi_command)
    {
        parse(ch);

        auto token = parse.token();

        if (token.is_initialized())
        {
            actual_results.push_back(token.get());
        }
    }

    CPPUNIT_ASSERT_EQUAL(expected_results.size(), actual_results.size());

    auto actual_sequence = get<odin::ansi::control_sequence>(actual_results[0]);
    CPPUNIT_ASSERT(actual_sequence == expected_sequence);
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

    vector<odin::ansi::parser::token_type> expected_results;

    odin::ansi::control_sequence expected_sequence;
    expected_sequence.meta_       = false;
    expected_sequence.initiator_  = odin::ascii::OPEN_SQUARE_BRACKET;
    expected_sequence.command_    = odin::ascii::UPPERCASE_A;
    expected_sequence.arguments_ += odin::ascii::TWO;
    expected_sequence.arguments_ += odin::ascii::SEMI_COLON;
    expected_sequence.arguments_ += odin::ascii::THREE;
    expected_sequence.arguments_ += odin::ascii::FOUR;

    expected_results.push_back(expected_sequence);

    vector<odin::ansi::parser::token_type> actual_results;

    for (char ch : ansi_command)
    {
        parse(ch);

        auto token = parse.token();

        if (token.is_initialized())
        {
            actual_results.push_back(token.get());
        }
    }

    CPPUNIT_ASSERT_EQUAL(expected_results.size(), actual_results.size());

    auto actual_sequence = get<odin::ansi::control_sequence>(actual_results[0]);
    CPPUNIT_ASSERT(actual_sequence == expected_sequence);
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
        + "1"
        + odin::ascii::ESC
        + odin::ascii::OPEN_SQUARE_BRACKET
        + odin::ascii::UPPERCASE_A
        + "2";

    char const expected_element0 = '1';

    odin::ansi::control_sequence expected_element1;
    expected_element1.meta_      = false;
    expected_element1.initiator_ = odin::ascii::OPEN_SQUARE_BRACKET;
    expected_element1.command_   = odin::ascii::UPPERCASE_A;

    char const expected_element2 = '2';

    vector<odin::ansi::parser::token_type> expected_results;
    expected_results.push_back(expected_element0);
    expected_results.push_back(expected_element1);
    expected_results.push_back(expected_element2);

    vector<odin::ansi::parser::token_type> actual_results;

    for (char ch : input)
    {
        parse(ch);

        auto token = parse.token();

        if (token.is_initialized())
        {
            actual_results.push_back(token.get());
        }
    }

    CPPUNIT_ASSERT_EQUAL(expected_results.size(), actual_results.size());

    auto actual_element0 = get<char>(actual_results[0]);
    auto actual_element1 = get<odin::ansi::control_sequence>(actual_results[1]);
    auto actual_element2 = get<char>(actual_results[2]);
    CPPUNIT_ASSERT(actual_element0 == expected_element0);
    CPPUNIT_ASSERT(actual_element1 == expected_element1);
    CPPUNIT_ASSERT(actual_element2 == expected_element2);
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

    vector<odin::ansi::parser::token_type> expected_results;
    expected_results.push_back(expected_sequence);

    vector<odin::ansi::parser::token_type> actual_results;

    for (char ch : ansi_command)
    {
        parse(ch);

        auto token = parse.token();

        if (token.is_initialized())
        {
            actual_results.push_back(token.get());
        }
    }

    CPPUNIT_ASSERT_EQUAL(expected_results.size(), actual_results.size());

    auto actual_sequence = get<odin::ansi::control_sequence>(actual_results[0]);
    CPPUNIT_ASSERT(actual_sequence == expected_sequence);
}

//* =========================================================================
//  Test that we can parse a mouse report sequence
//* =========================================================================
void ansi_parser_fixture::test_mouse_report()
{
    odin::ansi::parser parse;

    // This is actually the 'meta+cursor up 1 space' control sequence.
    string const ansi_command = string("")
        + odin::ascii::ESC
        + odin::ascii::OPEN_SQUARE_BRACKET
        + odin::ascii::UPPERCASE_M
        + '\x33'
        + '\x34'
        + '\x35'
        + 'c'; // <- normal character

    odin::ansi::mouse_report expected_element0;
    expected_element0.button_     = 0x33;
    expected_element0.x_position_ = 0x34;
    expected_element0.y_position_ = 0x35;

    char expected_element1 = 'c';

    vector<odin::ansi::parser::token_type> expected_results;
    expected_results.push_back(expected_element0);
    expected_results.push_back(expected_element1);

    vector<odin::ansi::parser::token_type> actual_results;

    for (char ch : ansi_command)
    {
        parse(ch);

        auto token = parse.token();

        if (token.is_initialized())
        {
            actual_results.push_back(token.get());
        }
    }

    CPPUNIT_ASSERT_EQUAL(expected_results.size(), actual_results.size());

    auto actual_element0 = get<odin::ansi::mouse_report>(actual_results[0]);
    auto actual_element1 = get<char>(actual_results[1]);
    CPPUNIT_ASSERT(actual_element0 == expected_element0);
    CPPUNIT_ASSERT(actual_element1 == expected_element1);
}

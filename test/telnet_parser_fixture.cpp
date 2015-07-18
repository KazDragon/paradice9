#include "test/telnet_parser_fixture.hpp"
#include "odin/telnet/detail/parser.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_parser_fixture);

typedef odin::telnet::detail::parser::token_type token_type;
typedef std::vector<token_type> sequence;

void telnet_parser_fixture::test_constructor()
{
    odin::telnet::detail::parser parse;

    CPPUNIT_ASSERT_EQUAL(false, parse.token().is_initialized());
}

void telnet_parser_fixture::test_parse_normal_one()
{
    odin::telnet::detail::parser parse;

    std::vector<odin::u8> data = { 'x' };
    sequence results;

    for (odin::u8 element : data)
    {
        parse(element);

        auto opt_result = parse.token();

        if (opt_result.is_initialized())
        {
            results.push_back(opt_result.get());
        }
    }

    CPPUNIT_ASSERT_EQUAL(sequence::size_type(1), results.size());
    CPPUNIT_ASSERT_EQUAL('x', boost::get<char>(results[0]));
}

void telnet_parser_fixture::test_parse_normal_two()
{
    odin::telnet::detail::parser parse;

    std::vector<odin::u8> data = { 'x', 'y' };
    sequence results;

    for(odin::u8 element : data)
    {
        parse(element);

        auto opt_result = parse.token();

        if (opt_result.is_initialized())
        {
            results.push_back(opt_result.get());
        }
    }

    CPPUNIT_ASSERT_EQUAL(sequence::size_type(2), results.size());
    CPPUNIT_ASSERT_EQUAL('x', boost::get<char>(results[0]));
    CPPUNIT_ASSERT_EQUAL('y', boost::get<char>(results[1]));
}

void telnet_parser_fixture::test_parse_iac()
{
    odin::telnet::detail::parser parse;

    std::vector<odin::u8> data = { odin::telnet::IAC };
    sequence results;

    for(odin::u8 element : data)
    {
        parse(element);

        auto opt_result = parse.token();

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

    std::vector<odin::u8> data = { odin::telnet::IAC, odin::telnet::IAC };
    sequence results;

    for(odin::u8 element : data)
    {
        parse(element);

        auto opt_result = parse.token();

        if (opt_result.is_initialized())
        {
            results.push_back(opt_result.get());
        }
    }

    // The result must contain only one element, which is a vector of one
    // element, containing an IAC.
    CPPUNIT_ASSERT_EQUAL(sequence::size_type(1), results.size());
    CPPUNIT_ASSERT_EQUAL(char(odin::telnet::IAC), boost::get<char>(results[0]));
}

void telnet_parser_fixture::test_parse_command()
{
    odin::telnet::detail::parser parse;

    std::vector<odin::u8> data = { odin::telnet::IAC, odin::telnet::NOP };
    sequence results;

    for (odin::u8 element : data)
    {
        parse(element);

        auto opt_result = parse.token();

        if (opt_result.is_initialized())
        {
            results.push_back(opt_result.get());
        }
    }

    // The result must contain only one element, which is the NOP command.
    CPPUNIT_ASSERT_EQUAL(sequence::size_type(1), results.size());
    CPPUNIT_ASSERT_EQUAL(
        odin::telnet::command(odin::telnet::NOP),
        boost::get<odin::telnet::command>(results[0]));
}

void telnet_parser_fixture::test_parse_negotiation()
{
    odin::telnet::detail::parser parse;

    std::vector<odin::u8> data =
        { odin::telnet::IAC, odin::telnet::WILL, odin::telnet::ECHO };
    sequence results;

    for(odin::u8 element : data)
    {
        parse(element);

        auto opt_result = parse.token();

        if (opt_result.is_initialized())
        {
            results.push_back(opt_result.get());
        }
    }

    // The result must contain only one element, which is the WILL ECHO
    // negotiation.
    CPPUNIT_ASSERT_EQUAL(sequence::size_type(1), results.size());

    auto element = boost::get<odin::telnet::negotiation_type>(results[0]);

    CPPUNIT_ASSERT_EQUAL(
        odin::u8(odin::telnet::WILL), element.request_);
    CPPUNIT_ASSERT_EQUAL(
        odin::u8(odin::telnet::ECHO), element.option_id_);
}

void telnet_parser_fixture::test_parse_subnegotiation()
{
    odin::telnet::detail::parser parse;

    std::vector<odin::u8> data =
    {
        odin::telnet::IAC, odin::telnet::SB, odin::telnet::ECHO,
        'x', 'y',
        odin::telnet::IAC, odin::telnet::SE
    };

    sequence results;

    for (odin::u8 element : data)
    {
        parse(element);

        auto opt_result = parse.token();

        if (opt_result.is_initialized())
        {
            results.push_back(opt_result.get());
        }
    }

    std::vector<odin::u8> expected_data = { 'x', 'y' };

    // The result must contain only one element, which is the subnegotiation
    CPPUNIT_ASSERT_EQUAL(sequence::size_type(1), results.size());

    auto element = boost::get<odin::telnet::subnegotiation_type>(results[0]);

    CPPUNIT_ASSERT_EQUAL(odin::telnet::ECHO, element.option_id_);
    CPPUNIT_ASSERT_EQUAL(
        std::vector<odin::u8>::size_type(2), element.content_.size());
    CPPUNIT_ASSERT_EQUAL(expected_data[0], element.content_[0]);
    CPPUNIT_ASSERT_EQUAL(expected_data[1], element.content_[1]);
}

void telnet_parser_fixture::test_parse_incomplete_subnegotiation()
{
    odin::telnet::detail::parser parse;

    std::vector<odin::u8> data =
    {
        odin::telnet::IAC, odin::telnet::SB, odin::telnet::ECHO,
        'x', 'y',
        odin::telnet::IAC
    };

    sequence results;

    for (odin::u8 element : data)
    {
        parse(element);

        auto opt_result = parse.token();

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

    std::vector<odin::u8> data =
    {
        'a', 'b', 'c', odin::telnet::IAC, odin::telnet::IAC,
        odin::telnet::IAC, odin::telnet::NOP,
        odin::telnet::IAC, odin::telnet::AYT,
        'd',
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::ECHO,
        odin::telnet::IAC, odin::telnet::DONT, odin::telnet::ECHO,
        odin::telnet::IAC, odin::telnet::SB,   odin::telnet::NAWS,
        'e', 'f',
        odin::telnet::IAC, odin::telnet::SE,
        odin::telnet::IAC, odin::telnet::SB,   odin::telnet::EXOPL,
        odin::telnet::IAC, odin::telnet::SE,
        'g', 'h',
        odin::telnet::IAC, odin::telnet::WONT, odin::telnet::EXOPL,
        odin::telnet::IAC, odin::telnet::IAC,
        odin::telnet::IAC, odin::telnet::IAC,
        'i'
    };

    sequence results;

    for (odin::u8 element : data)
    {
        parse(element);

        auto opt_result = parse.token();

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
        auto element0 = boost::get<char>(results[0]);
        auto element1 = boost::get<char>(results[1]);
        auto element2 = boost::get<char>(results[2]);
        auto element3 = boost::get<char>(results[3]);

        CPPUNIT_ASSERT_EQUAL('a',    element0);
        CPPUNIT_ASSERT_EQUAL('b',    element1);
        CPPUNIT_ASSERT_EQUAL('c',    element2);
        CPPUNIT_ASSERT_EQUAL('\xFF', element3);
    }

    // Element 4: telnet NOP
    {
        auto element4 = boost::get<odin::telnet::command>(results[4]);

        CPPUNIT_ASSERT_EQUAL(
            odin::telnet::command(odin::telnet::NOP), element4);
    }

    // Element 5: telnet AYT
    {
        auto element5 = boost::get<odin::telnet::command>(results[5]);

        CPPUNIT_ASSERT_EQUAL(
            odin::telnet::command(odin::telnet::AYT), element5);
    }

    // Element 6: 'd'
    {
        auto element6 = boost::get<char>(results[6]);

        CPPUNIT_ASSERT_EQUAL('d', element6);
    }

    // Element 7: telnet WILL ECHO
    {
        auto element7 = boost::get<odin::telnet::negotiation_type>(results[7]);

        CPPUNIT_ASSERT_EQUAL(odin::telnet::WILL, element7.request_);
        CPPUNIT_ASSERT_EQUAL(odin::telnet::ECHO, element7.option_id_);
    }

    // Element 8: telnet DONT ECHO
    {
        auto element8 = boost::get<odin::telnet::negotiation_type>(results[8]);

        CPPUNIT_ASSERT_EQUAL(odin::telnet::DONT, element8.request_);
        CPPUNIT_ASSERT_EQUAL(odin::telnet::ECHO, element8.option_id_);
    }

    // Element 9: telnet SB NAWS 'e' 'f'
    {
        auto element9 = boost::get<odin::telnet::subnegotiation_type>(results[9]);

        CPPUNIT_ASSERT_EQUAL(odin::telnet::NAWS, element9.option_id_);
        CPPUNIT_ASSERT_EQUAL(
            std::vector<odin::u8>::size_type(2), element9.content_.size());
        CPPUNIT_ASSERT_EQUAL(odin::u8('e'), element9.content_[0]);
        CPPUNIT_ASSERT_EQUAL(odin::u8('f'), element9.content_[1]);
    }

    // Element 10: telnet SB EXOPL
    {
        auto element10 = boost::get<odin::telnet::subnegotiation_type>(results[10]);

        CPPUNIT_ASSERT_EQUAL(odin::telnet::EXOPL, element10.option_id_);
        CPPUNIT_ASSERT_EQUAL(true, element10.content_.empty());
    }

    // Element 11, 12: 'g', 'h'
    {
        auto element11 = boost::get<char>(results[11]);
        auto element12 = boost::get<char>(results[12]);

        CPPUNIT_ASSERT_EQUAL('g', element11);
        CPPUNIT_ASSERT_EQUAL('h', element12);
    }

    // Element 13: telnet WONT EXOPL
    {
        auto element13 = boost::get<odin::telnet::negotiation_type>(results[13]);

        CPPUNIT_ASSERT_EQUAL(odin::telnet::WONT, element13.request_);
        CPPUNIT_ASSERT_EQUAL(odin::telnet::EXOPL, element13.option_id_);
    }

    // Element 14..16: '\xFF' '\xFF' 'i'
    {
        auto element14 = boost::get<char>(results[14]);
        auto element15 = boost::get<char>(results[15]);
        auto element16 = boost::get<char>(results[16]);

        CPPUNIT_ASSERT_EQUAL('\xFF', element14);
        CPPUNIT_ASSERT_EQUAL('\xFF', element15);
        CPPUNIT_ASSERT_EQUAL('i',    element16);
    }
}

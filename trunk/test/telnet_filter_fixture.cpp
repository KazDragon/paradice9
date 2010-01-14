#include "telnet_filter_fixture.hpp"
#include "odin/telnet/filter.hpp"
#include "odin/telnet/protocol.hpp"
#include <boost/function.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/typeof/typeof.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_filter_fixture);

using namespace std;
using namespace boost;

namespace bll = boost::lambda;

void telnet_filter_fixture::test_constructor()
{
    odin::telnet::filter filter;
}

void telnet_filter_fixture::test_inheritance()
{
    odin::telnet::filter    filter;
    odin::filter<odin::u8> &u8filter = filter;
    (void)u8filter;
}

void telnet_filter_fixture::test_filter_normal_char()
{
    // For any character except for IAC, this will normally be unfiltered.
    odin::telnet::filter filter;
    
    for (odin::u8 ch = 0; ch < odin::telnet::IAC; ++ch)
    {
        BOOST_AUTO(result, filter(ch));
        CPPUNIT_ASSERT_EQUAL(true, result.is_initialized());
        CPPUNIT_ASSERT_EQUAL(ch, result.get());
    }
}

void telnet_filter_fixture::test_filter_iac()
{
    // The first IAC in a sequence will be filtered out.
    odin::telnet::filter filter;
    
    BOOST_AUTO(result, filter(odin::telnet::IAC));
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
}

void telnet_filter_fixture::test_filter_iac_iac()
{
    // While the first IAC must be filtered out, the second is returned as
    // the IAC character.  This allows for the character represented by IAC
    // to be "escaped".
    odin::telnet::filter filter;

    BOOST_AUTO(result, filter(odin::telnet::IAC));
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::IAC);
    CPPUNIT_ASSERT_EQUAL(true, result.is_initialized());
    CPPUNIT_ASSERT_EQUAL(odin::u8(odin::telnet::IAC), result.get());
}

void telnet_filter_fixture::test_filter_iac_command()
{
    // Telnet commands apart from WILL, WONT, DO, DONT (which are categorised
    // as "negotiations" in this API) take no arguments.  Therefore, a normal
    // character immediately after one (we'll use AYT for the test) should
    // be returned normally.
    odin::telnet::filter filter;
    
    odin::telnet::command command = odin::telnet::command(0);
    function<void (odin::telnet::command)> command_handler = (
        bll::var(command) = bll::_1
    );
        
    filter.on_command(command_handler);
    
    BOOST_AUTO(result, filter(odin::telnet::IAC));
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::AYT);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::u8('x'));
    CPPUNIT_ASSERT_EQUAL(true, result.is_initialized());
    CPPUNIT_ASSERT_EQUAL(odin::u8('x'), result.get());
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::AYT, command);
}

void telnet_filter_fixture::test_filter_iac_will()
{
    // A telnet negotiation that begins with WILL, WONT, DO or DONT takes
    // an extra parameter that is the negotiation ID of the negotiation in
    // question.  We'll use ECHO as our test ID.
    odin::telnet::filter filter;
    
    odin::telnet::negotiation_request_type request = 
        odin::telnet::negotiation_request_type(0);
        
    odin::telnet::option_id_type option_id = odin::telnet::option_id_type(0);

    function<void (
        odin::telnet::negotiation_request_type
      , odin::telnet::option_id_type)
    > negotiation_handler = (
        bll::var(request)   = bll::_1
      , bll::var(option_id) = bll::_2
    );
      
    filter.on_negotiation(negotiation_handler);

    BOOST_AUTO(result, filter(odin::telnet::IAC));
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::WILL);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
   
    result = filter(odin::telnet::ECHO);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());

    result = filter(odin::u8('X'));
    CPPUNIT_ASSERT_EQUAL(true, result.is_initialized());
    CPPUNIT_ASSERT_EQUAL(odin::u8('X'), result.get());
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::WILL, request);
    CPPUNIT_ASSERT_EQUAL(odin::telnet::ECHO, option_id);
}

void telnet_filter_fixture::test_filter_iac_wont()
{
    // A telnet negotiation that begins with WILL, WONT, DO or DONT takes
    // an extra parameter that is the negotiation ID of the negotiation in
    // question.  We'll use ECHO as our test ID.
    odin::telnet::filter filter;
    
    odin::telnet::negotiation_request_type request = 
        odin::telnet::negotiation_request_type(0);
        
    odin::telnet::option_id_type option_id = odin::telnet::option_id_type(0);

    function<void (
        odin::telnet::negotiation_request_type
      , odin::telnet::option_id_type)
    > negotiation_handler = (
        bll::var(request)   = bll::_1
      , bll::var(option_id) = bll::_2
    );

    filter.on_negotiation(negotiation_handler);

    BOOST_AUTO(result, filter(odin::telnet::IAC));
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::WONT);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::ECHO);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());

    result = filter(odin::u8('X'));
    CPPUNIT_ASSERT_EQUAL(true, result.is_initialized());
    CPPUNIT_ASSERT_EQUAL(odin::u8('X'), result.get());
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::WONT, request);
    CPPUNIT_ASSERT_EQUAL(odin::telnet::ECHO, option_id);
}

void telnet_filter_fixture::test_filter_iac_do()
{
    // A telnet negotiation that begins with WILL, WONT, DO or DONT takes
    // an extra parameter that is the negotiation ID of the negotiation in
    // question.  We'll use ECHO as our test ID.
    odin::telnet::filter filter;
    
    odin::telnet::negotiation_request_type request = 
        odin::telnet::negotiation_request_type(0);
        
    odin::telnet::option_id_type option_id = odin::telnet::option_id_type(0);

    function<void (
        odin::telnet::negotiation_request_type
      , odin::telnet::option_id_type)
    > negotiation_handler = (
        bll::var(request)   = bll::_1
      , bll::var(option_id) = bll::_2
    );
    
    filter.on_negotiation(negotiation_handler);

    BOOST_AUTO(result, filter(odin::telnet::IAC));
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::DO);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::ECHO);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());

    result = filter(odin::u8('X'));
    CPPUNIT_ASSERT_EQUAL(true, result.is_initialized());
    CPPUNIT_ASSERT_EQUAL(odin::u8('X'), result.get());
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::DO,   request);
    CPPUNIT_ASSERT_EQUAL(odin::telnet::ECHO, option_id);
}

void telnet_filter_fixture::test_filter_iac_dont()
{
    // A telnet negotiation that begins with WILL, WONT, DO or DONT takes
    // an extra parameter that is the negotiation ID of the negotiation in
    // question.  We'll use ECHO as our test ID.
    odin::telnet::filter filter;
    
    odin::telnet::negotiation_request_type request = 
        odin::telnet::negotiation_request_type(0);
        
    odin::telnet::option_id_type option_id = odin::telnet::option_id_type(0);

    function<void (
        odin::telnet::negotiation_request_type
      , odin::telnet::option_id_type)
    > negotiation_handler = (
        bll::var(request)   = bll::_1
      , bll::var(option_id) = bll::_2
    );
    
    filter.on_negotiation(negotiation_handler);

    BOOST_AUTO(result, filter(odin::telnet::IAC));
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::DONT);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::ECHO);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());

    result = filter(odin::u8('X'));
    CPPUNIT_ASSERT_EQUAL(true, result.is_initialized());
    CPPUNIT_ASSERT_EQUAL(odin::u8('X'), result.get());
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::DONT, request);
    CPPUNIT_ASSERT_EQUAL(odin::telnet::ECHO, option_id);
}

void telnet_filter_fixture::test_filter_iac_sb_empty()
{
    // A telnet subnegotiation takes the form IAC SB ID <subnegotiation> IAC SE
    // This test case tests the case where the subnegotiation is empty.
    odin::telnet::filter filter;
    
    odin::telnet::option_id_type      option_id = 0;
    odin::telnet::subnegotiation_type subnegotiation;

    function<void (
        odin::telnet::option_id_type
      , odin::telnet::subnegotiation_type)
    > subnegotiation_handler = (
        bll::var(option_id) = bll::_1
      , bll::var(subnegotiation)    = bll::_2
    );
    
    filter.on_subnegotiation(subnegotiation_handler);
    
    BOOST_AUTO(result, filter(odin::telnet::IAC));
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::SB);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::ECHO);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::IAC);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());

    result = filter(odin::telnet::SE);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());

    result = filter(odin::u8('Z'));
    CPPUNIT_ASSERT_EQUAL(true, result.is_initialized());
    CPPUNIT_ASSERT_EQUAL(odin::u8('Z'), result.get());
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::ECHO, option_id);
    CPPUNIT_ASSERT_EQUAL(
        odin::telnet::subnegotiation_type::size_type(0)
      , subnegotiation.size());
}

void telnet_filter_fixture::test_filter_iac_sb_full()
{
    // A telnet subnegotiation takes the form IAC SB ID <subnegotiation> IAC SE
    // This test case tests the case where the subnegotiation has data.
    odin::telnet::filter filter;

    odin::telnet::option_id_type      option_id = 0;
    odin::telnet::subnegotiation_type subnegotiation;

    function<void (
        odin::telnet::option_id_type
      , odin::telnet::subnegotiation_type)
    > subnegotiation_handler = (
        bll::var(option_id) = bll::_1
      , bll::var(subnegotiation)    = bll::_2
    );
    
    filter.on_subnegotiation(subnegotiation_handler);
    
    BOOST_AUTO(result, filter(odin::telnet::IAC));
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::SB);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::ECHO);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::u8('a'));
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::IAC);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());

    result = filter(odin::telnet::SE);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());

    result = filter(odin::u8('Z'));
    CPPUNIT_ASSERT_EQUAL(true, result.is_initialized());
    CPPUNIT_ASSERT_EQUAL(odin::u8('Z'), result.get());

    CPPUNIT_ASSERT_EQUAL(odin::telnet::ECHO, option_id);
    CPPUNIT_ASSERT_EQUAL(
        odin::telnet::subnegotiation_type::size_type(1)
      , subnegotiation.size());
    CPPUNIT_ASSERT_EQUAL(odin::u8('a'), subnegotiation[0]);
}

void telnet_filter_fixture::test_filter_iac_sb_iac()
{
    // A telnet subnegotiation takes the form IAC SB ID <subnegotiation> IAC SE
    // This test case tests the case where the subnegotiation has an IAC as
    // part of the data.  This must be escaped out.
    odin::telnet::filter filter;

    odin::telnet::option_id_type      option_id = 0;
    odin::telnet::subnegotiation_type subnegotiation;

    function<void (
        odin::telnet::option_id_type
      , odin::telnet::subnegotiation_type)
    > subnegotiation_handler = (
        bll::var(option_id) = bll::_1
      , bll::var(subnegotiation)    = bll::_2
    );
    
    filter.on_subnegotiation(subnegotiation_handler);
    
    BOOST_AUTO(result, filter(odin::telnet::IAC));
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::SB);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::ECHO);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::IAC);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());

    result = filter(odin::telnet::IAC);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::IAC);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());

    result = filter(odin::telnet::SE);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());

    result = filter(odin::u8('Z'));
    CPPUNIT_ASSERT_EQUAL(true, result.is_initialized());
    CPPUNIT_ASSERT_EQUAL(odin::u8('Z'), result.get());

    CPPUNIT_ASSERT_EQUAL(odin::telnet::ECHO, option_id);
    CPPUNIT_ASSERT_EQUAL(
        odin::telnet::subnegotiation_type::size_type(1)
      , subnegotiation.size());
    CPPUNIT_ASSERT_EQUAL(odin::u8(odin::telnet::IAC), subnegotiation[0]);
}

void telnet_filter_fixture::test_filter_iac_sb_char()
{
    // A telnet subnegotiation takes the form IAC SB ID <subnegotiation> IAC SE
    // This test case tests the case where the subnegotiation erroneously
    // contains IAC followed by neither IAC nor SE.  This is an error
    // according to the protocol, but we will use IAC as an escape mechanism
    // and add the following character as part of the subnegotiation.
    odin::telnet::filter filter;

    odin::telnet::option_id_type      option_id = 0;
    odin::telnet::subnegotiation_type subnegotiation;

    function<void (
        odin::telnet::option_id_type
      , odin::telnet::subnegotiation_type)
    > subnegotiation_handler = (
        bll::var(option_id) = bll::_1
      , bll::var(subnegotiation)    = bll::_2
    );
    
    filter.on_subnegotiation(subnegotiation_handler);
    
    BOOST_AUTO(result, filter(odin::telnet::IAC));
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::SB);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::ECHO);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::IAC);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());

    result = filter(odin::u8('b'));
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());
    
    result = filter(odin::telnet::IAC);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());

    result = filter(odin::telnet::SE);
    CPPUNIT_ASSERT_EQUAL(false, result.is_initialized());

    result = filter(odin::u8('Z'));
    CPPUNIT_ASSERT_EQUAL(true, result.is_initialized());
    CPPUNIT_ASSERT_EQUAL(odin::u8('Z'), result.get());

    CPPUNIT_ASSERT_EQUAL(odin::telnet::ECHO, option_id);
    CPPUNIT_ASSERT_EQUAL(
        odin::telnet::subnegotiation_type::size_type(1)
      , subnegotiation.size());
    CPPUNIT_ASSERT_EQUAL(odin::u8('b'), subnegotiation[0]);
}

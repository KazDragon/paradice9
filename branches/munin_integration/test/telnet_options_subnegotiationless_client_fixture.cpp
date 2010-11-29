#include "telnet_options_subnegotiationless_client_fixture.hpp"
#include "odin/telnet/options/subnegotiationless_client.hpp"
#include "odin/telnet/stream.hpp"
#include "odin/telnet/negotiation_router.hpp"
#include "odin/telnet/subnegotiation_router.hpp"
#include "odin/telnet/protocol.hpp"
#include "fake_datastream.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_options_subnegotiationless_client_fixture);

using namespace std;
using namespace boost;

typedef fake_datastream<odin::u8, odin::u8> fake_byte_stream;

void telnet_options_subnegotiationless_client_fixture::test_constructor()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::negotiation_router> telnet_negotiation_router(
        new odin::telnet::negotiation_router);
    shared_ptr<odin::telnet::subnegotiation_router> telnet_subnegotiation_router(
        new odin::telnet::subnegotiation_router);

    odin::telnet::options::subnegotiationless_client<
        odin::telnet::ECHO
    > echo_client(
        telnet_stream
      , telnet_negotiation_router
      , telnet_subnegotiation_router);
    
    odin::telnet::options::subnegotiationless_client<
        odin::telnet::SUPPRESS_GOAHEAD
    > suppress_ga_client(        telnet_stream
      , telnet_negotiation_router
      , telnet_subnegotiation_router);

    CPPUNIT_ASSERT_EQUAL(odin::telnet::ECHO, echo_client.get_option_id());
    CPPUNIT_ASSERT_EQUAL(
        odin::telnet::SUPPRESS_GOAHEAD
      , suppress_ga_client.get_option_id());
}

void telnet_options_subnegotiationless_client_fixture::test_inheritance()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::negotiation_router> telnet_negotiation_router(
        new odin::telnet::negotiation_router);
    shared_ptr<odin::telnet::subnegotiation_router> telnet_subnegotiation_router(
        new odin::telnet::subnegotiation_router);

    odin::telnet::options::subnegotiationless_client<
        odin::telnet::ECHO
    > subnegotiationless_client(
        telnet_stream
      , telnet_negotiation_router
      , telnet_subnegotiation_router);
    
    odin::telnet::client_option &option = subnegotiationless_client;
    (void)option;
}
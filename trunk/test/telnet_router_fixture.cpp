#include "telnet_router_fixture.hpp"
#include "odin/telnet/router.hpp"
#include "odin/telnet/stream.hpp"
#include "fake_datastream.hpp"
#include <boost/lambda/lambda.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_router_fixture);

using namespace std;
using namespace boost;

namespace bll = boost::lambda;

typedef fake_datastream<odin::u8, odin::u8> fake_byte_stream;

void telnet_router_fixture::test_constructor()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    
    odin::telnet::router router(telnet_stream);
}

void telnet_router_fixture::test_register_negotiation()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    
    odin::telnet::router router(telnet_stream);
    
    odin::u32 called = 0;
    function<void ()> handler = (++bll::var(called));
    
    router.register_negotiation(
        odin::telnet::WILL
      , odin::telnet::KERMIT
      , handler);
    
    odin::u8 will_echo[] = 
    {
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::ECHO
    };
    
    telnet_stream->async_read(1, NULL);
    
    fake_stream->write_data_to_read(will_echo);
    
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    
    odin::u8 wont_kermit[] =
    {
        odin::telnet::IAC, odin::telnet::WONT, odin::telnet::KERMIT
    };
    
    fake_stream->write_data_to_read(wont_kermit);
    
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    
    odin::u8 will_kermit[] =
    {
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
    };
    
    fake_stream->write_data_to_read(will_kermit);
    
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
}

void telnet_router_fixture::test_unregister_negotiation()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    
    odin::telnet::router router(telnet_stream);
    
    odin::u32 called = 0;
    function<void ()> handler = (++bll::var(called));
    
    router.register_negotiation(
        odin::telnet::WILL
      , odin::telnet::KERMIT
      , handler);
    
    odin::u8 will_kermit[] =
    {
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
    };
    
    telnet_stream->async_read(1, NULL);
    
    fake_stream->write_data_to_read(will_kermit);
    
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    
    router.unregister_negotiation(
        odin::telnet::WILL
      , odin::telnet::ECHO);
    
    fake_stream->write_data_to_read(will_kermit);
    
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(2), called);
    
    router.unregister_negotiation(
        odin::telnet::WONT
      , odin::telnet::KERMIT);
    
    fake_stream->write_data_to_read(will_kermit);
    
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(3), called);

    router.unregister_negotiation(
        odin::telnet::WILL
      , odin::telnet::KERMIT);
    
    fake_stream->write_data_to_read(will_kermit);
    
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(3), called);
}

void telnet_router_fixture::test_register_subnegotiation()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    
    odin::telnet::router router(telnet_stream);
    
    odin::u32                         called = 0;
    odin::telnet::subnegotiation_type subnegotiation;
    boost::function<void (odin::telnet::subnegotiation_type)> callback = (
        ++bll::var(called),
        bll::var(subnegotiation) = bll::_1
    );
    
    router.register_subnegotiation(odin::telnet::KERMIT, callback);
    
    odin::u8 echo_subnegotiation[] =
    {
        odin::telnet::IAC, odin::telnet::SB, odin::telnet::ECHO
      , 'a', 'b'
      , odin::telnet::IAC, odin::telnet::SE
    };
    
    telnet_stream->async_read(1, NULL);
    
    fake_stream->write_data_to_read(echo_subnegotiation);
    
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    CPPUNIT_ASSERT_EQUAL(odin::telnet::subnegotiation_type(), subnegotiation);
    
    odin::u8 kermit_subnegotiation[] =
    {
        odin::telnet::IAC, odin::telnet::SB, odin::telnet::KERMIT
      , 'c', 'd'
      , odin::telnet::IAC, odin::telnet::SE
    };
    
    odin::u8 expected_data[] = { 'c', 'd' };
    odin::telnet::subnegotiation_type expected_subnegotiation(expected_data);
    
    fake_stream->write_data_to_read(kermit_subnegotiation);
    
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    CPPUNIT_ASSERT_EQUAL(expected_subnegotiation, subnegotiation);
}

void telnet_router_fixture::test_unregister_subnegotiation()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    
    odin::telnet::router router(telnet_stream);
    
    odin::u32                         called = 0;
    odin::telnet::subnegotiation_type subnegotiation;
    boost::function<void (odin::telnet::subnegotiation_type)> callback = (
        ++bll::var(called),
        bll::var(subnegotiation) = bll::_1
    );
    
    router.register_subnegotiation(odin::telnet::KERMIT, callback);
    
    odin::u8 kermit_subnegotiation[] =
    {
        odin::telnet::IAC, odin::telnet::SB, odin::telnet::KERMIT
      , 'c', 'd'
      , odin::telnet::IAC, odin::telnet::SE
    };
    
    odin::u8 expected_data[] = { 'c', 'd' };
    odin::telnet::subnegotiation_type expected_subnegotiation(expected_data);

    telnet_stream->async_read(1, NULL);
    
    fake_stream->write_data_to_read(kermit_subnegotiation);
    
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    CPPUNIT_ASSERT_EQUAL(expected_subnegotiation, subnegotiation);

    router.unregister_subnegotiation(odin::telnet::ECHO);

    fake_stream->write_data_to_read(kermit_subnegotiation);
    
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(2), called);
    
    router.unregister_subnegotiation(odin::telnet::KERMIT);

    fake_stream->write_data_to_read(kermit_subnegotiation);
    
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(2), called);
}

void telnet_router_fixture::test_unrouted_negotiation()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    
    odin::telnet::router router(telnet_stream);
    
    telnet_stream->async_read(1, NULL);
    
    {
        odin::u8 do_negotiation[] =
        {
            odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(do_negotiation);
        
        io_service.reset();
        io_service.run();
        
        odin::u8 expected_response[] =
        {
            odin::telnet::IAC, odin::telnet::WONT, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_response)
          , fake_stream->read_data_written());
    }

    {
        odin::u8 do_negotiation[] =
        {
            odin::telnet::IAC, odin::telnet::DONT, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(do_negotiation);
        
        io_service.reset();
        io_service.run();
        
        odin::u8 expected_response[] =
        {
            odin::telnet::IAC, odin::telnet::WONT, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_response)
          , fake_stream->read_data_written());
    }

    {
        odin::u8 do_negotiation[] =
        {
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(do_negotiation);
        
        io_service.reset();
        io_service.run();
        
        odin::u8 expected_response[] =
        {
            odin::telnet::IAC, odin::telnet::DONT, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_response)
          , fake_stream->read_data_written());
    }

    {
        odin::u8 do_negotiation[] =
        {
            odin::telnet::IAC, odin::telnet::WONT, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(do_negotiation);
        
        io_service.reset();
        io_service.run();
        
        odin::u8 expected_response[] =
        {
            odin::telnet::IAC, odin::telnet::DONT, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_response)
          , fake_stream->read_data_written());
    }
}

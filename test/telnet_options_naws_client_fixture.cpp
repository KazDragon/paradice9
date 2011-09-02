#include "telnet_options_naws_client_fixture.hpp"
#include "odin/telnet/options/naws_client.hpp"
#include "odin/telnet/stream.hpp"
#include "odin/telnet/command_router.hpp"
#include "odin/telnet/negotiation_router.hpp"
#include "odin/telnet/subnegotiation_router.hpp"
#include "odin/telnet/input_visitor.hpp"
#include "odin/telnet/protocol.hpp"
#include "fake_datastream.hpp"
#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_options_naws_client_fixture);

using namespace std;
using namespace boost;
using namespace boost::assign;

namespace bll = boost::lambda;

typedef fake_datastream<odin::u8, odin::u8> fake_byte_stream;


void telnet_options_naws_client_fixture::test_constructor()
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

    odin::telnet::options::naws_client naws_client(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::NAWS, naws_client.get_option_id());
}

void telnet_options_naws_client_fixture::test_inheritance()
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

    odin::telnet::options::naws_client naws_client(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
    odin::telnet::client_option &option = naws_client;
    (void)option;
}

void telnet_options_naws_client_fixture::test_on_size()
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

    odin::telnet::options::naws_client naws_client(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
    boost::function<void (odin::u16, odin::u16)> callback;
    
    naws_client.on_size(callback);
}

void telnet_options_naws_client_fixture::test_callback_no_callback()
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

    odin::telnet::options::naws_client naws_client(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
    naws_client.set_activatable(true);
    
    telnet_stream->async_read(1, NULL);
    
    fake_byte_stream::input_storage_type data =
        list_of(odin::telnet::IAC)(odin::telnet::WILL)(odin::telnet::NAWS);

    fake_stream->write_data_to_read(data);
        
    io_service.reset();
    io_service.run();


    data = 
        list_of(odin::telnet::IAC)(odin::telnet::SB)(odin::telnet::NAWS)
               (0)(1)(0)(0)
               (odin::telnet::IAC)(odin::telnet::SE);

    fake_stream->write_data_to_read(data);
     
    io_service.reset();
    io_service.run();
}

void telnet_options_naws_client_fixture::test_callback_width()
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

    odin::telnet::input_visitor visitor(
        shared_ptr<odin::telnet::command_router>()
      , telnet_negotiation_router
      , telnet_subnegotiation_router
      , NULL);

    odin::telnet::options::naws_client naws_client(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
    odin::u16 width  = 0;
    odin::u16 height = 0;
    boost::function<void (odin::u16, odin::u16)> naws_callback = (
        bll::var(width)  = bll::_1
      , bll::var(height) = bll::_2
    );
    
    naws_client.set_activatable(true);
    naws_client.on_size(naws_callback);

    odin::telnet::stream::input_callback_type callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );
    
    telnet_stream->async_read(1, callback);
    
    fake_byte_stream::input_storage_type data =
        list_of(odin::telnet::IAC)(odin::telnet::WILL)(odin::telnet::NAWS);
        
    fake_stream->write_data_to_read(data);
    
    io_service.reset();
    io_service.run();

    data =
        list_of(odin::telnet::IAC)(odin::telnet::SB)(odin::telnet::NAWS)
               (0)(1)(0)(0)
               (odin::telnet::IAC)(odin::telnet::SE);
               
    fake_stream->write_data_to_read(data);
    telnet_stream->async_read(1, callback);
 
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u16(1), width);
    CPPUNIT_ASSERT_EQUAL(odin::u16(0), height);

    data =
        list_of(odin::telnet::IAC)(odin::telnet::SB)(odin::telnet::NAWS)
               (0)(255)(255)(0)(0)
               (odin::telnet::IAC)(odin::telnet::SE);

    fake_stream->write_data_to_read(data);
    telnet_stream->async_read(1, callback);
 
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u16(255), width);
    CPPUNIT_ASSERT_EQUAL(odin::u16(0), height);

    data =
        list_of(odin::telnet::IAC)(odin::telnet::SB)(odin::telnet::NAWS)
               (1)(0)(0)(0)
               (odin::telnet::IAC)(odin::telnet::SE);

    fake_stream->write_data_to_read(data);
    telnet_stream->async_read(1, callback);
 
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u16(256), width);
    CPPUNIT_ASSERT_EQUAL(odin::u16(0), height);

    data =
        list_of(odin::telnet::IAC)(odin::telnet::SB)(odin::telnet::NAWS)
               (255)(255)(0)(0)(0)
               (odin::telnet::IAC)(odin::telnet::SE);

    fake_stream->write_data_to_read(data);
    telnet_stream->async_read(1, callback);
 
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u16(65280), width);
    CPPUNIT_ASSERT_EQUAL(odin::u16(0), height);
}

void telnet_options_naws_client_fixture::test_callback_height()
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

    odin::telnet::input_visitor visitor(
        shared_ptr<odin::telnet::command_router>()
      , telnet_negotiation_router
      , telnet_subnegotiation_router
      , NULL);

    odin::telnet::options::naws_client naws_client(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
    odin::u16 width  = 0;
    odin::u16 height = 0;
    boost::function<void (odin::u16, odin::u16)> naws_callback = (
        bll::var(width)  = bll::_1
      , bll::var(height) = bll::_2
    );
    
    naws_client.set_activatable(true);
    naws_client.on_size(naws_callback);
    
    odin::telnet::stream::input_callback_type callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );
    
    telnet_stream->async_read(1, callback);
    
    fake_byte_stream::input_storage_type data =
        list_of(odin::telnet::IAC)(odin::telnet::WILL)(odin::telnet::NAWS);

    fake_stream->write_data_to_read(data);
    
    io_service.reset();
    io_service.run();

    data =
        list_of(odin::telnet::IAC)(odin::telnet::SB)(odin::telnet::NAWS)
               (0)(0)(0)(1)
               (odin::telnet::IAC)(odin::telnet::SE);
    
    fake_stream->write_data_to_read(data);
    telnet_stream->async_read(1, callback);
 
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u16(0), width);
    CPPUNIT_ASSERT_EQUAL(odin::u16(1), height);

    data =
        list_of(odin::telnet::IAC)(odin::telnet::SB)(odin::telnet::NAWS)
               (0)(0)(0)(255)(255)
               (odin::telnet::IAC)(odin::telnet::SE);

    fake_stream->write_data_to_read(data);
    telnet_stream->async_read(1, callback);

    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u16(0), width);
    CPPUNIT_ASSERT_EQUAL(odin::u16(255), height);

    data =
        list_of(odin::telnet::IAC)(odin::telnet::SB)(odin::telnet::NAWS)
               (0)(0)(1)(0)
               (odin::telnet::IAC)(odin::telnet::SE);
    
    fake_stream->write_data_to_read(data);
    telnet_stream->async_read(1, callback);

    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u16(0), width);
    CPPUNIT_ASSERT_EQUAL(odin::u16(256), height);

    data =
        list_of(odin::telnet::IAC)(odin::telnet::SB)(odin::telnet::NAWS)
               (0)(0)(255)(255)(0)
               (odin::telnet::IAC)(odin::telnet::SE);
    
    fake_stream->write_data_to_read(data);
    telnet_stream->async_read(1, callback);

    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u16(0), width);
    CPPUNIT_ASSERT_EQUAL(odin::u16(65280), height);
}

void telnet_options_naws_client_fixture::test_subnegotiation_short()
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

    odin::telnet::input_visitor visitor(
        shared_ptr<odin::telnet::command_router>()
      , telnet_negotiation_router
      , telnet_subnegotiation_router
      , NULL);

    odin::telnet::options::naws_client naws_client(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);

    odin::u32 called = 0;    
    boost::function<void (odin::u16, odin::u16)> naws_callback = (
        ++bll::var(called)
    );
    
    naws_client.set_activatable(true);
    naws_client.on_size(naws_callback);
    
    odin::telnet::stream::input_callback_type callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );
    
    fake_byte_stream::input_storage_type data =
        list_of(odin::telnet::IAC)(odin::telnet::WILL)(odin::telnet::NAWS);

    fake_stream->write_data_to_read(data);
    telnet_stream->async_read(1, callback);
    
    io_service.reset();
    io_service.run();

    data =
        list_of(odin::telnet::IAC)(odin::telnet::SB)(odin::telnet::NAWS)
               (1)(1)(1) // Note: only three bytes
               (odin::telnet::IAC)(odin::telnet::SE);
    
    fake_stream->write_data_to_read(data);
    telnet_stream->async_read(1, callback);
 
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
}

void telnet_options_naws_client_fixture::test_subnegotiation_long()
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

    odin::telnet::input_visitor visitor(
        shared_ptr<odin::telnet::command_router>()
      , telnet_negotiation_router
      , telnet_subnegotiation_router
      , NULL);

    odin::telnet::options::naws_client naws_client(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);

    odin::u32 called = 0;    
    boost::function<void (odin::u16, odin::u16)> naws_callback = (
        ++bll::var(called)
    );
    
    naws_client.set_activatable(true);
    naws_client.on_size(naws_callback);
    
    odin::telnet::stream::input_callback_type callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );

    fake_byte_stream::input_storage_type data =    
        list_of(odin::telnet::IAC)(odin::telnet::WILL)(odin::telnet::NAWS);

    fake_stream->write_data_to_read(data);
    telnet_stream->async_read(1, callback);
    
    io_service.reset();
    io_service.run();

    data =
        list_of(odin::telnet::IAC)(odin::telnet::SB)(odin::telnet::NAWS)
               (1)(1)(1)(1)(1) // Note: five bytes
               (odin::telnet::IAC)(odin::telnet::SE);
    
    fake_stream->write_data_to_read(data);
    telnet_stream->async_read(1, callback);
 
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
}


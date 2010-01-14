#include "telnet_options_naws_client_fixture.hpp"
#include "odin/telnet/options/naws_client.hpp"
#include "odin/telnet/stream.hpp"
#include "odin/telnet/router.hpp"
#include "odin/telnet/protocol.hpp"
#include "fake_datastream.hpp"
#include <boost/lambda/lambda.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_options_naws_client_fixture);

using namespace std;
using namespace boost;

namespace bll = boost::lambda;

typedef fake_datastream<odin::u8, odin::u8> fake_byte_stream;

void telnet_options_naws_client_fixture::test_constructor()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));

    odin::telnet::options::naws_client naws_client(
        telnet_stream, telnet_router);
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::NAWS, naws_client.get_option_id());
}

void telnet_options_naws_client_fixture::test_inheritance()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));

    odin::telnet::options::naws_client naws_client(
        telnet_stream, telnet_router);
    
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
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));

    odin::telnet::options::naws_client naws_client(
        telnet_stream, telnet_router);
    
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
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));

    odin::telnet::options::naws_client naws_client(
        telnet_stream, telnet_router);
    
    naws_client.set_activatable(true);
    
    telnet_stream->async_read(1, NULL);
    
    {
        odin::u8 data[] = { 
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::NAWS
        };
        
        fake_stream->write_data_to_read(data);
        
        io_service.reset();
        io_service.run();
    }
    
    {
        odin::u8 data[] = {
            odin::telnet::IAC, odin::telnet::SB, odin::telnet::NAWS
          , 0, 1, 0, 0
          , odin::telnet::IAC, odin::telnet::SE
        };
        
        odin::runtime_array<odin::u8> array(data);
        
        fake_stream->write_data_to_read(array);
     
        io_service.reset();
        io_service.run();
    }
}

void telnet_options_naws_client_fixture::test_callback_width()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));

    odin::telnet::options::naws_client naws_client(
        telnet_stream, telnet_router);
    
    odin::u16 width  = 0;
    odin::u16 height = 0;
    boost::function<void (odin::u16, odin::u16)> callback = (
        bll::var(width)  = bll::_1
      , bll::var(height) = bll::_2
    );
    
    naws_client.set_activatable(true);
    naws_client.on_size(callback);
    
    telnet_stream->async_read(1, NULL);
    
    {
        odin::u8 data[] = { 
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::NAWS
        };
        
        fake_stream->write_data_to_read(data);
        
        io_service.reset();
        io_service.run();
    }
    
    {
        odin::u8 data[] = {
            odin::telnet::IAC, odin::telnet::SB, odin::telnet::NAWS
          , 0, 1, 0, 0
          , odin::telnet::IAC, odin::telnet::SE
        };
        
        odin::runtime_array<odin::u8> array(data);
        
        fake_stream->write_data_to_read(array);
     
        io_service.reset();
        io_service.run();
        
        CPPUNIT_ASSERT_EQUAL(odin::u16(1), width);
        CPPUNIT_ASSERT_EQUAL(odin::u16(0), height);
    }

    {
        odin::u8 data[] = {
            odin::telnet::IAC, odin::telnet::SB, odin::telnet::NAWS
          , 0, 255, 255, 0, 0
          , odin::telnet::IAC, odin::telnet::SE
        };
        
        odin::runtime_array<odin::u8> array(data);
        
        fake_stream->write_data_to_read(array);
     
        io_service.reset();
        io_service.run();
        
        CPPUNIT_ASSERT_EQUAL(odin::u16(255), width);
        CPPUNIT_ASSERT_EQUAL(odin::u16(0), height);
    }

    {
        odin::u8 data[] = {
            odin::telnet::IAC, odin::telnet::SB, odin::telnet::NAWS
          , 1, 0, 0, 0
          , odin::telnet::IAC, odin::telnet::SE
        };
        
        odin::runtime_array<odin::u8> array(data);
        
        fake_stream->write_data_to_read(array);
     
        io_service.reset();
        io_service.run();
        
        CPPUNIT_ASSERT_EQUAL(odin::u16(256), width);
        CPPUNIT_ASSERT_EQUAL(odin::u16(0), height);
    }

    {
        odin::u8 data[] = {
            odin::telnet::IAC, odin::telnet::SB, odin::telnet::NAWS
          , 255, 255, 0, 0, 0
          , odin::telnet::IAC, odin::telnet::SE
        };
        
        odin::runtime_array<odin::u8> array(data);
        
        fake_stream->write_data_to_read(array);
     
        io_service.reset();
        io_service.run();
        
        CPPUNIT_ASSERT_EQUAL(odin::u16(65280), width);
        CPPUNIT_ASSERT_EQUAL(odin::u16(0), height);
    }
}

void telnet_options_naws_client_fixture::test_callback_height()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));

    odin::telnet::options::naws_client naws_client(
        telnet_stream, telnet_router);
    
    odin::u16 width  = 0;
    odin::u16 height = 0;
    boost::function<void (odin::u16, odin::u16)> callback = (
        bll::var(width)  = bll::_1
      , bll::var(height) = bll::_2
    );
    
    naws_client.set_activatable(true);
    naws_client.on_size(callback);
    
    telnet_stream->async_read(1, NULL);
    
    {
        odin::u8 data[] = { 
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::NAWS
        };
        
        fake_stream->write_data_to_read(data);
        
        io_service.reset();
        io_service.run();
    }
    
    {
        odin::u8 data[] = {
            odin::telnet::IAC, odin::telnet::SB, odin::telnet::NAWS
          , 0, 0, 0, 1
          , odin::telnet::IAC, odin::telnet::SE
        };
        
        odin::runtime_array<odin::u8> array(data);
        
        fake_stream->write_data_to_read(array);
     
        io_service.reset();
        io_service.run();
        
        CPPUNIT_ASSERT_EQUAL(odin::u16(0), width);
        CPPUNIT_ASSERT_EQUAL(odin::u16(1), height);
    }

    {
        odin::u8 data[] = {
            odin::telnet::IAC, odin::telnet::SB, odin::telnet::NAWS
          , 0, 0, 0, 255, 255
          , odin::telnet::IAC, odin::telnet::SE
        };
        
        odin::runtime_array<odin::u8> array(data);
        
        fake_stream->write_data_to_read(array);
     
        io_service.reset();
        io_service.run();
        
        CPPUNIT_ASSERT_EQUAL(odin::u16(0), width);
        CPPUNIT_ASSERT_EQUAL(odin::u16(255), height);
    }

    {
        odin::u8 data[] = {
            odin::telnet::IAC, odin::telnet::SB, odin::telnet::NAWS
          , 0, 0, 1, 0
          , odin::telnet::IAC, odin::telnet::SE
        };
        
        odin::runtime_array<odin::u8> array(data);
        
        fake_stream->write_data_to_read(array);
     
        io_service.reset();
        io_service.run();
        
        CPPUNIT_ASSERT_EQUAL(odin::u16(0), width);
        CPPUNIT_ASSERT_EQUAL(odin::u16(256), height);
    }

    {
        odin::u8 data[] = {
            odin::telnet::IAC, odin::telnet::SB, odin::telnet::NAWS
          , 0, 0, 255, 255, 0
          , odin::telnet::IAC, odin::telnet::SE
        };
        
        odin::runtime_array<odin::u8> array(data);
        
        fake_stream->write_data_to_read(array);
     
        io_service.reset();
        io_service.run();
        
        CPPUNIT_ASSERT_EQUAL(odin::u16(0), width);
        CPPUNIT_ASSERT_EQUAL(odin::u16(65280), height);
    }
}

void telnet_options_naws_client_fixture::test_subnegotiation_short()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));

    odin::telnet::options::naws_client naws_client(
        telnet_stream, telnet_router);

    odin::u32 called = 0;    
    boost::function<void (odin::u16, odin::u16)> callback = (
        ++bll::var(called)
    );
    
    naws_client.set_activatable(true);
    naws_client.on_size(callback);
    
    telnet_stream->async_read(1, NULL);
    
    {
        odin::u8 data[] = { 
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::NAWS
        };
        
        fake_stream->write_data_to_read(data);
        
        io_service.reset();
        io_service.run();
    }
    
    {
        odin::u8 data[] = {
            odin::telnet::IAC, odin::telnet::SB, odin::telnet::NAWS
          , 1, 1, 1 // Note: only three bytes
          , odin::telnet::IAC, odin::telnet::SE
        };
        
        odin::runtime_array<odin::u8> array(data);
        
        fake_stream->write_data_to_read(array);
     
        io_service.reset();
        io_service.run();
        
        CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    }
}

void telnet_options_naws_client_fixture::test_subnegotiation_long()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));

    odin::telnet::options::naws_client naws_client(
        telnet_stream, telnet_router);

    odin::u32 called = 0;    
    boost::function<void (odin::u16, odin::u16)> callback = (
        ++bll::var(called)
    );
    
    naws_client.set_activatable(true);
    naws_client.on_size(callback);
    
    telnet_stream->async_read(1, NULL);
    
    {
        odin::u8 data[] = { 
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::NAWS
        };
        
        fake_stream->write_data_to_read(data);
        
        io_service.reset();
        io_service.run();
    }
    
    {
        odin::u8 data[] = {
            odin::telnet::IAC, odin::telnet::SB, odin::telnet::NAWS
          , 1, 1, 1, 1, 1 // Note: five bytes
          , odin::telnet::IAC, odin::telnet::SE
        };
        
        odin::runtime_array<odin::u8> array(data);
        
        fake_stream->write_data_to_read(array);
     
        io_service.reset();
        io_service.run();
        
        CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    }
}

#include "telnet_server_option_fixture.hpp"
#include "odin/telnet/server_option.hpp"
#include "odin/telnet/protocol.hpp"
#include "odin/telnet/router.hpp"
#include "odin/telnet/stream.hpp"
#include "fake_datastream.hpp"
#include <boost/lambda/lambda.hpp>
#include <vector>

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_server_option_fixture);

using namespace std;
using namespace boost;

namespace bll = boost::lambda;

typedef fake_datastream<odin::u8, odin::u8> fake_byte_stream;

class fake_server_option : public odin::telnet::server_option
{
public :
    fake_server_option(
        shared_ptr<odin::telnet::stream> stream
      , shared_ptr<odin::telnet::router> router)
        : odin::telnet::server_option(
            stream, router, odin::telnet::KERMIT)
    {
    }

    std::vector<odin::telnet::subnegotiation_type> subnegotiations_;
    
private :
    virtual void on_subnegotiation(
        odin::telnet::subnegotiation_type const &subnegotiation)
    {
        subnegotiations_.push_back(subnegotiation);
    }
};

void telnet_server_option_fixture::test_constructor()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));
        
    fake_server_option option(telnet_stream, telnet_router);
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::KERMIT, option.get_option_id());
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
}

void telnet_server_option_fixture::test_inactive_activate()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));
        
    fake_server_option option(telnet_stream, telnet_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    
    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (
        ++bll::var(request_completed)
    );
    
    option.on_request_complete(request_complete_handler);
    option.activate();

    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(true,  option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    
    odin::u8 expected_data[] =
    {
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
    };
    
    CPPUNIT_ASSERT_EQUAL(
        odin::make_runtime_array(expected_data)
      , fake_stream->read_data_written());
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), request_completed);
}

void telnet_server_option_fixture::test_inactive_activate_deny()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));
        
    fake_server_option option(telnet_stream, telnet_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    
    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (
        ++bll::var(request_completed)
    );
    
    option.on_request_complete(request_complete_handler);
    option.activate();

    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(true,  option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    
    odin::u8 expected_data[] =
    {
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
    };
    
    CPPUNIT_ASSERT_EQUAL(
        odin::make_runtime_array(expected_data)
      , fake_stream->read_data_written());
    
    odin::u8 response_data[] = 
    {
        odin::telnet::IAC, odin::telnet::DONT, odin::telnet::KERMIT
    };
    
    fake_stream->write_data_to_read(response_data);
    
    telnet_stream->async_read(1, NULL);
    
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);
}

void telnet_server_option_fixture::test_inactive_activate_allow()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));
        
    fake_server_option option(telnet_stream, telnet_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    
    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (
        ++bll::var(request_completed)
    );
    
    option.on_request_complete(request_complete_handler);
    option.activate();

    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(true,  option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    
    odin::u8 expected_data[] =
    {
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
    };
    
    CPPUNIT_ASSERT_EQUAL(
        odin::make_runtime_array(expected_data)
      , fake_stream->read_data_written());
    
    odin::u8 response_data[] = 
    {
        odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
    };
    
    fake_stream->write_data_to_read(response_data);
    
    telnet_stream->async_read(1, NULL);
    
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(true, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);
}

void telnet_server_option_fixture::test_inactive_deactivate()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));
        
    fake_server_option option(telnet_stream, telnet_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    
    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (++bll::var(request_completed));
    
    option.on_request_complete(request_complete_handler);
    option.deactivate();
    
    telnet_stream->async_read(1, NULL);
    
    io_service.run();

    odin::runtime_array<odin::u8> expected_array;
    
    CPPUNIT_ASSERT_EQUAL(expected_array, fake_stream->read_data_written());
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);        
}

void telnet_server_option_fixture::test_active_deactivate()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));
        
    fake_server_option option(telnet_stream, telnet_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());

    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (
        ++bll::var(request_completed)
    );
    
    option.on_request_complete(request_complete_handler);
    option.activate();

    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(true,  option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    
    {
        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_data)
          , fake_stream->read_data_written());
    }
    
    {
        odin::u8 response_data[] = 
        {
            odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
    }
    
    telnet_stream->async_read(1, NULL);
    
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(true, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);
    
    // Option is active HERE.

    option.deactivate();
    
    {
        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::WONT, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_data)
          , fake_stream->read_data_written());
    }
    
    CPPUNIT_ASSERT_EQUAL(true, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(true, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);
}

void telnet_server_option_fixture::test_active_deactivate_deny()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));
        
    fake_server_option option(telnet_stream, telnet_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());

    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (
        ++bll::var(request_completed)
    );
    
    option.on_request_complete(request_complete_handler);
    option.activate();

    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(true,  option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    
    {
        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_data)
          , fake_stream->read_data_written());
    }
    
    {
        odin::u8 response_data[] = 
        {
            odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
    }
    
    telnet_stream->async_read(1, NULL);
    
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(true, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);
    
    // Option is active HERE.

    option.deactivate();
    
    {
        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::WONT, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_data)
          , fake_stream->read_data_written());
    }
    
    CPPUNIT_ASSERT_EQUAL(true, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(true, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);
    
    {
        odin::u8 response_data[] =
        {
            odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
    }
    
    io_service.reset();
    io_service.run();

    CPPUNIT_ASSERT_EQUAL(true, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(odin::u32(2), request_completed);
}

void telnet_server_option_fixture::test_active_deactivate_allow()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));
        
    fake_server_option option(telnet_stream, telnet_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());

    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (
        ++bll::var(request_completed)
    );
    
    option.on_request_complete(request_complete_handler);
    option.activate();

    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(true,  option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    
    {
        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_data)
          , fake_stream->read_data_written());
    }
    
    {
        odin::u8 response_data[] = 
        {
            odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
    }
    
    telnet_stream->async_read(1, NULL);
    
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(true, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);
    
    // Option is active HERE.

    option.deactivate();
    
    {
        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::WONT, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_data)
          , fake_stream->read_data_written());
    }
    
    CPPUNIT_ASSERT_EQUAL(true, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(true, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);
    
    {
        odin::u8 response_data[] =
        {
            odin::telnet::IAC, odin::telnet::DONT, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
    }
    
    io_service.reset();
    io_service.run();

    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(odin::u32(2), request_completed);
}

void telnet_server_option_fixture::test_non_activatable_inactive_activate()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));
        
    fake_server_option option(telnet_stream, telnet_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_activatable());
    
    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (
        ++bll::var(request_completed)
    );
    
    option.on_request_complete(request_complete_handler);
    
    telnet_stream->async_read(1, NULL);
    
    odin::u8 request_data[] =
    {
        odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
    };
    
    fake_stream->write_data_to_read(request_data);
    
    io_service.run();
    
    odin::u8 expected_data[] =
    {
        odin::telnet::IAC, odin::telnet::WONT, odin::telnet::KERMIT
    };
    
    CPPUNIT_ASSERT_EQUAL(
        odin::make_runtime_array(expected_data)
      , fake_stream->read_data_written());
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), request_completed);

    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_activatable());
}

void telnet_server_option_fixture::test_non_activatable_active_activate()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));
        
    fake_server_option option(telnet_stream, telnet_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_activatable());
    
    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (
        ++bll::var(request_completed)
    );
    
    option.on_request_complete(request_complete_handler);
    
    telnet_stream->async_read(1, NULL);
    
    option.activate();
    
    io_service.run();
    
    {
        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_data)
          , fake_stream->read_data_written());
        
        CPPUNIT_ASSERT_EQUAL(false, option.is_active());
        CPPUNIT_ASSERT_EQUAL(true,  option.is_negotiating_activation());
        CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
        CPPUNIT_ASSERT_EQUAL(false, option.is_activatable());
        CPPUNIT_ASSERT_EQUAL(odin::u32(0), request_completed);
    }
    
    {
        odin::u8 response_data[] =
        {
            odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
        
        io_service.reset();
        io_service.run();

        CPPUNIT_ASSERT_EQUAL(true,  option.is_active());
        CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
        CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
        CPPUNIT_ASSERT_EQUAL(false, option.is_activatable());
        CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);
    }
    
    {
        odin::u8 response_data[] =
        {
            odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
        
        io_service.reset();
        io_service.run();
    
        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_data)
          , fake_stream->read_data_written());
        
        CPPUNIT_ASSERT_EQUAL(true,  option.is_active());
        CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
        CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
        CPPUNIT_ASSERT_EQUAL(false, option.is_activatable());
        CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);
    }        
}

void telnet_server_option_fixture::test_non_activatable_inactive_deactivate()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));
        
    fake_server_option option(telnet_stream, telnet_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_activatable());
    
    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (
        ++bll::var(request_completed)
    );
    
    option.on_request_complete(request_complete_handler);
    
    telnet_stream->async_read(1, NULL);
    
    odin::u8 request_data[] =
    {
        odin::telnet::IAC, odin::telnet::DONT, odin::telnet::KERMIT
    };
    
    fake_stream->write_data_to_read(request_data);
    
    io_service.run();
    
    odin::u8 expected_data[] =
    {
        odin::telnet::IAC, odin::telnet::WONT, odin::telnet::KERMIT
    };
    
    CPPUNIT_ASSERT_EQUAL(
        odin::make_runtime_array(expected_data)
      , fake_stream->read_data_written());
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), request_completed);

    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_activatable());
}

void telnet_server_option_fixture::test_non_activatable_active_deactivate()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));
        
    fake_server_option option(telnet_stream, telnet_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_activatable());
    
    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (
        ++bll::var(request_completed)
    );
    
    option.on_request_complete(request_complete_handler);
    
    telnet_stream->async_read(1, NULL);
    
    option.activate();
    
    io_service.run();
    
    {
        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_data)
          , fake_stream->read_data_written());
        
        CPPUNIT_ASSERT_EQUAL(false, option.is_active());
        CPPUNIT_ASSERT_EQUAL(true,  option.is_negotiating_activation());
        CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
        CPPUNIT_ASSERT_EQUAL(false, option.is_activatable());
        CPPUNIT_ASSERT_EQUAL(odin::u32(0), request_completed);
    }
    
    {
        odin::u8 response_data[] =
        {
            odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
        
        io_service.reset();
        io_service.run();

        CPPUNIT_ASSERT_EQUAL(true,  option.is_active());
        CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
        CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
        CPPUNIT_ASSERT_EQUAL(false, option.is_activatable());
        CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);
    }
    
    {
        odin::u8 response_data[] =
        {
            odin::telnet::IAC, odin::telnet::DONT, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
        
        io_service.reset();
        io_service.run();
    
        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::WONT, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_data)
          , fake_stream->read_data_written());
        
        CPPUNIT_ASSERT_EQUAL(false, option.is_active());
        CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
        CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
        CPPUNIT_ASSERT_EQUAL(false, option.is_activatable());
        CPPUNIT_ASSERT_EQUAL(odin::u32(2), request_completed);
    }        
}

void telnet_server_option_fixture::test_activatable_inactive_activate()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));
        
    fake_server_option option(telnet_stream, telnet_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_activatable());
    
    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (
        ++bll::var(request_completed)
    );
    
    option.on_request_complete(request_complete_handler);
    option.set_activatable(true);
    
    CPPUNIT_ASSERT_EQUAL(true, option.is_activatable());
    
    telnet_stream->async_read(1, NULL);
    
    odin::u8 request_data[] =
    {
        odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
    };
    
    fake_stream->write_data_to_read(request_data);
    
    io_service.run();
    
    odin::u8 expected_data[] =
    {
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
    };
    
    CPPUNIT_ASSERT_EQUAL(
        odin::make_runtime_array(expected_data)
      , fake_stream->read_data_written());
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);

    CPPUNIT_ASSERT_EQUAL(true,  option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(true,  option.is_activatable());
}

void telnet_server_option_fixture::test_activatable_active_activate()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));
        
    fake_server_option option(telnet_stream, telnet_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_activatable());
    
    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (
        ++bll::var(request_completed)
    );
    
    option.on_request_complete(request_complete_handler);
    option.set_activatable(true);
    
    CPPUNIT_ASSERT_EQUAL(true, option.is_activatable());
    
    telnet_stream->async_read(1, NULL);
    
    {
        option.activate();
        
        io_service.run();
    
        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_data)
          , fake_stream->read_data_written());
        
        CPPUNIT_ASSERT_EQUAL(odin::u32(0), request_completed);
    
        CPPUNIT_ASSERT_EQUAL(false,  option.is_active());
        CPPUNIT_ASSERT_EQUAL(true,  option.is_negotiating_activation());
        CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
        CPPUNIT_ASSERT_EQUAL(true,  option.is_activatable());
        
        odin::u8 response_data[] =
        {
            odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
        
        io_service.reset();
        io_service.run();

        CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);
    
        CPPUNIT_ASSERT_EQUAL(true,  option.is_active());
        CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
        CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
        CPPUNIT_ASSERT_EQUAL(true,  option.is_activatable());
    }
    
    {
        
        odin::u8 request_data[] =
        {
            odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(request_data);
        
        io_service.reset();
        io_service.run();

        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_data)
          , fake_stream->read_data_written());
        
        CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);
    
        CPPUNIT_ASSERT_EQUAL(true,  option.is_active());
        CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
        CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
        CPPUNIT_ASSERT_EQUAL(true,  option.is_activatable());
    }
}

void telnet_server_option_fixture::test_activatable_inactive_deactivate()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));
        
    fake_server_option option(telnet_stream, telnet_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_activatable());
    
    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (
        ++bll::var(request_completed)
    );
    
    option.on_request_complete(request_complete_handler);
    option.set_activatable(true);
    
    CPPUNIT_ASSERT_EQUAL(true, option.is_activatable());
    
    telnet_stream->async_read(1, NULL);
    
    odin::u8 request_data[] =
    {
        odin::telnet::IAC, odin::telnet::DONT, odin::telnet::KERMIT
    };
    
    fake_stream->write_data_to_read(request_data);
    
    io_service.run();
    
    odin::u8 expected_data[] =
    {
        odin::telnet::IAC, odin::telnet::WONT, odin::telnet::KERMIT
    };
    
    CPPUNIT_ASSERT_EQUAL(
        odin::make_runtime_array(expected_data)
      , fake_stream->read_data_written());
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), request_completed);

    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(true,  option.is_activatable());
}

void telnet_server_option_fixture::test_activatable_active_deactivate()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));
        
    fake_server_option option(telnet_stream, telnet_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_activatable());
    
    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (
        ++bll::var(request_completed)
    );
    
    option.on_request_complete(request_complete_handler);
    option.set_activatable(true);
    
    CPPUNIT_ASSERT_EQUAL(true, option.is_activatable());
    
    telnet_stream->async_read(1, NULL);
    
    {
        option.activate();
        
        io_service.run();
    
        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_data)
          , fake_stream->read_data_written());
        
        CPPUNIT_ASSERT_EQUAL(odin::u32(0), request_completed);
    
        CPPUNIT_ASSERT_EQUAL(false,  option.is_active());
        CPPUNIT_ASSERT_EQUAL(true,  option.is_negotiating_activation());
        CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
        CPPUNIT_ASSERT_EQUAL(true,  option.is_activatable());
        
        odin::u8 response_data[] =
        {
            odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
        
        io_service.reset();
        io_service.run();

        CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);
    
        CPPUNIT_ASSERT_EQUAL(true,  option.is_active());
        CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
        CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
        CPPUNIT_ASSERT_EQUAL(true,  option.is_activatable());
    }
    
    {
        odin::u8 request_data[] =
        {
            odin::telnet::IAC, odin::telnet::DONT, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(request_data);
        
        io_service.reset();
        io_service.run();

        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::WONT, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_data)
          , fake_stream->read_data_written());
        
        CPPUNIT_ASSERT_EQUAL(odin::u32(2), request_completed);
    
        CPPUNIT_ASSERT_EQUAL(false, option.is_active());
        CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
        CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
        CPPUNIT_ASSERT_EQUAL(true,  option.is_activatable());
    }
}

void telnet_server_option_fixture::test_inactive_subnegotiation()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));
        
    fake_server_option option(telnet_stream, telnet_router);
    
    telnet_stream->async_read(1, NULL);
    
    odin::u8 subnegotiation[] =
    {
        odin::telnet::IAC, odin::telnet::SB, odin::telnet::KERMIT
      , 'a', 'b'
      , odin::telnet::IAC, odin::telnet::SE
    };
    
    fake_stream->write_data_to_read(subnegotiation);
    
    io_service.run();

    CPPUNIT_ASSERT_EQUAL(true, option.subnegotiations_.empty());    
}

void telnet_server_option_fixture::test_active_subnegotiation()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::router> telnet_router(
        new odin::telnet::router(telnet_stream));
        
    fake_server_option option(telnet_stream, telnet_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    
    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (
        ++bll::var(request_completed)
    );
    
    option.on_request_complete(request_complete_handler);
    option.activate();

    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(true,  option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    
    odin::u8 expected_data[] =
    {
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
    };
    
    CPPUNIT_ASSERT_EQUAL(
        odin::make_runtime_array(expected_data)
      , fake_stream->read_data_written());
    
    odin::u8 response_data[] = 
    {
        odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
    };
    
    fake_stream->write_data_to_read(response_data);
    
    telnet_stream->async_read(1, NULL);
    
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(true, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);
    
    odin::u8 subnegotiation[] =
    {
        odin::telnet::IAC, odin::telnet::SB, odin::telnet::KERMIT
      , 'a', 'b'
      , odin::telnet::IAC, odin::telnet::SE
    };
    
    fake_stream->write_data_to_read(subnegotiation);
    
    io_service.reset();
    io_service.run();
    
    odin::u8 expected_subnegotiation[] =
    {
        'a', 'b'
    };
    
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), option.subnegotiations_.size());
    CPPUNIT_ASSERT_EQUAL(
        odin::telnet::subnegotiation_type(expected_subnegotiation)
      , option.subnegotiations_[0]);
}

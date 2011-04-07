#include "telnet_client_option_fixture.hpp"
#include "odin/telnet/client_option.hpp"
#include "odin/telnet/input_visitor.hpp"
#include "odin/telnet/protocol.hpp"
#include "odin/telnet/stream.hpp"
#include "odin/telnet/negotiation_router.hpp"
#include "odin/telnet/subnegotiation_router.hpp"
#include "fake_datastream.hpp"
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/algorithm.hpp>
#include <boost/lambda/bind.hpp>
#include <vector>

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_client_option_fixture);

using namespace std;
using namespace boost;

namespace bll = boost::lambda;

typedef fake_datastream<odin::u8, odin::u8> fake_byte_stream;

class fake_client_option : public odin::telnet::client_option
{
public :
    fake_client_option(
        shared_ptr<odin::telnet::stream> const                &stream
      , shared_ptr<odin::telnet::negotiation_router> const    &negotiation_router
      , shared_ptr<odin::telnet::subnegotiation_router> const &subnegotiation_router)
        : odin::telnet::client_option(
            stream
          , negotiation_router
          , subnegotiation_router
          , odin::telnet::KERMIT)
    {
    }

    std::vector<odin::telnet::subnegotiation_type> subnegotiations_;
    
    void send_subnegotiation(
        odin::telnet::subnegotiation_content_type const &content)
    {
        odin::telnet::client_option::send_subnegotiation(content);
    }
    
private :
    virtual void on_subnegotiation(
        odin::telnet::subnegotiation_type const &subnegotiation)
    {
        subnegotiations_.push_back(subnegotiation);
    }
};

void telnet_client_option_fixture::test_constructor()
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
        
    fake_client_option option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::KERMIT, option.get_option_id());
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
}

void telnet_client_option_fixture::test_inactive_activate()
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
        
    fake_client_option option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
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
        odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
    };
    
    CPPUNIT_ASSERT_EQUAL(
        odin::make_runtime_array(expected_data)
      , fake_stream->read_data_written());
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), request_completed);
}

void telnet_client_option_fixture::test_inactive_activate_deny()
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
    
    fake_client_option option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
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
        odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
    };
    
    CPPUNIT_ASSERT_EQUAL(
        odin::make_runtime_array(expected_data)
      , fake_stream->read_data_written());
    
    odin::u8 response_data[] = 
    {
        odin::telnet::IAC, odin::telnet::WONT, odin::telnet::KERMIT
    };
    
    fake_stream->write_data_to_read(response_data);
    
    function<void (
        odin::runtime_array<odin::telnet::stream::input_value_type>)> callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );
    
    telnet_stream->async_read(1, callback);
    
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);
}

void telnet_client_option_fixture::test_inactive_activate_allow()
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
    
    fake_client_option option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
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
        odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
    };
    
    CPPUNIT_ASSERT_EQUAL(
        odin::make_runtime_array(expected_data)
      , fake_stream->read_data_written());
    
    odin::u8 response_data[] = 
    {
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
    };
    
    fake_stream->write_data_to_read(response_data);
    
    function<void (
        odin::runtime_array<odin::telnet::stream::input_value_type>)> callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );
    
    telnet_stream->async_read(1, callback);
    
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(true, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);
}

void telnet_client_option_fixture::test_inactive_deactivate()
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
        
    fake_client_option option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    
    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (++bll::var(request_completed));
    
    option.on_request_complete(request_complete_handler);
    option.deactivate();
    
    function<void (
        odin::runtime_array<odin::telnet::stream::input_value_type>)> callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );
    
    telnet_stream->async_read(1, callback);
    
    io_service.reset();
    io_service.run();
    
    odin::runtime_array<odin::u8> expected_array;
    
    CPPUNIT_ASSERT_EQUAL(expected_array, fake_stream->read_data_written());
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);        
}

void telnet_client_option_fixture::test_active_deactivate()
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
        
    fake_client_option option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
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
            odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_data)
          , fake_stream->read_data_written());
    }
    
    {
        odin::u8 response_data[] = 
        {
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
    }
    
    function<void (
        odin::runtime_array<odin::telnet::stream::input_value_type>)> callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );
    
    telnet_stream->async_read(1, callback);
    
    io_service.reset();
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
            odin::telnet::IAC, odin::telnet::DONT, odin::telnet::KERMIT
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

void telnet_client_option_fixture::test_active_deactivate_deny()
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
        
    fake_client_option option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
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
            odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_data)
          , fake_stream->read_data_written());
    }
    
    {
        odin::u8 response_data[] = 
        {
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
    }
    
    function<void (
        odin::runtime_array<odin::telnet::stream::input_value_type>)> callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );
    
    telnet_stream->async_read(1, callback);
    
    io_service.reset();
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
            odin::telnet::IAC, odin::telnet::DONT, odin::telnet::KERMIT
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
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
    }
    
    telnet_stream->async_read(1, callback);

    io_service.reset();    
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(true, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(odin::u32(2), request_completed);
}

void telnet_client_option_fixture::test_active_deactivate_allow()
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
        
    fake_client_option option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
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
            odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
        };
        
        CPPUNIT_ASSERT_EQUAL(
            odin::make_runtime_array(expected_data)
          , fake_stream->read_data_written());
    }
    
    {
        odin::u8 response_data[] = 
        {
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
    }
    
    function<void (
        odin::runtime_array<odin::telnet::stream::input_value_type>)> callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );
    
    telnet_stream->async_read(1, callback);
    
    io_service.reset();
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
            odin::telnet::IAC, odin::telnet::DONT, odin::telnet::KERMIT
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
            odin::telnet::IAC, odin::telnet::WONT, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
    }
    
    telnet_stream->async_read(1, callback);

    io_service.reset();    
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(odin::u32(2), request_completed);
}

void telnet_client_option_fixture::test_non_activatable_inactive_activate()
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
        
    fake_client_option option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_activatable());
    
    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (
        ++bll::var(request_completed)
    );
    
    option.on_request_complete(request_complete_handler);

    odin::u8 request_data[] =
    {
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
    };
    
    fake_stream->write_data_to_read(request_data);
    
    function<void (
        odin::runtime_array<odin::telnet::stream::input_value_type>)> callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );
    
    telnet_stream->async_read(1, callback);
    
    io_service.reset();
    io_service.run();
    
    odin::u8 expected_data[] =
    {
        odin::telnet::IAC, odin::telnet::DONT, odin::telnet::KERMIT
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

void telnet_client_option_fixture::test_non_activatable_active_activate()
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
        
    fake_client_option option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_activatable());
    
    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (
        ++bll::var(request_completed)
    );
    
    option.on_request_complete(request_complete_handler);
    option.activate();
    
    function<void (
        odin::runtime_array<odin::telnet::stream::input_value_type>)> callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );
    
    telnet_stream->async_read(1, callback);
    
    io_service.reset();
    io_service.run();
    
    {
        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
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
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
        
        telnet_stream->async_read(1, callback);
    
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
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
        
        telnet_stream->async_read(1, callback);
    
        io_service.reset();    
        io_service.run();
        
        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
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

void telnet_client_option_fixture::test_non_activatable_inactive_deactivate()
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
        
    fake_client_option option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_activatable());
    
    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (
        ++bll::var(request_completed)
    );
    
    option.on_request_complete(request_complete_handler);
    
    odin::u8 request_data[] =
    {
        odin::telnet::IAC, odin::telnet::WONT, odin::telnet::KERMIT
    };
    
    fake_stream->write_data_to_read(request_data);
    
    function<void (
        odin::runtime_array<odin::telnet::stream::input_value_type>)> callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );
    
    telnet_stream->async_read(1, callback);
    
    io_service.reset();
    io_service.run();
    
    odin::u8 expected_data[] =
    {
        odin::telnet::IAC, odin::telnet::DONT, odin::telnet::KERMIT
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

void telnet_client_option_fixture::test_non_activatable_active_deactivate()
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
    
    fake_client_option option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_activatable());
    
    odin::u32 request_completed = 0;
    function<void ()> request_complete_handler = (
        ++bll::var(request_completed)
    );
    
    option.on_request_complete(request_complete_handler);
    option.activate();
    
    function<void (
        odin::runtime_array<odin::telnet::stream::input_value_type>)> callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );
    
    telnet_stream->async_read(1, callback);
    
    io_service.reset();
    io_service.run();
    
    {
        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
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
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
        
        telnet_stream->async_read(1, callback);
    
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
            odin::telnet::IAC, odin::telnet::WONT, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
        
        telnet_stream->async_read(1, callback);
    
        io_service.reset();    
        io_service.run();
        
        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::DONT, odin::telnet::KERMIT
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

void telnet_client_option_fixture::test_activatable_inactive_activate()
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
        
    fake_client_option option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
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
    
    odin::u8 request_data[] =
    {
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
    };
    
    fake_stream->write_data_to_read(request_data);
    
    function<void (
        odin::runtime_array<odin::telnet::stream::input_value_type>)> callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );
    
    telnet_stream->async_read(1, callback);
    
    io_service.reset();
    io_service.run();
    
    odin::u8 expected_data[] =
    {
        odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
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

void telnet_client_option_fixture::test_activatable_active_activate()
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
        
    fake_client_option option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
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
    
    function<void (
        odin::runtime_array<odin::telnet::stream::input_value_type>)> callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );
    
    telnet_stream->async_read(1, callback);
    
    {
        option.activate();
        
        io_service.reset();
        io_service.run();
        
        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
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
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
        
        telnet_stream->async_read(1, callback);
    
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
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(request_data);
        
        telnet_stream->async_read(1, callback);
    
        io_service.reset();    
        io_service.run();
        
        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
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

void telnet_client_option_fixture::test_activatable_inactive_deactivate()
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
        
    fake_client_option option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
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
    
    odin::u8 request_data[] =
    {
        odin::telnet::IAC, odin::telnet::WONT, odin::telnet::KERMIT
    };
    
    fake_stream->write_data_to_read(request_data);
    
    function<void (
        odin::runtime_array<odin::telnet::stream::input_value_type>)> callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );
    
    telnet_stream->async_read(1, callback);
    
    io_service.reset();
    io_service.run();
    
    odin::u8 expected_data[] =
    {
        odin::telnet::IAC, odin::telnet::DONT, odin::telnet::KERMIT
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

void telnet_client_option_fixture::test_activatable_active_deactivate()
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
        
    fake_client_option option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
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
    
    function<void (
        odin::runtime_array<odin::telnet::stream::input_value_type>)> callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );
    
    telnet_stream->async_read(1, callback);
    
    {
        option.activate();
        
        io_service.reset();
        io_service.run();
        
        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
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
            odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(response_data);
        
        telnet_stream->async_read(1, callback);
    
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
            odin::telnet::IAC, odin::telnet::WONT, odin::telnet::KERMIT
        };
        
        fake_stream->write_data_to_read(request_data);
        
        telnet_stream->async_read(1, callback);
    
        io_service.reset();    
        io_service.run();
        
        odin::u8 expected_data[] =
        {
            odin::telnet::IAC, odin::telnet::DONT, odin::telnet::KERMIT
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

void telnet_client_option_fixture::test_inactive_subnegotiation()
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
        
    fake_client_option option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
    odin::u8 subnegotiation[] =
    {
        odin::telnet::IAC, odin::telnet::SB, odin::telnet::KERMIT
      , 'a', 'b'
      , odin::telnet::IAC, odin::telnet::SE
    };
    
    fake_stream->write_data_to_read(subnegotiation);
    
    function<void (
        odin::runtime_array<odin::telnet::stream::input_value_type>)> callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );
    
    telnet_stream->async_read(1, callback);
    
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(true, option.subnegotiations_.empty());    
}

void telnet_client_option_fixture::test_active_subnegotiation()
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
        
    fake_client_option option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
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
        odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
    };
    
    CPPUNIT_ASSERT_EQUAL(
        odin::make_runtime_array(expected_data)
      , fake_stream->read_data_written());
    
    odin::u8 response_data[] = 
    {
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
    };
    
    fake_stream->write_data_to_read(response_data);
    
    function<void (
        odin::runtime_array<odin::telnet::stream::input_value_type>)> callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );
    
    telnet_stream->async_read(1, callback);
    
    io_service.reset();
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
    
    telnet_stream->async_read(1, callback);

    io_service.reset();    
    io_service.run();
    
    odin::u8 expected_subnegotiation_content[] =
    {
        'a', 'b'
    };
    
    odin::telnet::subnegotiation_type expected_subnegotiation;
    expected_subnegotiation.option_id_ = odin::telnet::KERMIT;
    expected_subnegotiation.content_   = 
        odin::telnet::subnegotiation_content_type(
            expected_subnegotiation_content
          , expected_subnegotiation_content + 2);
    
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), option.subnegotiations_.size());
    CPPUNIT_ASSERT_EQUAL(
        expected_subnegotiation.option_id_
      , option.subnegotiations_[0].option_id_);
    CPPUNIT_ASSERT(
        expected_subnegotiation.content_ == option.subnegotiations_[0].content_);
}

void telnet_client_option_fixture::test_send_subnegotiation()
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
        
    fake_client_option option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
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
        odin::telnet::IAC, odin::telnet::DO, odin::telnet::KERMIT
    };
    
    CPPUNIT_ASSERT_EQUAL(
        odin::make_runtime_array(expected_data)
      , fake_stream->read_data_written());
    
    odin::u8 response_data[] = 
    {
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::KERMIT
    };
    
    fake_stream->write_data_to_read(response_data);
    
    function<void (
        odin::runtime_array<odin::telnet::stream::input_value_type>)> callback =
    (
        bll::bind(&odin::telnet::apply_input_range, ref(visitor), bll::_1)
    );
    
    telnet_stream->async_read(1, callback);
    
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(true, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), request_completed);
    
    // Now test that sending the subnegotiation actually works.
    odin::telnet::subnegotiation_content_type content;
    content.push_back('a');
    content.push_back('b');
    content.push_back('c');
    
    option.send_subnegotiation(content);
    
    io_service.reset();
    io_service.run();

    odin::u8 expected_subnegotiation_data[] = {
        odin::telnet::IAC, odin::telnet::SB, odin::telnet::KERMIT
      , 'a', 'b', 'c'
      , odin::telnet::IAC, odin::telnet::SE
    };
    odin::runtime_array<odin::u8> expected_subnegotiation_array(
        expected_subnegotiation_data);
    
    odin::runtime_array<odin::u8> actual_array = 
        fake_stream->read_data_written();
        
    CPPUNIT_ASSERT_EQUAL(expected_subnegotiation_array, actual_array);
}


#include "telnet_options_terminal_type_client_fixture.hpp"
#include "odin/telnet/options/terminal_type_client.hpp"
#include "odin/telnet/protocol.hpp"
#include "odin/telnet/stream.hpp"
#include "odin/telnet/negotiation_router.hpp"
#include "odin/telnet/subnegotiation_router.hpp"
#include "odin/telnet/input_visitor.hpp"
#include "fake_datastream.hpp"
#include <boost/assign/list_of.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/typeof/typeof.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_options_terminal_type_client_fixture);

using namespace odin;
using namespace boost;
using namespace boost::assign;
namespace bll = boost::lambda;
using namespace std;

typedef fake_datastream<u8, u8> fake_byte_stream;

void telnet_options_terminal_type_client_fixture::test_constructor()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<u8, u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::negotiation_router> telnet_negotiation_router(
        new odin::telnet::negotiation_router);
    shared_ptr<odin::telnet::subnegotiation_router> telnet_subnegotiation_router(
        new odin::telnet::subnegotiation_router);
        
    odin::telnet::options::terminal_type_client option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::TERMINAL_TYPE, option.get_option_id());
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
}

void telnet_options_terminal_type_client_fixture::test_send_request()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<u8, u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::negotiation_router> telnet_negotiation_router(
        new odin::telnet::negotiation_router);
    shared_ptr<odin::telnet::subnegotiation_router> telnet_subnegotiation_router(
        new odin::telnet::subnegotiation_router);
        
    // Set up a lambda function that will always read from the stream and pass
    // it into the input visitor.
    odin::telnet::input_visitor input_visitor(
        shared_ptr<odin::telnet::command_router>()
      , telnet_negotiation_router
      , telnet_subnegotiation_router
      , NULL);

    odin::telnet::stream::input_storage_type  receive_array;
    odin::telnet::stream::input_callback_type on_read;
    
    on_read = (
        bll::var(receive_array) = bll::_1
      , bll::bind(
            &odin::telnet::stream::async_read
          , *bll::var(telnet_stream), 1, ref(on_read)
        )
    );
    telnet_stream->async_read(1, on_read);
    
    odin::telnet::options::terminal_type_client option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::TERMINAL_TYPE, option.get_option_id());
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    
    // First, activate the option.  This should send IAC DO TERMINAL_TYPE to
    // the server.
    option.activate();
    
    io_service.reset();
    io_service.run();

    fake_byte_stream::input_storage_type expected_data =
        list_of(odin::telnet::IAC)(odin::telnet::DO)
               (odin::telnet::TERMINAL_TYPE);    

    CPPUNIT_ASSERT(expected_data == fake_stream->read_data_written());    
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(true,  option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());

    // Respond to the activate with IAC WILL TERMINAL_TYPE.
    fake_stream->write_data_to_read(
        list_of(odin::telnet::IAC)(odin::telnet::WILL)
               (odin::telnet::TERMINAL_TYPE));

    io_service.reset();
    io_service.run();

    // Dispatch any input to the input visitor
    BOOST_AUTO(rxcurrent, receive_array.begin());
    for(; rxcurrent != receive_array.end(); ++rxcurrent)
    {
        apply_visitor(input_visitor, *rxcurrent);
    }
    receive_array.clear();

    // The option should now be active.    
    CPPUNIT_ASSERT_EQUAL(true,  option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    
    // Now: send a request for the client's terminal type.
    option.send_request();
    
    io_service.reset();
    io_service.run();
    
    expected_data =
        list_of(odin::telnet::IAC)(odin::telnet::SB)
               (odin::telnet::TERMINAL_TYPE)
               (odin::telnet::TERMINAL_TYPE_SEND)
               (odin::telnet::IAC)(odin::telnet::SE);

    CPPUNIT_ASSERT(expected_data == fake_stream->read_data_written());
}

void telnet_options_terminal_type_client_fixture::test_response()
{
    boost::asio::io_service          io_service;
    shared_ptr<fake_byte_stream>     fake_stream(
        new fake_datastream<u8, u8>(io_service));
    shared_ptr<odin::telnet::stream> telnet_stream(
        new odin::telnet::stream(fake_stream, io_service));
    shared_ptr<odin::telnet::negotiation_router> telnet_negotiation_router(
        new odin::telnet::negotiation_router);
    shared_ptr<odin::telnet::subnegotiation_router> telnet_subnegotiation_router(
        new odin::telnet::subnegotiation_router);
        
    // Set up a lambda function that will always read from the stream and pass
    // it into the input visitor, and will then request another read.
    odin::telnet::input_visitor input_visitor(
        shared_ptr<odin::telnet::command_router>()
      , telnet_negotiation_router
      , telnet_subnegotiation_router
      , NULL);

    odin::telnet::stream::input_storage_type  receive_array;
    odin::telnet::stream::input_callback_type on_read;
    
    on_read = (
        bll::var(receive_array) = bll::_1
      , bll::bind(
            &odin::telnet::stream::async_read
          , *bll::var(telnet_stream), 1, ref(on_read)
        )
    );
    telnet_stream->async_read(1, on_read);
    
    odin::telnet::options::terminal_type_client option(
        telnet_stream, telnet_negotiation_router, telnet_subnegotiation_router);
    
    // Set up a callback to set the data whenever a new terminal type is
    // discovered.
    string result;
    function<void (string)> on_terminal_type_detected = (
        bll::var(result) = bll::_1
    );
    option.on_terminal_type_detected(on_terminal_type_detected);
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::TERMINAL_TYPE, option.get_option_id());
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    
    // First, activate the option.  This should send IAC DO TERMINAL_TYPE to
    // the server.
    option.activate();
    
    io_service.reset();
    io_service.run();
    
    fake_byte_stream::input_storage_type expected_data =
        list_of(odin::telnet::IAC)(odin::telnet::DO)
               (odin::telnet::TERMINAL_TYPE);
        
    CPPUNIT_ASSERT(expected_data == fake_stream->read_data_written());
    CPPUNIT_ASSERT_EQUAL(false, option.is_active());
    CPPUNIT_ASSERT_EQUAL(true,  option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());

    // Respond to the activate with IAC WILL TERMINAL_TYPE.
    fake_stream->write_data_to_read(
        list_of(odin::telnet::IAC)(odin::telnet::WILL)
               (odin::telnet::TERMINAL_TYPE));

    io_service.reset();
    io_service.run();

    // Dispatch any input to the input visitor
    BOOST_AUTO(rxcurrent, receive_array.begin());
    for(; rxcurrent != receive_array.end(); ++rxcurrent)
    {
        apply_visitor(input_visitor, *rxcurrent);
    }
    receive_array.clear();

    // The option should now be active.    
    CPPUNIT_ASSERT_EQUAL(true,  option.is_active());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_activation());
    CPPUNIT_ASSERT_EQUAL(false, option.is_negotiating_deactivation());
    
    // Now: send a request for the client's terminal type.
    option.send_request();
    
    io_service.reset();
    io_service.run();
    
    expected_data =
        list_of(odin::telnet::IAC)(odin::telnet::SB)
               (odin::telnet::TERMINAL_TYPE)
               (odin::telnet::TERMINAL_TYPE_SEND)
               (odin::telnet::IAC)(odin::telnet::SE);
    
    CPPUNIT_ASSERT(expected_data == fake_stream->read_data_written());

    // Now simulate a response.
    fake_stream->write_data_to_read(
        list_of(odin::telnet::IAC)(odin::telnet::SB)
               (odin::telnet::TERMINAL_TYPE)
               (odin::telnet::TERMINAL_TYPE_IS)('t')('e')('s')('t')
               (odin::telnet::IAC)(odin::telnet::SE));
    
    string expected_response = "test";
    
    io_service.reset();
    io_service.run();
    
    // Dispatch any input to the input visitor
    for(rxcurrent = receive_array.begin();
        rxcurrent != receive_array.end();
        ++rxcurrent)
    {
        apply_visitor(input_visitor, *rxcurrent);
    }
    receive_array.clear();
    
    CPPUNIT_ASSERT_EQUAL(expected_response, result);
}


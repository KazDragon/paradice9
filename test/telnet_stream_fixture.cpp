#include "telnet_stream_fixture.hpp"
#include "odin/telnet/stream.hpp"
#include "odin/telnet/initiated_negotiation.hpp"
#include "fake_datastream.hpp"
#include <boost/function.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/exceptions.hpp>
#include <boost/typeof/typeof.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_stream_fixture);

using namespace std;
using namespace boost;

namespace bll = boost::lambda;

typedef fake_datastream<odin::u8, odin::u8> fake_bytestream;

void telnet_stream_fixture::test_constructor()
{
    // Tests that a telnet stream can be constructed from a byte
    // stream and an io service.
    boost::asio::io_service           io_service;
    shared_ptr<odin::io::byte_stream> fake_stream(
        new fake_bytestream(io_service));
    
    odin::telnet::stream stream(fake_stream, io_service);
}

void telnet_stream_fixture::test_inheritance()
{
    // Tests that a telnet stream is itself a byte stream.
    boost::asio::io_service           io_service;
    shared_ptr<odin::io::byte_stream> fake_stream(
        new fake_bytestream(io_service));
    
    odin::telnet::stream stream(fake_stream, io_service);
    
    odin::io::byte_stream &byte_stream = stream;
    (void)byte_stream;
}

void telnet_stream_fixture::test_sync_read_normal()
{
    // Tests that a synchronous read of a normal character yields a normal
    // character.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 expected_data[] = { 'X' };
    odin::runtime_array<odin::u8> expected_array(expected_data);
    
    fake_stream->write_data_to_read(expected_data);
    
    odin::runtime_array<odin::u8> actual_array(
        stream.read(odin::runtime_array<odin::u8>::size_type(1)));
    
    CPPUNIT_ASSERT_EQUAL(expected_array, actual_array);
}

void telnet_stream_fixture::test_async_read_normal()
{
    // Tests that an asynchronous read of a normal character yields a normal
    // character.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);

    odin::u8 expected_data[] = { 'X' };
    odin::runtime_array<odin::u8> expected_array(expected_data);
    
    fake_stream->write_data_to_read(expected_data);
    
    odin::runtime_array<odin::u8> actual_array;
    boost::function<void (odin::runtime_array<odin::u8>)> callback = (
        bll::var(actual_array) = bll::_1
    );

    stream.async_read(
        odin::runtime_array<odin::u8>::size_type(1)
      , callback);
    
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(expected_array, actual_array);
}

void telnet_stream_fixture::test_available_empty()
{
    // If a stream is empty, then available() must return an unitialised
    // optional signifying that a read would block.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(false, available.is_initialized());
}

void telnet_stream_fixture::test_available_normal()
{
    // If a stream has only normal data in it, then available() must return
    // the size of that normal data.  This is the amount that may be read
    // without blocking.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = { 'a', 'b', 'c', 'd', 'e' };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(array.size(), available.get());
}

void telnet_stream_fixture::test_available_normal_iac()
{
    // If a stream has normal data followed by a lone IAC, then that final
    // IAC might be a telnet byte (or it may be an escaped IAC - we just don't
    // know yet).  Therefore, it must not be counted in the data available to
    // read.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = { 'a', 'b', 'c', 'd', 'e', odin::telnet::IAC };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(array.size() - 1, available.get());
}

void telnet_stream_fixture::test_available_normal_iac_iac()
{
    // If a stream has normal data followed by a pair of IAC, then that counts
    // as a single literal IAC byte.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = { 
        'a', 'b', 'c', 'd', 'e', odin::telnet::IAC, odin::telnet::IAC 
    };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(array.size() - 1, available.get());
}

void telnet_stream_fixture::test_available_normal_iac_iac_normal()
{
    // If a stream has normal data followed by a pair of IAC, then that counts
    // as a single literal IAC byte.  This must be included as normal data.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = { 
        'a', 'b', 'c', 'd', 'e', odin::telnet::IAC, odin::telnet::IAC, 'f' 
    };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(array.size() - 1, available.get());
}

void telnet_stream_fixture::test_available_negotiation()
{
    // If the stream begins with a negotiation and then stops, then reading
    // may block.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = {
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::ECHO
    };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(false, available.is_initialized());
}

void telnet_stream_fixture::test_available_negotiation_normal()
{
    // If the stream begins with a negotiation and then continues with normal
    // data, then reading will not block until that normal data is exhausted.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = {
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::ECHO
      , 'a', 'b', 'c'
    };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(array.size() - 3, available.get());
}

void telnet_stream_fixture::test_available_during_async_read()
{
    // The method available() usually makes a synchronous read on the 
    // underlying stream in order to buffer up data and give a decent
    // response.  However, if there is currently an asynchronous read
    // ongoing, then this may cause data to be read out of sequence.  
    // Therefore, we test that available() returns uninitialised (for
    // "may block" and that no data is buffered (by checking the side-
    // effect that data will appear in order).
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 partial_data0[] = { 'a' };
    fake_stream->write_data_to_read(partial_data0);
    
    odin::u32                     called = 0;
    odin::runtime_array<odin::u8> result_array;
    boost::function<void (odin::runtime_array<odin::u8>)> callback = (
        ++bll::var(called)
      , bll::var(result_array) = bll::_1
    );
    
    stream.async_read(2, callback);
    io_service.run();
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(false, available.is_initialized());
    
    odin::u8 partial_data1[] = { 'b' };
    
    fake_stream->write_data_to_read(partial_data1);
    io_service.reset();
    io_service.run();
    
    odin::u8 expected_data[] = { 'a', 'b' };
    odin::runtime_array<odin::u8> expected_array(expected_data);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    CPPUNIT_ASSERT_EQUAL(expected_array, result_array); 
}

void telnet_stream_fixture::test_available_during_async_callback()
{
    // The method available() usually makes a synchronous read on the
    // underlying stream in order to buffer up data and give a decent
    // response.  This is suspended while an asynchronous read is active.
    // However, on the callback from an asynchronous read, it may well be
    // that a call to available() is performed in order to determine how
    // much the next asynchronous read should attempt to read.  The call
    // to available() must be allowed to be performed here.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = { 'a', 'b', 'c', 'd' };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(data);
    
    odin::u32                                                 called = 0;
    odin::runtime_array<odin::u8>                             result_array;
    boost::optional<odin::runtime_array<odin::u8>::size_type> available;
    
    boost::function<void (odin::runtime_array<odin::u8>)> callback = (
        ++bll::var(called)
      , bll::var(result_array) = bll::_1
      , bll::var(available)    = bll::bind(
            &odin::telnet::stream::available, &stream)
    );
    
    stream.async_read(1, callback);
    io_service.run();
    
    odin::u8 expected_data[] = { 'a' };
    odin::runtime_array<odin::u8> expected_array(expected_data);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    CPPUNIT_ASSERT_EQUAL(expected_array, result_array);
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(array.size() - 1, available.get());
}

void telnet_stream_fixture::test_sync_read_normal_iac()
{
    // If the stream ends in an IAC byte, then that must not be included
    // (yet) in the available bytes to read.  Similarly, if that byte is
    // read, it must block.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = { 'a', 'b', 'c', odin::telnet::IAC };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(array.size() - 1, available.get());
    
    odin::runtime_array<odin::u8> expected_array(data, array.size() - 1);
    CPPUNIT_ASSERT_EQUAL(expected_array, stream.read(available.get()));

    CPPUNIT_ASSERT_THROW(
        stream.read(odin::telnet::stream::input_size_type(1))
      , blocking_error);
}

void telnet_stream_fixture::test_sync_read_normal_iac_iac()
{
    // If the stream has a pair of IAC bytes together, they must act
    // as and be read as a single IAC byte. 
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = { 
        'a', 'b', 'c', odin::telnet::IAC, odin::telnet::IAC 
    };
    odin::runtime_array<odin::u8> array(data);
    
    odin::u8 expected_data[] = {
        'a', 'b', 'c', odin::telnet::IAC 
    };
    odin::runtime_array<odin::u8> expected_array(expected_data);
    
    fake_stream->write_data_to_read(array);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(array.size() - 1, available.get());

    CPPUNIT_ASSERT_EQUAL(expected_array, stream.read(available.get()));
    
    CPPUNIT_ASSERT_THROW(
        stream.read(odin::telnet::stream::input_size_type(1))
      , blocking_error);
}    

void telnet_stream_fixture::test_sync_read_normal_iac_iac_normal()
{
    // If the stream has a pair of IAC bytes together, they must act
    // as and be read as a single IAC byte. 
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = { 
        'a', 'b', 'c', odin::telnet::IAC, odin::telnet::IAC, 'd'
    };
    odin::runtime_array<odin::u8> array(data);
    
    odin::u8 expected_data[] = {
        'a', 'b', 'c', odin::telnet::IAC, 'd'
    };
    odin::runtime_array<odin::u8> expected_array(expected_data);
    
    fake_stream->write_data_to_read(array);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(array.size() - 1, available.get());

    CPPUNIT_ASSERT_EQUAL(expected_array, stream.read(available.get()));
    
    CPPUNIT_ASSERT_THROW(
        stream.read(odin::telnet::stream::input_size_type(1))
      , blocking_error);
}    

void telnet_stream_fixture::test_sync_read_command()
{
    // If a stream contains only a telnet command, then reading from it
    // will block, but will announce to a listener that the command has been
    // read.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = {
        odin::telnet::IAC, odin::telnet::AYT
    };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    odin::telnet::command command = odin::telnet::command(0);
    function<void (odin::telnet::command)> command_handler = (
        bll::var(command) = bll::_1
    );
        
    stream.on_command(command_handler);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(false, available.is_initialized());
    
    CPPUNIT_ASSERT_THROW(
        stream.read(odin::telnet::stream::input_size_type(1))
      , blocking_error);
    
    CPPUNIT_ASSERT_EQUAL(command, odin::telnet::AYT);
}

void telnet_stream_fixture::test_sync_read_command_normal()
{
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = {
        odin::telnet::IAC, odin::telnet::AYT, 'a'
    };
    odin::runtime_array<odin::u8> array(data);
    
    odin::u8 expected_data[] = { 'a' };
    odin::runtime_array<odin::u8> expected_array(expected_data);

    fake_stream->write_data_to_read(array);
    
    odin::telnet::command command = odin::telnet::command(0);
    function<void (odin::telnet::command)> command_handler = (
        bll::var(command) = bll::_1
    );
        
    stream.on_command(command_handler);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), available.get());
    
    CPPUNIT_ASSERT_EQUAL(expected_array, stream.read(expected_array.size()));

    CPPUNIT_ASSERT_THROW(
        stream.read(odin::telnet::stream::input_size_type(1))
      , blocking_error);
    
    CPPUNIT_ASSERT_EQUAL(command, odin::telnet::AYT);
}

void telnet_stream_fixture::test_sync_read_negotiation()
{
    // If a stream contains only a telnet negotiation, then reading from it
    // will block, but will announce to a listener that the negotiation has 
    // been read.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = {
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::ECHO
    };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    odin::telnet::negotiation_request_type request   = 
        odin::telnet::negotiation_request_type(0);
    odin::telnet::option_id_type           option_id = 0;
    
    function<void (
        odin::telnet::negotiation_request_type
      , odin::telnet::option_id_type)> negotiation_handler = (
        bll::var(request)   = bll::_1
      , bll::var(option_id) = bll::_2
    );
        
    stream.on_negotiation(negotiation_handler);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(false, available.is_initialized());
    
    CPPUNIT_ASSERT_THROW(
        stream.read(odin::telnet::stream::input_size_type(1))
      , blocking_error);
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::WILL, request);
    CPPUNIT_ASSERT_EQUAL(odin::telnet::ECHO, option_id);
}

void telnet_stream_fixture::test_sync_read_negotiation_normal()
{
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = {
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::ECHO, 'a'
    };
    odin::runtime_array<odin::u8> array(data);
    
    odin::u8 expected_data[] = { 'a' };
    odin::runtime_array<odin::u8> expected_array(expected_data);

    fake_stream->write_data_to_read(array);
    
    odin::telnet::negotiation_request_type request   = 
        odin::telnet::negotiation_request_type(0);
    odin::telnet::option_id_type           option_id = 0;
    
    function<void (
        odin::telnet::negotiation_request_type
      , odin::telnet::option_id_type)> negotiation_handler = (
        bll::var(request)   = bll::_1
      , bll::var(option_id) = bll::_2
    );
        
    stream.on_negotiation(negotiation_handler);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), available.get());

    CPPUNIT_ASSERT_EQUAL(expected_array, stream.read(expected_array.size()));
    
    CPPUNIT_ASSERT_THROW(
        stream.read(odin::telnet::stream::input_size_type(1))
      , blocking_error);
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::WILL, request);
    CPPUNIT_ASSERT_EQUAL(odin::telnet::ECHO, option_id);
}

void telnet_stream_fixture::test_sync_read_subnegotiation()
{
    // If a stream contains only a telnet subnegotiation, then reading from it
    // will block, but will announce to a listener that the subnegotiation has 
    // been read.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = {
        odin::telnet::IAC, odin::telnet::SB, odin::telnet::ECHO
      , 'x', 'y'
      , odin::telnet::IAC, odin::telnet::SE
    };
    odin::runtime_array<odin::u8> array(data);

    fake_stream->write_data_to_read(array);

    odin::u8 expected_data[] = { 'x', 'y' };
    odin::telnet::option_id_type expected_id(odin::telnet::ECHO);
    odin::telnet::subnegotiation_type    expected_array(expected_data);

    odin::telnet::option_id_type subnegotiation_id(0);
    odin::telnet::subnegotiation_type    subnegotiation;

    boost::function<void (
        odin::telnet::option_id_type
      , odin::telnet::subnegotiation_type)> subnegotiation_handler = (
        bll::var(subnegotiation_id) = bll::_1
      , bll::var(subnegotiation) = bll::_2
    );

    stream.on_subnegotiation(subnegotiation_handler);

    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(false, available.is_initialized());

    CPPUNIT_ASSERT_THROW(
        stream.read(odin::telnet::stream::input_size_type(1))
      , blocking_error);

    CPPUNIT_ASSERT_EQUAL(expected_id, subnegotiation_id);
    CPPUNIT_ASSERT_EQUAL(expected_array, subnegotiation);
}

void telnet_stream_fixture::test_sync_read_subnegotiation_normal()
{
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = {
        odin::telnet::IAC, odin::telnet::SB, odin::telnet::ECHO
      , 'x', 'y'
      , odin::telnet::IAC, odin::telnet::SE
      , 'z'
    };
    odin::runtime_array<odin::u8> array(data);

    odin::u8 expected_data[] = { 'z' };
    odin::runtime_array<odin::u8> expected_array(expected_data);
    
    fake_stream->write_data_to_read(array);

    odin::u8 expected_subnegotiation_data[] = { 'x', 'y' };
    odin::telnet::option_id_type expected_id(odin::telnet::ECHO);
    odin::telnet::subnegotiation_type    expected_subnegotiation_array(
        expected_subnegotiation_data);

    odin::telnet::option_id_type subnegotiation_id(0);
    odin::telnet::subnegotiation_type    subnegotiation;

    boost::function<void (
        odin::telnet::option_id_type
      , odin::telnet::subnegotiation_type)> subnegotiation_handler = (
        bll::var(subnegotiation_id) = bll::_1
      , bll::var(subnegotiation) = bll::_2
    );

    stream.on_subnegotiation(subnegotiation_handler);

    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), available.get());
    
    CPPUNIT_ASSERT_EQUAL(expected_array, stream.read(expected_array.size()));

    CPPUNIT_ASSERT_THROW(
        stream.read(odin::telnet::stream::input_size_type(1))
      , blocking_error);

    CPPUNIT_ASSERT_EQUAL(expected_id, subnegotiation_id);
    CPPUNIT_ASSERT_EQUAL(expected_subnegotiation_array, subnegotiation);
}

void telnet_stream_fixture::test_async_read_normal_iac()
{
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = { 'a', 'b', 'c', odin::telnet::IAC };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(array.size() - 1, available.get());
    
    odin::runtime_array<odin::u8> expected_array(data, array.size() - 1);
    CPPUNIT_ASSERT_EQUAL(expected_array, stream.read(available.get()));

    CPPUNIT_ASSERT_THROW(
        stream.read(odin::telnet::stream::input_size_type(1))
      , blocking_error);
}

void telnet_stream_fixture::test_async_read_normal_iac_iac()
{
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = { 
        'a', 'b', 'c', odin::telnet::IAC, odin::telnet::IAC 
    };
    odin::runtime_array<odin::u8> array(data);
    
    odin::u8 expected_data[] = {
        'a', 'b', 'c', odin::telnet::IAC 
    };
    odin::runtime_array<odin::u8> expected_array(expected_data);
    
    fake_stream->write_data_to_read(array);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), available.get());

    odin::runtime_array<odin::u8> actual_array;
    boost::function<void (odin::runtime_array<odin::u8>)> callback = (
        bll::var(actual_array) = bll::_1
    );
    
    stream.async_read(available.get(), callback);

    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(expected_array, actual_array);
    
    stream.async_read(
        odin::telnet::stream::input_size_type(1)
      , bll::throw_exception(bll::bind(bll::constructor<blocking_error>())));

    io_service.reset();
    io_service.run();    
}

void telnet_stream_fixture::test_async_read_normal_iac_iac_normal()
{
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = { 
        'a', 'b', 'c', odin::telnet::IAC, odin::telnet::IAC, 'd'
    };
    odin::runtime_array<odin::u8> array(data);
    
    odin::u8 expected_data[] = {
        'a', 'b', 'c', odin::telnet::IAC, 'd' 
    };
    odin::runtime_array<odin::u8> expected_array(expected_data);
    
    fake_stream->write_data_to_read(array);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), available.get());

    odin::runtime_array<odin::u8> actual_array;
    boost::function<void (odin::runtime_array<odin::u8>)> callback = (
        bll::var(actual_array) = bll::_1
    );
    
    stream.async_read(available.get(), callback);

    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(expected_array, actual_array);
    
    stream.async_read(
        odin::telnet::stream::input_size_type(1)
      , bll::throw_exception(bll::bind(bll::constructor<blocking_error>())));

    io_service.reset();
    io_service.run();    
}

void telnet_stream_fixture::test_async_read_command()
{
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = { 
        odin::telnet::IAC, odin::telnet::AYT
    };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(false, available.is_initialized());

    odin::runtime_array<odin::u8> actual_array;
    boost::function<void (odin::runtime_array<odin::u8>)> callback = (
        bll::var(actual_array) = bll::_1
    );

    odin::telnet::command command = odin::telnet::command(0);
    function<void (odin::telnet::command)> command_handler = (
        bll::var(command) = bll::_1
    );
        
    stream.on_command(command_handler);
    
    stream.async_read(odin::telnet::stream::input_size_type(1), callback);

    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(true, actual_array.empty());
    CPPUNIT_ASSERT_EQUAL(odin::telnet::AYT, command);
        
    stream.async_read(
        odin::telnet::stream::input_size_type(1)
      , bll::throw_exception(bll::bind(bll::constructor<blocking_error>())));

    io_service.reset();
    io_service.run();    
}

void telnet_stream_fixture::test_async_read_command_normal()
{
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = { 
        odin::telnet::IAC, odin::telnet::AYT, 'a'
    };
    odin::runtime_array<odin::u8> array(data);
    
    odin::u8 expected_data[] = { 'a' };
    odin::runtime_array<odin::u8> expected_array(expected_data);
    
    fake_stream->write_data_to_read(array);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), available.get());

    odin::runtime_array<odin::u8> actual_array;
    boost::function<void (odin::runtime_array<odin::u8>)> callback = (
        bll::var(actual_array) = bll::_1
    );

    odin::telnet::command command = odin::telnet::command(0);
    function<void (odin::telnet::command)> command_handler = (
        bll::var(command) = bll::_1
    );
        
    stream.on_command(command_handler);
    stream.async_read(expected_array.size(), callback);

    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(expected_array, actual_array);
    CPPUNIT_ASSERT_EQUAL(odin::telnet::AYT, command);
        
    stream.async_read(
        odin::telnet::stream::input_size_type(1)
      , bll::throw_exception(bll::bind(bll::constructor<blocking_error>())));

    io_service.reset();
    io_service.run();    
}

void telnet_stream_fixture::test_async_read_negotiation()
{
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = { 
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::ECHO
    };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(false, available.is_initialized());

    odin::runtime_array<odin::u8> actual_array;
    boost::function<void (odin::runtime_array<odin::u8>)> callback = (
        bll::var(actual_array) = bll::_1
    );

    odin::telnet::negotiation_request_type request   = 
        odin::telnet::negotiation_request_type(0);
    odin::telnet::option_id_type           option_id = 0;
    
    function<void (
        odin::telnet::negotiation_request_type
      , odin::telnet::option_id_type)> negotiation_handler = (
        bll::var(request)   = bll::_1
      , bll::var(option_id) = bll::_2
    );
        
    stream.on_negotiation(negotiation_handler);
    
    stream.async_read(odin::telnet::stream::input_size_type(1), callback);

    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(true, actual_array.empty());
    CPPUNIT_ASSERT_EQUAL(odin::telnet::WILL, request);
    CPPUNIT_ASSERT_EQUAL(odin::telnet::ECHO, option_id);
        
    stream.async_read(
        odin::telnet::stream::input_size_type(1)
      , bll::throw_exception(bll::bind(bll::constructor<blocking_error>())));

    io_service.reset();
    io_service.run();    
}

void telnet_stream_fixture::test_async_read_negotiation_normal()
{
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = { 
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::ECHO, 'a'
    };
    odin::runtime_array<odin::u8> array(data);
    
    odin::u8 expected_data[] = { 'a' };
    odin::runtime_array<odin::u8> expected_array(expected_data);
    
    fake_stream->write_data_to_read(array);
    
    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), available.get());

    odin::runtime_array<odin::u8> actual_array;
    boost::function<void (odin::runtime_array<odin::u8>)> callback = (
        bll::var(actual_array) = bll::_1
    );

    odin::telnet::negotiation_request_type request   = 
        odin::telnet::negotiation_request_type(0);
    odin::telnet::option_id_type           option_id = 0;
    
    function<void (
        odin::telnet::negotiation_request_type
      , odin::telnet::option_id_type)> negotiation_handler = (
        bll::var(request)   = bll::_1
      , bll::var(option_id) = bll::_2
    );
        
    stream.on_negotiation(negotiation_handler);
    stream.async_read(expected_array.size(), callback);

    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(expected_array, actual_array);
    CPPUNIT_ASSERT_EQUAL(odin::telnet::WILL, request);
    CPPUNIT_ASSERT_EQUAL(odin::telnet::ECHO, option_id);
        
    stream.async_read(
        odin::telnet::stream::input_size_type(1)
      , bll::throw_exception(bll::bind(bll::constructor<blocking_error>())));

    io_service.reset();
    io_service.run();    
}

void telnet_stream_fixture::test_async_read_subnegotiation()
{
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = {
        odin::telnet::IAC, odin::telnet::SB, odin::telnet::ECHO
      , 'x', 'y'
      , odin::telnet::IAC, odin::telnet::SE
    };
    odin::runtime_array<odin::u8> array(data);

    fake_stream->write_data_to_read(array);

    BOOST_AUTO(available, stream.available());
    CPPUNIT_ASSERT_EQUAL(false, available.is_initialized());
    
    odin::u8 expected_data[] = { 'x', 'y' };
    odin::telnet::option_id_type expected_id(odin::telnet::ECHO);
    odin::telnet::subnegotiation_type    expected_array(expected_data);

    odin::telnet::option_id_type subnegotiation_id(0);
    odin::telnet::subnegotiation_type    subnegotiation;

    boost::function<void (
        odin::telnet::option_id_type
      , odin::telnet::subnegotiation_type)> subnegotiation_handler = (
        bll::var(subnegotiation_id) = bll::_1
      , bll::var(subnegotiation) = bll::_2
    );

    odin::runtime_array<odin::u8> actual_array;
    boost::function<void (odin::runtime_array<odin::u8>)> callback = (
        bll::var(actual_array) = bll::_1
    );
      
    stream.on_subnegotiation(subnegotiation_handler);
    stream.async_read(odin::telnet::stream::input_size_type(1), callback);

    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(true, actual_array.empty());
    CPPUNIT_ASSERT_EQUAL(expected_id, subnegotiation_id);
    CPPUNIT_ASSERT_EQUAL(expected_array, subnegotiation);
        
    stream.async_read(
        odin::telnet::stream::input_size_type(1)
      , bll::throw_exception(bll::bind(bll::constructor<blocking_error>())));

    io_service.reset();
    io_service.run();    
}

void telnet_stream_fixture::test_async_read_subnegotiation_normal()
{
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);
    
    odin::u8 data[] = {
        odin::telnet::IAC, odin::telnet::SB, odin::telnet::ECHO
      , 'x', 'y'
      , odin::telnet::IAC, odin::telnet::SE
      , 'z'
    };
    odin::runtime_array<odin::u8> array(data);

    odin::u8 expected_data[] = { 'z' };
    odin::runtime_array<odin::u8> expected_array(expected_data);
    
    fake_stream->write_data_to_read(array);

    odin::u8 expected_subnegotiation_data[] = { 'x', 'y' };
    odin::telnet::option_id_type expected_id(odin::telnet::ECHO);
    odin::telnet::subnegotiation_type    expected_subnegotiation_array(
        expected_subnegotiation_data);

    odin::telnet::option_id_type subnegotiation_id(0);
    odin::telnet::subnegotiation_type    subnegotiation;

    boost::function<void (
        odin::telnet::option_id_type
      , odin::telnet::subnegotiation_type)> subnegotiation_handler = (
        bll::var(subnegotiation_id) = bll::_1
      , bll::var(subnegotiation) = bll::_2
    );

    stream.on_subnegotiation(subnegotiation_handler);
    
    odin::runtime_array<odin::u8> actual_array;
    boost::function<void (odin::runtime_array<odin::u8>)> callback = (
        bll::var(actual_array) = bll::_1
    );
    
    stream.async_read(expected_array.size(), callback);

    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(expected_array, actual_array);
    CPPUNIT_ASSERT_EQUAL(expected_id, subnegotiation_id);
    CPPUNIT_ASSERT_EQUAL(expected_subnegotiation_array, subnegotiation);
        
    stream.async_read(
        odin::telnet::stream::input_size_type(1)
      , bll::throw_exception(bll::bind(bll::constructor<blocking_error>())));

    io_service.reset();
    io_service.run();    
}

void telnet_stream_fixture::test_sync_write_normal()
{
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);

    odin::u8 data[] = { 'a', 'b', 'c' };
    odin::runtime_array<odin::u8> array(data);

    CPPUNIT_ASSERT_EQUAL(array.size(), stream.write(array));
    CPPUNIT_ASSERT_EQUAL(array, fake_stream->read_data_written());
}

void telnet_stream_fixture::test_sync_write_normal_iac()
{
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);

    odin::u8 data[] = { 'a', 'b', 'c', odin::telnet::IAC };
    odin::runtime_array<odin::u8> array(data);

    odin::u8 expected_data[] = {
        'a', 'b', 'c', odin::telnet::IAC, odin::telnet::IAC
    };
    odin::runtime_array<odin::u8> expected_array(expected_data);

    CPPUNIT_ASSERT_EQUAL(array.size(), stream.write(array));
    CPPUNIT_ASSERT_EQUAL(expected_array, fake_stream->read_data_written());
}

void telnet_stream_fixture::test_async_write_normal()
{
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);

    odin::u8 data[] = { 'a', 'b', 'c'  };
    odin::runtime_array<odin::u8> array(data);

    odin::runtime_array<odin::u8>::size_type amount_sent = 0;
    odin::telnet::stream::output_callback_type callback = (
        bll::var(amount_sent) = bll::_1
    );

    stream.async_write(data, callback);

    io_service.run();

    CPPUNIT_ASSERT_EQUAL(array.size(), amount_sent);
    CPPUNIT_ASSERT_EQUAL(array, fake_stream->read_data_written());
}

void telnet_stream_fixture::test_async_write_normal_iac()
{
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);

    odin::u8 data[] = { 
        'a', 'b', 'c', odin::telnet::IAC
    };
    odin::runtime_array<odin::u8> array(data);

    odin::u8 expected_data[] = {
        'a', 'b', 'c', odin::telnet::IAC, odin::telnet::IAC
    };
    odin::runtime_array<odin::u8> expected_array(expected_data);

    odin::runtime_array<odin::u8>::size_type amount_sent = 0;
    odin::telnet::stream::output_callback_type callback = (
        bll::var(amount_sent) = bll::_1
    );

    stream.async_write(data, callback);

    io_service.run();

    CPPUNIT_ASSERT_EQUAL(array.size(), amount_sent);
    CPPUNIT_ASSERT_EQUAL(expected_array, fake_stream->read_data_written());
}

void telnet_stream_fixture::test_send_command()
{
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);

    odin::u8 expected_data[] = {
        odin::telnet::IAC, odin::telnet::AYT
    };
    odin::runtime_array<odin::u8> expected_array(expected_data);

    stream.send_command(odin::telnet::AYT);

    CPPUNIT_ASSERT_EQUAL(expected_array, fake_stream->read_data_written());
}

void telnet_stream_fixture::test_send_negotiation()
{
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);

    odin::u8 expected_data[] = {
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::ECHO
    };
    odin::runtime_array<odin::u8> expected_array(expected_data);

    stream.send_negotiation(odin::telnet::WILL, odin::telnet::ECHO);

    CPPUNIT_ASSERT_EQUAL(expected_array, fake_stream->read_data_written());
}

void telnet_stream_fixture::test_send_subnegotiation()
{
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream        stream(fake_stream, io_service);

    odin::telnet::option_id_type subnegotiation_id = 
        odin::telnet::ECHO;

    odin::u8 subnegotiation_data[] = {
        'x', 'y'
    };
    odin::runtime_array<odin::u8> subnegotiation_array(subnegotiation_data);

    odin::u8 expected_data[] = {
        odin::telnet::IAC, odin::telnet::SB, odin::telnet::ECHO
      , 'x', 'y'
      , odin::telnet::IAC, odin::telnet::SE
    };
    odin::runtime_array<odin::u8> expected_array(expected_data);

    stream.send_subnegotiation(subnegotiation_id, subnegotiation_array);

    CPPUNIT_ASSERT_EQUAL(expected_array, fake_stream->read_data_written());
}

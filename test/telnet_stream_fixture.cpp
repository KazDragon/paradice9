#include "telnet_stream_fixture.hpp"
#include "odin/telnet/stream.hpp"
#include "odin/telnet/protocol.hpp"
#include "fake_datastream.hpp"
#include <boost/assign/list_of.hpp>
#include <boost/function.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/typeof/typeof.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_stream_fixture);

using namespace std;
using namespace boost;
using namespace boost::assign;

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
    // Tests that a telnet stream is a bidirectional stream of telnet symbols.
    typedef variant<
        odin::telnet::command_type
      , odin::telnet::negotiation_type
      , odin::telnet::subnegotiation_type
      , string
    > telnet_io_type;

    boost::asio::io_service           io_service;
    shared_ptr<odin::io::byte_stream> fake_stream(
        new fake_bytestream(io_service));

    odin::telnet::stream stream(fake_stream, io_service);
    
    odin::io::datastream<
        telnet_io_type
      , telnet_io_type
    > &ref = stream;
    
    (void)ref;
}

// Now that the particulars of the stream have been established, we can
// make typedefs to them for ease of use.
typedef odin::telnet::stream::input_size_type     input_size_type;
typedef odin::telnet::stream::input_value_type    input_value_type;
typedef odin::telnet::stream::input_storage_type  input_storage_type;
typedef odin::telnet::stream::input_callback_type input_callback_type;

void telnet_stream_fixture::test_sync_read_normal()
{
    // Tests that a synchronous read of a normal character yields a normal
    // character.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream       stream(fake_stream, io_service);

    fake_bytestream::input_storage_type data = list_of('X');    
    
    fake_stream->write_data_to_read(data);
    
    input_storage_type actual_variant_array(stream.read(input_size_type(1)));
    
    string expected_string("X");
    
    input_value_type expected_variant(expected_string);
    input_storage_type expected_variant_array = list_of(expected_variant);

    CPPUNIT_ASSERT_EQUAL(
        expected_variant_array.size()
      , actual_variant_array.size());
    
    BOOST_AUTO(element0, get<string>(actual_variant_array[0]));
    
    CPPUNIT_ASSERT_EQUAL(expected_string, element0);
}

void telnet_stream_fixture::test_sync_read_normal_iac()
{
    // Test that a stream followed by a lone IAC returns all the data up
    // to that IAC.  The final byte may turn out to be the beginning of
    // a telnet command, or might end up an escaped IAC.  It's not known,
    // so it must not be returned.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream       stream(fake_stream, io_service);

    fake_bytestream::input_storage_type data =
        list_of('a')('b')('c')('d')('e')(odin::telnet::IAC);
    
    fake_stream->write_data_to_read(data);
    input_storage_type actual_variant_array(stream.read(input_size_type(1)));
    
    string expected_string("abcde");
    
    input_value_type expected_variant(expected_string);
    input_storage_type expected_variant_array = list_of(expected_variant);

    CPPUNIT_ASSERT_EQUAL(
        expected_variant_array.size()
      , actual_variant_array.size());
    
    BOOST_AUTO(element0, get<string>(actual_variant_array[0]));
    
    CPPUNIT_ASSERT_EQUAL(expected_string, element0);
}

void telnet_stream_fixture::test_sync_read_normal_iac_iac()
{
    // Test that a stream followed by a double IAC returns all the data 
    // including the escaped IAC.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream       stream(fake_stream, io_service);
    
    fake_bytestream::input_storage_type data =
        list_of('a')('b')('c')('d')('e')
               (odin::telnet::IAC)(odin::telnet::IAC);

    fake_stream->write_data_to_read(data);
    input_storage_type actual_variant_array(stream.read(input_size_type(1)));
    
    string expected_string("abcde\xFF");
    
    input_value_type expected_variant(expected_string);
    input_storage_type expected_variant_array = list_of(expected_variant);

    CPPUNIT_ASSERT_EQUAL(
        expected_variant_array.size()
      , actual_variant_array.size());
    
    BOOST_AUTO(element0, get<string>(actual_variant_array[0]));
    
    CPPUNIT_ASSERT_EQUAL(expected_string, element0);
}

void telnet_stream_fixture::test_sync_read_normal_iac_then_iac()
{
    // Test that a stream followed by a lone IAC returns all the data up
    // to that IAC.  The final byte may turn out to be the beginning of
    // a telnet command, or might end up an escaped IAC.  It's not known,
    // so it must not be returned.
    // Then check that adding a second lone IAC and reading again returns
    // the escaped IAC.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream       stream(fake_stream, io_service);
    
    fake_bytestream::input_storage_type data =
        list_of('a')('b')('c')('d')('e')(odin::telnet::IAC);

    fake_stream->write_data_to_read(data);
    
    input_storage_type actual_variant_array(stream.read(input_size_type(1)));
    
    string expected_string("abcde");
    
    input_value_type expected_variant(expected_string);
    input_storage_type expected_variant_array = list_of(expected_variant);

    CPPUNIT_ASSERT_EQUAL(
        input_storage_type::size_type(1), actual_variant_array.size());
    
    BOOST_AUTO(element0, get<string>(actual_variant_array[0]));
    
    CPPUNIT_ASSERT_EQUAL(expected_string, element0);

    data = list_of(odin::telnet::IAC);
    fake_stream->write_data_to_read(data);

    actual_variant_array = stream.read(input_size_type(1));

    expected_string = "\xFF";
    
    expected_variant = expected_string;
    expected_variant_array = list_of(expected_variant);

    CPPUNIT_ASSERT_EQUAL(
        expected_variant_array.size()
      , actual_variant_array.size());
    
    element0 = get<string>(actual_variant_array[0]);
    
    CPPUNIT_ASSERT_EQUAL(expected_string, element0);
}

void telnet_stream_fixture::test_sync_read_command()
{
    // Tests that a synchronous read of a command yields a command.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream       stream(fake_stream, io_service);
    
    fake_bytestream::input_storage_type data =
        list_of(odin::telnet::IAC)(odin::telnet::NOP);

    fake_stream->write_data_to_read(data);
    input_storage_type actual_variant_array(stream.read(input_size_type(1)));
    
    odin::telnet::command_type expected_command = odin::telnet::NOP;
    
    input_value_type expected_variant(expected_command);
    input_storage_type expected_variant_array = list_of(expected_variant);

    CPPUNIT_ASSERT_EQUAL(
        expected_variant_array.size()
      , actual_variant_array.size());
    
    BOOST_AUTO(
        element0
      , get<odin::telnet::command_type>(actual_variant_array[0]));
    
    CPPUNIT_ASSERT(expected_command == element0);
}

void telnet_stream_fixture::test_sync_read_normal_command()
{
    // Tests that a synchronous read of a normal character string and
    // a command yields that.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream       stream(fake_stream, io_service);
    
    fake_bytestream::input_storage_type data =
        list_of('a')(odin::telnet::IAC)(odin::telnet::NOP);

    fake_stream->write_data_to_read(data);
    input_storage_type actual_variant_array(stream.read(input_size_type(2)));
    
    string expected_string("a");
    
    odin::telnet::command_type expected_command = odin::telnet::NOP;
    
    input_value_type expected_variant0(expected_string);
    input_value_type expected_variant1(expected_command);
    
    input_storage_type expected_variant_array(2);
    expected_variant_array[0] = expected_variant0;
    expected_variant_array[1] = expected_variant1;

    CPPUNIT_ASSERT_EQUAL(
        expected_variant_array.size()
      , actual_variant_array.size());
    
    BOOST_AUTO(
        element0
      , get<string>(actual_variant_array[0]));
    
    CPPUNIT_ASSERT_EQUAL(expected_string, element0);
    
    BOOST_AUTO(
        element1
      , get<odin::telnet::command_type>(actual_variant_array[1]));
    
    CPPUNIT_ASSERT_EQUAL(expected_command, element1);
}

void telnet_stream_fixture::test_sync_read_partial()
{
    // Tests that a synchronous read of part of an input stream leaves the
    // remainder available to be read.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream       stream(fake_stream, io_service);
    
    fake_bytestream::input_storage_type data =
        list_of('a')
               (odin::telnet::IAC)(odin::telnet::NOP)
               ('b')('c');
    
    fake_stream->write_data_to_read(data);
    input_storage_type actual_variant_array(stream.read(input_size_type(2)));
    
    {
        string expected_string0("a");
        
        odin::telnet::command_type expected_command1 = odin::telnet::NOP;
        
        input_value_type expected_variant0 = expected_string0;
        input_value_type expected_variant1 = expected_command1;
        
        input_storage_type expected_variant_array(2);
        expected_variant_array[0] = expected_variant0;
        expected_variant_array[1] = expected_variant1;
        
        CPPUNIT_ASSERT_EQUAL(
            expected_variant_array.size()
          , actual_variant_array.size());
        
        BOOST_AUTO(
            actual_element0
          , get<string>(actual_variant_array[0]));
        
        CPPUNIT_ASSERT_EQUAL(expected_string0, actual_element0);
        
        BOOST_AUTO(
            actual_element1
          , get<odin::telnet::command_type>(actual_variant_array[1]));
        
        CPPUNIT_ASSERT_EQUAL(expected_command1, actual_element1);
    }
    
    actual_variant_array = stream.read(input_size_type(1));
    
    {
        string           expected_string2("bc");
        input_value_type expected_variant2(expected_string2);
        input_storage_type expected_variant_array = list_of(expected_variant2);
        
        CPPUNIT_ASSERT_EQUAL(
            expected_variant_array.size()
          , actual_variant_array.size());
        
        BOOST_AUTO(
            actual_element2
          , get<string>(actual_variant_array[0]));
        
        CPPUNIT_ASSERT_EQUAL(expected_string2, actual_element2);
    }
}

void telnet_stream_fixture::test_sync_read_many()
{
    // Tests a synchronous read of a stream of many parts.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream       stream(fake_stream, io_service);
    
    fake_bytestream::input_storage_type data =
        list_of('a')('b')('c')
               (odin::telnet::IAC)(odin::telnet::NOP)
               (odin::telnet::IAC)(odin::telnet::AYT)
               (odin::telnet::IAC)(odin::telnet::SB)
                   (odin::telnet::NAWS)
                   (0)(1)(2)(3)
               (odin::telnet::IAC)(odin::telnet::SE)
               ('d')('e')('f');
    
    fake_stream->write_data_to_read(data);
    input_storage_type actual_variant_array(stream.read(input_size_type(5)));
    
    string           expected_string0("abc");
    input_value_type expected_variant0(expected_string0);

    odin::telnet::command_type expected_command1 = odin::telnet::NOP;
    input_value_type expected_variant1(expected_command1);
    
    odin::telnet::command_type expected_command2 = odin::telnet::AYT;
    input_value_type expected_variant2(expected_command2);
    
    odin::u8 subnegotiation3_data[] = { 0, 1, 2, 3 };
    vector<odin::u8> subnegotiation3_array(
        subnegotiation3_data
      , subnegotiation3_data + 4);
    odin::telnet::subnegotiation_type expected_subnegotiation3;
    expected_subnegotiation3.option_id_ = odin::telnet::NAWS;
    expected_subnegotiation3.content_   = subnegotiation3_array;
    input_value_type expected_variant3(expected_subnegotiation3);
    
    string           expected_string4("def");
    input_value_type expected_variant4(expected_string4);
    
    input_storage_type expected_variant_array(5);
    expected_variant_array[0] = expected_variant0;
    expected_variant_array[1] = expected_variant1;
    expected_variant_array[2] = expected_variant2;
    expected_variant_array[3] = expected_variant3;
    expected_variant_array[4] = expected_variant4;
    
    CPPUNIT_ASSERT_EQUAL(
        expected_variant_array.size()
      , actual_variant_array.size());
    
    BOOST_AUTO(
        element0
      , get<string>(actual_variant_array[0]));
    
    CPPUNIT_ASSERT_EQUAL(element0, expected_string0);
    
    BOOST_AUTO(
        element1
      , get<odin::telnet::command_type>(actual_variant_array[1]));
    
    CPPUNIT_ASSERT_EQUAL(element1, expected_command1);
    
    BOOST_AUTO(
        element2
      , get<odin::telnet::command_type>(actual_variant_array[2]));
    
    CPPUNIT_ASSERT_EQUAL(element2, expected_command2);
    
    BOOST_AUTO(
        element3
      , get<odin::telnet::subnegotiation_type>(actual_variant_array[3]));
    
    CPPUNIT_ASSERT_EQUAL(
        expected_subnegotiation3.option_id_
      , element3.option_id_);
    
    CPPUNIT_ASSERT(
        expected_subnegotiation3.content_ == element3.content_);
    
    BOOST_AUTO(
        element4
      , get<string>(actual_variant_array[4]));
    
    CPPUNIT_ASSERT_EQUAL(expected_string4, element4);
}

void telnet_stream_fixture::test_async_read_normal()
{
    // Tests an asynchronous read of a stream of normal characters.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream       stream(fake_stream, io_service);
    
    fake_bytestream::input_storage_type data =
        list_of('a')('b')('c');
    
    fake_stream->write_data_to_read(data);
    
    input_storage_type actual_variant_array;
    odin::u32        called = 0;
    
    input_callback_type input_callback = (
        bll::var(actual_variant_array) = bll::_1
      , ++bll::var(called)
    );
    
    stream.async_read(input_size_type(1), input_callback);
    
    // The callback may not occur synchronously.
    CPPUNIT_ASSERT(actual_variant_array.empty());
    
    io_service.run();
    
    // The callback must now have occurred.
    string           expected_string("abc");
    input_value_type expected_variant(expected_string);
    input_storage_type expected_variant_array = list_of(expected_variant);
    
    CPPUNIT_ASSERT_EQUAL(
        expected_variant_array.size()
      , actual_variant_array.size());
    
    CPPUNIT_ASSERT_EQUAL(
        expected_string
      , get<string>(actual_variant_array[0]));
}

void telnet_stream_fixture::test_available_empty()
{
    // Tests that available() on a stream containing no data yields a
    // "would block" response.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream       stream(fake_stream, io_service);
    
    CPPUNIT_ASSERT(!stream.available().is_initialized());
}

void telnet_stream_fixture::test_available_iac()
{
    // Tests that available() on a stream containing only an IAC yields a
    // "would block" response, because there is no complete token on the
    // stream.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream       stream(fake_stream, io_service);
    
    fake_bytestream::input_storage_type data =
        list_of(odin::telnet::IAC);

    fake_stream->write_data_to_read(data);
    
    CPPUNIT_ASSERT(!stream.available().is_initialized());
}

void telnet_stream_fixture::test_available_normal()
{
    // Tests that available() on a stream containing normal data will yield
    // a result of 1, being the one element that is an array of those data.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream       stream(fake_stream, io_service);
    
    fake_bytestream::input_storage_type data =
        list_of('a')('b')('c')('d')('e');

    fake_stream->write_data_to_read(data);
    
    CPPUNIT_ASSERT(stream.available().is_initialized());
    CPPUNIT_ASSERT_EQUAL(input_size_type(1), stream.available().get());
}

void telnet_stream_fixture::test_available_many()
{
    // Tests that available() on a stream containing many pieces of data will
    // yield an appropriate result.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream       stream(fake_stream, io_service);
    
    fake_bytestream::input_storage_type data =
        list_of('a')('b')('c')('e')
               (odin::telnet::IAC)(odin::telnet::WILL)(odin::telnet::NAWS)
               ('d')(odin::telnet::IAC)(odin::telnet::IAC)
               (odin::telnet::IAC)(odin::telnet::NOP)
               (odin::telnet::IAC)(odin::telnet::DONT)(odin::telnet::ECHO)
               ('e')('f');

    fake_stream->write_data_to_read(data);
    
    CPPUNIT_ASSERT(stream.available().is_initialized());
    CPPUNIT_ASSERT_EQUAL(input_size_type(6), stream.available().get());
}

void telnet_stream_fixture::test_available_during_async_read()
{
    // Tests that available() on a stream during an async read yields a
    // "would block."
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream       stream(fake_stream, io_service);

    fake_bytestream::input_storage_type data =
        list_of('a')('b')
               (odin::telnet::IAC)(odin::telnet::NOP);
               
    fake_stream->write_data_to_read(data);
    
    stream.async_read(1, NULL);    
    
    CPPUNIT_ASSERT(!stream.available().is_initialized());
}

void telnet_stream_fixture::test_available_during_async_callback()
{
    // Tests that available() on a stream during an async read yields a
    // "would block."
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream       stream(fake_stream, io_service);

    fake_bytestream::input_storage_type data =
        list_of('a')('b')
               (odin::telnet::IAC)(odin::telnet::NOP)
               ('c')('d');
            
    fake_stream->write_data_to_read(data);

    optional<input_size_type> available;
    input_callback_type callback = (
        bll::var(available) 
            = bll::bind(&odin::telnet::stream::available, &stream)
    );
    
    stream.async_read(1, callback);    
    
    io_service.run();
    
    CPPUNIT_ASSERT(available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(input_size_type(2), available.get());
}

typedef odin::telnet::stream::output_size_type     output_size_type;
typedef odin::telnet::stream::output_value_type    output_value_type;
typedef odin::telnet::stream::output_storage_type  output_storage_type;
typedef odin::telnet::stream::output_callback_type output_callback_type;

void telnet_stream_fixture::test_sync_write_nothing()
{
    // Tests that writing nothing to a telnet stream writes nothing to
    // the underlying straem.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream       stream(fake_stream, io_service);
    
    string            element0;
    output_value_type variant0(element0);
    output_storage_type variant_array = list_of(variant0);
    
    stream.write(variant_array);
    
    fake_bytestream::output_storage_type expected_data;
    CPPUNIT_ASSERT(expected_data == fake_stream->read_data_written());
}

void telnet_stream_fixture::test_sync_write_normal()
{
    // Tests that writing nothing to a telnet stream writes nothing to
    // the underlying stream.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream       stream(fake_stream, io_service);
    
    string            element0("abc");
    output_value_type variant0(element0);
    output_storage_type variant_array = list_of(variant0);
    
    stream.write(variant_array);
    
    fake_bytestream::output_storage_type expected_data =
        list_of('a')('b')('c');
    
    CPPUNIT_ASSERT(expected_data == fake_stream->read_data_written());
}

void telnet_stream_fixture::test_sync_write_normal_iac()
{
    // Tests that writing nothing to a telnet stream writes nothing to
    // the underlying stream.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream       stream(fake_stream, io_service);
    
    string            element0("abc\xFF");
    output_value_type variant0(element0);
    output_storage_type variant_array = list_of(variant0);
    
    stream.write(variant_array);

    fake_bytestream::output_storage_type expected_data =
        list_of('a')('b')('c')(odin::telnet::IAC)(odin::telnet::IAC);    
    
    CPPUNIT_ASSERT(expected_data == fake_stream->read_data_written());
}

void telnet_stream_fixture::test_async_write_normal()
{
    // Tests that writing nothing to a telnet stream writes nothing to
    // the underlying stream.
    boost::asio::io_service     io_service;
    shared_ptr<fake_bytestream> fake_stream(
        new fake_bytestream(io_service));
    odin::telnet::stream       stream(fake_stream, io_service);
    
    string            element0("abc");
    output_value_type variant0(element0);
    output_storage_type variant_array = list_of(variant0);
    
    odin::u32        called = 0;
    output_size_type output_size = 0;
    boost::function<void (output_size_type)> callback = (
        ++bll::var(called)
      , bll::var(output_size) = bll::_1
    );
    
    stream.async_write(variant_array, callback);
    
    // This may not happen synchronously.

    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    CPPUNIT_ASSERT_EQUAL(output_size_type(0), output_size);
    CPPUNIT_ASSERT_EQUAL(true, fake_stream->read_data_written().empty());
        
    io_service.run();

    fake_bytestream::output_storage_type expected_data = 
        list_of('a')('b')('c');

    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    CPPUNIT_ASSERT_EQUAL(variant_array.size(), output_size);    
    CPPUNIT_ASSERT(expected_data == fake_stream->read_data_written());
}

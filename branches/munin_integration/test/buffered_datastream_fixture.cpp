#include "buffered_datastream_fixture.hpp"
#include "fake_datastream.hpp"
#include "odin/io/buffered_datastream.hpp"
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/typeof/typeof.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(buffered_datastream_fixture);

using namespace boost;
namespace bll = boost::lambda;

typedef fake_datastream<odin::u8, odin::u8> fake_byte_stream;

void buffered_datastream_fixture::test_constructor()
{
    typedef odin::io::buffered_datastream
    <
        odin::u8, odin::u8
    > buffered_byte_stream;
    
    boost::asio::io_service           io_service;
    shared_ptr<odin::io::byte_stream> fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    buffered_byte_stream              buffered_stream(fake_stream, io_service);
}

typedef odin::io::buffered_datastream<odin::u8, odin::u8> buffered_byte_stream;

void buffered_datastream_fixture::test_inheritance()
{
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream> fake_stream(
        new fake_byte_stream(io_service));
    buffered_byte_stream              buffered_stream(fake_stream, io_service);
    
    odin::io::byte_stream& stream = buffered_stream;
    (void)stream;
}

void buffered_datastream_fixture::test_available_empty()
{
    // Test that calling available() on an empty datastream will yield in
    // a will-block.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    buffered_byte_stream              buffered_stream(fake_stream, io_service);
    
    BOOST_AUTO(available, buffered_stream.available());
    
    CPPUNIT_ASSERT_EQUAL(false, available.is_initialized());
}

void buffered_datastream_fixture::test_available_one()
{
    // Test that calling available() on an a datastream with an element in it
    // will give as result of 1 and will also read the element.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    buffered_byte_stream              buffered_stream(fake_stream, io_service);
    
    odin::u8 data[] = { 'x' };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    BOOST_AUTO(available, buffered_stream.available());
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(array.size(), available.get());
    
    // Test that the element has been read.
    CPPUNIT_ASSERT_EQUAL(false, fake_stream->available().is_initialized());
    
    // Test that, even though the element has been read, it is still available.
    available = buffered_stream.available();
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(array.size(), available.get());
}

void buffered_datastream_fixture::test_available_many()
{
    // Test that calling available() on an a datastream with many elements in it
    // will give the correct result and will also read the element.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    buffered_byte_stream              buffered_stream(fake_stream, io_service);
    
    odin::u8 data[] = { 'x', 'y', 'z' };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    BOOST_AUTO(available, buffered_stream.available());
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(array.size(), available.get());
    
    // Test that the element has been read.
    CPPUNIT_ASSERT_EQUAL(false, fake_stream->available().is_initialized());
    
    // Test that, even though the element has been read, it is still available.
    available = buffered_stream.available();
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(array.size(), available.get());
}

void buffered_datastream_fixture::test_sync_read_empty()
{
    // Test that a synchronous read from an empty stream will block.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    buffered_byte_stream              buffered_stream(fake_stream, io_service);
    
    CPPUNIT_ASSERT_THROW(
        buffered_stream.read(buffered_byte_stream::input_size_type(1))
      , blocking_error);
}

void buffered_datastream_fixture::test_sync_read_one_of_one()
{
    // Test that, when reading a single byte from a stream of one, it will
    // be read correctly.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    buffered_byte_stream              buffered_stream(fake_stream, io_service);
    
    odin::u8 data[] = { 'x' };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    CPPUNIT_ASSERT_EQUAL(array, buffered_stream.read(array.size()));
    CPPUNIT_ASSERT_EQUAL(false, fake_stream->available().is_initialized());
    CPPUNIT_ASSERT_EQUAL(false, buffered_stream.available().is_initialized());
}

void buffered_datastream_fixture::test_sync_read_one_of_one_available()
{
    // Test that, when reading a single byte from a stream of one, it will
    // be read correctly.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    buffered_byte_stream              buffered_stream(fake_stream, io_service);
    
    odin::u8 data[] = { 'x' };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    // Make a call to available().  This will cause the data to be buffered.
    CPPUNIT_ASSERT_EQUAL(true, buffered_stream.available().is_initialized());
    
    CPPUNIT_ASSERT_EQUAL(array, buffered_stream.read(array.size()));
    CPPUNIT_ASSERT_EQUAL(false, fake_stream->available().is_initialized());
    CPPUNIT_ASSERT_EQUAL(false, buffered_stream.available().is_initialized());
}

void buffered_datastream_fixture::test_sync_read_one_of_many()
{
    // Test that, when reading a single byte from a stream of many, it will
    // be read correctly.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    buffered_byte_stream              buffered_stream(fake_stream, io_service);
    
    odin::u8 data[] = { 'x', 'y', 'z' };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    CPPUNIT_ASSERT_EQUAL(array, buffered_stream.read(array.size()));
    CPPUNIT_ASSERT_EQUAL(false, fake_stream->available().is_initialized());
    CPPUNIT_ASSERT_EQUAL(false, buffered_stream.available().is_initialized());
}

void buffered_datastream_fixture::test_sync_read_one_of_many_available()
{
    // Test that, when reading many bytes from a stream of one, they will
    // be read correctly.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    buffered_byte_stream              buffered_stream(fake_stream, io_service);
    
    odin::u8 data[] = { 'x', 'y', 'z' };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    // Make a call to available().  This will cause the data to be buffered.
    CPPUNIT_ASSERT_EQUAL(true, buffered_stream.available().is_initialized());
    
    CPPUNIT_ASSERT_EQUAL(array, buffered_stream.read(array.size()));
    CPPUNIT_ASSERT_EQUAL(false, fake_stream->available().is_initialized());
    CPPUNIT_ASSERT_EQUAL(false, buffered_stream.available().is_initialized());
}

void buffered_datastream_fixture::test_sync_read_one_of_many_partially_available()
{
    // Test that, after calling available() (which buffers up data), reading
    // more than was available will call an additional read for the amount
    // missing.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    buffered_byte_stream              buffered_stream(fake_stream, io_service);
    
    odin::u8 available_data[] = { 'x' };
    odin::runtime_array<odin::u8> available_array(available_data);
    
    fake_stream->write_data_to_read(available_array);
    
    // Make a call to available().  This will cause the data to be buffered.
    CPPUNIT_ASSERT_EQUAL(true, buffered_stream.available().is_initialized());
    CPPUNIT_ASSERT_EQUAL(
        available_array.size()
      , buffered_stream.available().get());
    
    odin::u8 additional_data[] = { 'y', 'z' };
    odin::runtime_array<odin::u8> additional_array(additional_data);
    
    fake_stream->write_data_to_read(additional_data);
    
    odin::u8 expected_data[] = { 'x', 'y', 'z' };
    odin::runtime_array<odin::u8> expected_array(expected_data);
    
    CPPUNIT_ASSERT_EQUAL(
        expected_array
      , buffered_stream.read(expected_array.size()));
    CPPUNIT_ASSERT_EQUAL(false, fake_stream->available().is_initialized());
    CPPUNIT_ASSERT_EQUAL(false, buffered_stream.available().is_initialized());
}

void buffered_datastream_fixture::test_sync_write()
{
    // Test that write() sends data to the underlying stream.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    buffered_byte_stream              buffered_stream(fake_stream, io_service);
    
    odin::u8 data[] = { 'x', 'y', 'z' };
    odin::runtime_array<odin::u8> array(data);
    
    buffered_stream.write(array);
    
    CPPUNIT_ASSERT_EQUAL(array, fake_stream->read_data_written());
}

void buffered_datastream_fixture::test_async_read_empty()
{
    // Test that an async read on an empty stream does not immediately complete.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    shared_ptr<buffered_byte_stream>  buffered_stream(
        new buffered_byte_stream(fake_stream, io_service));
    
    odin::u32                     called = 0;
    odin::runtime_array<odin::u8> result;
    boost::function<void (odin::runtime_array<odin::u8>)> callback = (
        ++bll::var(called)
      , bll::var(result) = bll::_1
    );

    buffered_stream->async_read(1, callback);
    
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
}

void buffered_datastream_fixture::test_async_read_one_of_one()
{
    // Test that an async read on an empty stream does not immediately complete.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    shared_ptr<buffered_byte_stream>  buffered_stream(
        new buffered_byte_stream(fake_stream, io_service));
    
    odin::u32                     called = 0;
    odin::runtime_array<odin::u8> result;
    boost::function<void (odin::runtime_array<odin::u8>)> callback = (
        ++bll::var(called)
      , bll::var(result) = bll::_1
    );
    
    odin::u8 data[] = { 'x' };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    buffered_stream->async_read(1, callback);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    CPPUNIT_ASSERT_EQUAL(array, result);
}

void buffered_datastream_fixture::test_async_read_one_of_one_null_callback()
{
    // Test that an async read on an empty stream does not immediately complete.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    shared_ptr<buffered_byte_stream>  buffered_stream(
        new buffered_byte_stream(fake_stream, io_service));
    
    odin::u8 data[] = { 'x' };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    buffered_stream->async_read(1, NULL);
    
    io_service.run();
}

void buffered_datastream_fixture::test_async_read_one_of_one_available()
{
    // Test that an async read on an empty stream does not immediately complete.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    shared_ptr<buffered_byte_stream>  buffered_stream(
        new buffered_byte_stream(fake_stream, io_service));
    
    odin::u32                     called = 0;
    odin::runtime_array<odin::u8> result;
    boost::function<void (odin::runtime_array<odin::u8>)> callback = (
        ++bll::var(called)
      , bll::var(result) = bll::_1
    );
    
    odin::u8 data[] = { 'x' };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    CPPUNIT_ASSERT_EQUAL(true, buffered_stream->available().is_initialized());
    CPPUNIT_ASSERT_EQUAL(array.size(), buffered_stream->available().get());
    
    buffered_stream->async_read(1, callback);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    CPPUNIT_ASSERT_EQUAL(array, result);
}

void buffered_datastream_fixture::test_async_read_one_of_many()
{
    // Test that an async read on an empty stream does not immediately complete.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    shared_ptr<buffered_byte_stream>  buffered_stream(
        new buffered_byte_stream(fake_stream, io_service));
    
    odin::u32                     called = 0;
    odin::runtime_array<odin::u8> result;
    boost::function<void (odin::runtime_array<odin::u8>)> callback = (
        ++bll::var(called)
      , bll::var(result) = bll::_1
    );
    
    odin::u8 data[] = { 'x', 'y', 'z' };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    buffered_stream->async_read(1, callback);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    
    io_service.run();
    
    odin::u8 expected_data[] = { 'x' };
    odin::runtime_array<odin::u8> expected_array(expected_data);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    CPPUNIT_ASSERT_EQUAL(expected_array, result);
    
    CPPUNIT_ASSERT_EQUAL(false, fake_stream->available().is_initialized());
    CPPUNIT_ASSERT_EQUAL(true,  buffered_stream->available().is_initialized());
    CPPUNIT_ASSERT_EQUAL(
        array.size() - expected_array.size()
      , buffered_stream->available().get());
}

void buffered_datastream_fixture::test_async_read_one_of_many_available()
{
    // Test that an async read on an empty stream does not immediately complete.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    shared_ptr<buffered_byte_stream>  buffered_stream(
        new buffered_byte_stream(fake_stream, io_service));
    
    odin::u32                     called = 0;
    odin::runtime_array<odin::u8> result;
    boost::function<void (odin::runtime_array<odin::u8>)> callback = (
        ++bll::var(called)
      , bll::var(result) = bll::_1
    );
    
    odin::u8 data[] = { 'x', 'y', 'z' };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    CPPUNIT_ASSERT_EQUAL(true, buffered_stream->available().is_initialized());
    CPPUNIT_ASSERT_EQUAL(array.size(), buffered_stream->available().get());
    
    buffered_stream->async_read(1, callback);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    
    io_service.run();
    
    odin::u8 expected_data[] = { 'x' };
    odin::runtime_array<odin::u8> expected_array(expected_data);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    CPPUNIT_ASSERT_EQUAL(expected_array, result);
    
    CPPUNIT_ASSERT_EQUAL(false, fake_stream->available().is_initialized());
    CPPUNIT_ASSERT_EQUAL(true,  buffered_stream->available().is_initialized());
    CPPUNIT_ASSERT_EQUAL(
        array.size() - expected_array.size()
      , buffered_stream->available().get());
}

void buffered_datastream_fixture::test_async_read_many_of_many_partially_available()
{
    // Test that an async read on an empty stream does not immediately complete.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    shared_ptr<buffered_byte_stream>  buffered_stream(
        new buffered_byte_stream(fake_stream, io_service));
    
    odin::u32                     called = 0;
    odin::runtime_array<odin::u8> result;
    boost::function<void (odin::runtime_array<odin::u8>)> callback = (
        ++bll::var(called)
      , bll::var(result) = bll::_1
    );
    
    odin::u8 data[] = { 'x' };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(array);
    
    CPPUNIT_ASSERT_EQUAL(true, buffered_stream->available().is_initialized());
    CPPUNIT_ASSERT_EQUAL(array.size(), buffered_stream->available().get());

    odin::u8 more_data[] = { 'y', 'z' };
    odin::runtime_array<odin::u8> more_array(more_data);
    
    fake_stream->write_data_to_read(more_array);
    
    buffered_stream->async_read(3, callback);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    
    io_service.run();
    
    odin::u8 expected_data[] = { 'x', 'y', 'z' };
    odin::runtime_array<odin::u8> expected_array(expected_data);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    CPPUNIT_ASSERT_EQUAL(expected_array, result);
    
    CPPUNIT_ASSERT_EQUAL(false, fake_stream->available().is_initialized());
    CPPUNIT_ASSERT_EQUAL(false, buffered_stream->available().is_initialized());
}

void buffered_datastream_fixture::test_consecutive_async_read()
{
    // Test that an async read on an empty stream does not immediately complete.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    shared_ptr<buffered_byte_stream>  buffered_stream(
        new buffered_byte_stream(fake_stream, io_service));
    
    odin::u32                     called = 0;
    odin::runtime_array<odin::u8> result;
    boost::function<void (odin::runtime_array<odin::u8>)> callback = (
        ++bll::var(called)
      , bll::var(result) = bll::_1
    );
    
    odin::u8 data[] = { 'x', 'y' };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(data);
    
    buffered_stream->async_read(1, callback);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    
    io_service.run();
    
    odin::u8 expected_data0[] = { 'x' };
    odin::runtime_array<odin::u8> expected_array0(expected_data0);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    CPPUNIT_ASSERT_EQUAL(expected_array0, result);
    
    buffered_stream->async_read(1, callback);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    
    io_service.reset();
    io_service.run();

    odin::u8 expected_data1[] = { 'y' };
    odin::runtime_array<odin::u8> expected_array1(expected_data1);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(2), called);
    CPPUNIT_ASSERT_EQUAL(expected_array1, result);
}

void buffered_datastream_fixture::test_many_async_reads()
{
    // Test that an async read on an empty stream does not immediately complete.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    shared_ptr<buffered_byte_stream>  buffered_stream(
        new buffered_byte_stream(fake_stream, io_service));
    
    odin::u32                     called = 0;
    odin::runtime_array<odin::u8> result0;
    boost::function<void (odin::runtime_array<odin::u8>)> callback0 = (
        ++bll::var(called)
      , bll::var(result0) = bll::_1
    );
    
    odin::runtime_array<odin::u8> result1;
    boost::function<void (odin::runtime_array<odin::u8>)> callback1 = (
        ++bll::var(called)
      , bll::var(result1) = bll::_1
    );
    
    odin::u8 data[] = { 'x', 'y' };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(data);
    
    buffered_stream->async_read(1, callback0);
    buffered_stream->async_read(1, callback1);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    
    io_service.run();
    
    odin::u8 expected_data0[] = { 'x' };
    odin::runtime_array<odin::u8> expected_array0(expected_data0);

    odin::u8 expected_data1[] = { 'y' };
    odin::runtime_array<odin::u8> expected_array1(expected_data1);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(2), called);
    CPPUNIT_ASSERT_EQUAL(expected_array0, result0);
    CPPUNIT_ASSERT_EQUAL(expected_array1, result1);
}

void buffered_datastream_fixture::test_available_during_async_read()
{
    // Test that an async read on an empty stream does not immediately complete.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    shared_ptr<buffered_byte_stream>  buffered_stream(
        new buffered_byte_stream(fake_stream, io_service));
    
    odin::u8 data[] = { 'x', 'y' };
    odin::runtime_array<odin::u8> array(data);
    
    fake_stream->write_data_to_read(data);
    
    buffered_stream->async_read(1, NULL);
    
    CPPUNIT_ASSERT_EQUAL(false, buffered_stream->available().is_initialized());
}

void buffered_datastream_fixture::test_available_during_async_callback()
{
    // Test that an async read on an empty stream does not immediately complete.
    
    boost::asio::io_service           io_service;
    shared_ptr<fake_byte_stream>      fake_stream(
        new fake_byte_stream(io_service));
    shared_ptr<buffered_byte_stream>  buffered_stream(
        new buffered_byte_stream(fake_stream, io_service));
    
    odin::u8 data[] = { 'x', 'y' };
    odin::runtime_array<odin::u8> array(data);
    
    odin::u32                                       called = 0;
    optional<buffered_byte_stream::input_size_type> available;
    odin::runtime_array<odin::u8>                   result;
    
    boost::function<void (odin::runtime_array<odin::u8>)> callback = (
        ++bll::var(called)
      , bll::var(available) = bll::bind(
            &buffered_byte_stream::available, buffered_stream.get())
      , bll::var(result) = bll::_1
    );
    
    fake_stream->write_data_to_read(data);
    
    buffered_stream->async_read(1, callback);
    
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(true, available.is_initialized());
    CPPUNIT_ASSERT_EQUAL(array.size() - result.size(), available.get());
}

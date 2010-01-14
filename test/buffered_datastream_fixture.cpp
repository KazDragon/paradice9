#include "buffered_datastream_fixture.hpp"
#include "fake_datastream.hpp"
#include "odin/io/buffered_datastream.hpp"
#include <boost/shared_ptr.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(buffered_datastream_fixture);

using namespace boost;

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

void buffered_datastream_fixture::test_inheritance()
{
    typedef odin::io::buffered_datastream
    <
        odin::u8, odin::u8
    > buffered_byte_stream;
    
    boost::asio::io_service           io_service;
    shared_ptr<odin::io::byte_stream> fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    buffered_byte_stream              buffered_stream(fake_stream, io_service);
    
    odin::io::byte_stream& stream = buffered_stream;
    (void)stream;
}

void buffered_datastream_fixture::test_sync_read_empty()
{
    typedef odin::io::buffered_datastream
    <
        odin::u8, odin::u8
    > buffered_byte_stream;
    
    boost::asio::io_service           io_service;
    shared_ptr<odin::io::byte_stream> fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    buffered_byte_stream              buffered_stream(fake_stream, io_service);
    
    CPPUNIT_ASSERT_THROW(
        buffered_stream.read(buffered_byte_stream::input_size_type(1))
      , blocking_error);
}

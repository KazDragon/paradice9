#include "telnet_stream_fixture.hpp"
#include "odin/telnet/stream.hpp"
#include "fake_datastream.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_stream_fixture);

using namespace std;
using namespace boost;

void telnet_stream_fixture::test_constructor()
{
    boost::asio::io_service           io_service;
    shared_ptr<odin::io::byte_stream> fake_stream(
        new fake_datastream<odin::u8, odin::u8>(io_service));
    
    odin::telnet::stream stream(fake_stream, io_service);
}

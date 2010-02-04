#include "telnet_options_suppress_goahead_client_fixture.hpp"
#include "odin/telnet/options/suppress_goahead_client.hpp"
#include "odin/telnet/options/subnegotiationless_client.hpp"
#include "odin/telnet/stream.hpp"
#include "odin/telnet/router.hpp"
#include "odin/telnet/protocol.hpp"
#include "fake_datastream.hpp"
#include <boost/type_traits/is_same.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_options_suppress_goahead_client_fixture);

using namespace std;
using namespace boost;

typedef fake_datastream<odin::u8, odin::u8> fake_byte_stream;

void telnet_options_suppress_goahead_client_fixture::test_identity()
{
    typedef odin::telnet::options::subnegotiationless_client<
        odin::telnet::SUPPRESS_GOAHEAD
    > expected_suppress_goahead_client;
    
    CPPUNIT_ASSERT((
        is_same<
            expected_suppress_goahead_client
          , odin::telnet::options::suppress_goahead_client
        >::value));
}

#include "telnet_options_suppress_goahead_server_fixture.hpp"
#include "odin/telnet/options/suppress_goahead_server.hpp"
#include "odin/telnet/options/subnegotiationless_server.hpp"
#include "odin/telnet/protocol.hpp"
#include <boost/type_traits/is_same.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_options_suppress_goahead_server_fixture);

using namespace std;
using namespace boost;

void telnet_options_suppress_goahead_server_fixture::test_identity()
{
    typedef odin::telnet::options::subnegotiationless_server<
        odin::telnet::SUPPRESS_GOAHEAD
    > expected_suppress_goahead_server;
    
    CPPUNIT_ASSERT((
        is_same<
            expected_suppress_goahead_server
          , odin::telnet::options::suppress_goahead_server
        >::value));
}

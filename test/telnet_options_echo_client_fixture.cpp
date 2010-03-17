#include "telnet_options_echo_client_fixture.hpp"
#include "odin/telnet/options/echo_client.hpp"
#include "odin/telnet/options/subnegotiationless_client.hpp"
#include "odin/telnet/protocol.hpp"
#include <boost/type_traits/is_same.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_options_echo_client_fixture);

using namespace std;
using namespace boost;

void telnet_options_echo_client_fixture::test_identity()
{
    typedef odin::telnet::options::subnegotiationless_client<
        odin::telnet::ECHO
    > expected_echo_client;
    
    CPPUNIT_ASSERT((
        is_same<
            expected_echo_client
          , odin::telnet::options::echo_client
        >::value));
}

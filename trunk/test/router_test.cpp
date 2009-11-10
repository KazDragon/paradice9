#include "router_test.hpp"
#include "odin/router.hpp"
#include <boost/function.hpp>
#include <boost/lambda/lambda.hpp>
#include <algorithm>
#include <string>
#include <vector>

CPPUNIT_TEST_SUITE_REGISTRATION(router_fixture);

using namespace std;
using namespace boost;
using namespace boost::lambda;

struct first_element_policy
{
    static char key_from_message(string const& message)
    {
        return message[0]; 
    }
};

void router_fixture::test_registered_route()
{
    odin::router<char, string, first_element_policy> route;
    
    bool   called = false;
    string message;
    function<void (string)> fn = (
        var(called)  = true
      , var(message) = _1
    );
    
    route.register_route('x', fn);
    
    CPPUNIT_ASSERT_EQUAL(false, called);
    CPPUNIT_ASSERT_EQUAL(string(""), message);
    
    route("xyzzy");
    
    CPPUNIT_ASSERT_EQUAL(true, called);
    CPPUNIT_ASSERT_EQUAL(string("xyzzy"), message);
}

void router_fixture::test_unregistered_route()
{
    odin::router<char, string, first_element_policy> route;
    
    bool   called = false;
    string message;
    function<void (string)> fn = (
        var(called)  = true
      , var(message) = _1
    );
    
    // This explicitly doesn't happen.
    //route.register_route('x', fn);
    
    CPPUNIT_ASSERT_EQUAL(false, called);
    CPPUNIT_ASSERT_EQUAL(string(""), message);
    
    route("xyzzy");
    
    CPPUNIT_ASSERT_EQUAL(false, called);
    CPPUNIT_ASSERT_EQUAL(string(""), message);
}

void router_fixture::test_multiple_messages()
{
    odin::router<char, string, first_element_policy> route;
    
    int    called_0 = 0;
    string message_0;
    function<void (string)> fn_0 = (
        var(called_0)++
      , var(message_0) = _1
    );

    int    called_1 = 0;
    string message_1;
    function<void (string)> fn_1 = (
        var(called_1)++
      , var(message_1) = _1
    );

    int    called_2 = 0;
    string message_2;
    function<void (string)> fn_2 = (
        var(called_2)++
      , var(message_2) = _1
    );
    
    route.register_route('a', fn_0);
    route.register_route('b', fn_1);
    route.register_route('c', fn_2);
    
    vector<string> strings;
    strings.push_back("alpha");
    strings.push_back("beta");
    strings.push_back("kappa");
    strings.push_back("delta");
    strings.push_back("upsilon");
    strings.push_back("aardvark");
    
    CPPUNIT_ASSERT_EQUAL(0, called_0);
    CPPUNIT_ASSERT_EQUAL(0, called_1);
    CPPUNIT_ASSERT_EQUAL(0, called_2);
    CPPUNIT_ASSERT_EQUAL(string(""), message_0);
    CPPUNIT_ASSERT_EQUAL(string(""), message_1);
    CPPUNIT_ASSERT_EQUAL(string(""), message_1);

    for_each(strings.begin(), strings.end(), route);
    
    CPPUNIT_ASSERT_EQUAL(2, called_0);
    CPPUNIT_ASSERT_EQUAL(1, called_1);
    CPPUNIT_ASSERT_EQUAL(0, called_2);
    CPPUNIT_ASSERT_EQUAL(string("aardvark"), message_0);
    CPPUNIT_ASSERT_EQUAL(string("beta"), message_1);
    CPPUNIT_ASSERT_EQUAL(string(""), message_2);
    
}

void router_fixture::test_unregistered_handler()
{
    odin::router<char, string, first_element_policy> route;
    
    int    called_0 = 0;
    string message_0;
    function<void (string)> fn_0 = (
        var(called_0)++
      , var(message_0) = _1
    );
    
    int    called_1 = 0;
    string message_1;
    function<void (string)> fn_1 = (
        var(called_1)++
      , var(message_1) = _1
    );
    
    route.register_route('a', fn_0);
    route.set_unregistered_route(fn_1);
    
    CPPUNIT_ASSERT_EQUAL(0, called_0);
    CPPUNIT_ASSERT_EQUAL(0, called_1);
    CPPUNIT_ASSERT_EQUAL(string(""), message_0);
    CPPUNIT_ASSERT_EQUAL(string(""), message_1);
    
    route("alpha");
    route("beta");
    route("gamma");
    route("delta");
    
    CPPUNIT_ASSERT_EQUAL(1, called_0);
    CPPUNIT_ASSERT_EQUAL(3, called_1);
    CPPUNIT_ASSERT_EQUAL(string("alpha"), message_0);
    CPPUNIT_ASSERT_EQUAL(string("delta"), message_1);
}

#include "telnet_input_visitor_fixture.hpp"
#include "odin/telnet/input_visitor.hpp"
#include "odin/telnet/command_router.hpp"
#include "odin/telnet/negotiation_router.hpp"
#include "odin/telnet/subnegotiation_router.hpp"
#include "odin/telnet/stream.hpp"
#include <boost/function.hpp>
#include <boost/lambda/lambda.hpp>
#include <string>

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_input_visitor_fixture);

using namespace std;
using namespace boost;

namespace bll = boost::lambda;

void telnet_input_visitor_fixture::test_constructor()
{
    // Tests that a telnet input visitor can be constructed from a collection of
    // telnet routers and a handler for text.
    shared_ptr<odin::telnet::command_router> command_router(
        new odin::telnet::command_router);
    shared_ptr<odin::telnet::negotiation_router> negotiation_router(
        new odin::telnet::negotiation_router);
    shared_ptr<odin::telnet::subnegotiation_router> subnegotiation_router(
        new odin::telnet::subnegotiation_router);

    string    text;
    odin::u32 called = 0;
    function<void (string)> text_handler = 
    (
        bll::var(text) += bll::_1,
        bll::var(called)++
    );
    
    odin::telnet::input_visitor input_visitor(
        command_router
      , negotiation_router
      , subnegotiation_router
      , text_handler);
}

void telnet_input_visitor_fixture::test_visit_command()
{
    // Tests that a telnet input visitor can route commands received from a 
    // stream to the correct location.
    shared_ptr<odin::telnet::command_router> command_router(
        new odin::telnet::command_router);
    shared_ptr<odin::telnet::negotiation_router> negotiation_router(
        new odin::telnet::negotiation_router);
    shared_ptr<odin::telnet::subnegotiation_router> subnegotiation_router(
        new odin::telnet::subnegotiation_router);

    string text;
    function<void (string)> text_handler = 
    (
        bll::var(text) += bll::_1
    );
    
    odin::u32             called         = 0;
    odin::telnet::command actual_command = odin::telnet::command(0);
    function<void (odin::telnet::command)> callback = 
    (
        bll::var(actual_command) = bll::_1,
        bll::var(called)++
    );
    
    command_router->register_route(odin::telnet::AYT, callback);
    
    odin::telnet::input_visitor input_visitor(
        command_router
      , negotiation_router
      , subnegotiation_router
      , text_handler);
    
    odin::telnet::command expected_command = odin::telnet::AYT; 
    odin::telnet::stream::input_value_type variant_value(expected_command);
    
    apply_visitor(input_visitor, variant_value);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    CPPUNIT_ASSERT_EQUAL(expected_command, actual_command);
}

void telnet_input_visitor_fixture::test_visit_negotiation()
{
    // Tests that a telnet input visitor can route negotiations received from a
    // stream to the correct location.
    shared_ptr<odin::telnet::command_router> command_router(
        new odin::telnet::command_router);
    shared_ptr<odin::telnet::negotiation_router> negotiation_router(
        new odin::telnet::negotiation_router);
    shared_ptr<odin::telnet::subnegotiation_router> subnegotiation_router(
        new odin::telnet::subnegotiation_router);

    string text;
    function<void (string)> text_handler = 
    (
        bll::var(text) += bll::_1
    );
    
    odin::u32                      called = 0;
    odin::telnet::negotiation_type actual_negotiation;
    actual_negotiation.request_   = 0;
    actual_negotiation.option_id_ = 0;
    function<void (odin::telnet::negotiation_type)> callback = 
    (
        bll::var(actual_negotiation) = bll::_1,
        bll::var(called)++
    );
    
    odin::telnet::negotiation_type expected_negotiation;
    expected_negotiation.request_   = odin::telnet::DONT;
    expected_negotiation.option_id_ = odin::telnet::NAWS;
    
    negotiation_router->register_route(expected_negotiation, callback);
    
    odin::telnet::input_visitor input_visitor(
        command_router
      , negotiation_router
      , subnegotiation_router
      , text_handler);
    
    odin::telnet::stream::input_value_type variant_value(expected_negotiation);
    
    apply_visitor(input_visitor, variant_value);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    CPPUNIT_ASSERT_EQUAL(
        expected_negotiation.request_, actual_negotiation.request_);
    CPPUNIT_ASSERT_EQUAL(
        expected_negotiation.option_id_, actual_negotiation.option_id_);
}

void telnet_input_visitor_fixture::test_visit_subnegotiation()
{
    // Tests that a telnet input visitor can route subnegotiations received
    // from a stream to the correct location.
    shared_ptr<odin::telnet::command_router> command_router(
        new odin::telnet::command_router);
    shared_ptr<odin::telnet::negotiation_router> negotiation_router(
        new odin::telnet::negotiation_router);
    shared_ptr<odin::telnet::subnegotiation_router> subnegotiation_router(
        new odin::telnet::subnegotiation_router);

    string text;
    function<void (string)> text_handler = 
    (
        bll::var(text) += bll::_1
    );
    
    odin::u32                      called = 0;
    odin::telnet::subnegotiation_type actual_subnegotiation;
    actual_subnegotiation.option_id_ = 0;
    function<void (odin::telnet::subnegotiation_type)> callback = 
    (
        bll::var(actual_subnegotiation) = bll::_1,
        bll::var(called)++
    );
    
    odin::telnet::subnegotiation_type expected_subnegotiation;
    expected_subnegotiation.option_id_ = odin::telnet::NAWS;
    expected_subnegotiation.content_.push_back('x');
    expected_subnegotiation.content_.push_back('y');
    expected_subnegotiation.content_.push_back('z');
    
    subnegotiation_router->register_route(odin::telnet::NAWS, callback);
    
    odin::telnet::input_visitor input_visitor(
        command_router
      , negotiation_router
      , subnegotiation_router
      , text_handler);
    
    odin::telnet::stream::input_value_type variant_value =
        expected_subnegotiation;
    
    apply_visitor(input_visitor, variant_value);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    CPPUNIT_ASSERT_EQUAL(
        expected_subnegotiation.option_id_, actual_subnegotiation.option_id_);
    CPPUNIT_ASSERT(
        expected_subnegotiation.content_ == actual_subnegotiation.content_); 
}

void telnet_input_visitor_fixture::test_visit_text()
{
    // Tests that a telnet input visitor can route text received from a stream
    // to the correct location.
    shared_ptr<odin::telnet::command_router> command_router(
        new odin::telnet::command_router);
    shared_ptr<odin::telnet::negotiation_router> negotiation_router(
        new odin::telnet::negotiation_router);
    shared_ptr<odin::telnet::subnegotiation_router> subnegotiation_router(
        new odin::telnet::subnegotiation_router);
    
    odin::u32 called = 0;    
    string    text;
    function<void (string)> text_handler = 
    (
        bll::var(text) += bll::_1,
        bll::var(called)++
    );
    
    odin::telnet::input_visitor input_visitor(
        command_router
      , negotiation_router
      , subnegotiation_router
      , text_handler);
    
    string string_value = "foo";
    odin::telnet::stream::input_value_type variant_value(string_value);
    
    apply_visitor(input_visitor, variant_value);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    CPPUNIT_ASSERT_EQUAL(string_value, text);
}

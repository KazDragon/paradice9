#include "telnet_subnegotiation_router_fixture.hpp"
#include "odin/telnet/subnegotiation_router.hpp"
#include <boost/lambda/lambda.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_subnegotiation_router_fixture);

using namespace std;
using namespace boost;

namespace bll = boost::lambda;

void telnet_subnegotiation_router_fixture::test_constructor()
{
    odin::telnet::subnegotiation_router subnegotiation_router;
    (void)subnegotiation_router;
}

void telnet_subnegotiation_router_fixture::test_register_route()
{
    odin::telnet::subnegotiation_router subnegotiation_router;
    odin::telnet::subnegotiation_type   subnegotiation;
    odin::u32                           called  = 0;
    
    function<void (odin::telnet::subnegotiation_type)> 
        naws_subnegotiation_callback = (
        bll::var(subnegotiation) = bll::_1
      , ++bll::var(called)
    );
        
    odin::telnet::subnegotiation_type naws_subnegotiation;
    naws_subnegotiation.option_id_ = odin::telnet::NAWS;
    naws_subnegotiation.content_.push_back(odin::u8('n'));

    odin::telnet::subnegotiation_type echo_subnegotiation;
    echo_subnegotiation.option_id_ = odin::telnet::ECHO;
    echo_subnegotiation.content_.push_back(odin::u8('e'));
    
    subnegotiation_router.register_route(
        odin::telnet::NAWS, naws_subnegotiation_callback);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    
    subnegotiation_router(echo_subnegotiation);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    
    subnegotiation_router(naws_subnegotiation);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    CPPUNIT_ASSERT_EQUAL(
        naws_subnegotiation.option_id_
      , subnegotiation.option_id_);
    CPPUNIT_ASSERT(
        naws_subnegotiation.content_ == subnegotiation.content_);

    subnegotiation_router(naws_subnegotiation);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(2), called);
    CPPUNIT_ASSERT_EQUAL(
        naws_subnegotiation.option_id_
      , subnegotiation.option_id_);
    CPPUNIT_ASSERT(
        naws_subnegotiation.content_ == subnegotiation.content_);
}

void telnet_subnegotiation_router_fixture::test_unregister_route()
{
    odin::telnet::subnegotiation_router subnegotiation_router;
    odin::telnet::subnegotiation_type   subnegotiation;
    odin::u32                           called  = 0;
    
    function<void (odin::telnet::subnegotiation_type)> 
        naws_subnegotiation_callback = (
        bll::var(subnegotiation) = bll::_1
      , ++bll::var(called)
    );
        
    odin::telnet::subnegotiation_type naws_subnegotiation;
    naws_subnegotiation.option_id_ = odin::telnet::NAWS;
    naws_subnegotiation.content_.push_back(odin::u8('n'));

    odin::telnet::subnegotiation_type echo_subnegotiation;
    echo_subnegotiation.option_id_ = odin::telnet::ECHO;
    echo_subnegotiation.content_.push_back(odin::u8('e'));
    
    subnegotiation_router.register_route(
        odin::telnet::NAWS, naws_subnegotiation_callback);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    
    subnegotiation_router(echo_subnegotiation);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    
    subnegotiation_router(naws_subnegotiation);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    CPPUNIT_ASSERT_EQUAL(
        naws_subnegotiation.option_id_
      , subnegotiation.option_id_);
    CPPUNIT_ASSERT(
        naws_subnegotiation.content_ == subnegotiation.content_);

    subnegotiation_router.unregister_route(odin::telnet::NAWS);
    
    subnegotiation_router(naws_subnegotiation);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
}

void telnet_subnegotiation_router_fixture::test_unregistered_route()
{
    odin::telnet::subnegotiation_router subnegotiation_router;
    odin::telnet::subnegotiation_type   subnegotiation;
    odin::u32                           called  = 0;
    
    function<void (odin::telnet::subnegotiation_type)> 
        unregistered_callback = (
        bll::var(subnegotiation) = bll::_1
      , ++bll::var(called)
    );
        
    odin::telnet::subnegotiation_type naws_subnegotiation;
    naws_subnegotiation.option_id_ = odin::telnet::NAWS;
    naws_subnegotiation.content_.push_back(odin::u8('n'));

    odin::telnet::subnegotiation_type echo_subnegotiation;
    echo_subnegotiation.option_id_ = odin::telnet::ECHO;
    echo_subnegotiation.content_.push_back(odin::u8('e'));
    
    subnegotiation_router.set_unregistered_route(unregistered_callback);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    
    subnegotiation_router(echo_subnegotiation);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    CPPUNIT_ASSERT_EQUAL(
        echo_subnegotiation.option_id_
      , subnegotiation.option_id_);
    CPPUNIT_ASSERT(
        echo_subnegotiation.content_ == subnegotiation.content_);
    
    subnegotiation_router(naws_subnegotiation);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(2), called);
    CPPUNIT_ASSERT_EQUAL(
        naws_subnegotiation.option_id_
      , subnegotiation.option_id_);
    CPPUNIT_ASSERT(
        naws_subnegotiation.content_ == subnegotiation.content_);
}

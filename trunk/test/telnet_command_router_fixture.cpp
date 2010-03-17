#include "telnet_command_router_fixture.hpp"
#include "odin/telnet/command_router.hpp"
#include <boost/lambda/lambda.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_command_router_fixture);

using namespace std;
using namespace boost;

namespace bll = boost::lambda;

void telnet_command_router_fixture::test_constructor()
{
    odin::telnet::command_router command_router;
}

void telnet_command_router_fixture::test_register_route()
{
    odin::telnet::command_router command_router;
    odin::telnet::command_type   command = 0;
    odin::u32                    called  = 0;
    
    function<void (odin::telnet::command_type)> nop_callback = (
        bll::var(command) = bll::_1
      , ++bll::var(called)
    );
        
    command_router.register_route(
        odin::telnet::NOP
      , nop_callback);
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::command_type(0), command);
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    
    command_router(odin::telnet::NOP);
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::NOP, command);
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    
    command_router(odin::telnet::NOP);
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::NOP, command);
    CPPUNIT_ASSERT_EQUAL(odin::u32(2), called);
    
    command_router(odin::telnet::AYT);

    CPPUNIT_ASSERT_EQUAL(odin::telnet::NOP, command);
    CPPUNIT_ASSERT_EQUAL(odin::u32(2), called);
}

void telnet_command_router_fixture::test_unregister_route()
{
    odin::telnet::command_router command_router;
    odin::telnet::command_type   command = 0;
    odin::u32                    called  = 0;
    
    function<void (odin::telnet::command_type)> nop_callback = (
        bll::var(command) = bll::_1
      , ++bll::var(called)
    );
        
    command_router.register_route(
        odin::telnet::NOP
      , nop_callback);
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::command_type(0), command);
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    
    command_router(odin::telnet::NOP);
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::NOP, command);
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    
    command_router.unregister_route(odin::telnet::NOP);
    
    command_router(odin::telnet::NOP);

    CPPUNIT_ASSERT_EQUAL(odin::telnet::NOP, command);
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
}

void telnet_command_router_fixture::test_unregistered_route()
{
    odin::telnet::command_router command_router;
    odin::telnet::command_type   command = 0;
    odin::u32                    called  = 0;
    
    function<void (odin::telnet::command_type)> unreg_callback = (
        bll::var(command) = bll::_1
      , ++bll::var(called)
    );
        
    command_router.set_unregistered_route(unreg_callback);
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::command_type(0), command);
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    
    command_router(odin::telnet::NOP);
    
    CPPUNIT_ASSERT_EQUAL(odin::telnet::NOP, command);
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    
    command_router(odin::telnet::AYT);

    CPPUNIT_ASSERT_EQUAL(odin::telnet::AYT, command);
    CPPUNIT_ASSERT_EQUAL(odin::u32(2), called);
}

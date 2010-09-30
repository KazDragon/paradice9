#include "telnet_negotiation_router_fixture.hpp"
#include "odin/telnet/negotiation_router.hpp"
#include <boost/lambda/lambda.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_negotiation_router_fixture);

using namespace std;
using namespace boost;

namespace bll = boost::lambda;

void telnet_negotiation_router_fixture::test_constructor()
{
    odin::telnet::negotiation_router negotiation_router;
}

void telnet_negotiation_router_fixture::test_register_route()
{
    odin::telnet::negotiation_router negotiation_router;
    odin::telnet::negotiation_type   negotiation;
    odin::u32                        called  = 0;
    
    function<void (odin::telnet::negotiation_type)> will_naws_callback = (
        bll::var(negotiation) = bll::_1
      , ++bll::var(called)
    );
        
    odin::telnet::negotiation_type will_naws;
    will_naws.request_   = odin::telnet::WILL;
    will_naws.option_id_ = odin::telnet::NAWS;
    
    odin::telnet::negotiation_type wont_naws;
    will_naws.request_   = odin::telnet::WONT;
    will_naws.option_id_ = odin::telnet::NAWS;
    
    odin::telnet::negotiation_type will_echo;
    will_naws.request_   = odin::telnet::WILL;
    will_naws.option_id_ = odin::telnet::ECHO;
    
    negotiation_router.register_route(will_naws, will_naws_callback);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    
    negotiation_router(will_naws);
    
    CPPUNIT_ASSERT_EQUAL(will_naws.request_,   negotiation.request_);
    CPPUNIT_ASSERT_EQUAL(will_naws.option_id_, negotiation.option_id_);
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
    
    negotiation_router(will_naws);
    
    CPPUNIT_ASSERT_EQUAL(will_naws.request_,   negotiation.request_);
    CPPUNIT_ASSERT_EQUAL(will_naws.option_id_, negotiation.option_id_);
    CPPUNIT_ASSERT_EQUAL(odin::u32(2), called);
    
    negotiation_router(wont_naws);

    CPPUNIT_ASSERT_EQUAL(will_naws.request_,   negotiation.request_);
    CPPUNIT_ASSERT_EQUAL(will_naws.option_id_, negotiation.option_id_);
    CPPUNIT_ASSERT_EQUAL(odin::u32(2), called);

    negotiation_router(will_echo);

    CPPUNIT_ASSERT_EQUAL(will_naws.request_,   negotiation.request_);
    CPPUNIT_ASSERT_EQUAL(will_naws.option_id_, negotiation.option_id_);
    CPPUNIT_ASSERT_EQUAL(odin::u32(2), called);
}

void telnet_negotiation_router_fixture::test_unregister_route()
{
    odin::telnet::negotiation_router negotiation_router;
    odin::telnet::negotiation_type   negotiation;
    odin::u32                        called  = 0;
    
    function<void (odin::telnet::negotiation_type)> will_naws_callback = (
        bll::var(negotiation) = bll::_1
      , ++bll::var(called)
    );
        
    odin::telnet::negotiation_type will_naws;
    will_naws.request_   = odin::telnet::WILL;
    will_naws.option_id_ = odin::telnet::NAWS;
    
    negotiation_router.register_route(will_naws, will_naws_callback);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    
    negotiation_router(will_naws);
    
    CPPUNIT_ASSERT_EQUAL(will_naws.request_,   negotiation.request_);
    CPPUNIT_ASSERT_EQUAL(will_naws.option_id_, negotiation.option_id_);
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);

    negotiation_router.unregister_route(will_naws);
    
    negotiation_router(will_naws);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
}

void telnet_negotiation_router_fixture::test_unregistered_route()
{
    odin::telnet::negotiation_router negotiation_router;
    odin::telnet::negotiation_type   negotiation;
    odin::u32                        called  = 0;
    
    function<void (odin::telnet::negotiation_type)> will_naws_callback = (
        bll::var(negotiation) = bll::_1
      , ++bll::var(called)
    );
        
    odin::telnet::negotiation_type will_naws;
    will_naws.request_   = odin::telnet::WILL;
    will_naws.option_id_ = odin::telnet::NAWS;
    
    negotiation_router.set_unregistered_route(will_naws_callback);
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), called);
    
    negotiation_router(will_naws);
    
    CPPUNIT_ASSERT_EQUAL(will_naws.request_,   negotiation.request_);
    CPPUNIT_ASSERT_EQUAL(will_naws.option_id_, negotiation.option_id_);
    CPPUNIT_ASSERT_EQUAL(odin::u32(1), called);
}

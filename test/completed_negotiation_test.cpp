#include "completed_negotiation_test.hpp"
#include "odin/telnet/completed_negotiation.hpp"
#include "odin/telnet/initiated_negotiation.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_completed_negotiation_test);

void telnet_completed_negotiation_test::test_initiator()
{
    CPPUNIT_ASSERT(
        odin::telnet::completed_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WILL
          , odin::telnet::DO).get_initiator() == odin::telnet::local);
    
    CPPUNIT_ASSERT(
        odin::telnet::completed_negotiation(
            odin::telnet::remote
          , 0
          , odin::telnet::WILL
          , odin::telnet::DO).get_initiator() == odin::telnet::remote);
}

void telnet_completed_negotiation_test::test_local_request()
{
    CPPUNIT_ASSERT(
        odin::telnet::completed_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WILL
          , odin::telnet::DO).get_local_request() == odin::telnet::WILL);

    CPPUNIT_ASSERT(
        odin::telnet::completed_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WONT
          , odin::telnet::DONT).get_local_request() == odin::telnet::WONT);

    CPPUNIT_ASSERT(
        odin::telnet::completed_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::DO
          , odin::telnet::WILL).get_local_request() == odin::telnet::DO);

    CPPUNIT_ASSERT(
        odin::telnet::completed_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::DONT
          , odin::telnet::WONT).get_local_request() == odin::telnet::DONT);
}

void telnet_completed_negotiation_test::test_remote_request()
{
    CPPUNIT_ASSERT(
        odin::telnet::completed_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WILL
          , odin::telnet::DO).get_remote_request() == odin::telnet::DO);

    CPPUNIT_ASSERT(
        odin::telnet::completed_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WONT
          , odin::telnet::DONT).get_remote_request() == odin::telnet::DONT);

    CPPUNIT_ASSERT(
        odin::telnet::completed_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::DO
          , odin::telnet::WILL).get_remote_request() == odin::telnet::WILL);

    CPPUNIT_ASSERT(
        odin::telnet::completed_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::DONT
          , odin::telnet::WONT).get_remote_request() == odin::telnet::WONT);
}

void telnet_completed_negotiation_test::test_type()
{
    for (odin::telnet::negotiation_type type = 0;
         type < odin::telnet::negotiation_type(-1);
         ++type)
    {
        CPPUNIT_ASSERT(
            odin::telnet::completed_negotiation(
                odin::telnet::local
              , type
              , odin::telnet::WILL
              , odin::telnet::DO).get_type() == type);
    }
}

void telnet_completed_negotiation_test::test_equality()
{
    CPPUNIT_ASSERT(
        odin::telnet::completed_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WILL
          , odin::telnet::DO)
     == odin::telnet::completed_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WILL
          , odin::telnet::DO));
    
    CPPUNIT_ASSERT(!(
        odin::telnet::completed_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WILL
          , odin::telnet::DO)
     == odin::telnet::completed_negotiation(
            odin::telnet::remote
          , 0
          , odin::telnet::WILL
          , odin::telnet::DO)));
    
    CPPUNIT_ASSERT(!(
        odin::telnet::completed_negotiation(
            odin::telnet::local
          , 255
          , odin::telnet::WILL
          , odin::telnet::DO)
     == odin::telnet::completed_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WILL
          , odin::telnet::DO)));
    
    CPPUNIT_ASSERT(!(
        odin::telnet::completed_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WONT
          , odin::telnet::DO)
     == odin::telnet::completed_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WILL
          , odin::telnet::DO)));
    
    CPPUNIT_ASSERT(!(
        odin::telnet::completed_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WILL
          , odin::telnet::DONT)
     == odin::telnet::completed_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WILL
          , odin::telnet::DO)));
}

void telnet_completed_negotiation_test::test_assign()
{
    odin::telnet::completed_negotiation negotiation1(
        odin::telnet::local
      , 0
      , odin::telnet::WILL
      , odin::telnet::DO);
    
    odin::telnet::completed_negotiation negotiation2(
        odin::telnet::remote
      , 255
      , odin::telnet::DO
      , odin::telnet::WILL);
    
    negotiation1 = negotiation2;
    
    CPPUNIT_ASSERT(negotiation1 == negotiation2);
}

void telnet_completed_negotiation_test::test_copy()
{
    odin::telnet::completed_negotiation negotiation1(
        odin::telnet::local
      , 0
      , odin::telnet::WILL
      , odin::telnet::DO);
    
    odin::telnet::completed_negotiation negotiation2(negotiation1);
    
    CPPUNIT_ASSERT(negotiation1 == negotiation2);
}

void telnet_completed_negotiation_test::test_complete()
{
    odin::telnet::initiated_negotiation initiated_negotiation(
        odin::telnet::local
      , 0
      , odin::telnet::WILL);
    
    odin::telnet::completed_negotiation completed_negotiation(
        initiated_negotiation
      , odin::telnet::DO);
        
    CPPUNIT_ASSERT(
        completed_negotiation
     == odin::telnet::completed_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WILL
          , odin::telnet::DO));
}

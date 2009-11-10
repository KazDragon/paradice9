#include "initiated_negotiation_test.hpp"
#include "odin/telnet/initiated_negotiation.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_initiated_negotiation_test);

void telnet_initiated_negotiation_test::test_initiator()
{
    CPPUNIT_ASSERT(
        odin::telnet::initiated_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WILL).get_initiator() == odin::telnet::local);
    
    CPPUNIT_ASSERT(
        odin::telnet::initiated_negotiation(
            odin::telnet::remote
          , 0
          , odin::telnet::WILL).get_initiator() == odin::telnet::remote);
}

void telnet_initiated_negotiation_test::test_request()
{
    CPPUNIT_ASSERT(
        odin::telnet::initiated_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WILL).get_request() == odin::telnet::WILL);

    CPPUNIT_ASSERT(
        odin::telnet::initiated_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WONT).get_request() == odin::telnet::WONT);

    CPPUNIT_ASSERT(
        odin::telnet::initiated_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::DO).get_request() == odin::telnet::DO);

    CPPUNIT_ASSERT(
        odin::telnet::initiated_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::DONT).get_request() == odin::telnet::DONT);
}

void telnet_initiated_negotiation_test::test_type()
{
    for (odin::telnet::negotiation_type type = 0;
         type < odin::telnet::negotiation_type(-1);
         ++type)
    {
        CPPUNIT_ASSERT(
            odin::telnet::initiated_negotiation(
                odin::telnet::local
              , type
              , odin::telnet::WILL).get_type() == type);
    }
}

void telnet_initiated_negotiation_test::test_equality()
{
    CPPUNIT_ASSERT(
        odin::telnet::initiated_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WILL)
     == odin::telnet::initiated_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WILL));

    CPPUNIT_ASSERT(!(
        odin::telnet::initiated_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WILL)
     == odin::telnet::initiated_negotiation(
            odin::telnet::remote
          , 0
          , odin::telnet::WILL)));

    CPPUNIT_ASSERT(!(
        odin::telnet::initiated_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WILL)
     == odin::telnet::initiated_negotiation(
            odin::telnet::local
          , 1
          , odin::telnet::WILL)));

    CPPUNIT_ASSERT(!(
        odin::telnet::initiated_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WILL)
     == odin::telnet::initiated_negotiation(
            odin::telnet::local
          , 0
          , odin::telnet::WONT)));
}

void telnet_initiated_negotiation_test::test_assign()
{
    odin::telnet::initiated_negotiation negotiation1(
        odin::telnet::local
      , 0
      , odin::telnet::WILL);
    
    odin::telnet::initiated_negotiation negotiation2(
        odin::telnet::remote
      , 255
      , odin::telnet::DO);
    
    negotiation1 = negotiation2;
    
    CPPUNIT_ASSERT(negotiation1 == negotiation2);
}

void telnet_initiated_negotiation_test::test_copy()
{
    odin::telnet::initiated_negotiation negotiation1(
        odin::telnet::local
      , 0
      , odin::telnet::WILL);
    
    odin::telnet::initiated_negotiation negotiation2(negotiation1);
    
    CPPUNIT_ASSERT(negotiation1 == negotiation2);
}

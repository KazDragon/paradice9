#include "telnet_generator_fixture.hpp"
#include "odin/telnet/detail/generator.hpp"
#include "odin/telnet/protocol.hpp"
#include <boost/assign/list_of.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/variant.hpp>
#include <deque>

using namespace boost;
using namespace boost::assign;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(telnet_generator_fixture);

void telnet_generator_fixture::test_constructor()
{
    odin::telnet::detail::generator generate;
    (void)generate;
}

typedef boost::variant<
    odin::telnet::command
  , odin::telnet::negotiation_type
  , odin::telnet::subnegotiation_type
  , std::string
> element_type;

void telnet_generator_fixture::test_generate_empty()
{
    odin::telnet::detail::generator generate;
    
    deque<element_type> empty_buffer;
    
    BOOST_AUTO(begin, empty_buffer.begin());
    BOOST_AUTO(end,   empty_buffer.end());
    
    odin::telnet::detail::generator::result_type result = generate(begin, end);
    
    CPPUNIT_ASSERT_EQUAL(true, result.empty());
    CPPUNIT_ASSERT(begin == end);
}

void telnet_generator_fixture::test_generate_normal_empty()
{
    odin::telnet::detail::generator generate;
    
    string       element0;
    element_type variant0(element0);
    
    vector<element_type> variant_array = list_of(variant0);
    
    BOOST_AUTO(begin, variant_array.begin());
    BOOST_AUTO(end,   variant_array.end());
    
    odin::telnet::detail::generator::result_type result = generate(begin, end);
    
    vector<odin::u8> expected_array;
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), result.size());
    CPPUNIT_ASSERT(expected_array == result);
    CPPUNIT_ASSERT(begin == end);
}

void telnet_generator_fixture::test_generate_normal_one()
{
    odin::telnet::detail::generator generate;
    
    string       element0("x");
    element_type variant0(element0);
    
    vector<element_type> variant_array = list_of(variant0);
    
    BOOST_AUTO(begin, variant_array.begin());
    BOOST_AUTO(end,   variant_array.end());
    
    odin::telnet::detail::generator::result_type result = generate(begin, end);
    
    odin::u8 expected_data[] = { 'x' };
    vector<odin::u8> expected_array(expected_data, expected_data + 1);
    
    CPPUNIT_ASSERT_EQUAL(
        expected_array.size()
      , result.size());
    
    CPPUNIT_ASSERT(expected_array == result);
}

void telnet_generator_fixture::test_generate_normal_two()
{
    odin::telnet::detail::generator generate;
    
    string       element0("xy");
    element_type variant0(element0);
    
    vector<element_type> variant_array = list_of(variant0);
    
    BOOST_AUTO(begin, variant_array.begin());
    BOOST_AUTO(end,   variant_array.end());
    
    odin::telnet::detail::generator::result_type result = generate(begin, end);
    
    odin::u8 expected_data[] = { 'x', 'y' };
    vector<odin::u8> expected_array(expected_data, expected_data + 2);
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), result.size());
    CPPUNIT_ASSERT(expected_array == result);
    CPPUNIT_ASSERT(begin == end);
}

void telnet_generator_fixture::test_generate_normal_iac()
{
    odin::telnet::detail::generator generate;
    
    string       element0("\xFF");
    element_type variant0(element0);
    
    vector<element_type> variant_array = list_of(variant0);
    
    BOOST_AUTO(begin, variant_array.begin());
    BOOST_AUTO(end,   variant_array.end());
    
    odin::telnet::detail::generator::result_type result = generate(begin, end);
    
    odin::u8 expected_data[] = { odin::telnet::IAC, odin::telnet::IAC };
    vector<odin::u8> expected_array(expected_data, expected_data + 2);
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), result.size());
    CPPUNIT_ASSERT(expected_array == result);
    CPPUNIT_ASSERT(begin == end);
}

void telnet_generator_fixture::test_generate_command()
{
    odin::telnet::detail::generator generate;
    
    odin::telnet::command element0(odin::telnet::NOP);
    element_type          variant0(element0);
    
    vector<element_type> variant_array = list_of(variant0);
    
    BOOST_AUTO(begin, variant_array.begin());
    BOOST_AUTO(end,   variant_array.end());
    
    odin::telnet::detail::generator::result_type result = generate(begin, end);
    
    odin::u8 expected_data[] = { odin::telnet::IAC, odin::telnet::NOP };
    vector<odin::u8> expected_array(expected_data, expected_data + 2);
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), result.size());
    CPPUNIT_ASSERT(expected_array == result);
    CPPUNIT_ASSERT(begin == end);
}

void telnet_generator_fixture::test_generate_negotiation()
{
    odin::telnet::detail::generator generate;
    
    odin::telnet::negotiation_type element0;
    element0.request_   = odin::telnet::WILL;
    element0.option_id_ = odin::telnet::ECHO;

    element_type variant0(element0);
    
    vector<element_type> variant_array = list_of(variant0);
    
    BOOST_AUTO(begin, variant_array.begin());
    BOOST_AUTO(end,   variant_array.end());
    
    odin::telnet::detail::generator::result_type result = generate(begin, end);
    
    odin::u8 expected_data[] = { 
        odin::telnet::IAC, odin::telnet::WILL, odin::telnet::ECHO
    };
    
    vector<odin::u8> expected_array(expected_data, expected_data + 3);
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), result.size());
    CPPUNIT_ASSERT(expected_array == result);
    CPPUNIT_ASSERT(begin == end);
}

void telnet_generator_fixture::test_generate_subnegotiation()
{
    odin::telnet::detail::generator generate;
    
    odin::telnet::subnegotiation_type element0;
    element0.option_id_ = odin::telnet::ECHO;
    element0.content_   = list_of
        ('a')('b')('c')('\xFF')('d');

    element_type variant0(element0);
    
    vector<element_type> variant_array = list_of(variant0);
    
    BOOST_AUTO(begin, variant_array.begin());
    BOOST_AUTO(end,   variant_array.end());
    
    odin::telnet::detail::generator::result_type result = generate(begin, end);
    
    odin::u8 expected_data[] = { 
        odin::telnet::IAC, odin::telnet::SB, odin::telnet::ECHO
      , 'a', 'b', 'c', odin::telnet::IAC, odin::telnet::IAC, 'd'
      , odin::telnet::IAC, odin::telnet::SE
    };
    
    vector<odin::u8> expected_array(expected_data, expected_data + 11);
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), result.size());
    CPPUNIT_ASSERT(expected_array == result);
    CPPUNIT_ASSERT(begin == end);
}

void telnet_generator_fixture::test_generate_many()
{
    odin::telnet::detail::generator generate;

    string       element0("abcd");
    element_type variant0(element0);
    
    string       element1("ef\xFFg");
    element_type variant1(element1);
    
    odin::telnet::negotiation_type element2;
    element2.request_   = odin::telnet::WONT;
    element2.option_id_ = odin::telnet::EXOPL;
    element_type variant2(element2);
    
    odin::telnet::subnegotiation_type element3;
    element3.option_id_ = odin::telnet::EXOPL;
    element3.content_   = list_of('\xFF');
    element_type variant3(element3);
    
    vector<element_type> variant_array = 
        list_of(variant0)(variant1)(variant2)(variant3);
    
    BOOST_AUTO(begin, variant_array.begin());
    BOOST_AUTO(end,   variant_array.end());
    
    odin::telnet::detail::generator::result_type result = generate(begin, end);
    
    odin::u8 expected_data[] = {
        'a', 'b', 'c', 'd'
      , 'e', 'f', odin::telnet::IAC, odin::telnet::IAC, 'g'
      , odin::telnet::IAC, odin::telnet::WONT
        , odin::telnet::EXOPL
      , odin::telnet::IAC, odin::telnet::SB
        , odin::telnet::EXOPL
        , odin::telnet::IAC, odin::telnet::IAC
        , odin::telnet::IAC, odin::telnet::SE
    };
    
    vector<odin::u8> expected_array(expected_data, expected_data + 19);
    
    CPPUNIT_ASSERT_EQUAL(expected_array.size(), result.size());
    CPPUNIT_ASSERT(expected_array == result);
    CPPUNIT_ASSERT(begin == end);
}

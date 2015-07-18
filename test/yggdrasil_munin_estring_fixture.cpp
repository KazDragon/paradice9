#include "yggdrasil_munin_estring_fixture.hpp"
#include "yggdrasil/munin/estring.hpp"
#include "yggdrasil/munin/element.hpp"
#include "yggdrasil/munin/detail/string_to_elements_parser.hpp"
#include <boost/concept_check.hpp>
#include <iterator>
#include <limits>
#include <type_traits>
#include <cstring>

CPPUNIT_TEST_SUITE_REGISTRATION(yggdrasil_munin_estring_fixture);

void yggdrasil_munin_estring_fixture::test_container_concepts()
{
    BOOST_CONCEPT_ASSERT(
        (boost::RandomAccessContainer<yggdrasil::munin::estring>));
}

void yggdrasil_munin_estring_fixture::test_default_constructor()
{
    auto const str = yggdrasil::munin::estring{};
    
    CPPUNIT_ASSERT_EQUAL(size_t(0), str.size());
    CPPUNIT_ASSERT_EQUAL(true, str.empty());
    CPPUNIT_ASSERT_EQUAL(str.begin(), str.end());
    CPPUNIT_ASSERT_EQUAL(str.rbegin(), str.rend());
    CPPUNIT_ASSERT_EQUAL(
        std::numeric_limits<yggdrasil::munin::estring::size_type>::max(),
        str.max_size());
    CPPUNIT_ASSERT(str == yggdrasil::munin::estring{});
    CPPUNIT_ASSERT(!(str != yggdrasil::munin::estring{}));
}

void yggdrasil_munin_estring_fixture::test_charstar_constructor()
{
    char const *test_str = "foo";
    
    auto const str = yggdrasil::munin::estring(test_str);
    CPPUNIT_ASSERT_EQUAL(strlen(test_str), str.size());
    CPPUNIT_ASSERT_EQUAL(false, str.empty());
    CPPUNIT_ASSERT_EQUAL(str.begin() + str.size(), str.end());
    CPPUNIT_ASSERT_EQUAL(str.rbegin() - str.size(), str.rend());
    CPPUNIT_ASSERT_EQUAL(
        std::numeric_limits<yggdrasil::munin::estring::size_type>::max(),
                         str.max_size());
    CPPUNIT_ASSERT(!(str == yggdrasil::munin::estring{}));
    CPPUNIT_ASSERT(str == str);
    CPPUNIT_ASSERT(str != yggdrasil::munin::estring{});
    CPPUNIT_ASSERT(!(str != str));
    
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element(test_str[0]), str[0]);
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element(test_str[1]), str[1]);
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element(test_str[2]), str[2]);
}

void yggdrasil_munin_estring_fixture::test_string_constructor()
{
    std::string const test_str = "foo";
    
    auto const str = yggdrasil::munin::estring(test_str);
    CPPUNIT_ASSERT_EQUAL(test_str.size(), str.size());
    CPPUNIT_ASSERT_EQUAL(false, str.empty());
    CPPUNIT_ASSERT_EQUAL(str.begin() + str.size(), str.end());
    CPPUNIT_ASSERT_EQUAL(str.rbegin() - str.size(), str.rend());
    CPPUNIT_ASSERT_EQUAL(
        std::numeric_limits<yggdrasil::munin::estring::size_type>::max(),
                         str.max_size());
    CPPUNIT_ASSERT(!(str == yggdrasil::munin::estring{}));
    CPPUNIT_ASSERT(str == str);
    CPPUNIT_ASSERT(str != yggdrasil::munin::estring{});
    CPPUNIT_ASSERT(!(str != str));
    
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element(test_str[0]), str[0]);
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element(test_str[1]), str[1]);
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element(test_str[2]), str[2]);
}

void yggdrasil_munin_estring_fixture::test_literal_constructor()
{
    {
        using namespace yggdrasil::literals::estring_literals;
        
        CPPUNIT_ASSERT_EQUAL(
            yggdrasil::munin::estring("foo"), "foo"_es);
    }
    
    {
        using namespace yggdrasil::literals;

        CPPUNIT_ASSERT_EQUAL(
            yggdrasil::munin::estring("foo"), "foo"_es);
    }
    
    {
        using namespace yggdrasil::estring_literals;

        CPPUNIT_ASSERT_EQUAL(
            yggdrasil::munin::estring("foo"), "foo"_es);
    }
    
    {
        using namespace yggdrasil;

        CPPUNIT_ASSERT_EQUAL(
            yggdrasil::munin::estring("foo"), "foo"_es);
    }
}

void yggdrasil_munin_estring_fixture::test_supports_parsed_strings()
{
    using namespace yggdrasil::literals;
    
    auto const str = "\\i>bo\\xld"_es;
    
    yggdrasil::munin::attribute bold;
    bold.intensity_ = yggdrasil::graphics::intensity::bold;
    
    CPPUNIT_ASSERT_EQUAL(4ul, str.size());
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::munin::element('b', bold), str[0]);
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::munin::element('o', bold), str[1]);
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::munin::element('l', {}), str[2]);
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::munin::element('d', {}), str[3]);
}


void yggdrasil_munin_estring_fixture::test_string_append_operator()
{
    using namespace yggdrasil::literals;
    
    auto str = "foo"_es;
    str += "bar"_es;
    
    CPPUNIT_ASSERT_EQUAL(str, "foobar"_es);
}

void yggdrasil_munin_estring_fixture::test_string_concatenate_operator()
{
    using namespace yggdrasil::literals;
    
    CPPUNIT_ASSERT_EQUAL("foobar"_es, "foo"_es + "bar"_es);
}

void yggdrasil_munin_estring_fixture::test_string_assignment()
{
    auto str = yggdrasil::munin::estring("\\i>foo");
    
    auto bold = yggdrasil::munin::attribute();
    bold.intensity_ = yggdrasil::graphics::intensity::bold;
    
    auto faint = yggdrasil::munin::attribute();
    faint.intensity_ = yggdrasil::graphics::intensity::faint;
    
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element('f', bold), str[0]);
    
    str = "\\i<foo";
    
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element('f', faint), str[0]);
}

#include "test/yggdrasil_munin_create_model_from_ptree_fixture.hpp"
#include "yggdrasil/munin/ptree_model.hpp"
#include "yggdrasil/munin/model.hpp"
#include "yggdrasil/munin/basic_model.hpp"
#include "yggdrasil/munin/estring.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <sstream>

CPPUNIT_TEST_SUITE_REGISTRATION(yggdrasil_munin_create_model_from_ptree_fixture);

void yggdrasil_munin_create_model_from_ptree_fixture::test_empty_ptree_creates_default_model()
{
    std::stringstream is("{}");
    boost::property_tree::ptree tree;
    boost::property_tree::read_json(is, tree);

    auto model = yggdrasil::munin::model{
        yggdrasil::munin::create_model_from_ptree<
            yggdrasil::munin::basic_model
        >(tree)};

    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::extent{}, get_size(model));
}

void yggdrasil_munin_create_model_from_ptree_fixture::test_ptree_with_only_name_creates_default_model()
{
    // The "name" property on the outermost leaf isn't part of the model per
    // se, and should have no effect on the model.
    std::stringstream is(R"(
        { "name" : "test" }
    )");
    boost::property_tree::ptree tree;
    boost::property_tree::read_json(is, tree);

    auto model = yggdrasil::munin::model{
        yggdrasil::munin::create_model_from_ptree<
            yggdrasil::munin::basic_model
        >(tree)};

    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::extent{}, get_size(model));

    boost::any prop = get_property(model, "name");
    CPPUNIT_ASSERT(prop.empty());
}

void yggdrasil_munin_create_model_from_ptree_fixture::test_ptree_with_estring_property()
{
    using namespace yggdrasil::literals;

    // The model should add properties as described in the model description.
    // In particular, given a property with a given name and a string argument,
    // that string should be found in the model's properties.  Note: the model
    // should store the string value as an estring.
    std::stringstream is(R"(
        { "name" : "test",
          "properties" : [
            { "name" : "strprop",
              "type" : "estring",
              "default" : "success" }
          ]
        }
    )");

    boost::property_tree::ptree tree;
    boost::property_tree::read_json(is, tree);

    auto model = yggdrasil::munin::model{
        yggdrasil::munin::create_model_from_ptree<
            yggdrasil::munin::basic_model
        >(tree)};

    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::extent{}, get_size(model));

    boost::any prop = get_property(model, "strprop");
    CPPUNIT_ASSERT(!prop.empty());
    auto value = boost::any_cast<yggdrasil::munin::estring>(prop);
    CPPUNIT_ASSERT_EQUAL("success"_es, value);
}

void yggdrasil_munin_create_model_from_ptree_fixture::test_ptree_with_array_of_estring_property()
{
    using namespace yggdrasil::literals;

    // The model should add properties as described in the model description.
    // In particular, given a property with a given name and an argument that
    // is an array of strings, that array should be found in the model's
    // properties as a vector<estring>.
    std::stringstream is(R"(
        { "name" : "test",
          "properties" : [
            { "name" : "value",
              "type" : "array of estring",
              "default" : [
                "o-o",
                "-o-",
                "x-x"
              ]
            }
          ]
        }
    )");


    boost::property_tree::ptree tree;
    boost::property_tree::read_json(is, tree);

    auto model = yggdrasil::munin::model{
        yggdrasil::munin::create_model_from_ptree<
            yggdrasil::munin::basic_model
        >(tree)};

    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::extent{}, get_size(model));

    boost::any prop = get_property(model, "value");
    CPPUNIT_ASSERT(!prop.empty());
    auto value = boost::any_cast<std::vector<yggdrasil::munin::estring>>(prop);
    auto expected = std::vector<yggdrasil::munin::estring>{
        "o-o"_es,
        "-o-"_es,
        "x-x"_es
    };

    CPPUNIT_ASSERT(expected == value);
}

void yggdrasil_munin_create_model_from_ptree_fixture::test_ptree_with_char_property()
{
    // The model should add properties as described in the model description.
    // In particular, given a property with the type "char", it should store
    // the first character of the value as a char.
    std::stringstream is(R"(
        { "name" : "test",
          "properties" : [
            { "name" : "value",
              "type" : "char",
              "default" : "x"
            }
          ]
        }
    )");


    boost::property_tree::ptree tree;
    boost::property_tree::read_json(is, tree);

    auto model = yggdrasil::munin::model{
        yggdrasil::munin::create_model_from_ptree<
        yggdrasil::munin::basic_model
        >(tree)};

        CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::extent{}, get_size(model));

        boost::any prop = get_property(model, "value");
        CPPUNIT_ASSERT(!prop.empty());
        auto value = boost::any_cast<char>(prop);
        auto expected = char('x');

        CPPUNIT_ASSERT_EQUAL(expected, value);
}

void yggdrasil_munin_create_model_from_ptree_fixture::test_ptree_with_array_of_char_property()
{
    // The model should add properties as described in the model description.
    // In particular, given a property with a given name and an argument that
    // is an array of char, that array should be found in the model's
    // properties as a vector<char>.
    std::stringstream is(R"(
        { "name" : "test",
          "properties" : [
            { "name" : "value",
              "type" : "array of char",
              "default" : [
                "a",
                "b",
                "c",
                "d"
              ]
            }
          ]
        }
    )");


    boost::property_tree::ptree tree;
    boost::property_tree::read_json(is, tree);

    auto model = yggdrasil::munin::model{
        yggdrasil::munin::create_model_from_ptree<
            yggdrasil::munin::basic_model
        >(tree)};

    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::extent{}, get_size(model));

    boost::any prop = get_property(model, "value");
    CPPUNIT_ASSERT(!prop.empty());
    auto value = boost::any_cast<std::vector<char>>(prop);
    auto expected = std::vector<char>{'a', 'b', 'c', 'd'};

    CPPUNIT_ASSERT(expected == value);
}

void yggdrasil_munin_create_model_from_ptree_fixture::test_ptree_with_string_property()
{
    // The model should add properties as described in the model description.
    // In particular, given a property with a given name and a string argument,
    // that string should be found in the model's properties.  Note: the model
    // should store the string value as an estring.
    std::stringstream is(R"(
        { "name" : "test",
            "properties" : [
            { "name" : "strprop",
                "type" : "string",
                "default" : "success" }
            ]
        }
    )");

    boost::property_tree::ptree tree;
    boost::property_tree::read_json(is, tree);

    auto model = yggdrasil::munin::model{
        yggdrasil::munin::create_model_from_ptree<
        yggdrasil::munin::basic_model
    >(tree)};

    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::extent{}, get_size(model));

    boost::any prop = get_property(model, "strprop");
    CPPUNIT_ASSERT(!prop.empty());
    auto value = boost::any_cast<std::string>(prop);
    CPPUNIT_ASSERT_EQUAL(std::string("success"), value);
}

void yggdrasil_munin_create_model_from_ptree_fixture::test_ptree_with_array_of_string_property()
{
    // The model should add properties as described in the model description.
    // In particular, given a property with a given name and an argument that
    // is an array of string, that array should be found in the model's
    // properties as a vector<string>.
    std::stringstream is(R"(
        { "name" : "test",
          "properties" : [
            { "name" : "value",
              "type" : "array of string",
              "default" : [
                "alice",
                "betty",
                "carol"
              ]
            }
          ]
        }
    )");


    boost::property_tree::ptree tree;
    boost::property_tree::read_json(is, tree);

    auto model = yggdrasil::munin::model{
        yggdrasil::munin::create_model_from_ptree<
            yggdrasil::munin::basic_model
        >(tree)};

    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::extent{}, get_size(model));

    boost::any prop = get_property(model, "value");
    CPPUNIT_ASSERT(!prop.empty());
    auto value = boost::any_cast<std::vector<std::string>>(prop);
    auto expected = std::vector<std::string>{"alice", "betty", "carol"};

    CPPUNIT_ASSERT(expected == value);
}

void yggdrasil_munin_create_model_from_ptree_fixture::test_ptree_with_element_property()
{
    using namespace yggdrasil::literals;

    // The model should add properties as described in the model description.
    // In particular, given a property with the type "char", it should store
    // the first character of the value as a char.
    std::stringstream is(R"(
        { "name" : "test",
          "properties" : [
            { "name" : "value",
                "type" : "element",
                "default" : "\\i>b"
            }
          ]
        }
    )");


    boost::property_tree::ptree tree;
    boost::property_tree::read_json(is, tree);

    auto model = yggdrasil::munin::model{
        yggdrasil::munin::create_model_from_ptree<
        yggdrasil::munin::basic_model
    >(tree)};

    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::extent{}, get_size(model));

    boost::any prop = get_property(model, "value");
    CPPUNIT_ASSERT(!prop.empty());
    auto value = boost::any_cast<yggdrasil::munin::element>(prop);
    auto expected = yggdrasil::munin::element("\\i>b"_es[0]);

    CPPUNIT_ASSERT_EQUAL(expected, value);
}

void yggdrasil_munin_create_model_from_ptree_fixture::test_ptree_with_array_of_element_property()
{
    using namespace yggdrasil::literals;

    // The model should add properties as described in the model description.
    // In particular, given a property with a given name and an argument that
    // is an array of element, that array should be found in the model's
    // properties as a vector<element>.
    std::stringstream is(R"(
        { "name" : "test",
          "properties" : [
            { "name" : "value",
              "type" : "array of element",
              "default" : [
                "a",
                "\\i<x",
                "c"
              ]
            }
          ]
        }
    )");


    boost::property_tree::ptree tree;
    boost::property_tree::read_json(is, tree);

    auto model = yggdrasil::munin::model{
        yggdrasil::munin::create_model_from_ptree<
            yggdrasil::munin::basic_model
        >(tree)};

    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::extent{}, get_size(model));

    boost::any prop = get_property(model, "value");
    CPPUNIT_ASSERT(!prop.empty());
    auto value = boost::any_cast<std::vector<yggdrasil::munin::element>>(prop);
    auto expected = std::vector<yggdrasil::munin::element>{
        "a"_es[0], "\\i<x"_es[0], "c"_es[0]};

    CPPUNIT_ASSERT(expected == value);
}

void yggdrasil_munin_create_model_from_ptree_fixture::test_ptree_with_untyped_estring_property()
{
    using namespace yggdrasil::literals;

    // The model should add properties as described in the model description.
    // In particular, given a property with a given name and a string argument,
    // that string should be found in the model's properties.  Note: the model
    // should store the string value as an estring.
    std::stringstream is(R"(
        { "name" : "test",
          "properties" : [
            { "name" : "strprop",
              "default" : "success" }
          ]
        }
    )");

    boost::property_tree::ptree tree;
    boost::property_tree::read_json(is, tree);

    auto model = yggdrasil::munin::model{
        yggdrasil::munin::create_model_from_ptree<
            yggdrasil::munin::basic_model
        >(tree)};

    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::extent{}, get_size(model));

    boost::any prop = get_property(model, "strprop");
    CPPUNIT_ASSERT(!prop.empty());
    auto value = boost::any_cast<yggdrasil::munin::estring>(prop);
    CPPUNIT_ASSERT_EQUAL("success"_es, value);
}

void yggdrasil_munin_create_model_from_ptree_fixture::test_ptree_with_untyped_array_of_estring_property()
{
    using namespace yggdrasil::literals;

    // The model should add properties as described in the model description.
    // In particular, given a property with a given name and an argument that
    // is an array of strings, that array should be found in the model's
    // properties as a vector<estring>.
    std::stringstream is(R"(
        { "name" : "test",
          "properties" : [
            { "name" : "value",
              "default" : [
                "o-o",
                "-o-",
                "x-x"
              ]
            }
          ]
        }
    )");


    boost::property_tree::ptree tree;
    boost::property_tree::read_json(is, tree);

    auto model = yggdrasil::munin::model{
        yggdrasil::munin::create_model_from_ptree<
            yggdrasil::munin::basic_model
        >(tree)};

    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::extent{}, get_size(model));

    boost::any prop = get_property(model, "value");
    CPPUNIT_ASSERT(!prop.empty());
    auto value = boost::any_cast<std::vector<yggdrasil::munin::estring>>(prop);
    auto expected = std::vector<yggdrasil::munin::estring>{
        "o-o"_es,
        "-o-"_es,
        "x-x"_es
    };

    CPPUNIT_ASSERT(expected == value);
}

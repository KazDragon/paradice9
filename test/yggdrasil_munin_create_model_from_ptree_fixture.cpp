#include "test/yggdrasil_munin_create_model_from_ptree_fixture.hpp"
#include "yggdrasil/munin/ptree_model.hpp"
#include "yggdrasil/munin/model.hpp"
#include "yggdrasil/munin/basic_model.hpp"
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

void yggdrasil_munin_create_model_from_ptree_fixture::test_ptree_with_string_property_creates_model_with_that_property()
{
    // The model should add properties as described in the model description.
    // In particular, given a property with a given name and a string argument,
    // that string should be found in the model's properties.
    std::stringstream is(R"(
        { "name" : "test",
          "properties" : [
            { "name" : "strprop",
              "value" : "success" }
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

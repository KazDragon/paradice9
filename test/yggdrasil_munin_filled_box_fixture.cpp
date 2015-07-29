#include "yggdrasil_munin_filled_box_fixture.hpp"
#include "fake_resource_manager.hpp"
#include "yggdrasil/munin/filled_box.hpp"
#include "yggdrasil/munin/component.hpp"
#include "yggdrasil/munin/element.hpp"
#include "yggdrasil/munin/estring.hpp"
#include "yggdrasil/munin/model.hpp"
#include "yggdrasil/munin/canvas.hpp"
#include "yggdrasil/munin/rectangle.hpp"
#include "yggdrasil/kvasir/make_unique.hpp"
#include <boost/property_tree/json_parser.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(yggdrasil_munin_filled_box_fixture);

void yggdrasil_munin_filled_box_fixture::test_default_constructor()
{
    using namespace yggdrasil::literals;

    // Setup - populate the resource manager with only Filled Box's default
    // values.
    {
        auto resource_manager =
            yggdrasil::kvasir::make_unique<fake_resource_manager>();
        auto tree = boost::property_tree::ptree{};
        boost::property_tree::read_json(
            "../../yggdrasil/munin/res/filled_box.json",
            tree);
        resource_manager->add_default_properties("filled_box", tree);
        set_resource_manager(std::move(resource_manager));
    }

    auto box = yggdrasil::munin::component{yggdrasil::munin::filled_box()};
    auto model = box.get_model();
    auto brush = boost::any_cast<yggdrasil::munin::element>(
        model.get_property("brush"));
    auto expected = " "_es[0];

    CPPUNIT_ASSERT_EQUAL(expected, brush);
}

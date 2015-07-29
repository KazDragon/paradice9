#include "yggdrasil_munin_image_fixture.hpp"
#include "fake_resource_manager.hpp"
#include "yggdrasil/munin/image.hpp"
#include "yggdrasil/munin/component.hpp"
#include "yggdrasil/munin/estring.hpp"
#include "yggdrasil/munin/canvas.hpp"
#include "yggdrasil/munin/rectangle.hpp"
#include "yggdrasil/kvasir/make_unique.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

CPPUNIT_TEST_SUITE_REGISTRATION(yggdrasil_munin_image_fixture);

void yggdrasil_munin_image_fixture::image_with_no_args_draws_nothing()
{
    using namespace yggdrasil::literals;

    // Setup - populate the resource manager with only Image's default
    // values.
    {
        auto resource_manager =
            yggdrasil::kvasir::make_unique<fake_resource_manager>();
        auto tree = boost::property_tree::ptree{};
        boost::property_tree::read_json(
            "../../yggdrasil/munin/res/image.json",
            tree);
        resource_manager->add_default_properties("image", tree);
        set_resource_manager(std::move(resource_manager));
    }

    // When an image is default constructed, it should be initialised with
    // default properties.  When drawn, this should only paint the background
    // brush.
    auto image = yggdrasil::munin::component{yggdrasil::munin::image{}};
    auto background_brush = boost::any_cast<yggdrasil::munin::element>(
        get_property(get_model(image), "background_brush"));

    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element(' '), background_brush);

    auto value = boost::any_cast<std::vector<yggdrasil::munin::estring>>(
        get_property(get_model(image), "value"));

    CPPUNIT_ASSERT(
        std::vector<yggdrasil::munin::estring>{} == value);

    auto canvas = yggdrasil::munin::canvas({1, 1});
    canvas[0][0] = "x"_es[0];
    auto const region = yggdrasil::munin::rectangle({0, 0}, canvas.get_size());

    draw(image, canvas, region);

    CPPUNIT_ASSERT(canvas[0][0] == "x"_es[0]);
}

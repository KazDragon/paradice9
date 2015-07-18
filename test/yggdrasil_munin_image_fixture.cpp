#include "yggdrasil_munin_image_fixture.hpp"
#include "yggdrasil/munin/image.hpp"
#include "yggdrasil/munin/component.hpp"
#include "yggdrasil/munin/estring.hpp"
#include "yggdrasil/munin/canvas.hpp"
#include "yggdrasil/munin/rectangle.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(yggdrasil_munin_image_fixture);

void yggdrasil_munin_image_fixture::test_default_constructor()
{
    auto image = yggdrasil::munin::image();
}

void yggdrasil_munin_image_fixture::test_vector_constructor()
{
    using namespace yggdrasil::literals;

    auto image = yggdrasil::munin::image(
        std::vector<yggdrasil::munin::estring>{"foobar"_es});
}

void yggdrasil_munin_image_fixture::test_image_is_component()
{
    auto comp = yggdrasil::munin::component(yggdrasil::munin::image());
}

void yggdrasil_munin_image_fixture::test_size()
{
    auto comp = yggdrasil::munin::component(yggdrasil::munin::image());
    auto model = get_model(comp);

    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::extent(0, 0), get_size(model));

    auto new_size = yggdrasil::munin::extent(1, 1);
    set_size(model, new_size);

    CPPUNIT_ASSERT_EQUAL(new_size, get_size(model));
}

void yggdrasil_munin_image_fixture::test_background_brush()
{
    auto image = yggdrasil::munin::image();
    auto &model = image.get_model();

    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::munin::element(' '), model.get_background_brush());

    auto const new_brush = yggdrasil::munin::element('#');

    model.set_background_brush(new_brush);

    CPPUNIT_ASSERT_EQUAL(new_brush, model.get_background_brush());
}

void yggdrasil_munin_image_fixture::test_set_image()
{
    using namespace yggdrasil::literals;

    {
        auto image = yggdrasil::munin::image();
        auto &model = image.get_model();

        CPPUNIT_ASSERT(
            std::vector<yggdrasil::munin::estring>() == model.get_image());
    }

    {
        auto const im = std::vector<yggdrasil::munin::estring> {
            "foo"_es,
            "bar"_es,
            "baz"_es
        };

        auto image = yggdrasil::munin::image(im);
        auto &model = image.get_model();

        CPPUNIT_ASSERT(im == model.get_image());
    }

    {
        auto image = yggdrasil::munin::image();
        auto &model = image.get_model();

        auto const im = std::vector<yggdrasil::munin::estring> {
            "foo"_es,
            "bar"_es,
            "baz"_es
        };

        model.set_image(im);

        CPPUNIT_ASSERT(im == model.get_image());
    }
}

void yggdrasil_munin_image_fixture::test_draw()
{
    using namespace yggdrasil::literals;

    static auto const lines = std::vector<yggdrasil::munin::estring> {
        "012"_es,
        "345"_es,
        "678"_es
    };

    auto const image = yggdrasil::munin::component(
        yggdrasil::munin::image(lines));

    auto canvas = yggdrasil::munin::canvas({3, 3});
    auto const region = yggdrasil::munin::rectangle({0, 0}, canvas.get_size());

    draw(image, canvas, region);

    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element('0'), canvas[0][0]);
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element('1'), canvas[1][0]);
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element('2'), canvas[2][0]);
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element('3'), canvas[0][1]);
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element('4'), canvas[1][1]);
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element('5'), canvas[2][1]);
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element('6'), canvas[0][2]);
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element('7'), canvas[1][2]);
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element('8'), canvas[2][2]);
}

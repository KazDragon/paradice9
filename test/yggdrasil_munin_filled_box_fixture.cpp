#include "yggdrasil_munin_filled_box_fixture.hpp"
#include "yggdrasil/munin/filled_box.hpp"
#include "yggdrasil/munin/component.hpp"
#include "yggdrasil/munin/element.hpp"
#include "yggdrasil/munin/model.hpp"
#include "yggdrasil/munin/canvas.hpp"
#include "yggdrasil/munin/rectangle.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(yggdrasil_munin_filled_box_fixture);

void yggdrasil_munin_filled_box_fixture::test_default_constructor()
{
    auto box = std::move(yggdrasil::munin::filled_box());
}

void yggdrasil_munin_filled_box_fixture::test_element_constructor()
{
    auto box = yggdrasil::munin::filled_box(yggdrasil::munin::element('#'));
}

void yggdrasil_munin_filled_box_fixture::test_filled_box_is_component()
{
    auto comp = yggdrasil::munin::component(yggdrasil::munin::filled_box());
}

void yggdrasil_munin_filled_box_fixture::test_size()
{
    auto box = yggdrasil::munin::filled_box(yggdrasil::munin::element('#'));
    auto &model = box.get_model();
    
    set_size(model, yggdrasil::munin::extent(3, 2));
    
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::extent(3, 2), get_size(model));
}

void yggdrasil_munin_filled_box_fixture::test_draw()
{
    auto box = yggdrasil::munin::component(
        yggdrasil::munin::filled_box(yggdrasil::munin::element('#')));
    
    auto model = get_model(box);
    
    set_size(model, yggdrasil::munin::extent(1, 1));
    
    auto region = yggdrasil::munin::rectangle({0, 0}, get_size(model));
    auto canvas = yggdrasil::munin::canvas({2, 2});
    
    draw(box, canvas, region);
    
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::munin::element('#'), canvas[0][0]);
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::munin::element(), canvas[0][1]);
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::munin::element(), canvas[1][0]);
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::munin::element(), canvas[1][1]);
}

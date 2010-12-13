#include "munin_container_fixture.hpp"
#include "munin/container.hpp"
#include "fake_munin_container.hpp"
#include "fake_munin_component.hpp"
#include "fake_munin_layout.hpp"
#include "fake_munin_canvas.hpp"
#include <boost/lambda/lambda.hpp>

using namespace std;
using namespace boost;
using namespace odin;
namespace bll = boost::lambda;

CPPUNIT_TEST_SUITE_REGISTRATION(munin_container_fixture);

void munin_container_fixture::test_constructor()
{
    fake_container<char> container(' ');
    (void)container;
}

void munin_container_fixture::test_inheritance()
{                                   
    fake_container<char> container(' ');
    munin::component<char> &component = container;
    (void)component;
}

void munin_container_fixture::test_add_component()
{
    shared_ptr< fake_container<char> > container(new fake_container<char>(' '));
    CPPUNIT_ASSERT_EQUAL(u32(0), container->get_number_of_components());
    
    shared_ptr< fake_component<char> > component0(new fake_component<char>);
    container->add_component(component0);
    
    CPPUNIT_ASSERT_EQUAL(u32(1), container->get_number_of_components());
    CPPUNIT_ASSERT(container->get_component(0) == component0);
    
    shared_ptr< fake_component<char> > component1(new fake_component<char>);
    container->add_component(component1);
    
    CPPUNIT_ASSERT_EQUAL(u32(2), container->get_number_of_components());
    CPPUNIT_ASSERT(container->get_component(0) == component0);
    CPPUNIT_ASSERT(container->get_component(1) == component1);
}

void munin_container_fixture::test_remove_component()
{
    shared_ptr< fake_container<char> > container(new fake_container<char>(' '));
    CPPUNIT_ASSERT_EQUAL(u32(0), container->get_number_of_components());

    vector<munin::rectangle> regions;
    regions.push_back(munin::rectangle(munin::point(), munin::extent()));
    
    odin::u32 redraw_count = 0;
    boost::function<void (vector<munin::rectangle>)> callback = (
        ++bll::var(redraw_count)
    );

    container->on_redraw.connect(callback);
    CPPUNIT_ASSERT_EQUAL(u32(0), redraw_count);
    
    shared_ptr< fake_component<char> > component0(new fake_component<char>);
    container->add_component(component0);
    CPPUNIT_ASSERT_EQUAL(u32(1), redraw_count);
    
    CPPUNIT_ASSERT_EQUAL(u32(1), container->get_number_of_components());
    CPPUNIT_ASSERT(container->get_component(0) == component0);
    
    shared_ptr< fake_component<char> > component1(new fake_component<char>);
    container->add_component(component1);
    CPPUNIT_ASSERT_EQUAL(u32(2), redraw_count);
    
    CPPUNIT_ASSERT_EQUAL(u32(2), container->get_number_of_components());
    CPPUNIT_ASSERT(container->get_component(0) == component0);
    CPPUNIT_ASSERT(container->get_component(1) == component1);

    component0->on_redraw(regions);
    CPPUNIT_ASSERT_EQUAL(u32(3), redraw_count);
    
    component1->on_redraw(regions);
    CPPUNIT_ASSERT_EQUAL(u32(4), redraw_count);
    
    container->remove_component(component0);
    CPPUNIT_ASSERT_EQUAL(u32(5), redraw_count);
    
    component0->on_redraw(regions);
    // IMPORTANT: This must not redraw
    CPPUNIT_ASSERT_EQUAL(u32(5), redraw_count);
    
    component1->on_redraw(regions);
    CPPUNIT_ASSERT_EQUAL(u32(6), redraw_count);
    
    CPPUNIT_ASSERT_EQUAL(u32(1), container->get_number_of_components());
    CPPUNIT_ASSERT(container->get_component(0) == component1);
}

void munin_container_fixture::test_draw()
{
    shared_ptr< fake_container<char> > container(new fake_container<char>(' '));
    
    {
        shared_ptr< fake_component<char> > component(new fake_component<char>);
        component->set_brush('X');
        
        munin::extent size;
        size.width  = 2;
        size.height = 3;
        component->set_size(size);
        
        munin::point position;
        position.x = 0;
        position.y = 0;
        component->set_position(position);
        
        container->add_component(component);
    }
    
    {
        shared_ptr< fake_component<char> > component(new fake_component<char>);
        component->set_brush('Y');
        
        munin::extent size;
        size.width  = 2;
        size.height = 2;
        component->set_size(size);
        
        munin::point position;
        position.x = 1;
        position.y = 1;
        component->set_position(position);
        
        container->add_component(component);
    }
    
    fake_canvas<char> canvas;
    
    munin::rectangle region;
    region.origin.x    = 0;
    region.origin.y    = 0;
    region.size.width  = 3;
    region.size.height = 3;
    
    // With the components specified, the container should draw onto the
    // canvas as follows:
    //
    // "XX "    "   "    "XX "
    // "XX " -> " YY" -> "XYY"
    // "XX "    " YY"    "XYY"
    container->draw(canvas, region);
    
    CPPUNIT_ASSERT_EQUAL('X', canvas.values_[0][0]);
    CPPUNIT_ASSERT_EQUAL('X', canvas.values_[1][0]);
    CPPUNIT_ASSERT_EQUAL('X', canvas.values_[0][1]);
    CPPUNIT_ASSERT_EQUAL('X', canvas.values_[0][2]);
    
    CPPUNIT_ASSERT_EQUAL('Y', canvas.values_[1][1]);
    CPPUNIT_ASSERT_EQUAL('Y', canvas.values_[1][2]);
    CPPUNIT_ASSERT_EQUAL('Y', canvas.values_[2][1]);
    CPPUNIT_ASSERT_EQUAL('Y', canvas.values_[2][2]);
    
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[2][0]);
}

void munin_container_fixture::test_contained_container()
{
    shared_ptr< fake_component<char> > component(new fake_component<char>);
    
    {
        munin::point position;
        position.x = 1;
        position.y = 1;
        
        munin::extent size;
        size.width  = 2;
        size.height = 2;
        
        component->set_position(position);
        component->set_size(size);
        component->set_brush('#');
    }
    
    shared_ptr< fake_container<char> > inner_container(
        new fake_container<char>(' '));
    
    {
        munin::point position;
        position.x = 1;
        position.y = 1;
        
        munin::extent size;
        size.width  = 3;
        size.height = 3;
        
        inner_container->set_position(position);
        inner_container->set_size(size);
        inner_container->add_component(component);
    }
    
    shared_ptr< fake_container<char> > outer_container( 
        new fake_container<char>(' '));
    
    {
        munin::point position;
        position.x = 1;
        position.y = 1;
        
        munin::extent size;
        size.width  = 4;
        size.height = 4;
        
        outer_container->set_position(position);
        outer_container->set_size(size);
        outer_container->add_component(inner_container);
    }
    
    fake_canvas<char> canvas;
    
    {
        munin::rectangle region;
        region.origin.x    = 0;
        region.origin.y    = 0;
        region.size.width  = 5;
        region.size.height = 5;
        
        outer_container->draw(canvas, region);
    }
    
    // The outer container is from (1,1)->(5,5).  Inside that,
    // the inner container is from (1,1)->(4,4), which is (2,2)->(5,5) in
    // the outer container's canvas.
    // The component is from (1,1)->(3,3), which is (3,3)->(5,5) in the
    // outer container's canvas.
    // 
    // So, with the components specified, the container should draw onto the
    // graphics canvas as follows:
    //
    //  01234
    // 0
    // 1
    // 2
    // 3   ##
    // 4   ##
    
    CPPUNIT_ASSERT_EQUAL('#', canvas.values_[3][3]);
    CPPUNIT_ASSERT_EQUAL('#', canvas.values_[3][4]);
    CPPUNIT_ASSERT_EQUAL('#', canvas.values_[4][3]);
    CPPUNIT_ASSERT_EQUAL('#', canvas.values_[4][4]);
    
    // The rest should be spaces.
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[0][0]);
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[1][0]);
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[2][0]);
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[3][0]);
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[4][0]);
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[0][1]);
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[1][1]);
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[2][1]);
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[3][1]);
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[4][1]);
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[0][2]);
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[1][2]);
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[2][2]);
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[3][2]);
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[4][2]);
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[0][3]);
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[1][3]);
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[2][3]);
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[0][4]);
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[1][4]);
    CPPUNIT_ASSERT_EQUAL(' ', canvas.values_[2][4]);
}

void munin_container_fixture::test_overlap_same_layer()
{
    shared_ptr< fake_container<char> > container(new fake_container<char>(' '));
    shared_ptr< fake_component<char> > component0(new fake_component<char>);
    shared_ptr< fake_component<char> > component1(new fake_component<char>);
    
    // Set the components to be so:
    //
    //  012
    // 0##
    // 1#++
    // 2 ++
    //
    // Note: (1,1) is also occupied by component0 but, because it was added
    // first, it is "lower" than component1 on the same layer.
    
    munin::extent container_size;
    container_size.width  = 3;
    container_size.height = 3;
    container->set_size(container_size);
    
    munin::extent component_size;
    component_size.width  = 2;
    component_size.height = 2;
    component0->set_size(component_size);
    component1->set_size(component_size);
    
    munin::point component0_position;
    component0_position.x = 0;
    component0_position.y = 0;
    component0->set_position(component0_position);
    component0->set_brush('#');
    
    munin::point component1_position;
    component1_position.x = 1;
    component1_position.y = 1;
    component1->set_position(component1_position);
    component1->set_brush('+');
    
    container->add_component(component0);
    container->add_component(component1);

    munin::rectangle region;
    region.size   = container->get_size();
    region.origin = container->get_position();

    fake_canvas<char> canvas;
    container->draw(canvas, region);
    
    CPPUNIT_ASSERT_EQUAL('#', char(canvas[0][0]));
    CPPUNIT_ASSERT_EQUAL('#', char(canvas[0][1]));
    CPPUNIT_ASSERT_EQUAL(' ', char(canvas[0][2]));
    CPPUNIT_ASSERT_EQUAL('#', char(canvas[1][0]));
    CPPUNIT_ASSERT_EQUAL('+', char(canvas[1][1]));
    CPPUNIT_ASSERT_EQUAL('+', char(canvas[1][2]));
    CPPUNIT_ASSERT_EQUAL(' ', char(canvas[2][0]));
    CPPUNIT_ASSERT_EQUAL('+', char(canvas[2][1]));
    CPPUNIT_ASSERT_EQUAL('+', char(canvas[2][2]));
}

void munin_container_fixture::test_overlap_different_layer()
{
    shared_ptr< fake_container<char> > container(new fake_container<char>(' '));
    shared_ptr< fake_component<char> > component0(new fake_component<char>);
    shared_ptr< fake_component<char> > component1(new fake_component<char>);
    
    // Set the components to be so:
    //
    //  012
    // 0##
    // 1##+
    // 2 ++
    //
    // Note: (1,1) is also occupied by component1 but, because we added
    // component0 as a higher layer than component1, it gets priority when
    // painting.
    
    munin::extent container_size;
    container_size.width  = 3;
    container_size.height = 3;
    container->set_size(container_size);
    
    munin::extent component_size;
    component_size.width  = 2;
    component_size.height = 2;
    component0->set_size(component_size);
    component1->set_size(component_size);
    
    munin::point component0_position;
    component0_position.x = 0;
    component0_position.y = 0;
    component0->set_position(component0_position);
    component0->set_brush('#');
    
    munin::point component1_position;
    component1_position.x = 1;
    component1_position.y = 1;
    component1->set_position(component1_position);
    component1->set_brush('+');
    
    container->add_component(component0, boost::any(), munin::HIGHEST_LAYER);
    container->add_component(component1, boost::any(), munin::LOWEST_LAYER);

    munin::rectangle region;
    region.size   = container->get_size();
    region.origin = container->get_position();

    fake_canvas<char> canvas;
    container->draw(canvas, region);
    
    CPPUNIT_ASSERT_EQUAL('#', char(canvas[0][0]));
    CPPUNIT_ASSERT_EQUAL('#', char(canvas[0][1]));
    CPPUNIT_ASSERT_EQUAL(' ', char(canvas[0][2]));
    CPPUNIT_ASSERT_EQUAL('#', char(canvas[1][0]));
    CPPUNIT_ASSERT_EQUAL('#', char(canvas[1][1]));
    CPPUNIT_ASSERT_EQUAL('+', char(canvas[1][2]));
    CPPUNIT_ASSERT_EQUAL(' ', char(canvas[2][0]));
    CPPUNIT_ASSERT_EQUAL('+', char(canvas[2][1]));
    CPPUNIT_ASSERT_EQUAL('+', char(canvas[2][2]));
}

void munin_container_fixture::test_set_focus()
{
    shared_ptr< fake_container<char> > container(new fake_container<char>(' '));
    shared_ptr< fake_component<char> > component0(new fake_component<char>);
    shared_ptr< fake_component<char> > component1(new fake_component<char>);
    
    container->add_component(component0);
    container->add_component(component1);
    
    CPPUNIT_ASSERT_EQUAL(false, container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, component0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, component1->has_focus());
    
    container->set_focus();

    CPPUNIT_ASSERT_EQUAL(true, container->has_focus());
    CPPUNIT_ASSERT_EQUAL(true, component0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, component1->has_focus());
    
    container->lose_focus();

    CPPUNIT_ASSERT_EQUAL(false, container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, component0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, component1->has_focus());
}

void munin_container_fixture::test_set_focus_cant_focus()
{
    shared_ptr< fake_container<char> > container(new fake_container<char>(' '));
    shared_ptr< fake_component<char> > component0(new fake_component<char>);
    shared_ptr< fake_component<char> > component1(new fake_component<char>);
    
    container->add_component(component0);
    container->add_component(component1);
    
    CPPUNIT_ASSERT_EQUAL(true, container->can_focus());
        
    component0->set_can_focus(false);
    CPPUNIT_ASSERT_EQUAL(true, container->can_focus());
    
    component1->set_can_focus(false);
    CPPUNIT_ASSERT_EQUAL(false, container->can_focus());

    container->set_focus();
    
    CPPUNIT_ASSERT_EQUAL(false, container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, component0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, component1->has_focus());
}

void munin_container_fixture::test_next_focus()
{
    // Test that Next Focus will cycle through the components in a depth-first
    // order.  That is, sub_components 00, 01, 10, 11.  Other components will
    // be skipped over because they may not be focused.
    shared_ptr< fake_container<char> > main_container(new fake_container<char>(' '));
    shared_ptr< fake_container<char> > sub_container0(new fake_container<char>(' '));
    shared_ptr< fake_container<char> > sub_container1(new fake_container<char>(' '));
    shared_ptr< fake_component<char> > sub_component00(new fake_component<char>);
    shared_ptr< fake_component<char> > sub_component01(new fake_component<char>);
    shared_ptr< fake_container<char> > mid_container(new fake_container<char>(' '));
    shared_ptr< fake_component<char> > mid_component0(new fake_component<char>);
    shared_ptr< fake_component<char> > mid_component1(new fake_component<char>);
    shared_ptr< fake_component<char> > sub_component10(new fake_component<char>);
    shared_ptr< fake_component<char> > sub_component11(new fake_component<char>);
    
    sub_container0->add_component(sub_component00);
    sub_container0->add_component(sub_component01);
    
    mid_container->add_component(mid_component0);
    mid_container->add_component(mid_component1);
    
    sub_container1->add_component(sub_component10);
    sub_container1->add_component(sub_component11);
    
    main_container->add_component(sub_container0);
    main_container->add_component(mid_container);
    main_container->add_component(sub_container1);
    
    mid_component0->set_can_focus(false);
    mid_component1->set_can_focus(false);
    
    CPPUNIT_ASSERT_EQUAL(false, main_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_container0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component00->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component01->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_container1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component10->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component11->has_focus());
    
    // Set the focus to the first component.
    main_container->set_focus();
    
    CPPUNIT_ASSERT_EQUAL(true, main_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(true, sub_container0->has_focus());
    CPPUNIT_ASSERT_EQUAL(true, sub_component00->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component01->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_container1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component10->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component11->has_focus());
    
    // Cycle to the next component.
    main_container->focus_next();
    
    CPPUNIT_ASSERT_EQUAL(true, main_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(true, sub_container0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component00->has_focus());
    CPPUNIT_ASSERT_EQUAL(true, sub_component01->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_container1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component10->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component11->has_focus());
    
    // And so on...
    main_container->focus_next();
    
    CPPUNIT_ASSERT_EQUAL(true, main_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_container0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component00->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component01->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component1->has_focus());
    CPPUNIT_ASSERT_EQUAL(true, sub_container1->has_focus());
    CPPUNIT_ASSERT_EQUAL(true, sub_component10->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component11->has_focus());
    
    main_container->focus_next();
    
    CPPUNIT_ASSERT_EQUAL(true, main_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_container0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component00->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component01->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component1->has_focus());
    CPPUNIT_ASSERT_EQUAL(true, sub_container1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component10->has_focus());
    CPPUNIT_ASSERT_EQUAL(true, sub_component11->has_focus());
    
    // Check that the focus is lost after the last component loses focus.
    main_container->focus_next();
    
    CPPUNIT_ASSERT_EQUAL(false, main_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_container0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component00->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component01->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_container1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component10->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component11->has_focus());
    
    // Finally, check that the next focus of an unfocused container is the
    // first subcomponent (just like set_focus()).
    main_container->focus_next();
    
    CPPUNIT_ASSERT_EQUAL(true, main_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(true, sub_container0->has_focus());
    CPPUNIT_ASSERT_EQUAL(true, sub_component00->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component01->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_container1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component10->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component11->has_focus());
}

void munin_container_fixture::test_previous_focus()
{
    // Test that Previous Focus will cycle through the components in a 
    // depth-first reverse order.  That is, sub_components 11, 10, 01, 00.
    // Other components will be skipped over because they may not be focused.
    shared_ptr< fake_container<char> > main_container(new fake_container<char>(' '));
    shared_ptr< fake_container<char> > sub_container0(new fake_container<char>(' '));
    shared_ptr< fake_component<char> > sub_component00(new fake_component<char>);
    shared_ptr< fake_component<char> > sub_component01(new fake_component<char>);
    shared_ptr< fake_container<char> > mid_container(new fake_container<char>(' '));
    shared_ptr< fake_component<char> > mid_component0(new fake_component<char>);
    shared_ptr< fake_component<char> > mid_component1(new fake_component<char>);
    shared_ptr< fake_container<char> > sub_container1(new fake_container<char>(' '));
    shared_ptr< fake_component<char> > sub_component10(new fake_component<char>);
    shared_ptr< fake_component<char> > sub_component11(new fake_component<char>);
    
    sub_container0->add_component(sub_component00);
    sub_container0->add_component(sub_component01);
    
    mid_container->add_component(mid_component0);
    mid_container->add_component(mid_component1);
    
    sub_container1->add_component(sub_component10);
    sub_container1->add_component(sub_component11);
    
    main_container->add_component(sub_container0);
    main_container->add_component(mid_container);
    main_container->add_component(sub_container1);
    
    mid_component0->set_can_focus(false);
    mid_component1->set_can_focus(false);

    CPPUNIT_ASSERT_EQUAL(false, main_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_container0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component00->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component01->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_container1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component10->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component11->has_focus());

    // Test that setting the previous focus of a component that does not have
    // focus causes the last subcomponent to gain focus.
    main_container->focus_previous();
    
    CPPUNIT_ASSERT_EQUAL(true, main_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_container0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component00->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component01->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component1->has_focus());
    CPPUNIT_ASSERT_EQUAL(true, sub_container1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component10->has_focus());
    CPPUNIT_ASSERT_EQUAL(true, sub_component11->has_focus());
    
    // Now test that settings the previous focus cycles backwards through the
    // components: 11, 10, 01, 00.
    main_container->focus_previous();
    
    CPPUNIT_ASSERT_EQUAL(true, main_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_container0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component00->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component01->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component1->has_focus());
    CPPUNIT_ASSERT_EQUAL(true, sub_container1->has_focus());
    CPPUNIT_ASSERT_EQUAL(true, sub_component10->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component11->has_focus());
    
    main_container->focus_previous();
    
    CPPUNIT_ASSERT_EQUAL(true, main_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(true, sub_container0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component00->has_focus());
    CPPUNIT_ASSERT_EQUAL(true, sub_component01->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_container1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component10->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component11->has_focus());
    
    main_container->focus_previous();
    
    CPPUNIT_ASSERT_EQUAL(true, main_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(true, sub_container0->has_focus());
    CPPUNIT_ASSERT_EQUAL(true, sub_component00->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component01->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_container1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component10->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component11->has_focus());
    
    // And finally, setting the previous focus of the first component should
    // leave them all unfocused.
    main_container->focus_previous();
    
    CPPUNIT_ASSERT_EQUAL(false, main_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_container0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component00->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component01->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_container->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, mid_component1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_container1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component10->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, sub_component11->has_focus());
}

void munin_container_fixture::test_event()
{
    shared_ptr< fake_container<char> > container(new fake_container<char>(' '));
    shared_ptr< fake_component<char> > component0(new fake_component<char>);
    shared_ptr< fake_component<char> > component1(new fake_component<char>);
    
    any component0_event;
    function<void (any)> component0_event_handler = (
        bll::var(component0_event) = bll::_1
    );
    component0->set_event_handler(component0_event_handler);
    
    any component1_event;
    function<void (any)> component1_event_handler = (
        bll::var(component1_event) = bll::_1
    );
    component1->set_event_handler(component1_event_handler);
    
    container->add_component(component0);
    container->add_component(component1);

    CPPUNIT_ASSERT_EQUAL(true, component0_event.empty());
    CPPUNIT_ASSERT_EQUAL(true, component1_event.empty());
    
    // No component has focus.  therefore, the container should swallow the
    // event, since it would have no idea where to pass it on to.  In practice,
    // this should not occur, since the Window should only pass the event on
    // to its focused component.
    container->event(string("unfocused container event"));
    
    CPPUNIT_ASSERT_EQUAL(true, component0_event.empty());
    CPPUNIT_ASSERT_EQUAL(true, component1_event.empty());
    
    // Set the focus to the first component.  This is the place to which the
    // event should go.
    component0->set_focus();
    
    string component0_event_string = "test component0 event";
    container->event(component0_event_string);
    
    CPPUNIT_ASSERT_EQUAL(false, component0_event.empty());
    CPPUNIT_ASSERT_EQUAL(true, component1_event.empty());
    CPPUNIT_ASSERT_EQUAL(
        component0_event_string
      , any_cast<string>(component0_event));
    
    component0_event = any();
    
    // Set the focus to the second component.  The event should hit this next.
    component1->set_focus();
    
    string component1_event_string = "test component0 event";
    container->event(component1_event_string);
    
    CPPUNIT_ASSERT_EQUAL(true, component0_event.empty());
    CPPUNIT_ASSERT_EQUAL(false, component1_event.empty());
    CPPUNIT_ASSERT_EQUAL(
        component0_event_string
      , any_cast<string>(component1_event));
}

void munin_container_fixture::test_get_focussed_component()
{
    shared_ptr< fake_container<char> > container(new fake_container<char>(' '));
    shared_ptr< fake_component<char> > component0(new fake_component<char>);
    shared_ptr< fake_component<char> > component1(new fake_component<char>);
    
    container->add_component(component0);
    container->add_component(component1);
    
    CPPUNIT_ASSERT(
        container->get_focussed_component()
     == shared_ptr< munin::component<char> >());
    
    component0->set_focus();
    
    CPPUNIT_ASSERT(container->get_focussed_component() == component0);
    
    component1->set_focus();
    
    CPPUNIT_ASSERT(container->get_focussed_component() == component1);
}

void munin_container_fixture::test_cursor_state()
{
    shared_ptr< fake_container<char> > container(new fake_container<char>(' '));
    shared_ptr< fake_component<char> > component0(new fake_component<char>);
    shared_ptr< fake_component<char> > component1(new fake_component<char>);
    
    container->add_component(component0);
    container->add_component(component1);

    bool cursor_state = false;
    function<void(bool)> callback = (bll::var(cursor_state) = bll::_1);
    
    // The basic component should have an event that can be used to signify
    // cursor state changes.
    container->on_cursor_state_changed.connect(callback);
    
    // Since no components have a cursor state set, the container also should
    // have the cursor state unset.
    CPPUNIT_ASSERT_EQUAL(false, container->get_cursor_state());
    CPPUNIT_ASSERT_EQUAL(false, cursor_state);
    
    // Even though we enable a component's cursor, it does not have focus,
    // so the container's cursor state remains false.
    component0->set_cursor_state(true);
    
    CPPUNIT_ASSERT_EQUAL(false, container->get_cursor_state());
    CPPUNIT_ASSERT_EQUAL(false, cursor_state);

    // By setting the focus to the second (non-cursor) component, we should
    // still have no cursor.
    component1->set_focus();
    
    CPPUNIT_ASSERT_EQUAL(false, container->get_cursor_state());
    CPPUNIT_ASSERT_EQUAL(false, cursor_state);

    // By setting focus to the first (with-cursor) component, we should finally
    // have a cursor.
    component0->set_focus();

    CPPUNIT_ASSERT_EQUAL(true, container->get_cursor_state());
    CPPUNIT_ASSERT_EQUAL(true, cursor_state);
                               
    // By setting the cursor state of the focussed component off, we should
    // no longer have a cursor.
    component0->set_cursor_state(false);
    
    CPPUNIT_ASSERT_EQUAL(false, container->get_cursor_state());
    CPPUNIT_ASSERT_EQUAL(false, cursor_state);
    
    // By setting the cursor state back on, we should again have a cursor.
    component0->set_cursor_state(true);
    
    CPPUNIT_ASSERT_EQUAL(true, container->get_cursor_state());
    CPPUNIT_ASSERT_EQUAL(true, cursor_state);
}

void munin_container_fixture::test_cursor_position()
{
    shared_ptr< fake_container<char> > container(new fake_container<char>(' '));
    shared_ptr< fake_component<char> > component0(new fake_component<char>);
    shared_ptr< fake_component<char> > component1(new fake_component<char>);
    
    container->add_component(component0);
    container->add_component(component1);

    munin::point component0_position;
    component0_position.x = 1;
    component0_position.y = 2;
    component0->set_position(component0_position);
    
    munin::point component1_position;
    component1_position.x = 5;
    component1_position.y = 7;
    component1->set_position(component1_position);
    
    munin::point default_position;
    default_position.x = 0;
    default_position.y = 0;
    
    munin::point cursor_position = default_position;
    
    function<void(munin::point)> callback = 
        (bll::var(cursor_position) = bll::_1);
    
    // The basic component should have an event that can be used to signify
    // cursor position changes.
    container->on_cursor_position_changed.connect(callback);
    
    // Since no components have a cursor state set, the container also should
    // have a default cursor position;
    CPPUNIT_ASSERT_EQUAL(default_position, container->get_cursor_position());
    CPPUNIT_ASSERT_EQUAL(default_position, cursor_position);
    
    // Even though we enable a component's cursor, it does not have focus,
    // so the container's cursor position remains the default.
    component0->set_cursor_state(true);
    
    CPPUNIT_ASSERT_EQUAL(default_position, container->get_cursor_position());
    CPPUNIT_ASSERT_EQUAL(default_position, cursor_position);

    // By setting the focus to the second (non-cursor) component, we should
    // still have no cursor.
    component1->set_focus();
    
    CPPUNIT_ASSERT_EQUAL(default_position, container->get_cursor_position());
    CPPUNIT_ASSERT_EQUAL(default_position, cursor_position);

    // By setting focus to the first (with-cursor) component, we should finally
    // have a cursor.  Because it will be reported as the default position
    // within the component, it will in fact be the same as that component's
    // position within the container.
    component0->set_focus();

    CPPUNIT_ASSERT_EQUAL(component0_position, container->get_cursor_position());
    CPPUNIT_ASSERT_EQUAL(component0_position, cursor_position);
    
    // By changing the cursor position within the component, we should get
    // an event update telling us that it has moved.
    munin::point new_position;
    new_position.x = 1;
    new_position.y = 1;
    component0->set_cursor_position(new_position);
    
    CPPUNIT_ASSERT_EQUAL(
        new_position + container->get_position() + component0->get_position()
      , container->get_cursor_position());
    CPPUNIT_ASSERT_EQUAL(new_position, cursor_position);
        
}


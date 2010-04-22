#include "munin_container_fixture.hpp"
#include "munin/container.hpp"
#include "fake_munin_container.hpp"
#include "fake_munin_component.hpp"
#include "fake_munin_layout.hpp"
#include "fake_munin_graphics_context.hpp"
#include <boost/lambda/lambda.hpp>

using namespace std;
using namespace boost;
using namespace odin;
namespace bll = boost::lambda;

CPPUNIT_TEST_SUITE_REGISTRATION(munin_container_fixture);

void munin_container_fixture::test_constructor()
{
    fake_container<char> container;
    (void)container;
}

void munin_container_fixture::test_inheritance()
{
    fake_container<char> container;
    munin::component<char> &component = container;
    (void)component;
}

void munin_container_fixture::test_add_component()
{
    shared_ptr< fake_container<char> > container(new fake_container<char>);
    CPPUNIT_ASSERT_EQUAL(u32(0), container->get_number_of_components());
    
    shared_ptr< fake_component<char> > component0(new fake_component<char>);
    container->add_component(component0);
    
    CPPUNIT_ASSERT_EQUAL(u32(1), container->get_number_of_components());
    CPPUNIT_ASSERT(container->get_component(0) == component0);
    
    shared_ptr< munin::component<char> > parent_of_component0 =
        component0->get_parent();
        
    CPPUNIT_ASSERT(parent_of_component0 == container);

    shared_ptr< fake_component<char> > component1(new fake_component<char>);
    container->add_component(component1);
    
    shared_ptr< munin::component<char> > parent_of_component1 =
        component1->get_parent();
        
    CPPUNIT_ASSERT(parent_of_component1 == container);
    
    CPPUNIT_ASSERT_EQUAL(u32(2), container->get_number_of_components());
    CPPUNIT_ASSERT(container->get_component(0) == component0);
    CPPUNIT_ASSERT(container->get_component(1) == component1);
}

void munin_container_fixture::test_remove_component()
{
    shared_ptr< fake_container<char> > container(new fake_container<char>);
    CPPUNIT_ASSERT_EQUAL(u32(0), container->get_number_of_components());
    
    shared_ptr< fake_component<char> > component0(new fake_component<char>);
    container->add_component(component0);
    
    CPPUNIT_ASSERT_EQUAL(u32(1), container->get_number_of_components());
    CPPUNIT_ASSERT(container->get_component(0) == component0);
    
    shared_ptr< munin::component<char> > parent_of_component0 =
        component0->get_parent();
        
    CPPUNIT_ASSERT(parent_of_component0 == container);

    shared_ptr< fake_component<char> > component1(new fake_component<char>);
    container->add_component(component1);
    
    shared_ptr< munin::component<char> > parent_of_component1 =
        component1->get_parent();

    CPPUNIT_ASSERT(parent_of_component1 == container);
    
    CPPUNIT_ASSERT_EQUAL(u32(2), container->get_number_of_components());
    CPPUNIT_ASSERT(container->get_component(0) == component0);
    CPPUNIT_ASSERT(container->get_component(1) == component1);
    
    container->remove_component(component0);
    
    parent_of_component0 = component0->get_parent();
    CPPUNIT_ASSERT(
        parent_of_component0 == shared_ptr< munin::component<char> >());

    CPPUNIT_ASSERT_EQUAL(u32(1), container->get_number_of_components());
    CPPUNIT_ASSERT(container->get_component(0) == component1);
}

void munin_container_fixture::test_draw()
{
    shared_ptr< fake_container<char> > container(new fake_container<char>);
    
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
    
    fake_graphics_context<char> graphics_context;
    
    munin::rectangle region;
    region.origin.x    = 0;
    region.origin.y    = 0;
    region.size.width  = 3;
    region.size.height = 3;
    
    munin::point offset;
    offset.x = 0;
    offset.y = 0;
    
    // With the components specified, the container should draw onto the
    // graphics context as follows:
    //
    // "XX "    "   "    "XX "
    // "XX " -> " YY" -> "XYY"
    // "XX "    " YY"    "XYY"
    container->draw(graphics_context, offset, region);
    
    CPPUNIT_ASSERT_EQUAL('X', graphics_context.values_[0][0]);
    CPPUNIT_ASSERT_EQUAL('X', graphics_context.values_[1][0]);
    CPPUNIT_ASSERT_EQUAL('X', graphics_context.values_[0][1]);
    CPPUNIT_ASSERT_EQUAL('X', graphics_context.values_[0][2]);
    
    CPPUNIT_ASSERT_EQUAL('Y', graphics_context.values_[1][1]);
    CPPUNIT_ASSERT_EQUAL('Y', graphics_context.values_[1][2]);
    CPPUNIT_ASSERT_EQUAL('Y', graphics_context.values_[2][1]);
    CPPUNIT_ASSERT_EQUAL('Y', graphics_context.values_[2][2]);
    
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[2][0]);
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
        new fake_container<char>);
    
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
        new fake_container<char>);
    
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
    
    fake_graphics_context<char> graphics_context;
    
    {
        munin::rectangle region;
        region.origin.x    = 0;
        region.origin.y    = 0;
        region.size.width  = 5;
        region.size.height = 5;
        
        munin::point offset;
        offset.x = 0;
        offset.y = 0;
        
        outer_container->draw(graphics_context, offset, region);
    }
    
    // The outer container is from (1,1)->(5,5).  Inside that,
    // the inner container is from (1,1)->(4,4), which is (2,2)->(5,5) in
    // the outer container's context.
    // The component is from (1,1)->(3,3), which is (3,3)->(5,5) in the
    // outer container's context.
    // 
    // So, with the components specified, the container should draw onto the
    // graphics context as follows:
    //
    //  01234
    // 0
    // 1
    // 2
    // 3   ##
    // 4   ##
    
    CPPUNIT_ASSERT_EQUAL('#', graphics_context.values_[3][3]);
    CPPUNIT_ASSERT_EQUAL('#', graphics_context.values_[3][4]);
    CPPUNIT_ASSERT_EQUAL('#', graphics_context.values_[4][3]);
    CPPUNIT_ASSERT_EQUAL('#', graphics_context.values_[4][4]);
    
    // The rest should be zeroed.
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[0][0]);
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[1][0]);
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[2][0]);
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[3][0]);
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[4][0]);
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[0][1]);
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[1][1]);
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[2][1]);
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[3][1]);
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[4][1]);
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[0][2]);
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[1][2]);
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[2][2]);
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[3][2]);
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[4][2]);
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[0][3]);
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[1][3]);
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[2][3]);
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[0][4]);
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[1][4]);
    CPPUNIT_ASSERT_EQUAL('\0', graphics_context.values_[2][4]);
}

void munin_container_fixture::test_layout()
{
    shared_ptr< fake_container<char> > container(new fake_container<char>);
    shared_ptr< fake_layout<char> >    layout(new fake_layout<char>);
    
    CPPUNIT_ASSERT(container->get_layout() == NULL);
    
    container->set_layout(layout);
    
    CPPUNIT_ASSERT(container->get_layout() == layout);
    CPPUNIT_ASSERT_EQUAL(u32(0), container->get_number_of_components());
    CPPUNIT_ASSERT_EQUAL(u32(0), layout->get_number_of_components());
    
    shared_ptr< fake_component<char> >  component0(new fake_component<char>);
    container->add_component(component0);
    
    CPPUNIT_ASSERT_EQUAL(u32(1), container->get_number_of_components());
    CPPUNIT_ASSERT_EQUAL(u32(1), layout->get_number_of_components());
    CPPUNIT_ASSERT(container->get_component(0) == layout->get_component(0));
    CPPUNIT_ASSERT_EQUAL(true, layout->get_hint(0).empty());
    
    shared_ptr< fake_component<char> >  component1(new fake_component<char>);
    std::string hint1 = "foo";
    boost::any  any_hint1 = hint1;
    container->add_component(component1, any_hint1);
    
    CPPUNIT_ASSERT_EQUAL(u32(2), container->get_number_of_components());
    CPPUNIT_ASSERT_EQUAL(u32(2), layout->get_number_of_components());
    CPPUNIT_ASSERT(container->get_component(0) == layout->get_component(0));
    CPPUNIT_ASSERT_EQUAL(true, layout->get_hint(0).empty());
    CPPUNIT_ASSERT(container->get_component(1) == layout->get_component(1));
    CPPUNIT_ASSERT_EQUAL(
        hint1
      , boost::any_cast<std::string>(layout->get_hint(1)));
}

void munin_container_fixture::test_overlap_same_layer()
{
    CPPUNIT_FAIL("Not Implemented");
}

void munin_container_fixture::test_overlap_different_layer()
{
    CPPUNIT_FAIL("Not Implemented");
}

#include "munin_layout_fixture.hpp"
#include "munin/layout.hpp"
#include "fake_munin_layout.hpp"
#include "fake_munin_component.hpp"
#include "fake_munin_container.hpp"
#include <string>

using namespace std;
using namespace odin;

CPPUNIT_TEST_SUITE_REGISTRATION(munin_layout_fixture);

void munin_layout_fixture::test_constructor()
{
    fake_layout<char> layout;
    (void)layout;
}

void munin_layout_fixture::test_add_component()
{
    boost::shared_ptr< fake_layout<char> > layout(
        new fake_layout<char>);
    
    CPPUNIT_ASSERT_EQUAL(u32(0), layout->get_number_of_components());
    
    boost::shared_ptr< fake_component<char> > component0(
        new fake_component<char>);    

    boost::shared_ptr< fake_component<char> > component1(
        new fake_component<char>);    
    
    boost::any hint0 = string("hint0");
    boost::any hint1 = string("hint1");
    
    layout->add_component(component0, hint0);
    layout->add_component(component1, hint1);
    
    CPPUNIT_ASSERT_EQUAL(u32(2), layout->get_number_of_components());
    CPPUNIT_ASSERT(layout->get_component(0) == component0);
    CPPUNIT_ASSERT(layout->get_component(1) == component1);
    
    boost::any stored_hint0 = layout->get_hint(0);
    boost::any stored_hint1 = layout->get_hint(1);
    
    CPPUNIT_ASSERT_EQUAL(
        boost::any_cast<string>(hint0)
      , boost::any_cast<string>(stored_hint0));
    CPPUNIT_ASSERT_EQUAL(
        boost::any_cast<string>(hint1)
      , boost::any_cast<string>(stored_hint1));
}

void munin_layout_fixture::test_remove_component()
{
    boost::shared_ptr< fake_layout<char> > layout(
        new fake_layout<char>);
    
    boost::shared_ptr< fake_component<char> > component0(
        new fake_component<char>);    

    boost::shared_ptr< fake_component<char> > component1(
        new fake_component<char>);    
    
    boost::any hint0 = string("hint0");
    boost::any hint1 = string("hint1");
    
    layout->add_component(component0, hint0);
    layout->add_component(component1, hint1);

    layout->remove_component(component0);

    CPPUNIT_ASSERT_EQUAL(u32(1), layout->get_number_of_components());
    CPPUNIT_ASSERT(layout->get_component(0) == component1);

    boost::any stored_hint0 = layout->get_hint(0);

    CPPUNIT_ASSERT_EQUAL(
        boost::any_cast<string>(hint1)
      , boost::any_cast<string>(stored_hint0));
}

void munin_layout_fixture::test_layout()
{
    boost::shared_ptr< fake_layout<char> > layout(
        new fake_layout<char>);
    
    boost::shared_ptr< fake_component<char> > component0(
        new fake_component<char>);    

    boost::shared_ptr< fake_component<char> > component1(
        new fake_component<char>);    
    
    boost::shared_ptr< fake_container<char> > container0(
        new fake_container<char>);
    
    boost::any hint0 = string("hint0");
    boost::any hint1 = string("hint1");
    
    layout->add_component(component0, hint0);
    layout->add_component(component1, hint1);

    CPPUNIT_ASSERT_EQUAL(false, layout->do_layout_called);
    
    (*layout)(container0);

    CPPUNIT_ASSERT_EQUAL(true, layout->do_layout_called);
}

void munin_layout_fixture::test_preferred_size()
{
    boost::shared_ptr< fake_layout<char> > layout(new fake_layout<char>);
    
    munin::extent preferred_size;
    preferred_size.width  = 5;
    preferred_size.height = 6;
    layout->set_preferred_size(preferred_size);
    
    munin::extent actual_preferred_size = layout->get_preferred_size();
    CPPUNIT_ASSERT_EQUAL(preferred_size.width, actual_preferred_size.width);
    CPPUNIT_ASSERT_EQUAL(preferred_size.height, actual_preferred_size.height);
}

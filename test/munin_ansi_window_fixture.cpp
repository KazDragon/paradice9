#include "munin_ansi_window_fixture.hpp"
#include "munin/ansi/window.hpp"
#include "munin/ansi/protocol.hpp"
#include "fake_munin_component.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/lambda/lambda.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(munin_ansi_window_fixture);

using namespace boost;
using namespace std;
using namespace odin;

namespace bll = boost::lambda;

void munin_ansi_window_fixture::test_constructor()
{
    boost::asio::io_service io_service;
    munin::ansi::window window(io_service);
    
    shared_ptr<munin::ansi::container> content = window.get_content();
    
    CPPUNIT_ASSERT(content != NULL);
}

void munin_ansi_window_fixture::test_repaint_registration()
{
    boost::asio::io_service io_service;
    munin::ansi::window window(io_service);
    
    function<void (vector<char>)> fn;
    window.on_repaint.connect(fn);
}

void munin_ansi_window_fixture::test_repaint_chain()
{
    boost::asio::io_service io_service;
    munin::ansi::window window(io_service);

    u32          called = 0;
    vector<char> paint_data;
    function<void (std::vector<char>)> fn = (
        ++bll::var(called)
      , bll::var(paint_data) = bll::_1
    );
    
    window.on_repaint.connect(fn);
    
    // Nothing should be called yet.
    CPPUNIT_ASSERT_EQUAL(u32(0),    called);
    CPPUNIT_ASSERT_EQUAL(size_t(0), paint_data.size());
    
    // Run the IO service.  The content of the window by default is a size 0
    // rectangle, hence nothing need ever be repainted.
    io_service.reset();
    io_service.run();
    CPPUNIT_ASSERT_EQUAL(u32(0),    called);
    CPPUNIT_ASSERT_EQUAL(size_t(0), paint_data.size());
    
    // Set the size of the content container.  This should notify the window,
    // who should ask the container to lay itself out if appropriate and draw
    // itself anew.  However, having no layout and no components, it should be
    // exactly the same as it was by default, and should cause no repaint.
    munin::extent content_size;
    content_size.width  = 1;
    content_size.height = 1;
    window.get_content()->set_size(content_size);
    
    io_service.reset();
    io_service.run();
    CPPUNIT_ASSERT_EQUAL(u32(0),    called);
    CPPUNIT_ASSERT_EQUAL(size_t(0), paint_data.size());
    
    // Add a component to the container.  By default, we will make the brush
    // the same as the background.  Its size is 0.
    shared_ptr< fake_component<munin::ansi::element_type> > component(
        new fake_component<munin::ansi::element_type>);
    munin::ansi::element_type brush;
    brush.first = ' ';
    component->set_brush(brush);

    window.get_content()->add_component(component);

    io_service.reset();
    io_service.run();
    CPPUNIT_ASSERT_EQUAL(u32(0),    called);
    CPPUNIT_ASSERT_EQUAL(size_t(0), paint_data.size());
    
    // We shall set the size of the component to match the container.  This
    // now gives it form.  However, it still should not be drawn because it
    // is the same as the background.
    component->set_size(content_size);

    io_service.reset();
    io_service.run();
    CPPUNIT_ASSERT_EQUAL(u32(0),    called);
    CPPUNIT_ASSERT_EQUAL(size_t(0), paint_data.size());
    
    // Finally, we set the brush of the component.  This should cause a repaint.
    brush.first = '#';
    component->set_brush(brush);

    // We expect the following to occur: the current cursor position is saved.
    // Then, we go to the co-ordinates of the part to be drawn.  In our case,
    // that is the co-ordinates where are cursor already is, so is skipped.
    // Then, the component is drawn, and the cursor position is restored.
    vector<char> expected_data;
    expected_data.push_back(munin::ansi::ESCAPE);
    expected_data.push_back(munin::ansi::ANSI_SEQUENCE);
    expected_data.push_back(munin::ansi::SAVE_CURSOR_POSITION);
    expected_data.push_back('#');
    expected_data.push_back(munin::ansi::ESCAPE);
    expected_data.push_back(munin::ansi::ANSI_SEQUENCE);
    expected_data.push_back(munin::ansi::RESTORE_CURSOR_POSITION);
    
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(u32(1),               called);
    CPPUNIT_ASSERT_EQUAL(expected_data.size(), paint_data.size());
    CPPUNIT_ASSERT_EQUAL(expected_data[0],     paint_data[0]);
    CPPUNIT_ASSERT_EQUAL(expected_data[1],     paint_data[1]);
    CPPUNIT_ASSERT_EQUAL(expected_data[2],     paint_data[2]);
    CPPUNIT_ASSERT_EQUAL(expected_data[3],     paint_data[3]);
    CPPUNIT_ASSERT_EQUAL(expected_data[4],     paint_data[4]);
    CPPUNIT_ASSERT_EQUAL(expected_data[5],     paint_data[5]);
    CPPUNIT_ASSERT_EQUAL(expected_data[6],     paint_data[6]);
}

void munin_ansi_window_fixture::test_repaint_data()
{
    boost::asio::io_service io_service;
    munin::ansi::window window(io_service);

    u32          called = 0;
    vector<char> paint_data;
    function<void (std::vector<char>)> fn = (
        ++bll::var(called)
      , bll::var(paint_data) = bll::_1
    );
    
    window.on_repaint.connect(fn);
    
    // Set our content pane to have a non-zero size.
    munin::extent content_size;
    content_size.width  = 1;
    content_size.height = 1;
    window.get_content()->set_size(content_size);
    
    // Add a component to it that requires drawing.
    shared_ptr< fake_component<munin::ansi::element_type> > component(
        new fake_component<munin::ansi::element_type>);
    munin::ansi::element_type brush;
    brush.first = '+';
    component->set_brush(brush);
    component->set_size(content_size);

    window.get_content()->add_component(component);

    // We expect the following to occur: the current cursor position is saved.
    // Then, we go to the co-ordinates of the part to be drawn.  In our case,
    // that is the co-ordinates where are cursor already is, so is skipped.
    // Then, the component is drawn, and the cursor position is restored.
    vector<char> expected_data;
    expected_data.push_back(munin::ansi::ESCAPE);
    expected_data.push_back(munin::ansi::ANSI_SEQUENCE);
    expected_data.push_back(munin::ansi::SAVE_CURSOR_POSITION);
    expected_data.push_back('+');
    expected_data.push_back(munin::ansi::ESCAPE);
    expected_data.push_back(munin::ansi::ANSI_SEQUENCE);
    expected_data.push_back(munin::ansi::RESTORE_CURSOR_POSITION);
    
    io_service.reset();
    io_service.run();
    CPPUNIT_ASSERT_EQUAL(u32(1),               called);
    CPPUNIT_ASSERT_EQUAL(expected_data.size(), paint_data.size());
    CPPUNIT_ASSERT_EQUAL(expected_data[0],     paint_data[0]);
    CPPUNIT_ASSERT_EQUAL(expected_data[1],     paint_data[1]);
    CPPUNIT_ASSERT_EQUAL(expected_data[2],     paint_data[2]);
    CPPUNIT_ASSERT_EQUAL(expected_data[3],     paint_data[3]);
    CPPUNIT_ASSERT_EQUAL(expected_data[4],     paint_data[4]);
    CPPUNIT_ASSERT_EQUAL(expected_data[5],     paint_data[5]);
    CPPUNIT_ASSERT_EQUAL(expected_data[6],     paint_data[6]);
    
    // Set the brush to be the same.  We know this causes a redraw event in
    // out fake component, but it must be filtered out because there are no
    // changes.
    paint_data.clear();
    expected_data.clear();
    component->set_brush(brush);

    io_service.reset();
    io_service.run();
    CPPUNIT_ASSERT_EQUAL(u32(1),               called);
    CPPUNIT_ASSERT_EQUAL(expected_data.size(), paint_data.size());
    
    // Set the brush to be different.  This must cause a repaint.  
    paint_data.clear();
    expected_data.clear();
    brush.first = '#';
    component->set_brush(brush);
    
    // We expect the following to occur: the current cursor position is saved.
    // Then, we go to the co-ordinates of the part to be drawn.  In our case,
    // that is the co-ordinates where are cursor already is, so is skipped.
    // Then, the component is drawn, and the cursor position is restored.
    expected_data.push_back(munin::ansi::ESCAPE);
    expected_data.push_back(munin::ansi::ANSI_SEQUENCE);
    expected_data.push_back(munin::ansi::SAVE_CURSOR_POSITION);
    expected_data.push_back('#');
    expected_data.push_back(munin::ansi::ESCAPE);
    expected_data.push_back(munin::ansi::ANSI_SEQUENCE);
    expected_data.push_back(munin::ansi::RESTORE_CURSOR_POSITION);
    
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(u32(2),               called);
    CPPUNIT_ASSERT_EQUAL(expected_data.size(), paint_data.size());
    CPPUNIT_ASSERT_EQUAL(expected_data[0],     paint_data[0]);
    CPPUNIT_ASSERT_EQUAL(expected_data[1],     paint_data[1]);
    CPPUNIT_ASSERT_EQUAL(expected_data[2],     paint_data[2]);
    CPPUNIT_ASSERT_EQUAL(expected_data[3],     paint_data[3]);
    CPPUNIT_ASSERT_EQUAL(expected_data[4],     paint_data[4]);
    CPPUNIT_ASSERT_EQUAL(expected_data[5],     paint_data[5]);
    CPPUNIT_ASSERT_EQUAL(expected_data[6],     paint_data[6]);
}

void munin_ansi_window_fixture::test_repaint_many_redraws()
{
    boost::asio::io_service io_service;
    munin::ansi::window window(io_service);

    u32          called = 0;
    vector<char> paint_data;
    function<void (std::vector<char>)> fn = (
        ++bll::var(called)
      , bll::var(paint_data) = bll::_1
    );
    
    window.on_repaint.connect(fn);
    
    // Set our content pane to have a non-zero size.
    munin::extent content_size;
    content_size.width  = 2;
    content_size.height = 1;
    window.get_content()->set_size(content_size);
    
    // Add components to it that requires drawing.
    shared_ptr< fake_component<munin::ansi::element_type> > component0(
        new fake_component<munin::ansi::element_type>);
    munin::ansi::element_type brush;
    brush.first = '#';
    component0->set_brush(brush);
    
    munin::point component0_origin;
    component0_origin.x = 0;
    component0_origin.y = 0;
    
    munin::extent component0_size;
    component0_size.width  = 1;
    component0_size.height = 1;
    
    component0->set_position(component0_origin);
    component0->set_size(component0_size);

    window.get_content()->add_component(component0);

    shared_ptr< fake_component<munin::ansi::element_type> > component1(
        new fake_component<munin::ansi::element_type>);
    brush.first = '+';
    component1->set_brush(brush);
    
    munin::point component1_origin;
    component1_origin.x = 1;
    component1_origin.y = 0;
    
    munin::extent component1_size;
    component1_size.width  = 1;
    component1_size.height = 1;
    
    component1->set_position(component1_origin);
    component1->set_size(component1_size);
    
    window.get_content()->add_component(component1);
    
    
    // We expect the following to occur: the current cursor position is saved.
    // Then, we go to the co-ordinates of the part to be drawn.  In our case,
    // that is the co-ordinates where are cursor already is, so is skipped.
    // Then, component0 is drawn, then component1 is drawn, and the cursor
    // position is restored.
    // This should all happen in one repaint update, despite there having been
    // multiple redraws.
    vector<char> expected_data;
    expected_data.push_back(munin::ansi::ESCAPE);
    expected_data.push_back(munin::ansi::ANSI_SEQUENCE);
    expected_data.push_back(munin::ansi::SAVE_CURSOR_POSITION);
    expected_data.push_back('#');
    expected_data.push_back('+');
    expected_data.push_back(munin::ansi::ESCAPE);
    expected_data.push_back(munin::ansi::ANSI_SEQUENCE);
    expected_data.push_back(munin::ansi::RESTORE_CURSOR_POSITION);
    
    io_service.reset();
    io_service.run();
    CPPUNIT_ASSERT_EQUAL(u32(1),               called);
    CPPUNIT_ASSERT_EQUAL(expected_data.size(), paint_data.size());
    CPPUNIT_ASSERT_EQUAL(expected_data[0],     paint_data[0]);
    CPPUNIT_ASSERT_EQUAL(expected_data[1],     paint_data[1]);
    CPPUNIT_ASSERT_EQUAL(expected_data[2],     paint_data[2]);
    CPPUNIT_ASSERT_EQUAL(expected_data[3],     paint_data[3]);
    CPPUNIT_ASSERT_EQUAL(expected_data[4],     paint_data[4]);
    CPPUNIT_ASSERT_EQUAL(expected_data[5],     paint_data[5]);
    CPPUNIT_ASSERT_EQUAL(expected_data[6],     paint_data[6]);
    CPPUNIT_ASSERT_EQUAL(expected_data[7],     paint_data[7]);
}

void munin_ansi_window_fixture::test_event()
{
    boost::asio::io_service io_service;
    munin::ansi::window window(io_service);
    
    shared_ptr<munin::ansi::container> container(window.get_content());
    shared_ptr< fake_component<munin::ansi::element_type> > component0(
        new fake_component<munin::ansi::element_type>);
    shared_ptr< fake_component<munin::ansi::element_type> > component1(
        new fake_component<munin::ansi::element_type>);
    
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
    window.event(string("unfocused container event"));
    
    CPPUNIT_ASSERT_EQUAL(true, component0_event.empty());
    CPPUNIT_ASSERT_EQUAL(true, component1_event.empty());
    
    // Set the focus to the first component.  This is the place to which the
    // event should go.
    component0->set_focus();
    
    string component0_event_string = "test component0 event";
    window.event(component0_event_string);
    
    CPPUNIT_ASSERT_EQUAL(false, component0_event.empty());
    CPPUNIT_ASSERT_EQUAL(true, component1_event.empty());
    CPPUNIT_ASSERT_EQUAL(
        component0_event_string
      , any_cast<string>(component0_event));
    
    component0_event = any();
    
    // Set the focus to the second component.  The event should hit this next.
    component1->set_focus();
    
    string component1_event_string = "test component0 event";
    window.event(component1_event_string);
    
    CPPUNIT_ASSERT_EQUAL(true, component0_event.empty());
    CPPUNIT_ASSERT_EQUAL(false, component1_event.empty());
    CPPUNIT_ASSERT_EQUAL(
        component0_event_string
      , any_cast<string>(component1_event));
}


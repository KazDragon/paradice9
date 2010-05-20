#include "munin_ansi_window_fixture.hpp"
#include "munin/ansi/window.hpp"
#include "munin/ansi/protocol.hpp"
#include "munin/ansi/ansi_types.hpp"
#include "fake_munin_component.hpp"
#include "fake_munin_container.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/lambda/lambda.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(munin_ansi_window_fixture);

using namespace boost;
using namespace std;
using namespace odin;

namespace bll = boost::lambda;

typedef munin::ansi::element_type           ansi_element_type;
typedef munin::component<ansi_element_type> ansi_component;
typedef munin::ansi::container              ansi_container;
typedef fake_component<ansi_element_type>   fake_ansi_component;
typedef fake_container<ansi_element_type>   fake_ansi_container;
typedef shared_ptr<ansi_component>          ansi_component_ptr;
typedef shared_ptr<ansi_container>          ansi_container_ptr;
typedef shared_ptr<fake_ansi_component>     fake_ansi_component_ptr;
typedef shared_ptr<fake_ansi_container>     fake_ansi_container_ptr;

void munin_ansi_window_fixture::test_constructor()
{
    boost::asio::io_service io_service;
    munin::ansi::window window(io_service);
    
    ansi_container_ptr content = window.get_content();
    
    CPPUNIT_ASSERT(content != NULL);
}

void munin_ansi_window_fixture::test_repaint_registration()
{
    boost::asio::io_service io_service;
    munin::ansi::window window(io_service);
    
    function<void (string)> fn;
    window.on_repaint.connect(fn);
}

void munin_ansi_window_fixture::test_repaint_chain()
{
    boost::asio::io_service io_service;
    munin::ansi::window window(io_service);

    u32    called = 0;
    string paint_data;
    function<void (string)> fn = (
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
    fake_ansi_component_ptr component(new fake_ansi_component);
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

    // We expect the following to occur:
    string expected_data =
        "\x1B[H" // Go to (0,0)
        "#"      // Data
        "\x1B[H" // Go to (0,0) again.
        ;
        
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(u32(1), called);
    CPPUNIT_ASSERT_EQUAL(expected_data.size(), paint_data.size());
    
    for (size_t index = 0; index < expected_data.size(); ++index)
    {
        // These are wrapped in ints to make them accessible from the CPPUNIT
        // text interface.  If they were chars, they'd write the ANSI codes
        // all over the place.
        CPPUNIT_ASSERT_EQUAL(int(expected_data[index]), int(paint_data[index]));
    }
}

void munin_ansi_window_fixture::test_repaint_data()
{
    boost::asio::io_service io_service;
    munin::ansi::window window(io_service);

    u32    called = 0;
    string paint_data;
    function<void (string)> fn = (
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
    fake_ansi_component_ptr component(new fake_ansi_component);
    munin::ansi::element_type brush;
    brush.first = '+';
    component->set_brush(brush);
    component->set_size(content_size);

    window.get_content()->add_component(component);

    // We expect the following to occur:
    string expected_data =
        "\x1B[H" // Go to (0,0)
        "+"      // Data
        "\x1B[H" // Go to (0,0) again.
        ;
    
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(u32(1),               called);
    CPPUNIT_ASSERT_EQUAL(expected_data.size(), paint_data.size());
    
    for (size_t index = 0; index < expected_data.size(); ++index)
    {
        // These are wrapped in ints to make them accessible from the CPPUNIT
        // text interface.  If they were chars, they'd write the ANSI codes
        // all over the place.
        CPPUNIT_ASSERT_EQUAL(int(expected_data[index]), int(paint_data[index]));
    }
    
    // Set the brush to be the same.  We know this causes a redraw event in
    // out fake component, but it must be filtered out because there are no
    // changes.
    component->set_brush(brush);

    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(u32(1), called);
    
    // Set the brush to be different.  This must cause a repaint.  
    brush.first = '#';
    component->set_brush(brush);
    
    // We expect the following to occur:
    expected_data =
        "\x1B[H" // Go to (0,0)
        "#"      // Data
        "\x1B[H" // Go to (0,0) again.
        ;
    
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(u32(2),               called);
    CPPUNIT_ASSERT_EQUAL(expected_data.size(), paint_data.size());
    
    for (size_t index = 0; index < expected_data.size(); ++index)
    {
        // These are wrapped in ints to make them accessible from the CPPUNIT
        // text interface.  If they were chars, they'd write the ANSI codes
        // all over the place.
        CPPUNIT_ASSERT_EQUAL(int(expected_data[index]), int(paint_data[index]));
    }
}

void munin_ansi_window_fixture::test_repaint_many_redraws()
{
    boost::asio::io_service io_service;
    munin::ansi::window window(io_service);

    u32    called = 0;
    string paint_data;
    function<void (string)> fn = (
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
    fake_ansi_component_ptr component0(new fake_ansi_component);
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

    fake_ansi_component_ptr component1(new fake_ansi_component);
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

    string expected_string =
        "\x1B[H"    // Go to point (0,0)
        "#+"        // Data
        "\x1B[2H"   // Go to point (1,0) (ANSI is 1-based), which is the final
                    // resting point of the cursor (since no component has
                    // both focus and a cursor).
        ;
        
    io_service.reset();
    io_service.run();

    CPPUNIT_ASSERT_EQUAL(u32(1),                 called);
    CPPUNIT_ASSERT_EQUAL(expected_string.size(), paint_data.size());
    
    for (size_t index = 0; index < expected_string.size(); ++index)
    {
        // These are wrapped in ints to make them accessible from the CPPUNIT
        // text interface.  If they were chars, they'd write the ANSI codes
        // all over the place.
        CPPUNIT_ASSERT_EQUAL(
            int(expected_string[index])
          , int(paint_data[index]));
    }
}

void munin_ansi_window_fixture::test_repaint_many_lines()
{
    boost::asio::io_service io_service;
    munin::ansi::window window(io_service);
    
    u32    called = 0;
    string paint_data;
    function<void (string)> fn = (
        ++bll::var(called)
      , bll::var(paint_data) = bll::_1
    );
    
    window.on_repaint.connect(fn);
    
    ansi_container_ptr container(window.get_content());
    fake_ansi_component_ptr component0(new fake_ansi_component);
    fake_ansi_component_ptr component1(new fake_ansi_component);
    
    // Construct a window that is 7x7, and has components as follows:
    //
    //  0123456
    // 0
    // 1 ###
    // 2 ###
    // 3 ##+++
    // 4   +++
    // 5   +++
    // 6
    container->set_size(munin::extent(7,7));
    
    component0->set_brush(ansi_element_type('#', munin::ansi::attribute()));
    component0->set_position(munin::point(1, 1));
    component0->set_size(munin::extent(3,3));
    
    container->add_component(component0);
    
    component1->set_brush(ansi_element_type('+', munin::ansi::attribute()));
    component1->set_position(munin::point(3,3));
    component1->set_size(munin::extent(3,3));
    
    container->add_component(component1);
    
    string expected_string =
        "\x1B[2;2H" // Go to point (1,1) (ANSI has 1-based co-ordinates).
        "###"       // Data
        "\x1B[2;3H" // Go to point (1,2)
        "###"       // Data
        "\x1B[2;4H" // Go to point (1,3)
        "##+++"     // Data
        "\x1B[4;5H" // Go to point (3,4)
        "+++"       // Data
        "\x1B[4;6H" // Go to point (3,5)
        "+++"       // Data
        "\x1B[7;7H" // There is no focused component, so the cursor goes to
                    // the bottom-right corner.
        ;
        
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(u32(1), called);
    CPPUNIT_ASSERT_EQUAL(expected_string.size(), paint_data.size());
    
    for (size_t index = 0; index < expected_string.size(); ++index)
    {
        CPPUNIT_ASSERT_EQUAL(
            int(expected_string[index]), 
            int(paint_data[index]));
    }

    // Next, move the component so:
    
    //  0123456      0123456                  
    // 0            0                                    
    // 1 ###        1 ###                                       
    // 2 ###     -> 2 #+++                                           
    // 3 ##+++      3 #+++                                         
    // 4   +++      4  +++                                     
    // 5   +++      5                                          
    // 6            6     
    
    // We expect two regions to be redrawn: the region "from" and the region
    // "to".  That is, (3,3):(3,3) and (2,2):(3,3).
    // This amounts to the following slices (underscore is space):
    //     (2,2):(3,1) "+++"
    //     (2,3):(4,1) "+++_"
    //     (2,4):(4,1) "+++_"
    //     (3,5):(3,1) "___"
    munin::point new_position(2,2);
    component1->set_position(new_position);
    
    expected_string =
        "\x1B[3;3H" // Go to point (2,2) (ANSI is 1-based)
        "+++"       // Data
        "\x1B[3;4H" // Go to point (2,3)
        "+++ "      // Data
        "\x1B[3;5H" // Go to point (2,4)
        "+++ "      // Data
        "\x1B[4;6H" // Go to point (3,5)
        "   "       // Data
        "\x1B[7;7H" // There is no focused component, so the cursor goes to
                    // the bottom-right corner.
        ;
        
    io_service.reset();
    io_service.run();
    
    CPPUNIT_ASSERT_EQUAL(u32(2), called);
    CPPUNIT_ASSERT_EQUAL(expected_string.size(), paint_data.size());
    
    for (size_t index = 0; index < expected_string.size(); ++index)
    {
        CPPUNIT_ASSERT_EQUAL(
            int(expected_string[index]), 
            int(paint_data[index]));
    }
}

void munin_ansi_window_fixture::test_event()
{
    boost::asio::io_service io_service;
    munin::ansi::window window(io_service);
    
    ansi_container_ptr container(window.get_content());
    fake_ansi_component_ptr component0(new fake_ansi_component);
    fake_ansi_component_ptr component1(new fake_ansi_component);

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

void munin_ansi_window_fixture::test_cursor_state()
{
    boost::asio::io_service io_service;
    munin::ansi::window window(io_service);

    odin::u32 called = 0;
    string    paint_data;
    function<void (string)> callback = (
        ++bll::var(called)
      , bll::var(paint_data) = bll::_1
    );
    
    window.on_repaint.connect(callback);
    
    ansi_container_ptr      container(window.get_content());
    container->set_size(munin::extent(5, 5));
    
    fake_ansi_component_ptr component0(new fake_ansi_component);
    component0->set_brush(
        munin::ansi::element_type('#', munin::ansi::attribute()));
    component0->set_position(munin::point(2,2));
    component0->set_size(munin::extent(3,3));
    
    container->add_component(component0);
    component0->set_focus();
    
    io_service.reset();
    io_service.run();
    
    // From the code above, we expect the following to be painted.
    string expected_string =
        "\x1B[3;3H" // Go to position (2,2) (ANSI is 1-based)
        "###"       // Data
        "\x1B[3;4H" // Go to position (2,3)
        "###"       // Data
        "\x1B[3;5H" // Go to position (2,4)
        "###"       // Data
        "\x1B[5;5H" // Settle the cursor in the bottom right.
        ;
        
    CPPUNIT_ASSERT_EQUAL(u32(1), called);
    CPPUNIT_ASSERT_EQUAL(expected_string.size(), paint_data.size());
    
    for (size_t index = 0; index < expected_string.size(); ++index)
    {
        CPPUNIT_ASSERT_EQUAL(
            int(expected_string[index]), 
            int(paint_data[index]));
    }
    
    // Now, set the cursor to be in the middle of the component.
    component0->set_cursor_position(munin::point(1,1));
    
    io_service.reset();
    io_service.run();
    
    // Because the cursor isn't actually enabled right now, no repaint should
    // have been necessary.
    CPPUNIT_ASSERT_EQUAL(u32(1), called);
    
    // Finally, set the cursor to be enabled.
    component0->set_cursor_state(true);
    
    io_service.reset();
    io_service.run();
    
    // From the code above, we expect the cursor to be moved to the point
    // specified.  It is (1,1) in the component, and the component is at
    // (2,2).  That means the cursor is at (3,3) (which is (4,4) in ANSI).
    expected_string = "\x1B[4;4H";
    
    CPPUNIT_ASSERT_EQUAL(u32(2), called);
    CPPUNIT_ASSERT_EQUAL(expected_string.size(), paint_data.size());
    
    for (size_t index = 0; index < expected_string.size(); ++index)
    {
        CPPUNIT_ASSERT_EQUAL(
            int(expected_string[index]), 
            int(paint_data[index]));
    }
    
    // Change the cursor position within the component.
    component0->set_cursor_position(munin::point(2,2));
    
    io_service.reset();
    io_service.run();
    
    // This should cause the new cursor position to be painted.
    expected_string = "\x1B[5;5H";

    CPPUNIT_ASSERT_EQUAL(u32(3), called);
    CPPUNIT_ASSERT_EQUAL(expected_string.size(), paint_data.size());
    
    for (size_t index = 0; index < expected_string.size(); ++index)
    {
        CPPUNIT_ASSERT_EQUAL(
            int(expected_string[index]), 
            int(paint_data[index]));
    }
}


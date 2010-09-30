#include "munin_text_area_fixture.hpp"
#include "munin/text_area.hpp"
#include "munin/component.hpp"
#include "munin/ansi/ansi_types.hpp"
#include "fake_munin_canvas.hpp"
#include "fake_munin_container.hpp"
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace boost;
using namespace odin;

CPPUNIT_TEST_SUITE_REGISTRATION(munin_text_area_fixture);

void munin_text_area_fixture::test_constructor()
{
    boost::shared_ptr<munin::text_area> text_area(new munin::text_area);
    
    // An input area should always have a cursor available, and it should start
    // in the top left.
    CPPUNIT_ASSERT_EQUAL(true, text_area->get_cursor_state());
    CPPUNIT_ASSERT_EQUAL(
        munin::point(0, 0), text_area->get_cursor_position());
}

void munin_text_area_fixture::test_inheritance()
{
    boost::shared_ptr<munin::text_area> text_area(new munin::text_area);
    boost::shared_ptr<
        munin::component<munin::ansi::element_type>
    > text_area_ref = text_area;
    (void)text_area_ref;
}

void munin_text_area_fixture::test_add_text()
{
    // Test that an input area can handle textual additions.  This involves
    // several steps:

    // First, test that it can accept text at the beginning of a line.
    // The text will be drawn, and the cursor moved to the right.
    
    // Next, test that it can accept text in the middle of the line.  This
    // should be as before.
    
    // Third, test that it can accept text at the end of a line.  This should
    // draw the text, and move the cursor to the beginning of the next line.

    // Finally, test that text can be accepted the end of the last line.
    // The should act just like before, except the cursor will now be out
    // of the orignal size of the component.  The component's size will have
    // also changed.
    boost::shared_ptr<munin::text_area> text_area(new munin::text_area);
    munin::point  position(0, 0);
    munin::extent size(3, 2);    
    text_area->set_position(position);
    text_area->set_size(size);
    
    CPPUNIT_ASSERT_EQUAL(true,     text_area->get_cursor_state());
    CPPUNIT_ASSERT_EQUAL(size,     text_area->get_size());
    CPPUNIT_ASSERT_EQUAL(position, text_area->get_cursor_position());
    
    // By default, the drawn area should be blank.  We check this by setting
    // the entire canvas to a '#' character, and then checking that the 
    // input area sets it all to unattributed spaces.
    fake_canvas<munin::ansi::element_type> canvas;

    munin::ansi::element_type background_brush = 
        munin::ansi::element_type('#', munin::ansi::attribute());
        
    for (s32 row = 0; row < size.height; ++row)
    {
        for (s32 column = 0; column < size.width; ++column)
        {
            canvas[column][row] = background_brush;
        }
    }
    
    // First test: add a string of one character ("a"), and see if it is 
    // painted correctly, and that the cursor is moved to the right.
    text_area->insert_text(string("a"));
    
    text_area->draw(
        canvas, munin::point(0, 0), munin::rectangle(position, size));
    
    CPPUNIT_ASSERT_EQUAL(
        munin::ansi::element_type('a', munin::ansi::attribute())
      , munin::ansi::element_type(canvas[0][0]));
    CPPUNIT_ASSERT_EQUAL(
        munin::ansi::element_type(' ', munin::ansi::attribute())
      , munin::ansi::element_type(canvas[1][0]));
    CPPUNIT_ASSERT_EQUAL(
        munin::ansi::element_type(' ', munin::ansi::attribute())
      , munin::ansi::element_type(canvas[2][0]));
    CPPUNIT_ASSERT_EQUAL(
        munin::ansi::element_type(' ', munin::ansi::attribute())
      , munin::ansi::element_type(canvas[0][1]));
    CPPUNIT_ASSERT_EQUAL(
        munin::ansi::element_type(' ', munin::ansi::attribute())
      , munin::ansi::element_type(canvas[1][1]));
    CPPUNIT_ASSERT_EQUAL(
        munin::ansi::element_type(' ', munin::ansi::attribute())
      , munin::ansi::element_type(canvas[2][1]));
    
    CPPUNIT_ASSERT_EQUAL(
        munin::point(1, 0)
      , text_area->get_cursor_position());
    
    // Next: add a string of another character, to see if it can be painted
    // correctly, and that the cursor is again moved.
    text_area->insert_text("b");
    
    text_area->draw(
        canvas, munin::point(0, 0), munin::rectangle(position, size));
    
    CPPUNIT_ASSERT_EQUAL(
        munin::ansi::element_type('a', munin::ansi::attribute())
      , munin::ansi::element_type(canvas[0][0]));
    CPPUNIT_ASSERT_EQUAL(
        munin::ansi::element_type('b', munin::ansi::attribute())
      , munin::ansi::element_type(canvas[1][0]));
    CPPUNIT_ASSERT_EQUAL(
        munin::ansi::element_type(' ', munin::ansi::attribute())
      , munin::ansi::element_type(canvas[2][0]));
    CPPUNIT_ASSERT_EQUAL(
        munin::ansi::element_type(' ', munin::ansi::attribute())
      , munin::ansi::element_type(canvas[0][1]));
    CPPUNIT_ASSERT_EQUAL(
        munin::ansi::element_type(' ', munin::ansi::attribute())
      , munin::ansi::element_type(canvas[1][1]));
    CPPUNIT_ASSERT_EQUAL(
        munin::ansi::element_type(' ', munin::ansi::attribute())
      , munin::ansi::element_type(canvas[2][1]));
    
    CPPUNIT_ASSERT_EQUAL(
        munin::point(2, 0)
      , text_area->get_cursor_position());
    
    // Now an important test: adding another element should paint that
    // character and move the cursor to the beginning of the next line.
    text_area->insert_text("c");
    
    text_area->draw(
        canvas, munin::point(0, 0), munin::rectangle(position, size));
    
    CPPUNIT_ASSERT_EQUAL(
        munin::ansi::element_type('a', munin::ansi::attribute())
      , munin::ansi::element_type(canvas[0][0]));
    CPPUNIT_ASSERT_EQUAL(
        munin::ansi::element_type('b', munin::ansi::attribute())
      , munin::ansi::element_type(canvas[1][0]));
    CPPUNIT_ASSERT_EQUAL(
        munin::ansi::element_type('c', munin::ansi::attribute())
      , munin::ansi::element_type(canvas[2][0]));
    CPPUNIT_ASSERT_EQUAL(
        munin::ansi::element_type(' ', munin::ansi::attribute())
      , munin::ansi::element_type(canvas[0][1]));
    CPPUNIT_ASSERT_EQUAL(
        munin::ansi::element_type(' ', munin::ansi::attribute())
      , munin::ansi::element_type(canvas[1][1]));
    CPPUNIT_ASSERT_EQUAL(
        munin::ansi::element_type(' ', munin::ansi::attribute())
      , munin::ansi::element_type(canvas[2][1]));
    
    CPPUNIT_ASSERT_EQUAL(
        munin::point(0, 1)
      , text_area->get_cursor_position());
    
    // TODO: check that strings of multiple characters will be added.
    // TODO: check that a string over the last line will cause the text area
    // to expand.
}


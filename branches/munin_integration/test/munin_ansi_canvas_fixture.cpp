#include "munin_ansi_canvas_fixture.hpp"
#include "munin/ansi/ansi_canvas.hpp"
#include <stdexcept>

using namespace std;
using namespace boost;

CPPUNIT_TEST_SUITE_REGISTRATION(munin_ansi_canvas_fixture);

void munin_ansi_canvas_fixture::test_constructor()
{
    munin::ansi::ansi_canvas canvas;
    (void)canvas;
}

void munin_ansi_canvas_fixture::test_inheritance()
{
    munin::ansi::ansi_canvas                  canvas;
    munin::canvas<munin::ansi::element_type> &canvas_ref = canvas;
    
    (void)canvas_ref;
}

void munin_ansi_canvas_fixture::test_size()
{
    munin::ansi::ansi_canvas canvas;
    
    munin::extent size = canvas.get_size();
    
    CPPUNIT_ASSERT_EQUAL(odin::s32(0), size.width);
    CPPUNIT_ASSERT_EQUAL(odin::s32(0), size.height);
    
    munin::extent new_size;
    new_size.width  = 80;
    new_size.height = 24;
    canvas.set_size(new_size);
    
    size = canvas.get_size();

    CPPUNIT_ASSERT_EQUAL(odin::s32(80), size.width);
    CPPUNIT_ASSERT_EQUAL(odin::s32(24), size.height);
}

void munin_ansi_canvas_fixture::test_array_indexing()
{
    munin::ansi::ansi_canvas canvas;
    
    munin::extent size;
    size.width  = 80;
    size.height = 24;
    canvas.set_size(size);
    
    // By default, the canvas should be full of unattributed spaces.
    munin::ansi::element_type element = make_pair(
        ' '
      , munin::ansi::attribute());    
    
    for (odin::s32 row = 0; row < size.height; ++row)
    {
        for (odin::s32 column = 0; column < size.width; ++column)
        {
            CPPUNIT_ASSERT_EQUAL(
                element
              , static_cast<munin::ansi::element_type>(canvas[column][row]));
        }
    }
    
    // Populate the canvas with something appropriate.
    for (odin::s32 row = 0; row < size.height; ++row)
    {
        for (odin::s32 column = 0; column < size.width; ++column)
        {
            canvas[column][row] = make_pair(
                char(column * row)
              , munin::ansi::attribute());
        }
    }
    
    // Assert that everything has been set correctly.
    for (odin::s32 row = 0; row < size.height; ++row)
    {
        for (odin::s32 column = 0; column < size.width; ++column)
        {
            element = canvas[column][row];
            
            CPPUNIT_ASSERT_EQUAL(char(column * row), element.first);
        }
    }
}

void munin_ansi_canvas_fixture::test_array_resizing()
{
    munin::ansi::ansi_canvas canvas;
    
    munin::extent size;
    size.width  = 80;
    size.height = 24;
    canvas.set_size(size);
    
    // Populate the canvas with something appropriate.
    for (odin::s32 row = 0; row < size.height; ++row)
    {
        for (odin::s32 column = 0; column < size.width; ++column)
        {
            canvas[column][row] = make_pair(
                char(column * row)
              , munin::ansi::attribute());
        }
    }
    
    // Assert that everything has been set correctly.
    for (odin::s32 row = 0; row < size.height; ++row)
    {
        for (odin::s32 column = 0; column < size.width; ++column)
        {
            munin::ansi::element_type element = canvas[column][row];
            
            CPPUNIT_ASSERT_EQUAL(char(column * row), element.first);
        }
    }
    
    // Resize the canvas to be one wider and one taller.
    ++size.width;
    ++size.height;
    canvas.set_size(size);
    
    // Assert that all the original data remains and that the new space is
    // correctly filled with blanks.
    for (odin::s32 row = 0; row < size.height; ++row)
    {
        for (odin::s32 column = 0; column < size.width; ++column)
        {
            munin::ansi::element_type element = canvas[column][row];
         
            if (row == size.height - 1 || column == size.width - 1)
            {
                CPPUNIT_ASSERT_EQUAL(' ', element.first);
            }
            else
            {
                CPPUNIT_ASSERT_EQUAL(char(column * row), element.first);
            }
        }
    }
    
    // Resize the canvas to be two smaller and two shorter.
    size.width -= 2;
    size.height -= 2;
    canvas.set_size(size);

    // Assert that all the original data remains.
    for (odin::s32 row = 0; row < size.height; ++row)
    {
        for (odin::s32 column = 0; column < size.width; ++column)
        {
            munin::ansi::element_type element = canvas[column][row];
            
            CPPUNIT_ASSERT_EQUAL(char(column * row), element.first);
        }
    }
}

void munin_ansi_canvas_fixture::test_out_of_range()
{
    munin::ansi::ansi_canvas canvas;
    
    munin::extent size;
    size.width  = 80;
    size.height = 24;
    canvas.set_size(size);

    munin::ansi::element_type element;
    
    // Check that going out of range on either the width or height causes an
    // exception to be thrown.
    CPPUNIT_ASSERT_THROW(
        element = canvas[size.width][size.height - 1]
      , std::out_of_range);

    CPPUNIT_ASSERT_THROW(
        element = canvas[size.width - 1][size.height]
      , std::out_of_range);

    CPPUNIT_ASSERT_THROW(
        canvas[size.width][size.height - 1] = element
      , std::out_of_range);
    
    CPPUNIT_ASSERT_THROW(
        canvas[size.width - 1][size.height] = element
      , std::out_of_range);
}

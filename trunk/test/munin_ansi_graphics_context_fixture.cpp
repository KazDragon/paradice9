#include "munin_ansi_graphics_context_fixture.hpp"
#include "munin/ansi/graphics_context.hpp"
#include <stdexcept>

using namespace std;
using namespace boost;

CPPUNIT_TEST_SUITE_REGISTRATION(munin_ansi_graphics_context_fixture);

void munin_ansi_graphics_context_fixture::test_constructor()
{
    munin::ansi::graphics_context context;
    (void)context;
}

void munin_ansi_graphics_context_fixture::test_inheritance()
{
    munin::ansi::graphics_context context;
    
    munin::graphics_context<
        munin::ansi::element_type
    > &context_ref = context;
    
    (void)context_ref;
}

void munin_ansi_graphics_context_fixture::test_size()
{
    munin::ansi::graphics_context context;
    
    munin::extent size = context.get_size();
    
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), size.width);
    CPPUNIT_ASSERT_EQUAL(odin::u32(0), size.height);
    
    munin::extent new_size;
    new_size.width  = 80;
    new_size.height = 24;
    context.set_size(new_size);
    
    size = context.get_size();

    CPPUNIT_ASSERT_EQUAL(odin::u32(80), size.width);
    CPPUNIT_ASSERT_EQUAL(odin::u32(24), size.height);
}

void munin_ansi_graphics_context_fixture::test_array_indexing()
{
    munin::ansi::graphics_context context;
    
    munin::extent size;
    size.width  = 80;
    size.height = 24;
    context.set_size(size);
    
    // By default, the context should be full of unattributed spaces.
    munin::ansi::element_type element = make_pair(
        ' '
      , optional<munin::ansi::attribute>());    
    
    for (odin::u32 row = 0; row < size.height; ++row)
    {
        for (odin::u32 column = 0; column < size.width; ++column)
        {
            CPPUNIT_ASSERT_EQUAL(
                element
              , static_cast<munin::ansi::element_type>(context[column][row]));
        }
    }
    
    // Populate the graphics context with something appropriate.
    for (odin::u32 row = 0; row < size.height; ++row)
    {
        for (odin::u32 column = 0; column < size.width; ++column)
        {
            context[column][row] = make_pair(
                char(column * row)
              , optional<munin::ansi::attribute>());
        }
    }
    
    // Assert that everything has been set correctly.
    for (odin::u32 row = 0; row < size.height; ++row)
    {
        for (odin::u32 column = 0; column < size.width; ++column)
        {
            element = context[column][row];
            
            CPPUNIT_ASSERT_EQUAL(char(column * row), element.first);
            CPPUNIT_ASSERT_EQUAL(false, element.second.is_initialized());
        }
    }
}

void munin_ansi_graphics_context_fixture::test_array_resizing()
{
    munin::ansi::graphics_context context;
    
    munin::extent size;
    size.width  = 80;
    size.height = 24;
    context.set_size(size);
    
    // Populate the graphics context with something appropriate.
    for (odin::u32 row = 0; row < size.height; ++row)
    {
        for (odin::u32 column = 0; column < size.width; ++column)
        {
            context[column][row] = make_pair(
                char(column * row)
              , optional<munin::ansi::attribute>());
        }
    }
    
    // Assert that everything has been set correctly.
    for (odin::u32 row = 0; row < size.height; ++row)
    {
        for (odin::u32 column = 0; column < size.width; ++column)
        {
            munin::ansi::element_type element = context[column][row];
            
            CPPUNIT_ASSERT_EQUAL(char(column * row), element.first);
            CPPUNIT_ASSERT_EQUAL(false, element.second.is_initialized());
        }
    }
    
    // Resize the context to be one wider and one taller.
    ++size.width;
    ++size.height;
    context.set_size(size);
    
    // Assert that all the original data remains and that the new space is
    // correctly filled with blanks.
    for (odin::u32 row = 0; row < size.height; ++row)
    {
        for (odin::u32 column = 0; column < size.width; ++column)
        {
            munin::ansi::element_type element = context[column][row];
         
            if (row == size.height - 1 || column == size.width - 1)
            {
                CPPUNIT_ASSERT_EQUAL(' ', element.first);
            }
            else
            {
                CPPUNIT_ASSERT_EQUAL(char(column * row), element.first);
            }
            
            CPPUNIT_ASSERT_EQUAL(false, element.second.is_initialized());
        }
    }
    
    // Resize the context to be two smaller and two shorter.
    size.width -= 2;
    size.height -= 2;
    context.set_size(size);

    // Assert that all the original data remains.
    for (odin::u32 row = 0; row < size.height; ++row)
    {
        for (odin::u32 column = 0; column < size.width; ++column)
        {
            munin::ansi::element_type element = context[column][row];
            
            CPPUNIT_ASSERT_EQUAL(char(column * row), element.first);
            CPPUNIT_ASSERT_EQUAL(false, element.second.is_initialized());
        }
    }
}

void munin_ansi_graphics_context_fixture::test_out_of_range()
{
    munin::ansi::graphics_context context;
    
    munin::extent size;
    size.width  = 80;
    size.height = 24;
    context.set_size(size);

    munin::ansi::element_type element;
    
    // Check that going out of range on either the width or height causes an
    // exception to be thrown.
    CPPUNIT_ASSERT_THROW(
        element = context[size.width][size.height - 1]
      , std::out_of_range);

    
    CPPUNIT_ASSERT_THROW(
        element = context[size.width - 1][size.height]
      , std::out_of_range);

    CPPUNIT_ASSERT_THROW(
        context[size.width][size.height - 1] = element
      , std::out_of_range);

    
    CPPUNIT_ASSERT_THROW(
        context[size.width - 1][size.height] = element
      , std::out_of_range);
}

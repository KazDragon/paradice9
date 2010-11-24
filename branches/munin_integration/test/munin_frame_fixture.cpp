#include "munin_frame_fixture.hpp"
#include "munin/ansi/frame.hpp"
#include "munin/component.hpp"
#include "munin/ansi/ansi_types.hpp"
#include "fake_munin_canvas.hpp"
#include "fake_munin_container.hpp"
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace boost;
using namespace odin;

CPPUNIT_TEST_SUITE_REGISTRATION(munin_frame_fixture);

void munin_frame_fixture::test_constructor()
{
    boost::shared_ptr< munin::ansi::frame> frame(new munin::ansi::frame);
    (void)frame;
}

void munin_frame_fixture::test_inheritance()
{
    boost::shared_ptr<munin::ansi::frame> frame(new munin::ansi::frame);
    boost::shared_ptr<
        munin::component<munin::ansi::element_type>
    > frame_ref = frame;
    (void)frame_ref;
}

void munin_frame_fixture::test_draw()
{
    boost::shared_ptr<munin::ansi::frame> frame(new munin::ansi::frame);
    
    // The default horizontal brushes are "-" character.
    // The default vertical brushes are "|" character.
    // The default corner brushes are "+" characters.
    // The default border thicknesses are 1.
    // There is no text on the borders. 
    
    // Therefore, a default frame with extents (5,3) should look like:
    //
    // +---+
    // |   |
    // +---+
    munin::extent size(5,3);
    frame->set_size(size);
    
    // In addition, it should NOT fill the centre.  Therefore, we initialise
    // our canvas with non-space characters.
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
    
    frame->draw(
        canvas
      , munin::point(0,0)
      , munin::rectangle(munin::point(0,0), size)); 
    
    // The final canvas should look like this:
    //
    // +---+
    // |###|
    // +---+
    CPPUNIT_ASSERT_EQUAL('+', munin::ansi::element_type(canvas[0][0]).first);
    CPPUNIT_ASSERT_EQUAL('-', munin::ansi::element_type(canvas[1][0]).first);
    CPPUNIT_ASSERT_EQUAL('-', munin::ansi::element_type(canvas[2][0]).first);
    CPPUNIT_ASSERT_EQUAL('-', munin::ansi::element_type(canvas[3][0]).first);
    CPPUNIT_ASSERT_EQUAL('+', munin::ansi::element_type(canvas[4][0]).first);
    CPPUNIT_ASSERT_EQUAL('|', munin::ansi::element_type(canvas[0][1]).first);
    CPPUNIT_ASSERT_EQUAL('#', munin::ansi::element_type(canvas[1][1]).first);
    CPPUNIT_ASSERT_EQUAL('#', munin::ansi::element_type(canvas[2][1]).first);
    CPPUNIT_ASSERT_EQUAL('#', munin::ansi::element_type(canvas[3][1]).first);
    CPPUNIT_ASSERT_EQUAL('|', munin::ansi::element_type(canvas[4][1]).first);
    CPPUNIT_ASSERT_EQUAL('+', munin::ansi::element_type(canvas[0][2]).first);
    CPPUNIT_ASSERT_EQUAL('-', munin::ansi::element_type(canvas[1][2]).first);
    CPPUNIT_ASSERT_EQUAL('-', munin::ansi::element_type(canvas[2][2]).first);
    CPPUNIT_ASSERT_EQUAL('-', munin::ansi::element_type(canvas[3][2]).first);
    CPPUNIT_ASSERT_EQUAL('+', munin::ansi::element_type(canvas[4][2]).first);
}

void munin_frame_fixture::test_draw_with_offsets()
{
    typedef fake_container<munin::ansi::element_type> container;
    munin::ansi::element_type default_brush('@', munin::ansi::attribute());
    // The previous test established that the frame can draw without 
    // overwriting the background.  Here we test that it draws correctly when
    // embedded in containers in space.
    
    // Container0 is the top level container (extents = (8,8))
    // Container1 is a container within container0, and is at position
    // (1,2).  It fills up the remainder of the space (extents = (7,6)) 
    // frame is within container 1 at position (2,1) and fills up a space with
    // extents (5,3).
    boost::shared_ptr<container> container0(new container(default_brush));
    boost::shared_ptr<container> container1(new container(default_brush));
    boost::shared_ptr<munin::ansi::frame> frame(new munin::ansi::frame);
    
    container0->set_size(munin::extent(8,8));
    
    container1->set_position(munin::point(1,2));
    container1->set_size(munin::extent(7,6));
    container0->add_component(container1);
    
    frame->set_position(munin::point(2,1));
    frame->set_size(munin::extent(5,3));
    container1->add_component(frame);

    fake_canvas<munin::ansi::element_type> canvas;
    container0->draw(
        canvas
      , munin::point(0,0)
      , munin::rectangle(munin::point(0,0), munin::extent(8,8)));
    
    // The final result is that the frame should be positioned at (3,3)
    // and extend (5,3).
    CPPUNIT_ASSERT_EQUAL('+', munin::ansi::element_type(canvas[3][3]).first);
    CPPUNIT_ASSERT_EQUAL('-', munin::ansi::element_type(canvas[4][3]).first);
    CPPUNIT_ASSERT_EQUAL('-', munin::ansi::element_type(canvas[5][3]).first);
    CPPUNIT_ASSERT_EQUAL('-', munin::ansi::element_type(canvas[6][3]).first);
    CPPUNIT_ASSERT_EQUAL('+', munin::ansi::element_type(canvas[7][3]).first);
    CPPUNIT_ASSERT_EQUAL('|', munin::ansi::element_type(canvas[3][4]).first);
    CPPUNIT_ASSERT_EQUAL('@', munin::ansi::element_type(canvas[4][4]).first);
    CPPUNIT_ASSERT_EQUAL('@', munin::ansi::element_type(canvas[5][4]).first);
    CPPUNIT_ASSERT_EQUAL('@', munin::ansi::element_type(canvas[6][4]).first);
    CPPUNIT_ASSERT_EQUAL('|', munin::ansi::element_type(canvas[7][4]).first);
    CPPUNIT_ASSERT_EQUAL('+', munin::ansi::element_type(canvas[3][5]).first);
    CPPUNIT_ASSERT_EQUAL('-', munin::ansi::element_type(canvas[4][5]).first);
    CPPUNIT_ASSERT_EQUAL('-', munin::ansi::element_type(canvas[5][5]).first);
    CPPUNIT_ASSERT_EQUAL('-', munin::ansi::element_type(canvas[6][5]).first);
    CPPUNIT_ASSERT_EQUAL('+', munin::ansi::element_type(canvas[7][5]).first);
}


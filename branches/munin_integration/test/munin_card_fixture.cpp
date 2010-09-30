#include "munin_card_fixture.hpp"
#include "munin/card.hpp"
#include "fake_munin_canvas.hpp"
#include "fake_munin_component.hpp"
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace boost;
using namespace odin;

namespace bll = lambda;

CPPUNIT_TEST_SUITE_REGISTRATION(munin_card_fixture);

void munin_card_fixture::test_constructor()
{
    shared_ptr< munin::card<char> > card(new munin::card<char>);
    (void)card;

    CPPUNIT_ASSERT_EQUAL(u32(0), card->get_number_of_faces());
}

void munin_card_fixture::test_inheritance()
{
    shared_ptr< munin::card<char> > card(new munin::card<char>);
    shared_ptr< munin::component<char> > card_ref = card;
    (void)card_ref;

    CPPUNIT_ASSERT_EQUAL(u32(0), card->get_number_of_faces());
}

void munin_card_fixture::test_add_face()
{
    shared_ptr< munin::card<char> > card(new munin::card<char>);

    shared_ptr< munin::component<char> > component0(new fake_component<char>);
    shared_ptr< munin::component<char> > component1(new fake_component<char>);

    CPPUNIT_ASSERT_EQUAL(u32(0), card->get_number_of_faces());

    card->add_face(component0, "component0");
    CPPUNIT_ASSERT_EQUAL(u32(1), card->get_number_of_faces());

    card->add_face(component1, "component1");
    CPPUNIT_ASSERT_EQUAL(u32(2), card->get_number_of_faces());
}

void munin_card_fixture::test_draw_no_faces()
{
    shared_ptr< munin::card<char> > card(new munin::card<char>);
    munin::extent size(2, 2);
    card->set_size(size);

    // The default card should not draw anything.  Therefore, we fill the
    // canvas with junk to show that.
    fake_canvas<char> canvas;

    char background_brush = '@';
    for (s32 row = 0; row < size.height; ++row)
    {
        for (s32 column = 0; column < size.width; ++column)
        {
            canvas[column][row] = background_brush;
        }
    }

    card->draw(
        canvas,
        munin::point(0, 0),
        munin::rectangle(munin::point(0, 0), size));

    CPPUNIT_ASSERT_EQUAL('@', char(canvas[0][0]));
    CPPUNIT_ASSERT_EQUAL('@', char(canvas[0][1]));
    CPPUNIT_ASSERT_EQUAL('@', char(canvas[1][0]));
    CPPUNIT_ASSERT_EQUAL('@', char(canvas[1][1]));
}

void munin_card_fixture::test_draw_first_face()
{
    munin::extent size(2, 2);

    shared_ptr< munin::card<char> > card(new munin::card<char>);
    card->set_position(munin::point(0, 0));
    card->set_size(size);

    shared_ptr< fake_component<char> > component0(new fake_component<char>);
    component0->set_brush('#');
    component0->set_position(munin::point(0, 0));
    component0->set_size(size);

    shared_ptr< fake_component<char> > component1(new fake_component<char>);
    component1->set_brush('%');
    component1->set_position(munin::point(0, 0));
    component1->set_size(size);

    card->add_face(component0, "0");
    card->add_face(component1, "1");

    card->select_face("0");

    // Since we have selected the first card (component0, called "0"),
    // we should draw that component.  The entire component should be filled
    // with the '#' character.
    fake_canvas<char> canvas;

    card->draw(
        canvas,
        munin::point(0, 0),
        munin::rectangle(munin::point(0, 0), size));

    CPPUNIT_ASSERT_EQUAL('#', char(canvas[0][0]));
    CPPUNIT_ASSERT_EQUAL('#', char(canvas[0][1]));
    CPPUNIT_ASSERT_EQUAL('#', char(canvas[1][0]));
    CPPUNIT_ASSERT_EQUAL('#', char(canvas[1][1]));
}

void munin_card_fixture::test_draw_second_face()
{
    munin::extent size(2, 2);

    shared_ptr< munin::card<char> > card(new munin::card<char>);
    card->set_position(munin::point(0, 0));
    card->set_size(size);

    shared_ptr< fake_component<char> > component0(new fake_component<char>);
    component0->set_brush('#');
    component0->set_position(munin::point(0, 0));
    component0->set_size(size);

    shared_ptr< fake_component<char> > component1(new fake_component<char>);
    component1->set_brush('%');
    component1->set_position(munin::point(0, 0));
    component1->set_size(size);

    card->add_face(component0, "0");
    card->add_face(component1, "1");

    card->select_face("1");

    // Since we have selected the first card (component1, called "1"),
    // we should draw that component.  The entire component should be filled
    // with the '%' character.
    fake_canvas<char> canvas;

    card->draw(
        canvas,
        munin::point(0, 0),
        munin::rectangle(munin::point(0, 0), size));

    CPPUNIT_ASSERT_EQUAL('%', char(canvas[0][0]));
    CPPUNIT_ASSERT_EQUAL('%', char(canvas[0][1]));
    CPPUNIT_ASSERT_EQUAL('%', char(canvas[1][0]));
    CPPUNIT_ASSERT_EQUAL('%', char(canvas[1][1]));
}

void munin_card_fixture::test_select_face()
{
    munin::extent size(2, 2);

    shared_ptr< munin::card<char> > card(new munin::card<char>);
    card->set_position(munin::point(0, 0));
    card->set_size(size);

    shared_ptr< fake_component<char> > component0(new fake_component<char>);
    component0->set_brush('#');
    component0->set_position(munin::point(0, 0));
    component0->set_size(size);

    shared_ptr< fake_component<char> > component1(new fake_component<char>);
    component1->set_brush('%');
    component1->set_position(munin::point(0, 0));
    component1->set_size(size);

    card->add_face(component0, "0");
    card->add_face(component1, "1");

    vector<munin::rectangle> regions;
    function<void (vector<munin::rectangle>)> on_redraw = (
        bll::var(regions) = bll::_1
    );

    card->on_redraw.connect(on_redraw);

    CPPUNIT_ASSERT_EQUAL(
        vector<munin::rectangle>::size_type(0)
      , regions.size());

    // Check that selecting the first face fires an on_redraw event for the
    // entire object.
    card->select_face("0");

    CPPUNIT_ASSERT_EQUAL(
        vector<munin::rectangle>::size_type(1)
      , regions.size());
    CPPUNIT_ASSERT_EQUAL(
        munin::rectangle(munin::point(0, 0), size)
      , regions[0]);

    // Check that selecting the same object does not fire an on_redraw event.
    regions.clear();

    card->select_face("0");

    CPPUNIT_ASSERT_EQUAL(
        vector<munin::rectangle>::size_type(0)
      , regions.size());

    // Check that selecting a different object also fires an on_redraw event.
    card->select_face("1");

    CPPUNIT_ASSERT_EQUAL(
        vector<munin::rectangle>::size_type(1)
      , regions.size());
    CPPUNIT_ASSERT_EQUAL(
        munin::rectangle(munin::point(0, 0), size)
      , regions[0]);
}

void munin_card_fixture::test_can_focus()
{
    munin::extent size(2, 2);

    shared_ptr< munin::card<char> > card(new munin::card<char>);
    card->set_position(munin::point(0, 0));
    card->set_size(size);

    // There are no faces to the card, so it cannot focus any of them.
    CPPUNIT_ASSERT_EQUAL(false, card->can_focus());

    shared_ptr< fake_component<char> > component0(new fake_component<char>);
    component0->set_brush('#');
    component0->set_position(munin::point(0, 0));
    component0->set_size(size);

    shared_ptr< fake_component<char> > component1(new fake_component<char>);
    component1->set_brush('%');
    component1->set_position(munin::point(0, 0));
    component1->set_size(size);

    card->add_face(component0, "0");
    card->add_face(component1, "1");

    component0->set_can_focus(false);
    component1->set_can_focus(false);

    // There are focuses, but they cannot have a focus, so card should not either.
    CPPUNIT_ASSERT_EQUAL(false, card->can_focus());

    component0->set_can_focus(true);

    // Though at least one card can have a focus, none is selected, so we 
    // cannot receive focus.
    CPPUNIT_ASSERT_EQUAL(false, card->can_focus());

    card->select_face("0");

    // The current face showing can have focus, so the card should be able to
    // as well.
    CPPUNIT_ASSERT_EQUAL(true, card->can_focus());

    card->select_face("1");

    // We have moved to a face that cannot have focus, therefore the card 
    // shouldn't either.
    CPPUNIT_ASSERT_EQUAL(false, card->can_focus());
}

void munin_card_fixture::test_set_focus()
{
    munin::extent size(2, 2);

    shared_ptr< munin::card<char> > card(new munin::card<char>);
    card->set_position(munin::point(0, 0));
    card->set_size(size);

    shared_ptr< fake_component<char> > component0(new fake_component<char>);
    component0->set_brush('#');
    component0->set_position(munin::point(0, 0));
    component0->set_size(size);
    component0->set_can_focus(true);

    shared_ptr< fake_component<char> > component1(new fake_component<char>);
    component1->set_brush('%');
    component1->set_position(munin::point(0, 0));
    component1->set_size(size);
    component1->set_can_focus(true);

    card->add_face(component0, "0");
    card->add_face(component1, "1");

    shared_ptr< munin::component<char> > last_focus_set;
    function<void (shared_ptr< munin::component<char> >)> on_focus_set =
    (
        bll::var(last_focus_set) = bll::_1
    );

    shared_ptr< munin::component<char> > last_focus_lost;
    function<void (shared_ptr< munin::component<char> >)> on_focus_lost =
    (
        bll::var(last_focus_lost) = bll::_1
    );

    component0->on_focus_set.connect(bind(on_focus_set, component0));
    component0->on_focus_lost.connect(bind(on_focus_lost, component0));
    component1->on_focus_set.connect(bind(on_focus_set, component1));
    component1->on_focus_lost.connect(bind(on_focus_lost, component1));
    card->on_focus_set.connect(bind(on_focus_set, card));
    card->on_focus_lost.connect(bind(on_focus_lost, card));

    CPPUNIT_ASSERT_EQUAL(false, component0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, component1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, card->has_focus());

    // Test that, if no face is showing, a set_focus call will fail to set
    // any focus.
    card->set_focus();

    CPPUNIT_ASSERT_EQUAL(false, component0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, component1->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, card->has_focus());

    CPPUNIT_ASSERT(last_focus_set  == shared_ptr< munin::component<char> >());
    CPPUNIT_ASSERT(last_focus_lost == shared_ptr< munin::component<char> >());

    // After selecting a face, set_focus() should set the focus both for the
    // face and the card itself.
    card->select_face("0");

    card->set_focus();

    CPPUNIT_ASSERT_EQUAL(true,  component0->has_focus());
    CPPUNIT_ASSERT_EQUAL(false, component1->has_focus());
    CPPUNIT_ASSERT_EQUAL(true,  card->has_focus());

    CPPUNIT_ASSERT(last_focus_set  == card);
    CPPUNIT_ASSERT(last_focus_lost == shared_ptr< munin::component<char> >());
}



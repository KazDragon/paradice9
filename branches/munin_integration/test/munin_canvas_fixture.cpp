#include "munin_canvas_fixture.hpp"
#include "fake_munin_canvas.hpp"
#include "odin/types.hpp"
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <algorithm>
#include <utility>

CPPUNIT_TEST_SUITE_REGISTRATION(munin_canvas_fixture);

using namespace std;

struct test_attribute
{
    test_attribute(int id = 0) : id_(id) {}
    int id_;
};

typedef fake_canvas< pair<char, test_attribute> > test_canvas;

void munin_canvas_fixture::test_constructor()
{
    test_canvas();
}

void munin_canvas_fixture::test_array_notation()
{
    test_canvas                               tctx;
    munin::canvas<test_canvas::element_type> &ctx = tctx;
    
    ctx[0][0] = make_pair('a', test_attribute());
    ctx[1][0] = make_pair('b', test_attribute());
    ctx[0][1] = make_pair('c', test_attribute());
    
    CPPUNIT_ASSERT_EQUAL('a', tctx.values_[0][0].first);
    CPPUNIT_ASSERT_EQUAL('b', tctx.values_[1][0].first);
    CPPUNIT_ASSERT_EQUAL('c', tctx.values_[0][1].first);
    
    CPPUNIT_ASSERT_EQUAL('a', test_canvas::element_type(ctx[0][0]).first);
    CPPUNIT_ASSERT_EQUAL('b', test_canvas::element_type(ctx[1][0]).first);
    CPPUNIT_ASSERT_EQUAL('c', test_canvas::element_type(ctx[0][1]).first);
}

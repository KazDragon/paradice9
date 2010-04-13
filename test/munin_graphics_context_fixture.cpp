#include "munin_graphics_context_fixture.hpp"
#include "fake_munin_graphics_context.hpp"
#include "odin/types.hpp"
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <algorithm>
#include <utility>

CPPUNIT_TEST_SUITE_REGISTRATION(munin_graphics_context_fixture);

using namespace std;

struct test_attribute
{
    test_attribute(int id = 0) : id_(id) {}
    int id_;
};

typedef fake_graphics_context< pair<char, test_attribute> > 
    test_graphics_context;

void munin_graphics_context_fixture::test_constructor()
{
    test_graphics_context();
}

void munin_graphics_context_fixture::test_array_notation()
{
    test_graphics_context tctx;
    munin::graphics_context<
        test_graphics_context::element_type
    > &ctx = tctx;
    
    ctx[0][0] = make_pair('a', test_attribute());
    ctx[1][0] = make_pair('b', test_attribute());
    ctx[0][1] = make_pair('c', test_attribute());
    
    CPPUNIT_ASSERT_EQUAL('a', tctx.values_[0][0].first);
    CPPUNIT_ASSERT_EQUAL('b', tctx.values_[1][0].first);
    CPPUNIT_ASSERT_EQUAL('c', tctx.values_[0][1].first);
    
    CPPUNIT_ASSERT_EQUAL('a', test_graphics_context::element_type(ctx[0][0]).first);
    CPPUNIT_ASSERT_EQUAL('b', test_graphics_context::element_type(ctx[1][0]).first);
    CPPUNIT_ASSERT_EQUAL('c', test_graphics_context::element_type(ctx[0][1]).first);
}

#include "yggdrasil_munin_rectangle_fixture.hpp"
#include "yggdrasil/munin/rectangle.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(yggdrasil_munin_rectangle_fixture);

void yggdrasil_munin_rectangle_fixture::test_default_constructor()
{
    auto rect = yggdrasil::munin::rectangle();
    CPPUNIT_ASSERT_EQUAL(0, rect.origin.x);
    CPPUNIT_ASSERT_EQUAL(0, rect.origin.y);
    CPPUNIT_ASSERT_EQUAL(0, rect.size.width);
    CPPUNIT_ASSERT_EQUAL(0, rect.size.height);
}

void yggdrasil_munin_rectangle_fixture::test_origin_size_constructor()
{
    auto origin = yggdrasil::munin::point(10, 20);
    auto size = yggdrasil::munin::extent(30, 40);
    auto rect = yggdrasil::munin::rectangle(origin, size);
    
    CPPUNIT_ASSERT_EQUAL(origin, rect.origin);
    CPPUNIT_ASSERT_EQUAL(size, rect.size);
}

void yggdrasil_munin_rectangle_fixture::test_point_plus_extent_constructor()
{
    auto origin = yggdrasil::munin::point(10, 20);
    auto size = yggdrasil::munin::extent(30, 40);
    auto rect = origin + size;
    
    CPPUNIT_ASSERT_EQUAL(origin, rect.origin);
    CPPUNIT_ASSERT_EQUAL(size, rect.size);
}

yggdrasil::munin::rectangle return_rectangle(yggdrasil::munin::rectangle rect)
{
    return rect;
}

void yggdrasil_munin_rectangle_fixture::test_initializer_list_constructor()
{
    auto origin = yggdrasil::munin::point(10, 20);
    auto size = yggdrasil::munin::extent(30, 40);
    
    auto rect = return_rectangle({{10, 20}, {30, 40}});
    CPPUNIT_ASSERT_EQUAL(origin, rect.origin);
    CPPUNIT_ASSERT_EQUAL(size, rect.size);
}

void yggdrasil_munin_rectangle_fixture::test_equality_operator()
{
    auto lhs = yggdrasil::munin::rectangle({10, 20}, {30, 40});
    auto rhs = yggdrasil::munin::rectangle({50, 60}, {70, 80});
    
    CPPUNIT_ASSERT(lhs == lhs);
    CPPUNIT_ASSERT(!(lhs == rhs));
}

void yggdrasil_munin_rectangle_fixture::test_inequality_operator()
{
    auto lhs = yggdrasil::munin::rectangle({10, 20}, {30, 40});
    auto rhs = yggdrasil::munin::rectangle({50, 60}, {70, 80});
    
    CPPUNIT_ASSERT(lhs != rhs);
    CPPUNIT_ASSERT(!(lhs != lhs));
}

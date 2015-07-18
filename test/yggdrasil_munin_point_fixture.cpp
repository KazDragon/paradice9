#include "yggdrasil_munin_point_fixture.hpp"
#include "yggdrasil/munin/point.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(yggdrasil_munin_point_fixture);

void yggdrasil_munin_point_fixture::test_default_constructor()
{
    yggdrasil::munin::point pt;
    CPPUNIT_ASSERT_EQUAL(0, pt.x);
    CPPUNIT_ASSERT_EQUAL(0, pt.y);
}

void yggdrasil_munin_point_fixture::test_xy_constructor()
{
    yggdrasil::munin::point pt(7, 8);
    CPPUNIT_ASSERT_EQUAL(7, pt.x);
    CPPUNIT_ASSERT_EQUAL(8, pt.y);
}

static yggdrasil::munin::point return_point(yggdrasil::munin::point pt)
{ 
    return pt; 
}

void yggdrasil_munin_point_fixture::test_initializer_list_constructor()
{
    auto pt = return_point({7, 8});
    CPPUNIT_ASSERT_EQUAL(7, pt.x);
    CPPUNIT_ASSERT_EQUAL(8, pt.y);
}

void yggdrasil_munin_point_fixture::test_increment_operator()
{
    yggdrasil::munin::point lhs(5, 10);
    yggdrasil::munin::point rhs(16, 21);
    
    lhs += rhs;
    
    CPPUNIT_ASSERT_EQUAL(21, lhs.x);
    CPPUNIT_ASSERT_EQUAL(31, lhs.y);
    
    CPPUNIT_ASSERT_EQUAL(16, rhs.x);
    CPPUNIT_ASSERT_EQUAL(21, rhs.y);
}

void yggdrasil_munin_point_fixture::test_decrement_operator()
{
    yggdrasil::munin::point lhs(5, 10);
    yggdrasil::munin::point rhs(16, 21);
    
    lhs -= rhs;
    
    CPPUNIT_ASSERT_EQUAL(-11, lhs.x);
    CPPUNIT_ASSERT_EQUAL(-11, lhs.y);
    
    CPPUNIT_ASSERT_EQUAL(16, rhs.x);
    CPPUNIT_ASSERT_EQUAL(21, rhs.y);
}

void yggdrasil_munin_point_fixture::test_equality_operator()
{
    yggdrasil::munin::point lhs(5, 10);
    yggdrasil::munin::point rhs(16, 21);
    
    CPPUNIT_ASSERT(lhs == lhs);
    CPPUNIT_ASSERT(!(lhs == rhs));
}

void yggdrasil_munin_point_fixture::test_inequality_operator()
{
    yggdrasil::munin::point lhs(5, 10);
    yggdrasil::munin::point rhs(16, 21);
    
    CPPUNIT_ASSERT(lhs != rhs);
    CPPUNIT_ASSERT(!(lhs != lhs));
}

void yggdrasil_munin_point_fixture::test_operator_plus()
{
    yggdrasil::munin::point lhs(5, 10);
    yggdrasil::munin::point rhs(16, 21);
    
    yggdrasil::munin::point result = lhs + rhs;
    
    CPPUNIT_ASSERT(result == yggdrasil::munin::point(21, 31));
}

void yggdrasil_munin_point_fixture::test_operator_minus()
{
    yggdrasil::munin::point lhs(5, 10);
    yggdrasil::munin::point rhs(16, 21);
    
    yggdrasil::munin::point result = lhs - rhs;
    
    CPPUNIT_ASSERT(result == yggdrasil::munin::point(-11, -11));
}

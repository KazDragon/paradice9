#include "yggdrasil_munin_extent_fixture.hpp"
#include "yggdrasil/munin/extent.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(yggdrasil_munin_extent_fixture);

void yggdrasil_munin_extent_fixture::test_default_constructor()
{
    yggdrasil::munin::extent ext;
    CPPUNIT_ASSERT_EQUAL(0, ext.width);
    CPPUNIT_ASSERT_EQUAL(0, ext.height);
}

void yggdrasil_munin_extent_fixture::test_wh_constructor()
{
    yggdrasil::munin::extent ext(7, 8);
    CPPUNIT_ASSERT_EQUAL(7, ext.width);
    CPPUNIT_ASSERT_EQUAL(8, ext.height);
}

static yggdrasil::munin::extent return_extent(yggdrasil::munin::extent e)
{ 
    return e; 
}

void yggdrasil_munin_extent_fixture::test_initializer_list_constructor()
{
    auto ext = return_extent({7, 8});
    CPPUNIT_ASSERT_EQUAL(7, ext.width);
    CPPUNIT_ASSERT_EQUAL(8, ext.height);
}

void yggdrasil_munin_extent_fixture::test_increment_operator()
{
    yggdrasil::munin::extent lhs(5, 10);
    yggdrasil::munin::extent rhs(16, 21);
    
    lhs += rhs;
    
    CPPUNIT_ASSERT_EQUAL(21, lhs.width);
    CPPUNIT_ASSERT_EQUAL(31, lhs.height);
    
    CPPUNIT_ASSERT_EQUAL(16, rhs.width);
    CPPUNIT_ASSERT_EQUAL(21, rhs.height);
}

void yggdrasil_munin_extent_fixture::test_decrement_operator()
{
    yggdrasil::munin::extent lhs(5, 10);
    yggdrasil::munin::extent rhs(16, 21);
    
    lhs -= rhs;
    
    CPPUNIT_ASSERT_EQUAL(-11, lhs.width);
    CPPUNIT_ASSERT_EQUAL(-11, lhs.height);
    
    CPPUNIT_ASSERT_EQUAL(16, rhs.width);
    CPPUNIT_ASSERT_EQUAL(21, rhs.height);
}

void yggdrasil_munin_extent_fixture::test_equality_operator()
{
    yggdrasil::munin::extent lhs(5, 10);
    yggdrasil::munin::extent rhs(16, 21);
    
    CPPUNIT_ASSERT(lhs == lhs);
    CPPUNIT_ASSERT(!(lhs == rhs));
}

void yggdrasil_munin_extent_fixture::test_inequality_operator()
{
    yggdrasil::munin::extent lhs(5, 10);
    yggdrasil::munin::extent rhs(16, 21);
    
    CPPUNIT_ASSERT(lhs != rhs);
    CPPUNIT_ASSERT(!(lhs != lhs));
}

void yggdrasil_munin_extent_fixture::test_operator_plus()
{
    yggdrasil::munin::extent lhs(5, 10);
    yggdrasil::munin::extent rhs(16, 21);
    
    yggdrasil::munin::extent result = lhs + rhs;
    
    CPPUNIT_ASSERT(result == yggdrasil::munin::extent(21, 31));
}

void yggdrasil_munin_extent_fixture::test_operator_minus()
{
    yggdrasil::munin::extent lhs(5, 10);
    yggdrasil::munin::extent rhs(16, 21);
    
    yggdrasil::munin::extent result = lhs - rhs;
    
    CPPUNIT_ASSERT(result == yggdrasil::munin::extent(-11, -11));
}

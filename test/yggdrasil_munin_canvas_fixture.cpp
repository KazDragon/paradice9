#include "yggdrasil_munin_canvas_fixture.hpp"
#include "yggdrasil/munin/canvas.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(yggdrasil_munin_canvas_fixture);

void yggdrasil_munin_canvas_fixture::test_default_constructor()
{
    auto cvs = yggdrasil::munin::canvas();
    
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::munin::extent(0, 0),
        cvs.get_size());
}

void yggdrasil_munin_canvas_fixture::test_size_constructor()
{
    auto cvs = yggdrasil::munin::canvas(
        yggdrasil::munin::extent(1, 2));
    
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::munin::extent(1, 2),
        cvs.get_size());
}

void yggdrasil_munin_canvas_fixture::test_set_size()
{
    auto cvs = yggdrasil::munin::canvas();
    cvs.set_size(yggdrasil::munin::extent(3, 4));
    
    CPPUNIT_ASSERT_EQUAL(
        yggdrasil::munin::extent(3, 4),
        cvs.get_size());
}

void yggdrasil_munin_canvas_fixture::test_brackets_operator()
{
    auto cvs = yggdrasil::munin::canvas({2, 2});
    auto default_elem = yggdrasil::munin::element{};
    
    CPPUNIT_ASSERT_EQUAL(default_elem, cvs[0][0]);
    CPPUNIT_ASSERT_EQUAL(default_elem, cvs[0][1]);
    CPPUNIT_ASSERT_EQUAL(default_elem, cvs[1][0]);
    CPPUNIT_ASSERT_EQUAL(default_elem, cvs[1][1]);
    
    auto x_elem = yggdrasil::munin::element('X');
    cvs[0][0] = x_elem;
    
    auto y_elem = yggdrasil::munin::element('Y');
    cvs[1][0] = y_elem;
    
    auto z_elem = yggdrasil::munin::element('Z');
    cvs[0][1] = z_elem;
    
    auto w_elem = yggdrasil::munin::element('W');
    cvs[1][1] = w_elem;
    
    CPPUNIT_ASSERT_EQUAL(x_elem, cvs[0][0]);
    CPPUNIT_ASSERT_EQUAL(y_elem, cvs[1][0]);
    CPPUNIT_ASSERT_EQUAL(z_elem, cvs[0][1]);
    CPPUNIT_ASSERT_EQUAL(w_elem, cvs[1][1]);
}

void yggdrasil_munin_canvas_fixture::test_offset_by()
{
    auto cvs = yggdrasil::munin::canvas({4, 3});
    cvs[2][1] = 'X';

    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::extent(0, 0), cvs.get_offset());
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element(), cvs[0][0]);
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element('X'), cvs[2][1]);
    
    cvs.offset_by({2, 1});
    
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::extent(2, 1), cvs.get_offset());
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::extent(4, 3), cvs.get_size());
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element(), cvs[-2][-1]);
    CPPUNIT_ASSERT_EQUAL(yggdrasil::munin::element('X'), cvs[0][0]);
}

#ifndef YGGDRASIL_MUNIN_FILLED_BOX_FIXTURE_HPP_
#define YGGDRASIL_MUNIN_FILLED_BOX_FIXTURE_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class yggdrasil_munin_filled_box_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(yggdrasil_munin_filled_box_fixture);
        CPPUNIT_TEST(test_default_constructor);
    CPPUNIT_TEST_SUITE_END();

public :
    void test_default_constructor();
};

#endif

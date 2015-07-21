#ifndef YGGDRASIL_MUNIN_IMAGE_FIXTURE_HPP_
#define YGGDRASIL_MUNIN_IMAGE_FIXTURE_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class yggdrasil_munin_image_fixture : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(yggdrasil_munin_image_fixture);
        CPPUNIT_TEST(image_with_no_args_provides_default_image);
    CPPUNIT_TEST_SUITE_END();
    
public :
    void image_with_no_args_provides_default_image();
};

#endif

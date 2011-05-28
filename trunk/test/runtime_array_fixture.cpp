#include "runtime_array_fixture.hpp"
#include "odin/small_buffer.hpp"
#include "odin/runtime_array.hpp"
#include <sstream>

CPPUNIT_TEST_SUITE_REGISTRATION(runtime_array_fixture);

using namespace std;
using namespace boost;

typedef odin::runtime_array<odin::u8> default_runtime_array;

void runtime_array_fixture::test_container_concept()
{
    // Test typedefs:
    typedef default_runtime_array::value_type      value_type;
    typedef default_runtime_array::iterator        iterator;
    typedef default_runtime_array::const_iterator  const_iterator;
    typedef default_runtime_array::reference       reference;
    typedef default_runtime_array::const_reference const_reference;
    typedef default_runtime_array::pointer         pointer;
    typedef default_runtime_array::difference_type difference_type;
    typedef default_runtime_array::size_type       size_type;

    // Test Container functions:
    default_runtime_array       array1;
    default_runtime_array       array2;
    default_runtime_array const const_array;

    // Test begin() exists
    iterator(array1.begin());
    const_iterator(const_array.begin());

    // Test end() exists
    iterator(array1.end());
    const_iterator(const_array.end());

    // Test size() exists
    array1.size();
    const_array.size();

    // Test max_size() exists.
    array1.max_size();
    const_array.max_size();

    // Test empty() exists.
    array1.empty();
    const_array.empty();

    // Test swap() exists.
    array1.swap(array2);

    // Test LessThanComparable concept
    array1 < array2;

    // Test EqualityComparable concept
    array1 == array2;
    array1 != array2;

    // Test Assignable concept
    array1 = array2;
}

void runtime_array_fixture::test_default_constructor()
{
    odin::runtime_array<odin::u8> array;
    CPPUNIT_ASSERT_EQUAL(odin::runtime_array<odin::u8>::size_type(0), array.size());
}

void runtime_array_fixture::test_null_construction()
{
    odin::runtime_array<odin::u8> array(NULL, 0);
    CPPUNIT_ASSERT_EQUAL(odin::runtime_array<odin::u8>::size_type(0), array.size());
}

void runtime_array_fixture::test_sized_array_construction()
{
    odin::u8  data[] = {0x00, 0x01, 0x02};
    odin::runtime_array<odin::u8>::size_type array_size = sizeof(data) / sizeof(*data);
    odin::runtime_array<odin::u8> array(data, array_size);

    CPPUNIT_ASSERT_EQUAL(array_size, array.size());
    CPPUNIT_ASSERT_EQUAL(data[0], array[0]);
    CPPUNIT_ASSERT_EQUAL(data[1], array[1]);
    CPPUNIT_ASSERT_EQUAL(data[2], array[2]);
}

void runtime_array_fixture::test_size_construction()
{
    odin::runtime_array<odin::u8> array(odin::runtime_array<odin::u8>::size_type(4));
    CPPUNIT_ASSERT_EQUAL(odin::runtime_array<odin::u8>::size_type(4), array.size());
}

void runtime_array_fixture::test_array_construction()
{
    odin::u8  data[] = {0x00, 0x01, 0x02};
    odin::runtime_array<odin::u8> array(data);

    CPPUNIT_ASSERT_EQUAL(sizeof(data) / sizeof(*data), array.size());
    CPPUNIT_ASSERT_EQUAL(data[0], array[0]);
    CPPUNIT_ASSERT_EQUAL(data[1], array[1]);
    CPPUNIT_ASSERT_EQUAL(data[2], array[2]);
}

void runtime_array_fixture::test_construct_different_storage()
{
    odin::u8 array[] = {0x00, 0x01, 0x02};
    odin::runtime_array< odin::u8, odin::small_buffer<odin::u8, 8> > array0(array);
    odin::runtime_array< odin::u8, odin::small_buffer<odin::u8, 2> > array1(array0);

    CPPUNIT_ASSERT(array0 == array1);
}

void runtime_array_fixture::test_carray()
{
    odin::u8  data[] = {0x00, 0x01, 0x02};
    odin::runtime_array<odin::u8> array(data);

    CPPUNIT_ASSERT_EQUAL(sizeof(data) / sizeof(*data), array.size());
    CPPUNIT_ASSERT_EQUAL(data[0], array[0]);
    CPPUNIT_ASSERT_EQUAL(data[1], array[1]);
    CPPUNIT_ASSERT_EQUAL(data[2], array[2]);

    odin::u8* pdata = array.c_array();
    CPPUNIT_ASSERT_EQUAL(array[0], pdata[0]);
    CPPUNIT_ASSERT_EQUAL(array[1], pdata[1]);
    CPPUNIT_ASSERT_EQUAL(array[2], pdata[2]);

    pdata[1] = 0xFF;

    CPPUNIT_ASSERT_EQUAL(odin::runtime_array<odin::u8>::value_type(0xFF), array[1]);
}

void runtime_array_fixture::test_equality()
{
    odin::u8 array[] = {0x00, 0x01, 0x02};
    odin::runtime_array<odin::u8> array0(array);
    odin::runtime_array<odin::u8> array1(array);

    CPPUNIT_ASSERT(array0 == array1);

    array0[1]++;

    CPPUNIT_ASSERT(!(array0 == array1));
}

void runtime_array_fixture::test_inequality()
{
    odin::u8 array[] = {0x00, 0x01, 0x02};
    odin::runtime_array<odin::u8> array0(array);
    odin::runtime_array<odin::u8> array1(array);

    CPPUNIT_ASSERT(!(array0 != array1));

    array0[1]++;

    CPPUNIT_ASSERT(array0 != array1);
}

void runtime_array_fixture::test_copy_constructor()
{
    odin::u8 array[] = {0x00, 0x01, 0x02};
    odin::runtime_array<odin::u8> array0(array);
    odin::runtime_array<odin::u8> array1(array0);

    CPPUNIT_ASSERT(array0 == array1);

    array0[1]++;

    CPPUNIT_ASSERT(array0 != array1);
}

void runtime_array_fixture::test_swap()
{
    odin::u8 data0[] = {0x00, 0x01, 0x02};
    odin::u8 data1[] = {0xFF, 0xFE, 0xFD};

    odin::runtime_array<odin::u8> original_array0(data0);
    odin::runtime_array<odin::u8> original_array1(data1);

    odin::runtime_array<odin::u8> array0(original_array0);
    odin::runtime_array<odin::u8> array1(original_array1);

    CPPUNIT_ASSERT(array0 == original_array0);
    CPPUNIT_ASSERT(array1 == original_array1);
    CPPUNIT_ASSERT(array0 != array1);

    array0.swap(array1);

    CPPUNIT_ASSERT(array0 == original_array1);
    CPPUNIT_ASSERT(array1 == original_array0);
    CPPUNIT_ASSERT(array0 != array1);
}

void runtime_array_fixture::test_assignment()
{
    odin::u8 array[] = {0x00, 0x01, 0x02};
    odin::runtime_array<odin::u8> array0(array);
    odin::runtime_array<odin::u8> array1;

    CPPUNIT_ASSERT(!(array0 == array1));

    array1 = array0;

    CPPUNIT_ASSERT(array0 == array1);

    array0[1]++;

    CPPUNIT_ASSERT(!(array0 == array1));
}

void runtime_array_fixture::test_concatenation()
{
    odin::u8 array0[] = {0x00, 0x01};
    odin::u8 array1[] = {0x02, 0x03};
    
    odin::runtime_array<odin::u8> rarray0(array0);
    odin::runtime_array<odin::u8> rarray1(array1);
    
    odin::u8 expected_array[] = {0x00, 0x01, 0x02, 0x03};
    odin::runtime_array<odin::u8> expected_rarray(expected_array);
    
    CPPUNIT_ASSERT(expected_rarray == rarray0 + rarray1);
    
    rarray0 += rarray1;
    CPPUNIT_ASSERT(expected_rarray == rarray0);
}

void runtime_array_fixture::test_vector_storage()
{
    odin::u8 array[] = {0x00, 0x01, 0x02};

    // Test empty constructor.
    odin::runtime_array< odin::u8, vector<odin::u8> > array0;

    // Test size constructor.
    odin::runtime_array< odin::u8, vector<odin::u8> > array1(3);
    copy(array, array + sizeof(array), array1.begin());

    // Test sized array constructor
    odin::runtime_array< odin::u8, vector<odin::u8> > array2(array, sizeof(array));
    
    // Test that Runtime Arrays with different storages may be compared with
    // each other.
    CPPUNIT_ASSERT(array2 == odin::runtime_array<odin::u8>(array));
    CPPUNIT_ASSERT(!(array2 != odin::runtime_array<odin::u8>(array)));
}

void runtime_array_fixture::test_stream_out()
{
    odin::u8 data[] = { 0x01, 0x02, 0x03, 0x04 };
    odin::runtime_array<odin::u8> array(data);

    std::stringstream stream;
    stream << array;

    CPPUNIT_ASSERT_EQUAL(string("[1,2,3,4]"), stream.str());
}


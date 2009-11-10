#include "small_buffer_test.hpp"
#include "odin/small_buffer.hpp"
#include "fake_allocator.hpp"
#include <boost/iterator/counting_iterator.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(small_buffer_fixture);

using namespace std;
using namespace boost;

static const unsigned int static_data_size = 8;

typedef odin::fake_allocator<unsigned char> default_small_allocator;

typedef odin::small_buffer
<
    unsigned char
  , static_data_size
  , default_small_allocator
> default_small_buffer;

void small_buffer_fixture::test_container_concept()
{
    // Test typedefs:
    typedef default_small_buffer::value_type      value_type;
    typedef default_small_buffer::iterator        iterator;
    typedef default_small_buffer::const_iterator  const_iterator;
    typedef default_small_buffer::reference       reference;
    typedef default_small_buffer::const_reference const_reference;
    typedef default_small_buffer::pointer         pointer;
    typedef default_small_buffer::difference_type difference_type;
    typedef default_small_buffer::size_type       size_type;

    // Test Container functions:
    default_small_buffer       buffer1;
    default_small_buffer       buffer2;
    default_small_buffer const const_buffer;

    // Test begin() exists
    iterator(buffer1.begin());
    const_iterator(const_buffer.begin());

    // Test end() exists
    iterator(buffer1.end());
    const_iterator(const_buffer.end());

    // Test size() exists
    buffer1.size();
    const_buffer.size();

    // Test max_size() exists.
    buffer1.max_size();
    const_buffer.max_size();

    // Test empty() exists.
    buffer1.empty();
    const_buffer.empty();

    // Test swap() exists.
    buffer1.swap(buffer2);

    // Test LessThanComparable concept
    buffer1 < buffer2;

    // Test EqualityComparable concept
    buffer1 == buffer2;
    buffer1 != buffer2;

    // Test Assignable concept
    buffer1 = buffer2;
}

void small_buffer_fixture::test_default_constructor()
{
    default_small_allocator::reset();
    
    default_small_buffer buffer;
    CPPUNIT_ASSERT_EQUAL(size_t(0), buffer.size());
    CPPUNIT_ASSERT_EQUAL(size_t(0), buffer.max_size());
    CPPUNIT_ASSERT_EQUAL(true, buffer.empty());
    
    CPPUNIT_ASSERT_EQUAL(size_t(0), default_small_allocator::allocation_count_);
}

void small_buffer_fixture::test_size_constructor_short()
{
    default_small_allocator::reset();
    
    default_small_buffer buffer(static_data_size);
    CPPUNIT_ASSERT_EQUAL(size_t(static_data_size), buffer.size());
    CPPUNIT_ASSERT_EQUAL(size_t(static_data_size), buffer.max_size());
    CPPUNIT_ASSERT_EQUAL(false, buffer.empty());
    
    CPPUNIT_ASSERT_EQUAL(size_t(0), default_small_allocator::allocation_count_);
}

void small_buffer_fixture::test_size_constructor_long()
{
    default_small_allocator::reset();
    
    default_small_buffer buffer(static_data_size + 1);
    CPPUNIT_ASSERT_EQUAL(size_t(static_data_size + 1), buffer.size());
    CPPUNIT_ASSERT_EQUAL(size_t(static_data_size + 1), buffer.max_size());
    CPPUNIT_ASSERT_EQUAL(false, buffer.empty());
    
    CPPUNIT_ASSERT_EQUAL(size_t(1), default_small_allocator::allocation_count_);
}

void small_buffer_fixture::test_static_array_constructor_short()
{
    unsigned char array[static_data_size];
    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size),
        array);

    default_small_buffer buffer(array);
    CPPUNIT_ASSERT_EQUAL(size_t(static_data_size), buffer.size());
    CPPUNIT_ASSERT_EQUAL(size_t(static_data_size), buffer.max_size());
    CPPUNIT_ASSERT_EQUAL(false, buffer.empty());

    for (unsigned char index = 0; index < buffer.size(); ++index)
    {
        CPPUNIT_ASSERT_EQUAL(index, buffer[index]);
    }
}

void small_buffer_fixture::test_static_array_constructor_long()
{
    unsigned char array[static_data_size + 1];
    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size + 1),
        array);

    default_small_buffer buffer(array);
    CPPUNIT_ASSERT_EQUAL(size_t(static_data_size + 1), buffer.size());
    CPPUNIT_ASSERT_EQUAL(size_t(static_data_size + 1), buffer.max_size());
    CPPUNIT_ASSERT_EQUAL(false, buffer.empty());

    for (unsigned char index = 0; index < buffer.size(); ++index)
    {
        CPPUNIT_ASSERT_EQUAL(index, buffer[index]);
    }
}

void small_buffer_fixture::test_sized_array_constructor_short()
{
    unsigned char array[static_data_size];
    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size),
        array);

    default_small_buffer buffer(array, static_data_size);
    CPPUNIT_ASSERT_EQUAL(default_small_buffer::size_type(static_data_size), buffer.size());
    CPPUNIT_ASSERT_EQUAL(default_small_buffer::size_type(static_data_size), buffer.max_size());
    CPPUNIT_ASSERT_EQUAL(false, buffer.empty());

    for (default_small_buffer::size_type index = 0; index < buffer.size(); ++index)
    {
        CPPUNIT_ASSERT_EQUAL(array[index], buffer[index]);
    }
}

void small_buffer_fixture::test_sized_array_constructor_long()
{
    unsigned char array[static_data_size + 1];
    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size + 1),
        array);

    default_small_buffer buffer(array, static_data_size + 1);
    CPPUNIT_ASSERT_EQUAL(default_small_buffer::size_type(static_data_size + 1), buffer.size());
    CPPUNIT_ASSERT_EQUAL(default_small_buffer::size_type(static_data_size + 1), buffer.max_size());
    CPPUNIT_ASSERT_EQUAL(false, buffer.empty());

    for (default_small_buffer::size_type index = 0; index < buffer.size(); ++index)
    {
        CPPUNIT_ASSERT_EQUAL(array[index], buffer[index]);
    }
}

void small_buffer_fixture::test_assignment_via_array_operator_short()
{
    unsigned char array[static_data_size];
    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size),
        array);

    default_small_buffer buffer(array);
    CPPUNIT_ASSERT_EQUAL(size_t(static_data_size), buffer.size());

    for (unsigned char index = 0; index < buffer.size(); ++index)
    {
        CPPUNIT_ASSERT_EQUAL(index, buffer[index]);
        buffer[index] = buffer[index] + 1;
    }

    for (unsigned char index = 0; index < buffer.size(); ++index)
    {
        CPPUNIT_ASSERT_EQUAL((unsigned char)(index + 1), buffer[index]);
    }
}

void small_buffer_fixture::test_assignment_via_array_operator_long()
{
    unsigned char array[static_data_size + 1];
    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size + 1),
        array);

    default_small_buffer buffer(array);
    CPPUNIT_ASSERT_EQUAL(size_t(static_data_size + 1), buffer.size());

    for (unsigned char index = 0; index < buffer.size(); ++index)
    {
        CPPUNIT_ASSERT_EQUAL(index, buffer[index]);
        buffer[index] = buffer[index] + 1;
    }

    for (unsigned char index = 0; index < buffer.size(); ++index)
    {
        CPPUNIT_ASSERT_EQUAL((unsigned char)(index + 1), buffer[index]);
    }
}

void small_buffer_fixture::test_equality_short()
{
    unsigned char array0[static_data_size];
    unsigned char array1[static_data_size];

    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size),
        array0);

    copy(
        counting_iterator<unsigned char>(1),
        counting_iterator<unsigned char>(static_data_size + 1),
        array1);

    default_small_buffer buffer0(array0);
    default_small_buffer buffer1(array0);
    default_small_buffer buffer2(array1);

    CPPUNIT_ASSERT(buffer0 == buffer1);
    CPPUNIT_ASSERT(!(buffer0 == buffer2));
    CPPUNIT_ASSERT(!(buffer2 == buffer1));

    CPPUNIT_ASSERT(!(buffer0 == default_small_buffer()));
}

void small_buffer_fixture::test_equality_long()
{
    unsigned char array0[static_data_size + 1];
    unsigned char array1[static_data_size + 1];

    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size + 1),
        array0);

    copy(
        counting_iterator<unsigned char>(1),
        counting_iterator<unsigned char>(static_data_size + 2),
        array1);

    default_small_buffer buffer0(array0);
    default_small_buffer buffer1(array0);
    default_small_buffer buffer2(array1);

    CPPUNIT_ASSERT(buffer0 == buffer1);
    CPPUNIT_ASSERT(!(buffer0 == buffer2));
    CPPUNIT_ASSERT(!(buffer2 == buffer1));

    CPPUNIT_ASSERT(!(buffer0 == default_small_buffer()));
}

void small_buffer_fixture::test_inequality_short()
{
    unsigned char array0[static_data_size];
    unsigned char array1[static_data_size];

    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size),
        array0);

    copy(
        counting_iterator<unsigned char>(1),
        counting_iterator<unsigned char>(static_data_size + 1),
        array1);

    default_small_buffer buffer0(array0);
    default_small_buffer buffer1(array0);
    default_small_buffer buffer2(array1);

    CPPUNIT_ASSERT(!(buffer0 != buffer1));
    CPPUNIT_ASSERT(buffer0 != buffer2);
    CPPUNIT_ASSERT(buffer2 != buffer1);

    CPPUNIT_ASSERT(buffer0 != default_small_buffer());
}

void small_buffer_fixture::test_inequality_long()
{
    unsigned char array0[static_data_size + 1];
    unsigned char array1[static_data_size + 1];

    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size + 1),
        array0);

    copy(
        counting_iterator<unsigned char>(1),
        counting_iterator<unsigned char>(static_data_size + 2),
        array1);

    default_small_buffer buffer0(array0);
    default_small_buffer buffer1(array0);
    default_small_buffer buffer2(array1);

    CPPUNIT_ASSERT(!(buffer0 != buffer1));
    CPPUNIT_ASSERT(buffer0 != buffer2);
    CPPUNIT_ASSERT(buffer2 != buffer1);

    CPPUNIT_ASSERT(buffer0 != default_small_buffer());
}

void small_buffer_fixture::test_less_than()
{
    // Test when both arrays are the same size:
    {
        unsigned char array0[static_data_size];
        unsigned char array1[static_data_size];

        copy(
            counting_iterator<unsigned char>(0),
            counting_iterator<unsigned char>(static_data_size),
            array0);

        copy(
            counting_iterator<unsigned char>(0),
            counting_iterator<unsigned char>(static_data_size),
            array1);

        default_small_buffer buffer0(array0);
        default_small_buffer buffer1(array1);

        // Test when both have the same data
        CPPUNIT_ASSERT_EQUAL(false, buffer0 < buffer1);

        // Test when a value in the LHS is smaller
        buffer0[1]--;

        CPPUNIT_ASSERT_EQUAL(true, buffer0 < buffer1);

        // Test when a value in the LHS is greater
        CPPUNIT_ASSERT_EQUAL(false, buffer1 < buffer0);
    }

    // Test when the left array is smaller
    {
        unsigned char array0[static_data_size - 1];
        unsigned char array1[static_data_size];

        copy(
            counting_iterator<unsigned char>(0),
            counting_iterator<unsigned char>(static_data_size - 1),
            array0);

        copy(
            counting_iterator<unsigned char>(0),
            counting_iterator<unsigned char>(static_data_size),
            array1);

        default_small_buffer buffer0(array0);
        default_small_buffer buffer1(array1);

        CPPUNIT_ASSERT_EQUAL(true, buffer0 < buffer1);
    }

    // Test when the left array is larger
    {
        unsigned char array0[static_data_size];
        unsigned char array1[static_data_size - 1];

        copy(
            counting_iterator<unsigned char>(0),
            counting_iterator<unsigned char>(static_data_size),
            array0);

        copy(
            counting_iterator<unsigned char>(0),
            counting_iterator<unsigned char>(static_data_size - 1),
            array1);

        default_small_buffer buffer0(array0);
        default_small_buffer buffer1(array1);

        CPPUNIT_ASSERT_EQUAL(false, buffer0 < buffer1);
    }
}

void small_buffer_fixture::test_copy_constructor_short()
{
    unsigned char array[static_data_size];
    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size),
        array);

    default_small_buffer buffer0(array);
    CPPUNIT_ASSERT_EQUAL(size_t(static_data_size), buffer0.size());

    default_small_buffer buffer1(buffer0);
    CPPUNIT_ASSERT_EQUAL(size_t(static_data_size), buffer1.size());

    for (unsigned char index = 0; index < buffer0.size(); ++index)
    {
        CPPUNIT_ASSERT_EQUAL(buffer0[index], buffer1[index]);
    }

    buffer1[0]++;

    CPPUNIT_ASSERT(buffer0[0] != buffer1[0]);
}

void small_buffer_fixture::test_copy_constructor_long()
{
    unsigned char array[static_data_size + 1];
    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size + 1),
        array);

    default_small_buffer buffer0(array);
    CPPUNIT_ASSERT_EQUAL(size_t(static_data_size + 1), buffer0.size());

    default_small_buffer buffer1(buffer0);
    CPPUNIT_ASSERT_EQUAL(size_t(static_data_size + 1), buffer1.size());

    for (unsigned char index = 0; index < buffer0.size(); ++index)
    {
        CPPUNIT_ASSERT_EQUAL(buffer0[index], buffer1[index]);
    }

    buffer1[0]++;

    CPPUNIT_ASSERT(buffer0[0] != buffer1[0]);
}

void small_buffer_fixture::test_swap_short_to_short()
{
    unsigned char array0[static_data_size];
    unsigned char array1[static_data_size];

    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size),
        array0);

    copy(
        counting_iterator<unsigned char>(1),
        counting_iterator<unsigned char>(static_data_size + 1),
        array1);

    default_small_buffer oReferenceBuffer0(array0);
    default_small_buffer oReferenceBuffer1(array1);

    default_small_buffer buffer0(array0);
    default_small_buffer buffer1(array1);

    CPPUNIT_ASSERT(oReferenceBuffer0 == buffer0);
    CPPUNIT_ASSERT(oReferenceBuffer1 == buffer1);

    buffer0.swap(buffer1);

    CPPUNIT_ASSERT(oReferenceBuffer0 == buffer1);
    CPPUNIT_ASSERT(oReferenceBuffer1 == buffer0);
}

void small_buffer_fixture::test_swap_long_to_long()
{
    unsigned char array0[static_data_size + 1];
    unsigned char array1[static_data_size + 1];

    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size + 1),
        array0);

    copy(
        counting_iterator<unsigned char>(1),
        counting_iterator<unsigned char>(static_data_size + 2),
        array1);

    default_small_buffer oReferenceBuffer0(array0);
    default_small_buffer oReferenceBuffer1(array1);

    default_small_buffer buffer0(array0);
    default_small_buffer buffer1(array1);

    CPPUNIT_ASSERT(oReferenceBuffer0 == buffer0);
    CPPUNIT_ASSERT(oReferenceBuffer1 == buffer1);

    buffer0.swap(buffer1);

    CPPUNIT_ASSERT(oReferenceBuffer0 == buffer1);
    CPPUNIT_ASSERT(oReferenceBuffer1 == buffer0);
}

void small_buffer_fixture::test_swap_short_to_long()
{
    unsigned char array0[static_data_size];
    unsigned char array1[static_data_size + 1];

    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size),
        array0);

    copy(
        counting_iterator<unsigned char>(1),
        counting_iterator<unsigned char>(static_data_size + 2),
        array1);

    default_small_buffer oReferenceBuffer0(array0);
    default_small_buffer oReferenceBuffer1(array1);

    default_small_buffer buffer0(array0);
    default_small_buffer buffer1(array1);

    CPPUNIT_ASSERT(oReferenceBuffer0 == buffer0);
    CPPUNIT_ASSERT(oReferenceBuffer1 == buffer1);

    buffer0.swap(buffer1);

    CPPUNIT_ASSERT(oReferenceBuffer0 == buffer1);
    CPPUNIT_ASSERT(oReferenceBuffer1 == buffer0);
}

void small_buffer_fixture::test_swap_long_to_short()
{
    unsigned char array0[static_data_size + 1];
    unsigned char array1[static_data_size];

    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size + 1),
        array0);

    copy(
        counting_iterator<unsigned char>(1),
        counting_iterator<unsigned char>(static_data_size + 1),
        array1);

    default_small_buffer oReferenceBuffer0(array0);
    default_small_buffer oReferenceBuffer1(array1);

    default_small_buffer buffer0(array0);
    default_small_buffer buffer1(array1);

    CPPUNIT_ASSERT(oReferenceBuffer0 == buffer0);
    CPPUNIT_ASSERT(oReferenceBuffer1 == buffer1);

    buffer0.swap(buffer1);

    CPPUNIT_ASSERT(oReferenceBuffer0 == buffer1);
    CPPUNIT_ASSERT(oReferenceBuffer1 == buffer0);
}

void small_buffer_fixture::test_assignment_short_to_short()
{
    unsigned char array0[static_data_size];
    unsigned char array1[static_data_size];

    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size),
        array0);

    copy(
        counting_iterator<unsigned char>(1),
        counting_iterator<unsigned char>(static_data_size + 1),
        array1);

    default_small_buffer buffer0(array0);
    default_small_buffer buffer1(array1);

    CPPUNIT_ASSERT(buffer0 != buffer1);
    buffer1 = buffer0;
    CPPUNIT_ASSERT(buffer0 == buffer1);
    buffer0[0]++;
    CPPUNIT_ASSERT(buffer0 != buffer1);
}

void small_buffer_fixture::test_assignment_long_to_short()
{
    unsigned char array0[static_data_size + 1];
    unsigned char array1[static_data_size];

    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size + 1),
        array0);

    copy(
        counting_iterator<unsigned char>(1),
        counting_iterator<unsigned char>(static_data_size + 1),
        array1);

    default_small_buffer buffer0(array0);
    default_small_buffer buffer1(array1);

    CPPUNIT_ASSERT(buffer0 != buffer1);
    buffer1 = buffer0;
    CPPUNIT_ASSERT(buffer0 == buffer1);
    buffer0[0]++;
    CPPUNIT_ASSERT(buffer0 != buffer1);
}

void small_buffer_fixture::test_assignment_short_to_long()
{
    unsigned char array0[static_data_size];
    unsigned char array1[static_data_size + 1];

    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size),
        array0);

    copy(
        counting_iterator<unsigned char>(1),
        counting_iterator<unsigned char>(static_data_size + 2),
        array1);

    default_small_buffer buffer0(array0);
    default_small_buffer buffer1(array1);

    CPPUNIT_ASSERT(buffer0 != buffer1);
    buffer1 = buffer0;
    CPPUNIT_ASSERT(buffer0 == buffer1);
    buffer0[0]++;
    CPPUNIT_ASSERT(buffer0 != buffer1);
}

void small_buffer_fixture::test_assignment_long_to_long()
{
    unsigned char array0[static_data_size + 1];
    unsigned char array1[static_data_size + 1];

    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size + 1),
        array0);

    copy(
        counting_iterator<unsigned char>(1),
        counting_iterator<unsigned char>(static_data_size + 2),
        array1);

    default_small_buffer buffer0(array0);
    default_small_buffer buffer1(array1);

    CPPUNIT_ASSERT(buffer0 != buffer1);
    buffer1 = buffer0;
    CPPUNIT_ASSERT(buffer0 == buffer1);
    buffer0[0]++;
    CPPUNIT_ASSERT(buffer0 != buffer1);
}

void small_buffer_fixture::test_stl_algorithm_short()
{
    unsigned char array0[static_data_size];

    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size),
        array0);

    default_small_buffer buffer0(array0);
    default_small_buffer buffer1(buffer0.size());

    copy(buffer0.begin(), buffer0.end(), buffer1.begin());
}

void small_buffer_fixture::test_stl_algorithm_long()
{
    unsigned char array0[static_data_size + 1];

    copy(
        counting_iterator<unsigned char>(0),
        counting_iterator<unsigned char>(static_data_size + 1),
        array0);

    default_small_buffer buffer0(array0);
    default_small_buffer buffer1(buffer0.size());

    copy(buffer0.begin(), buffer0.end(), buffer1.begin());

    CPPUNIT_ASSERT(buffer0 == buffer1);

    default_small_buffer const kbuffer0(buffer0);
    
    copy(kbuffer0.begin(), kbuffer0.end(), buffer1.begin());

    CPPUNIT_ASSERT(kbuffer0 == buffer1);
}

void small_buffer_fixture::test_access_out_of_bounds()
{
    default_small_buffer buffer;
    CPPUNIT_ASSERT_THROW(buffer[0], std::range_error);

    default_small_buffer const& rkbuffer(buffer);
    CPPUNIT_ASSERT_THROW(rkbuffer[0], std::range_error);
}

void small_buffer_fixture::test_non_pod()
{
    // The standard string is a non-pod (Plain Old Data) class.  Non-pods 
    // cannot go into unions, which forces the Small Buffer to act differently.
    // This just tests that the dynamic data is created and destroyed correctly.
    typedef odin::fake_allocator<string> string_allocator;
    
    typedef odin::small_buffer
    <
        string
      , static_data_size
      , string_allocator
    > string_buffer;
    
    {
        string_allocator::reset();
        
        string_buffer array0;
        string_buffer array1(static_data_size);
        string_buffer array2(static_data_size + 1);
        
        CPPUNIT_ASSERT_EQUAL(size_t(2), string_allocator::allocation_count_);
        CPPUNIT_ASSERT_EQUAL(size_t(0), string_allocator::deallocation_count_);
    }
    
    // The allocation count is 2; one for each of the buffers that had data
    // in it.
    CPPUNIT_ASSERT_EQUAL(size_t(2), string_allocator::allocation_count_);
    
    // The deallocation count is 3, because even the empty buffer has its
    // NULL pointer deallocated.
    CPPUNIT_ASSERT_EQUAL(size_t(3), string_allocator::deallocation_count_);
}

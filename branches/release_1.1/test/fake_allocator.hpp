#ifndef ODIN_TEST_FAKE_ALLOCATOR_HPP_
#define ODIN_TEST_FAKE_ALLOCATOR_HPP_

#include <limits>

namespace odin {
    
template <class T>
class fake_allocator;

template <class T>
class fake_allocator
{
public :
    typedef size_t     size_type;
    typedef ptrdiff_t  difference_type;
    typedef T         *pointer;
    typedef T const   *const_pointer;
    typedef T         &reference;
    typedef T const   &const_reference;
    typedef T          value_type;
    
    template <class U>
    struct rebind
    {
        typedef fake_allocator<U> other;
    };
    
    pointer address(reference value) const
    {
        return &value;
    }
    
    const_pointer address(const_reference value) const
    {
        return &value;
    }
    
    size_type max_size() const throw()
    {
        return std::numeric_limits<size_t>::max() / sizeof(T);
    }
    
    pointer allocate(
        size_type  num
      , void      *hint = 0)
    {
        ++allocation_count_;
        last_allocation_size_ = num;
        return (pointer)(::operator new(num * sizeof(T)));
    }
    
    void construct(pointer p, const T &value)
    {
        new ((void*)p)T(value);
    }
    
    void destroy(pointer p)
    {
        p->~T();
    }
    
    void deallocate(pointer p, size_type num)
    {
        ++deallocation_count_;
        ::operator delete((void*)p);
    }
    
    // And now, statistics and stuff
    static unsigned int allocation_count_;
    static size_type    last_allocation_size_;
    static unsigned int deallocation_count_;
    
    static void reset()
    {
        allocation_count_     = 0;
        last_allocation_size_ = 0;
        deallocation_count_   = 0;
    }
    
};

template <class T1, class T2>
bool operator==(
    fake_allocator<T1> const&,
    fake_allocator<T2> const&)
{
    return true;
}

template <class T1, class T2>
bool operator!=(
    fake_allocator<T1> const&,
    fake_allocator<T2> const&)
{
    return false;
}

template <class ValueType>
unsigned int fake_allocator<ValueType>::allocation_count_;

template <class ValueType>
typename fake_allocator<ValueType>::size_type 
    fake_allocator<ValueType>::last_allocation_size_;

template <class ValueType>
unsigned int fake_allocator<ValueType>::deallocation_count_;

}

#endif

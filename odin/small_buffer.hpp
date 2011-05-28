// ==========================================================================
// Odin Small Buffer.
//
// Copyright (C) 2009 Matthew Chaplain, All Rights Reserved.  
// This file is covered by the MIT Licence:
//
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.
// ==========================================================================
#ifndef ODIN_SMALL_BUFFER_HPP_
#define ODIN_SMALL_BUFFER_HPP_

#include "odin/types.hpp"
#include <boost/format.hpp>
#include <boost/type_traits/is_pod.hpp>
#include <algorithm>
#include <memory>

namespace odin {

namespace small_buffer_detail
{
    // It is possible that the client code that is using a small_buffer<> is 
    // trying to contain a non-POD (Plain Old Data) type.  Non-POD types 
    // *cannot* be stored in a union.  Hence, we must specialise in that case
    // so that the buffer is always dynamically allocated.  In this case, the
    // small buffer optimization is not really applied, but it makes client
    // code that is attempting to use it simpler.
    template <class ValueType, odin::u32 StaticDataSize, bool IsPodType>
    struct small_buffer_union
        // Implements get_static_buffer()
        // Implements get_dynamic_buffer()
    ;
}

//* =========================================================================
/// \class small_buffer
/// \brief Implements a small buffer optimisation.
/// \param ValueType the type of the values that the small buffer contains.
/// \param StaticDataSize the size of the data that is considered "small".
/// \param Allocator an allocator for the buffer.
///
/// The small buffer optimization includes a union of both a small, statically 
/// allocated array and a dynamically allocated array.  The idea is that if 
/// small arrays are the common case, it will be cheaper to skip the memory
/// allocation and just use the statically allocated array instead.  However, 
/// the array must still be useful for larger types, where the memory must be 
/// allocated.  The class template small_buffer<> implements this optimisation.
//* =========================================================================
template 
<
    class  ValueType, 
    odin::u32 StaticDataSize,
    class  Allocator = std::allocator<ValueType> 
>
class small_buffer
{
    // A handy compile-time constant that looks to see whether ValueType
    // is a POD (Plain Old Data) type or not.
    static const bool is_pod_ = boost::is_pod<ValueType>::value;

public :
    // These are the typedefs that allow small_buffer<> to fulfil the Container
    // concept.
    typedef ValueType        value_type;
    typedef ValueType*       iterator;
    typedef ValueType const *const_iterator;
    typedef ValueType&       reference;
    typedef ValueType const &const_reference;
    typedef ValueType*       pointer;
    typedef std::ptrdiff_t   difference_type;
    typedef std::size_t      size_type;

    //* =====================================================================
    /// \brief Default constructor.
    ///
    /// Constructs a buffer of zero elements.
    //* =====================================================================
    small_buffer()
        : size_(0)
    {
    }

    //* =====================================================================
    /// \brief Size constructor
    ///
    /// Constructs a buffer of size default-constructed elements.
    //* =====================================================================
    explicit small_buffer(size_type size)
        : size_(size)
    {
        AllocateBuffer();
    }

    //* =====================================================================
    /// \brief Array constructor.
    ///
    /// Constructs a buffer of size elements, where each element is copied
    /// sequentially from array.
    //* =====================================================================
    small_buffer(ValueType const *array, size_type size)
        : size_(size)
    {
        AllocateBuffer();
        std::copy(array, array + size, begin());
    }

    //* =====================================================================
    /// \brief Statically-sized array constructor
    ///
    /// Constructs a buffer, where each element is copied sequentially from
    /// array.
    //* =====================================================================
    template <size_type DataSize>
    small_buffer(ValueType const (&array)[DataSize])
        : size_(DataSize)
    {
        AllocateBuffer();
        std::copy(array, array + DataSize, begin());
    }

    //* =====================================================================
    /// \brief Copy constructor
    ///
    /// Constructs a buffer where each element is copied sequentially from
    /// other.
    //* =====================================================================
    small_buffer(small_buffer const &other)
        : size_(other.size_)
    {
        AllocateBuffer();
        std::copy(other.begin(), other.end(), begin());
    }

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    ~small_buffer()
    {
        for (iterator current = begin(); current != end(); ++current)
        {
            allocator_.destroy(current);
        }
        
        if (!is_pod_ || size_ > StaticDataSize)
        {
            allocator_.deallocate(union_.get_dynamic_buffer(), 0);
        }
    }

    //* =====================================================================
    /// \brief Assignment operator
    ///
    /// Copies the elements of other into this buffer, reducing or expanding
    /// the buffer as necessary.
    //* =====================================================================
    small_buffer &operator=(small_buffer other)
    {
        swap(other);
        return *this;
    }

    //* =====================================================================
    /// \brief Obtain the size of this buffer.
    //* =====================================================================
    size_type size() const
    {
        return size_;
    }

    //* =====================================================================
    /// \brief Obtain the maximum size of this buffer.
    //* =====================================================================
    size_type max_size() const
    {
        // small_buffer has a fixed size, so max_size() and size() will always
        // yield the same result.
        //
        // TODO: since this can be assigned into or swapped, is this correct?
        return size();
    }

    //* =====================================================================
    /// \brief Query the buffer to see if it is empty.
    /// \return true if the buffer is empty, false otherwise.
    //* =====================================================================
    bool empty() const
    {
        return size() == 0;
    }

    //* =====================================================================
    /// \brief Swaps the contents of this buffer with the contents of nother
    /// buffer.
    //* =====================================================================
    void swap(small_buffer &other)
    {
        // Because of the small buffer optimisation, there are several cases
        // to take care of here:
        //   - if both this and the other object are small arrays,
        //   - if both this and the other object are long arrays,
        //   - if only this is a small array,
        //   - if only the other object is a small array.

        // On top of this, the entire function cannot throw, because swap
        // operations may not throw.  So, only the following may be used:
        //   - swap operations (which may not throw),
        //   - pointer operations (which may not throw),
        //   - operations on arithmetic types (which may not throw).
        if (!is_pod_)
        {
            // Since this is not a pod type, it is always dynamically allocated.
            // Hence, we can just swap the pointers.
            std::swap(
                union_.get_dynamic_buffer()
              , other.union_.get_dynamic_buffer());
        }
        else if (size_ > StaticDataSize)
        {
            if (other.size_ > StaticDataSize)
            {
                // Both buffers are long buffers, which means we can just
                // swap the array pointers.
                std::swap(
                    union_.get_dynamic_buffer()
                  , other.union_.get_dynamic_buffer());
            }
            else
            {
                // This buffer is long, but the other is short.
                ValueType *cached_array = union_.get_dynamic_buffer();

                std::copy(
                    other.union_.get_static_buffer(), 
                    other.union_.get_static_buffer() + other.size_, 
                    union_.get_static_buffer());

                other.union_.get_dynamic_buffer() = cached_array;
            }
        }
        else
        {
            if (other.size_ > StaticDataSize)
            {
                // The other buffer is long, but this one is short.
                ValueType *cached_array = other.union_.get_dynamic_buffer();

                std::copy(
                    union_.get_static_buffer(),
                    union_.get_static_buffer() + size_,
                    other.union_.get_static_buffer());

                union_.get_dynamic_buffer() = cached_array;
            }
            else
            {
                // Both buffers are short, so we can do a straight swap over
                // the two statically allocated buffers.
                size_type const maximum_size = (std::max)(size_, other.size_);
                
                std::swap_ranges(
                    begin()
                  , begin() + maximum_size 
                  , other.begin());
            }
        }

        std::swap(size_, other.size_);
    }

    //* =====================================================================
    /// \brief Obtain the iterator to the start of the buffer.
    //* =====================================================================
    iterator begin()
    {
        return SelectArray();
    }

    //* =====================================================================
    /// \brief Obtain the iterator to the start of the buffer.
    //* =====================================================================
    const_iterator begin() const
    {
        return SelectArray();
    }

    //* =====================================================================
    /// \brief Obtain the iterator to one past the end of the buffer.
    //* =====================================================================
    iterator end()
    {
        return SelectArray() + size_;
    }

    //* =====================================================================
    /// \brief Obtain the iterator to one past the end of the buffer.
    //* =====================================================================
    const_iterator end() const
    {
        return SelectArray() + size_;
    }

    //* =====================================================================
    /// \brief Obtain the indexth element of the buffer.
    /// \note This is a checked access and will throw std::range_error
    /// if the index is out of bounds.
    //* =====================================================================
    reference operator[](size_type index)
    {
        if (index >= size_)
        {
            throw std::range_error(boost::str(boost::format(
                "Attempt to access array index %d in an array of %d elements.")
              % odin::u32(index)
              % odin::u32(size_)));
        }

        return SelectArray()[index];
    }

    //* =====================================================================
    /// \brief Obtain the indexth element of the buffer.
    /// \note This is a checked access and will throw std::range_error
    /// if the index is out of bounds.
    //* =====================================================================
    const_reference operator[](size_type index) const
    {
        if (index >= size_)
        {
            throw std::range_error(boost::str(boost::format(
                "Attempt to access array index %d in an array of %d elements.")
              % odin::u32(index)
              % odin::u32(size_)));
        }

        return SelectArray()[index];
    }

private :
    typedef small_buffer_detail::small_buffer_union
    <
        ValueType, 
        StaticDataSize, 
        is_pod_
    > TUnion;

    TUnion     union_;
    size_type  size_;
    Allocator allocator_;

    //* =====================================================================
    //  \brief Ensures that any necessary memory allocation takes place.
    //* =====================================================================
    void AllocateBuffer()
    {
        if (!is_pod_ || size_ > StaticDataSize)
        {
            ValueType* poNewDynamicBuffer = allocator_.allocate(size_);

            std::uninitialized_fill_n(
                poNewDynamicBuffer, size_, ValueType());

            union_.get_dynamic_buffer() = poNewDynamicBuffer;
        }
    }

    //* =====================================================================
    //  \brief Selects the appropriate union member and returns it.
    //* =====================================================================
    ValueType* SelectArray()
    {
        return size_ > StaticDataSize 
             ? union_.get_dynamic_buffer()
             : union_.get_static_buffer();
    }

    //* =====================================================================
    //  \brief Selects the appropriate union member and returns it.
    //* =====================================================================
    ValueType const *SelectArray() const
    {
        return size_ > StaticDataSize 
             ? union_.get_dynamic_buffer()
             : union_.get_static_buffer();
    }
};

//* =========================================================================
/// \fn Equality operator
/// \brief tests for equality of all elements for two small buffers.
/// \return true if each element in lhs compares equal with its respective
/// element of rhs.
//* =========================================================================
template <class ValueType, odin::u32 StaticDataSize, class Allocator>
bool operator==(
    small_buffer<ValueType, StaticDataSize, Allocator> const &lhs,
    small_buffer<ValueType, StaticDataSize, Allocator> const &rhs)
{
    return lhs.size() == rhs.size()
        && std::mismatch(
               lhs.begin()
             , lhs.end()
             , rhs.begin()) == std::make_pair(lhs.end(), rhs.end());
}

//* =========================================================================
/// \fn Inequality operator
/// \brief tests for inequality of any elements for two small buffers.
/// \return true if any element in lhs does not compare equal with its 
/// respective element of rhs.
//* =========================================================================
template <class ValueType, odin::u32 StaticDataSize, class Allocator>
bool operator!=(
    small_buffer<ValueType, StaticDataSize, Allocator> const &lhs,
    small_buffer<ValueType, StaticDataSize, Allocator> const &rhs)
{
    return !(lhs == rhs);
}

//* =========================================================================
/// \fn Less-than operator
/// \brief tests that the lhs small buffer is lexicographically less than 
/// the rhs buffer.
/// \return true if lhs is lexicographically less than rhs, false otherwise.
//* =========================================================================
template <class ValueType, odin::u32 StaticDataSize, class Allocator>
bool operator<(
    small_buffer<ValueType, StaticDataSize, Allocator> const &lhs,
    small_buffer<ValueType, StaticDataSize, Allocator> const &rhs)
{
    return lexicographical_compare(
        lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

namespace small_buffer_detail
{
    // See the forward-declaration of this class at the top of the file for
    // more information.
    template <class ValueType, odin::u32 StaticDataSize>
    struct small_buffer_union<ValueType, StaticDataSize, true>
    {
        ValueType* get_static_buffer()
        {
            return m_aoStaticBuffer;
        }

        ValueType const *get_static_buffer() const
        {
            return m_aoStaticBuffer;
        }

        ValueType*& get_dynamic_buffer()
        {
            return m_poDynamicBuffer;
        }

        ValueType *const &get_dynamic_buffer() const
        {
            return m_poDynamicBuffer;
        }

        union
        {
            ValueType  m_aoStaticBuffer[StaticDataSize];
            ValueType* m_poDynamicBuffer;
        };
    };

    template <class ValueType, odin::u32 StaticDataSize>
    struct small_buffer_union<ValueType, StaticDataSize, false>
    {
        small_buffer_union()
            : m_poDynamicBuffer(NULL)
        {
        }

        ValueType* get_static_buffer()
        {
            return m_poDynamicBuffer;
        }

        ValueType const *get_static_buffer() const
        {
            return m_poDynamicBuffer;
        }

        ValueType *&get_dynamic_buffer()
        {
            return m_poDynamicBuffer;
        }

        ValueType const *const get_dynamic_buffer() const
        {
            return m_poDynamicBuffer;
        }

        ValueType* m_poDynamicBuffer;
    };
}

}

#endif

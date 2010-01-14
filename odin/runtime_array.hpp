// ==========================================================================
// Odin Runtime Array.
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
#ifndef ODIN_RUNTIME_ARRAY_HPP_
#define ODIN_RUNTIME_ARRAY_HPP_

#include "small_buffer.hpp"

namespace odin {

namespace runtime_array_detail
{
    BOOST_STATIC_CONSTANT(std::size_t, STATIC_ARRAY_SIZE = 8);
}

//* =========================================================================
/// \brief A class template to handle arbitrarily-length arrays of types.
/// \param T The type of element that the array contains.
/// \param Storage An underlying type that contains the data.
/// It must also model the STL Container concept.  By default, we use the
/// odin::small_buffer<> Container, which implements the Small Buffer
/// optimisation.
///
/// A class that is used to describe and manage an array of T.  It is a 
/// model of the STL's Container concept.
//* =========================================================================
template <
    class T, 
    class Storage = odin::small_buffer<
        T, runtime_array_detail::STATIC_ARRAY_SIZE
    >
>
class runtime_array
{
public :
    // These typedefs are used to satisfy the Container concept requirements.
    typedef typename Storage::value_type      value_type;
    typedef typename Storage::iterator        iterator;
    typedef typename Storage::const_iterator  const_iterator;
    typedef typename Storage::reference       reference;
    typedef typename Storage::const_reference const_reference;
    typedef typename Storage::pointer         pointer;
    typedef typename Storage::difference_type difference_type;
    typedef typename Storage::size_type       size_type;

    //* =====================================================================
    /// \brief Default constructor
    ///
    /// Constructs a runtime array of 0 elements.
    //* =====================================================================
    runtime_array()
    {
    }

    //* =====================================================================
    /// \brief Statically-sized array constructor
    /// \warning This is not a requirement of the Container concept.  
    /// Therefore, this may not compile when used with Containers that do not
    /// implement a similar constructor.  In this case, use the sized-array
    /// constructor instead.
    ///
    /// Constructs a runtime-array whose elements are copies of those in
    /// array.
    //* =====================================================================
    template <size_type ArraySize>
    runtime_array(T const (&array)[ArraySize])
        : storage_(array)
    {
    }

    //* =====================================================================
    /// \brief Size constructor
    ///
    /// Constructs an array of a given size, where each element is default-
    /// constructed.
    //* =====================================================================
    explicit runtime_array(size_type size)
        : storage_(size)
    {
    }

    //* =====================================================================
    /// \brief Copy constructor from a runtime array with a different storage
    /// policy.
    ///
    /// Constructs a runtime array where each element is a copy of the 
    /// respective element of other.
    //* =====================================================================
    template <class OtherStorage>
    runtime_array(runtime_array<T, OtherStorage> const &other)
        : storage_(other.size())
    {
        copy(other.begin(), other.end(), storage_.begin());
    }

    //* =====================================================================
    /// \brief Sized-array constructor.
    ///
    /// Constructs a runtime array where each element is a copy of the first
    /// size elements of array.
    //* =====================================================================
    runtime_array(T const *array, size_type size)
        : storage_(size)
    {
        std::copy(array, array + size, storage_.begin());
    }

    //* =====================================================================
    /// \brief Obtain an iterator to the beginning of the array.
    //* =====================================================================
    iterator begin()
    {
        return storage_.begin();
    }

    //* =====================================================================
    /// \brief Obtain an iterator to the beginning of the array.
    //* =====================================================================
    const_iterator begin() const
    {
        return storage_.begin();
    }

    //* =====================================================================
    /// \brief Obtain an iterator to one past the end of the array.
    //* =====================================================================
    iterator end()
    {
        return storage_.end();
    }

    //* =====================================================================
    /// \brief Obtain an iterator to one past the end of the array.
    //* =====================================================================
    const_iterator end() const
    {
        return storage_.end();
    }

    //* =====================================================================
    /// \brief Test to see if the array is empty.
    /// \return true if the array has no elements, false otherwise.
    //* =====================================================================
    bool empty() const
    {
        return storage_.empty();
    }

    //* =====================================================================
    /// \brief Obtain the number of elements in the array.
    //* =====================================================================
    size_type size() const
    {
        return storage_.size();
    }

    //* =====================================================================
    /// \brief Obtain the maximum size of the array.
    //* =====================================================================
    size_type max_size() const
    {
        return storage_.max_size();
    }

    //* =====================================================================
    /// \brief Obtain the indexth element of the array.
    //* =====================================================================
    reference operator[](size_type index)
    {
        return storage_[index];
    }

    //* =====================================================================
    /// \brief Obtain the indexth element of the array.
    //* =====================================================================
    const_reference operator[](size_type index) const
    {
        return storage_[index];
    }

    //* =====================================================================
    /// \brief Swaps the contents of this array with the contents of other.
    //* =====================================================================
    void swap(runtime_array &other)
    {
        storage_.swap(other.storage_);
    }

    //* =====================================================================
    /// \brief Obtain the array as a C-style array.
    //* =====================================================================
    value_type* c_array()
    {
        return &*storage_.begin();
    }

    //* =====================================================================
    /// \brief Less-than operator
    /// \return true if lhs compares less than rhs, false otherwise.
    //* =====================================================================
    template <class OtherStorage>
    friend bool operator<(
        runtime_array<T, Storage>      const& lhs,
        runtime_array<T, OtherStorage> const& rhs)
    {
        return lhs.storage_ < rhs.storage_;
    }

private :
    Storage storage_;
};

//* =========================================================================
/// \fn Equality operator
/// \brief tests for equality of all elements for two small buffers.
/// \return true if each element in lhs compares equal with its respective
/// element of rhs.
//* =========================================================================
template <class T, class Storage0, class Storage1>
bool operator==(
    runtime_array<T, Storage0> const& lhs,
    runtime_array<T, Storage1> const& rhs)
{
    return lhs.size() == rhs.size()
        && std::mismatch(
             lhs.begin(), 
             lhs.end(),
             rhs.begin()) == std::make_pair(lhs.end(), rhs.end());
}

//* =========================================================================
/// \fn Inequality operator
/// \brief tests for inequality of any elements for two small buffers.
/// \return true if any element in lhs does not compare equal with its 
/// respective element of rhs.
//* =========================================================================
template <class T, class Storage0, class Storage1>
bool operator!=(
    runtime_array<T, Storage0> const& lhs,
    runtime_array<T, Storage1> const& rhs)
{
    return !(lhs == rhs);
}

//* =========================================================================
/// \fn Streaming Output operator
/// \brief Streams the array to textual output in the format: "[x,y,z]".
//* =========================================================================
template <class T, class Storage>
std::ostream& operator<<(
    std::ostream&                    out, 
    runtime_array<T, Storage> const &array)
{
    typedef typename runtime_array<T, Storage>::size_type ArraySizeType;
    ArraySizeType nSize = array.size();

    if (nSize == 0)
    {
        out << "[]";
    }
    else
    {
        out << "[";

        for (ArraySizeType nIndex = 0; nIndex < nSize - 1; ++nIndex)
        {
            out << array[nIndex] << ",";
        }

        out << array[nSize - 1] << "]";
    }

    return out;
}

//* =========================================================================
/// \fn Streaming Output operator
/// \brief Streams the array to textual output in the format: "[x,y,z]".
/// \note This is overloaded for unsigned char so that it comes out in a
/// numeric format, rather than a text format.
//* =========================================================================
template <class Storage>
std::ostream& operator<<(
    std::ostream&                           out, 
    runtime_array<odin::u8, Storage> const &array)
{
    typedef typename runtime_array<odin::u8, Storage>::size_type ArraySizeType;
    ArraySizeType nSize = array.size();

    if (nSize == 0)
    {
        out << "[]";
    }
    else
    {
        out << "[";

        for (ArraySizeType nIndex = 0; nIndex < nSize - 1; ++nIndex)
        {
            out << int(array[nIndex]) << ",";
        }

        out << int(array[nSize - 1]) << "]";
    }

    return out;
}

//* =========================================================================
/// \fn make_runtime_array<ValueType, Size>
/// \param ValueType the type of the runtime_array.
/// \param Size the size of the array
/// \brief A helper function to create a runtime_array<> from a statically-
/// initialised array.
//* =========================================================================
template <class ValueType, size_t Size>
runtime_array<ValueType> make_runtime_array(ValueType (&array)[Size])
{
    return runtime_array<ValueType>(array);
}

}

#endif

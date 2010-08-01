// ==========================================================================
// Odin Data Filter.
//
// Copyright (C) 2003 Matthew Chaplain, All Rights Reserved.
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
#ifndef ODIN_FILTER_HPP_
#define ODIN_FILTER_HPP_

#include <boost/call_traits.hpp>
#include <boost/optional.hpp>
#include <functional>

namespace odin {

//* =========================================================================
/// \brief A function object that, when given input, will filter out any
/// unwanted input.
/// \param T the type of object that is to be subjected to filtration.
///
/// \par Implementation
/// Create a derived class and override do_filter(T) to either return true
/// if the passed-in value is acceptible and hence passes the filter, or 
/// false if it is unacceptible and not to be returned.
/// \code
/// // Filters out odd numbers
/// class odd_filter : public filter<int>
/// {
/// private :
///     bool do_filter(int value)
///     {
///         // The value is acceptible if it is even.
///         return (value % 2) == 0; 
///     }
/// };
/// \endcode
///
/// \par Usage
/// Create an instance of the filter.  For example,
/// \code
/// odd_filter my_odd_filter;
/// \endcode
/// Apply some input to the filter as if it were a function:
/// \code
/// // Yields a boost::optional<int> that is empty.
/// odd_filter::result_type result1 = my_odd_filter(3);
///
/// // Yields a boost::optional<int> that has the value 4.
/// odd_filter::result_type result2 = my_odd_filter(4);
/// \endcode
//* =========================================================================
template <typename T>
class filter 
    : public std::unary_function
      <
          typename boost::call_traits<T>::param_type
        , boost::optional<T> 
      >
{
private :
    typedef std::unary_function
    <
        typename boost::call_traits<T>::param_type
      , boost::optional<T>
    > parent_type;
    
public :
    //* =====================================================================
    /// \brief The type that results from calling operator().  This is
    /// boost::optional<T>.
    //* =====================================================================
    typedef typename parent_type::result_type result_type;

    //* =====================================================================
    /// \brief The type that is passed into operator().  This is 
    /// boost::optional<callable type of T>
    //* =====================================================================
    typedef typename parent_type::argument_type argument_type;
    
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~filter()
    {
    }

    //* =====================================================================
    /// \brief Apply filtration to t.
    /// \return Either a boost::optional<> object containing t if it passed
    /// filtration, or an empty boost::optional<> object otherwise.
    //* =====================================================================
    result_type operator()(argument_type t)
    {
        return do_filter(t) ? result_type(t) : result_type();
    }
   
private :
    //* =====================================================================
    /// \brief Implemented by the derived class.  Test whether t is
    /// to be accepted or filtered away.
    /// \return true if t can be returned from operator(), false if it is
    /// to be consumed by the filter.
    //* =====================================================================
    virtual bool do_filter(argument_type t) = 0;
};
    
}

#endif


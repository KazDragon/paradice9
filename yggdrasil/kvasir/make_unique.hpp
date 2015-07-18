/*
 *    Copyright (c) 2014 Matthew Chaplain <matthew.chaplain@gmail.com>
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 *
 *    The above copyright notice and this permission notice shall be
 *    included in all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef YGGDRASIL_KVASIR_MAKE_UNIQUE_HPP_
#define YGGDRASIL_KVASIR_MAKE_UNIQUE_HPP_

#include <memory>

namespace yggdrasil { namespace kvasir {

/**
 * @brief A function that implements C++14's make_unique in C++11, or simply
 * forwards to the C++14 version, depending on what is available.
 */
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    // unique_ptr<> came into existence without make_unique(), and this was
    // corrected in C++14.
    
    /* TODO: these macros also need to look at the library version for GNU.
#if (__cplusplus == 201103L)
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
#elif (__cplusplus > 201103L)
    return std::make_unique(std::forward<Args>(args)...));
#endif
    */
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

}}

#endif

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
#ifndef YGGDRASIL_KVASIR_INITIALIZER_HPP_
#define YGGDRASIL_KVASIR_INITIALIZER_HPP_

#include <utility>

namespace yggdrasil { namespace kvasir {

/**
 * @brief Creates an initializer list from an aggregate.
 *
 * In a template deduction context, aggregates do not translate into initializer
 * lists automatically.  This small template is used to force the issue.
 *
 * @par Example Usage
 * @code
 * template_function(il({a, b, c, d}));
 * @endcode
 */
template <typename T>
std::initializer_list<T> il(std::initializer_list<T> &&li) {
    return li;
}

}}

#endif

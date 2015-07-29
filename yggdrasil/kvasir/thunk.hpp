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
#ifndef YGGDRASIL_KVASIR_THUNK_HPP_
#define YGGDRASIL_KVASIR_THUNK_HPP_

/**
 * @def MEMBER_THUNK(fun)
 * @brief Generates a free function that converts f(o, args...) to o.f(args...)
 */
#define MEMBER_THUNK(fun) \
template <typename Arg0, typename... Args> \
auto fun(Arg0 &&arg0, Args&&... args) \
{ \
    return arg0.fun(std::forward<Args>(args)...); \
}


/**
 * @def FRIEND_THUNK(fun)
 * @brief Generates a friend function that converts f(o, args...) to
 * o.self_->f_(args...), for use in the concept/model pattern.
 */
#define FRIEND_THUNK(fun) \
template <typename Arg0, typename... Args> \
friend auto fun(Arg0 &&arg0, Args&&... args) \
{ \
    return arg0.self_->fun##_(std::forward<Args>(args)...); \
}

/**
 * @def SELF_THUNK(fun)
 * @brief Generates a function that converts o.f(args...) to
 * o.self_->f_(args...), for use in the concept/model pattern.
 */
#define SELF_THUNK(fun) \
template <typename... Args> \
auto fun(Args&&... args) \
{ \
    return self_->fun##_(std::forward<Args>(args)...); \
}

/**
 * @def SELF_THUNK_C(fun)
 * @brief Generates a function that converts o.f(args...) const to
 * o.self_->f_(args...) const, for use in the concept/model pattern.
 */
#define SELF_THUNK_C(fun) \
template <typename... Args> \
auto fun(Args&&... args) const \
{ \
    return self_->fun##_(std::forward<Args>(args)...); \
}

#endif

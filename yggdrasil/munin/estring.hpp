// ==========================================================================
// Munin Element String.
//
// Copyright (C) 2014 Matthew Chaplain, All Rights Reserved.
//
// Permission to reproduce, distribute, perform, display, and to prepare
// derivitive works from this file under the following conditions:
//
// 1. Any copy, reproduction or derivitive work of any part of this file
//    contains this copyright notice and licence in its entirety.
//
// 2. The rights granted to you under this license automatically terminate
//    should you attempt to assert any patent claims against the licensor
//    or contributors, which in any way restrict the ability of any party
//    from using this software or portions thereof in any form under the
//    terms of this license.
//
// Disclaimer: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
//             KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//             WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
//             PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//             OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
//             OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
//             OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//             SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ==========================================================================
#ifndef YGGDRASIL_MUNIN_ESTRING_HPP_
#define YGGDRASIL_MUNIN_ESTRING_HPP_

#include "yggdrasil/munin/element.hpp"
#include <iosfwd>
#include <string>
#include <vector>
#include <cstddef>

namespace yggdrasil { namespace munin {

class element;

//* =========================================================================
/// \brief A class that represents strings of elements.
/// \par
/// Note: models an STL container.
//* =========================================================================
class estring
{
public :
    //* =====================================================================
    /// Container Typedefs
    //* =====================================================================
    typedef element           value_type;
    typedef value_type       &reference;
    typedef value_type const &const_reference;
    typedef element          *pointer;
    typedef element const    *const_pointer;
    typedef element          *iterator;
    typedef element const    *const_iterator;
    typedef element const    *const_reverse_iterator;
    typedef std::ptrdiff_t    difference_type;
    typedef std::size_t       size_type;

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    estring();

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    estring(char const *text);

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    estring(char const *text, std::size_t len);

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    estring(std::string const &text);

    //* =====================================================================
    /// \brief Returns the number of elements in the string.
    //* =====================================================================
    std::size_t size() const;

    //* =====================================================================
    /// \brief Returns an iterator to the beginning of the string.
    //* =====================================================================
    iterator begin();

    //* =====================================================================
    /// \brief Returns an iterator to the beginning of the string.
    //* =====================================================================
    const_iterator begin() const;

    //* =====================================================================
    /// \brief Returns a reverse iterator to the reverse beginning of the
    /// string.
    //* =====================================================================
    const_reverse_iterator rbegin() const;

    //* =====================================================================
    /// \brief Returns an iterator to the end of the string.
    //* =====================================================================
    iterator end();

    //* =====================================================================
    /// \brief Returns an iterator to the end of the string.
    //* =====================================================================
    const_iterator end() const;

    //* =====================================================================
    /// \brief Returns a reverse iterator to the reverse end of the string
    //* =====================================================================
    const_reverse_iterator rend() const;

    //* =====================================================================
    /// \brief Returns an iterator to the beginning of the string.
    //* =====================================================================
    const_iterator cbegin();

    //* =====================================================================
    /// \brief Returns an iterator to the end of the string.
    //* =====================================================================
    const_iterator cend();

    //* =====================================================================
    /// \brief Swaps the contents of this and another string.
    //* =====================================================================
    void swap(estring &other);

    //* =====================================================================
    /// \brief Returns the maximum size of the string allowed.
    //* =====================================================================
    size_type max_size() const;

    //* =====================================================================
    /// \brief Returns whether the estring is empty or not.
    //* =====================================================================
    bool empty() const;

    //* =====================================================================
    /// \brief Array access operator
    //* =====================================================================
    const_reference operator[](size_type index) const;

    //* =====================================================================
    /// \brief Append operator
    //* =====================================================================
    estring &operator+=(estring const &rhs);

    //* =====================================================================
    /// \brief Equality operator
    //* =====================================================================
    friend bool operator==(estring const &lhs, estring const &rhs);

    //* =====================================================================
    /// \brief Inequality operator
    //* =====================================================================
    friend bool operator!=(estring const &lhs, estring const &rhs);

    //* =====================================================================
    /// \brief Streaming operator
    //* =====================================================================
    friend std::ostream &operator<<(std::ostream &out, estring const &es);

private :
    std::vector<element> elements_;
};

//* =========================================================================
/// \brief Concatenation
//* =========================================================================
estring operator+(estring lhs, estring const &rhs);

}

inline namespace literals { inline namespace estring_literals {

//* =========================================================================
/// \brief Construct an estring from literals using "foo"_es;
//* =========================================================================
yggdrasil::munin::estring operator ""_es(
    char const *text,
    std::size_t length);

}}}

#endif

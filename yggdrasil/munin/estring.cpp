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
#include "yggdrasil/munin/estring.hpp"
#include "yggdrasil/munin/element.hpp"
#include "yggdrasil/munin/detail/string_to_elements.hpp"
#include <limits>
#include <cstring>

namespace yggdrasil { namespace munin {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
estring::estring(const char* text)
  : elements_(detail::string_to_elements(text))
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
estring::estring(const char* text, std::size_t len)
  : elements_(detail::string_to_elements(text, len))
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
estring::estring(const std::string& text)
  : elements_(detail::string_to_elements(text))
{
}

// ==========================================================================
// SIZE
// ==========================================================================
std::size_t estring::size() const
{
    return elements_.size();
}

// ==========================================================================
// BEGIN
// ==========================================================================
estring::const_iterator estring::begin() const
{
    return &*elements_.begin();
}

// ==========================================================================
// RBEGIN
// ==========================================================================
estring::const_reverse_iterator estring::rbegin() const
{
    return &*elements_.rbegin();
}

// ==========================================================================
// END
// ==========================================================================
estring::const_iterator estring::end() const
{
    return &*elements_.end();
}

// ==========================================================================
// REND
// ==========================================================================
estring::const_reverse_iterator estring::rend() const
{
    return &*elements_.rend();
}

// ==========================================================================
// MAX_SIZE
// ==========================================================================
estring::size_type estring::max_size() const
{
    return std::numeric_limits<size_type>::max();
}

// ==========================================================================
// EMPTY
// ==========================================================================
bool estring::empty() const
{
    return elements_.empty();
}

// ==========================================================================
// OPERATOR []
// ==========================================================================
estring::const_reference estring::operator[](estring::size_type index) const
{
    return elements_[index];
}

// ==========================================================================
// OPERATOR +=
// ==========================================================================
estring &estring::operator+=(estring const &rhs)
{
    elements_.insert(elements_.end(), rhs.begin(), rhs.end());
    return *this;
}

// ==========================================================================
// OPERATOR ==
// ==========================================================================
bool operator==(estring const &lhs, estring const &rhs)
{
    return lhs.elements_ == rhs.elements_;
}

// ==========================================================================
// OPERATOR !=
// ==========================================================================
bool operator!=(estring const &lhs, estring const &rhs)
{
    return !(lhs == rhs);
}

// ==========================================================================
// OPERATOR <<
// ==========================================================================
std::ostream &operator<<(std::ostream &out, estring const &es)
{
    out << "estring[";

    for (auto const &elem : es.elements_)
    {
        out << elem << ", ";
    }

    out << "]";

    return out;
}

// ==========================================================================
// OPERATOR +
// ==========================================================================
estring operator+(estring lhs, estring const &rhs)
{
    return lhs += rhs;
}

}

inline namespace literals { inline namespace estring_literals {

yggdrasil::munin::estring operator ""_es(char const *text, std::size_t len)
{
    return yggdrasil::munin::estring(text, len);
}

}}}

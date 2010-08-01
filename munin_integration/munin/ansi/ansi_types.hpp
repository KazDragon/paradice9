// ==========================================================================
// Munin ANSI Types.
//
// Copyright (C) 2010 Matthew Chaplain, All Rights Reserved.
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
#ifndef MUNIN_ANSI_TYPES_HPP_
#define MUNIN_ANSI_TYPES_HPP_

#include "munin/ansi/attribute.hpp"
#include <boost/optional.hpp>
#include <iosfwd>
#include <utility>

namespace munin { namespace ansi {

//* =========================================================================
/// \brief The type of an element in an ANSI component.  Each element is a
/// normal character and can optionally have attributes (such as boldness,
/// colour, font, etc.) applied to it.  A lack of an attribute indicates that
/// the character has the "default" attribute.
//* =========================================================================
typedef std::pair<
    char
  , boost::optional<munin::ansi::attribute>
> element_type;
  
bool operator==(element_type const &lhs, element_type const &rhs);

std::ostream &operator<<(std::ostream &out, element_type const &element);

}}

#endif

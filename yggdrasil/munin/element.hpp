// ==========================================================================
// Munin Element.
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
#ifndef YGGDRASIL_MUNIN_ELEMENT_HPP_
#define YGGDRASIL_MUNIN_ELEMENT_HPP_

#include "yggdrasil/munin/attribute.hpp"
#include "yggdrasil/munin/glyph.hpp"
#include <iosfwd>

namespace yggdrasil { namespace munin {
    
//* =========================================================================
/// \brief Represents a single picture element that can be rendered on
/// a canvas.  Each element is a character glyph and a set of attributes 
/// (such as boldness, colour, etc.).
//* =========================================================================
struct element
{
    //* =====================================================================
    /// \brief Default Constructor
    /// \par
    /// Constructs an element with a character of '\0' in the default locale
    /// and character set, with no special attributes.
    //* =====================================================================
    element();
    
    //* =====================================================================
    /// \brief Constructor
    /// \par
    /// Constructs the element with the given parameters.
    //* =====================================================================
    element(
        glyph gly,
        attribute attr = {});

    //* =====================================================================
    /// \brief Default Copy Assignment
    //* =====================================================================
    element &operator=(element const &other) = default;
    
    //* =====================================================================
    /// \brief Glyph Assignment
    //* =====================================================================
    element &operator=(glyph const &gly);
    
    glyph     glyph_;
    attribute attribute_;
};

//* =========================================================================
/// \brief Equality Operator
//* =========================================================================
bool operator==(element const &lhs, element const &rhs);

//* =========================================================================
/// \brief Inequality Operator
//* =========================================================================
bool operator!=(element const &lhs, element const &rhs);

//* =========================================================================
/// \brief Output stream operator
//* =========================================================================
std::ostream &operator<<(std::ostream &out, element const &element);

}}

#endif

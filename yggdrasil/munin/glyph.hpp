// ==========================================================================
// Munin ANSI Glyph.
//
// Copyright (C) 2011 Matthew Chaplain, All Rights Reserved.
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
#ifndef YGGDRASIL_MUNIN_ANSI_GLYPH_HPP_
#define YGGDRASIL_MUNIN_ANSI_GLYPH_HPP_

#include <iosfwd>
#include "yggdrasil/bragi/ansi/protocol.hpp"

namespace yggdrasil { namespace munin {

//* =========================================================================
/// \brief A structure that carries around the character attributes of an
/// ANSI element.
//* =========================================================================
struct glyph
{
    //* =====================================================================
    /// \brief Default Constructor
    //* =====================================================================
    glyph(
        char character     = ' ',
        char character_set = character_set::CHARACTER_SET_G0,
        char locale        = character_set::LOCALE_US_ASCII);

    // Character
    char character_;
    
    // Character Set Attributes
    char character_set_;
    char locale_;
};

bool operator==(glyph const &lhs, glyph const &rhs);
bool operator!=(glyph const &lhs, glyph const &rhs);

std::ostream &operator<<(std::ostream &out, glyph const &attr);

//* =========================================================================
/// \brief Returns whether a particular glyph is printable.
//* =========================================================================
bool is_printable(glyph const &gly);

}}

#endif

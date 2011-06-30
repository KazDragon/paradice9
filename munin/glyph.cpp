// ==========================================================================
// Munin ANSI Glyph Constants.
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
#include "munin/glyph.hpp"
#include "odin/ansi/protocol.hpp"
#include <iostream>

namespace munin {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
glyph::glyph(char character, char character_set, char locale)
    : character_(character)
    , character_set_(character_set)
    , locale_(locale)
{
}

// ==========================================================================
// OPERATOR==
// ==========================================================================
bool operator==(glyph const &lhs, glyph const &rhs)
{
    return lhs.character_     == rhs.character_
        && lhs.character_set_ == rhs.character_set_
        && lhs.locale_        == rhs.locale_;
}

// ==========================================================================
// OPERATOR<<
// ==========================================================================
std::ostream &operator<<(std::ostream &out, glyph const &attr)
{
    out << "glyph[";

    // TODO: add glyphs.

    out << "]";

    return out;
}

}


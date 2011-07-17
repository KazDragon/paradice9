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
#ifndef MUNIN_ANSI_GLYPH_HPP_
#define MUNIN_ANSI_GLYPH_HPP_

#include <iosfwd>
#include "odin/ansi/protocol.hpp"

namespace munin {

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
        char character     = '\0'
      , char character_set = odin::ansi::character_set::CHARACTER_SET_G0
      , char locale        = odin::ansi::character_set::LOCALE_US_ASCII);

    // Character
    char character_;
    
    // Character Set Attributes
    char character_set_;
    char locale_;
};

bool operator==(glyph const &lhs, glyph const &rhs);

std::ostream &operator<<(std::ostream &out, glyph const &attr);

// Some common glyphs

glyph const single_lined_top_right_corner(
    char(191)
  , odin::ansi::character_set::CHARACTER_SET_G0
  , odin::ansi::character_set::LOCALE_SCO);

glyph const double_lined_top_left_corner(
    char(201)
  , odin::ansi::character_set::CHARACTER_SET_G0
  , odin::ansi::character_set::LOCALE_SCO);
    
glyph const double_lined_top_right_corner(
    char(187)
  , odin::ansi::character_set::CHARACTER_SET_G0
  , odin::ansi::character_set::LOCALE_SCO);

glyph const double_lined_bottom_left_corner(
    char(200)
  , odin::ansi::character_set::CHARACTER_SET_G0
  , odin::ansi::character_set::LOCALE_SCO);

glyph const double_lined_bottom_right_corner(
    char(188)
  , odin::ansi::character_set::CHARACTER_SET_G0
  , odin::ansi::character_set::LOCALE_SCO);
    
glyph const double_lined_horizontal_beam(
    char(205)
  , odin::ansi::character_set::CHARACTER_SET_G0
  , odin::ansi::character_set::LOCALE_SCO);

glyph const double_lined_vertical_beam(
    char(186)
  , odin::ansi::character_set::CHARACTER_SET_G0
  , odin::ansi::character_set::LOCALE_SCO);

}

#endif

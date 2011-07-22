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

// Some uncommon glyphs that are very useful.

#define DECLARE_G0SCO_GLYPH(name, val) \
static glyph const name ( \
    char(val) \
  , odin::ansi::character_set::CHARACTER_SET_G0 \
  , odin::ansi::character_set::LOCALE_SCO) \

// Single lined box drawing
DECLARE_G0SCO_GLYPH(single_lined_top_left_corner, 218);
DECLARE_G0SCO_GLYPH(single_lined_top_right_corner, 191);
DECLARE_G0SCO_GLYPH(single_lined_bottom_left_corner, 192);
DECLARE_G0SCO_GLYPH(single_lined_bottom_right_corner, 217);
DECLARE_G0SCO_GLYPH(single_lined_horizontal_beam, 196);
DECLARE_G0SCO_GLYPH(single_lined_vertical_beam, 179);
DECLARE_G0SCO_GLYPH(single_lined_top_tee, 194);
DECLARE_G0SCO_GLYPH(single_lined_left_tee, 195);
DECLARE_G0SCO_GLYPH(single_lined_bottom_tee, 193);
DECLARE_G0SCO_GLYPH(single_lined_right_tee, 180);
DECLARE_G0SCO_GLYPH(single_lined_cross, 197);

// Double-lined box drawing
DECLARE_G0SCO_GLYPH(double_lined_top_left_corner, 201);
DECLARE_G0SCO_GLYPH(double_lined_top_right_corner, 187);
DECLARE_G0SCO_GLYPH(double_lined_bottom_left_corner, 200);
DECLARE_G0SCO_GLYPH(double_lined_bottom_right_corner, 188);
DECLARE_G0SCO_GLYPH(double_lined_horizontal_beam, 205);
DECLARE_G0SCO_GLYPH(double_lined_vertical_beam, 186);
DECLARE_G0SCO_GLYPH(double_lined_top_tee, 203);
DECLARE_G0SCO_GLYPH(double_lined_left_tee, 204);
DECLARE_G0SCO_GLYPH(double_lined_bottom_tee, 202);
DECLARE_G0SCO_GLYPH(double_lined_right_tee, 185);
DECLARE_G0SCO_GLYPH(double_lined_cross, 206);

// Mix-lined box drawing.  v and h describe whether the single-line component
// is horizontal or vertical
DECLARE_G0SCO_GLYPH(mix_lined_vright_tee, 181);
DECLARE_G0SCO_GLYPH(mix_lined_hright_tee, 182);
DECLARE_G0SCO_GLYPH(mix_lined_htop_right_corner, 183);
DECLARE_G0SCO_GLYPH(mix_lined_vtop_right_corner, 184);
DECLARE_G0SCO_GLYPH(mix_lined_hbottom_right_corner, 189);
DECLARE_G0SCO_GLYPH(mix_lined_vbottom_right_corner, 190);
DECLARE_G0SCO_GLYPH(mix_lined_vleft_tee, 198);
DECLARE_G0SCO_GLYPH(mix_lined_hleft_tee, 199);
DECLARE_G0SCO_GLYPH(mix_lined_vbottom_tee, 207);
DECLARE_G0SCO_GLYPH(mix_lined_hbottom_tee, 208);
DECLARE_G0SCO_GLYPH(mix_lined_vtop_tee, 209);
DECLARE_G0SCO_GLYPH(mix_lined_htop_tee, 210);
DECLARE_G0SCO_GLYPH(mix_lined_hbottom_left_corner, 211);
DECLARE_G0SCO_GLYPH(mix_lined_vbottom_left_corner, 212);
DECLARE_G0SCO_GLYPH(mix_lined_vtop_left_corner, 213);
DECLARE_G0SCO_GLYPH(mix_lined_htop_left_corner, 214);
DECLARE_G0SCO_GLYPH(mix_lined_hcross, 215);
DECLARE_G0SCO_GLYPH(mix_lined_vcross, 216);

#undef DECLARE_G0SCO_GLYPH

}

#endif

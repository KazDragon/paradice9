// ==========================================================================
// Munin SCO Glyphs.
//
// Copyright (C) 2015 Matthew Chaplain, All Rights Reserved.
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
#ifndef MUNIN_SCO_GLYPHS_HPP_
#define MUNIN_SCO_GLYPHS_HPP_

#include <terminalpp/glyph.hpp>

namespace munin {
    
// Some uncommon glyphs that are very useful.

#define DECLARE_SCO_GLYPH(name, val) \
static constexpr terminalpp::glyph const name ( \
    char(val) \
  , terminalpp::ansi::charset::sco )

// Single lined box drawing
DECLARE_SCO_GLYPH(single_lined_top_left_corner, 218);
DECLARE_SCO_GLYPH(single_lined_top_right_corner, 191);
DECLARE_SCO_GLYPH(single_lined_bottom_left_corner, 192);
DECLARE_SCO_GLYPH(single_lined_bottom_right_corner, 217);
DECLARE_SCO_GLYPH(single_lined_horizontal_beam, 196);
DECLARE_SCO_GLYPH(single_lined_vertical_beam, 179);
DECLARE_SCO_GLYPH(single_lined_top_tee, 194);
DECLARE_SCO_GLYPH(single_lined_left_tee, 195);
DECLARE_SCO_GLYPH(single_lined_bottom_tee, 193);
DECLARE_SCO_GLYPH(single_lined_right_tee, 180);
DECLARE_SCO_GLYPH(single_lined_cross, 197);

// Double-lined box drawing
DECLARE_SCO_GLYPH(double_lined_top_left_corner, 201);
DECLARE_SCO_GLYPH(double_lined_top_right_corner, 187);
DECLARE_SCO_GLYPH(double_lined_bottom_left_corner, 200);
DECLARE_SCO_GLYPH(double_lined_bottom_right_corner, 188);
DECLARE_SCO_GLYPH(double_lined_horizontal_beam, 205);
DECLARE_SCO_GLYPH(double_lined_vertical_beam, 186);
DECLARE_SCO_GLYPH(double_lined_top_tee, 203);
DECLARE_SCO_GLYPH(double_lined_left_tee, 204);
DECLARE_SCO_GLYPH(double_lined_bottom_tee, 202);
DECLARE_SCO_GLYPH(double_lined_right_tee, 185);
DECLARE_SCO_GLYPH(double_lined_cross, 206);

// Mix-lined box drawing.  v and h describe whether the single-line component
// is horizontal or vertical
DECLARE_SCO_GLYPH(mix_lined_vright_tee, 181);
DECLARE_SCO_GLYPH(mix_lined_hright_tee, 182);
DECLARE_SCO_GLYPH(mix_lined_htop_right_corner, 183);
DECLARE_SCO_GLYPH(mix_lined_vtop_right_corner, 184);
DECLARE_SCO_GLYPH(mix_lined_hbottom_right_corner, 189);
DECLARE_SCO_GLYPH(mix_lined_vbottom_right_corner, 190);
DECLARE_SCO_GLYPH(mix_lined_vleft_tee, 198);
DECLARE_SCO_GLYPH(mix_lined_hleft_tee, 199);
DECLARE_SCO_GLYPH(mix_lined_vbottom_tee, 207);
DECLARE_SCO_GLYPH(mix_lined_hbottom_tee, 208);
DECLARE_SCO_GLYPH(mix_lined_vtop_tee, 209);
DECLARE_SCO_GLYPH(mix_lined_htop_tee, 210);
DECLARE_SCO_GLYPH(mix_lined_hbottom_left_corner, 211);
DECLARE_SCO_GLYPH(mix_lined_vbottom_left_corner, 212);
DECLARE_SCO_GLYPH(mix_lined_vtop_left_corner, 213);
DECLARE_SCO_GLYPH(mix_lined_htop_left_corner, 214);
DECLARE_SCO_GLYPH(mix_lined_hcross, 215);
DECLARE_SCO_GLYPH(mix_lined_vcross, 216);

// Other drawing characters.
DECLARE_SCO_GLYPH(left_guillemet, 174);
DECLARE_SCO_GLYPH(right_guillemet, 175);

#undef DECLARE_SCO_GLYPH

}

#endif

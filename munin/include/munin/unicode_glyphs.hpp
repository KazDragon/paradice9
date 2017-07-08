// ==========================================================================
// Munin Unicode Glyphs.
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
#ifndef UNICODE_GLYPHS_HPP
#define UNICODE_GLYPHS_HPP

#include <terminalpp/glyph.hpp>

namespace munin { inline namespace unicode {

// Some uncommon glyphs that are very useful.

static constexpr terminalpp::glyph const single_lined_top_left_corner("\U0000250C");
static constexpr terminalpp::glyph const single_lined_top_right_corner("\U00002510");
static constexpr terminalpp::glyph const single_lined_bottom_left_corner("\U00002514");
static constexpr terminalpp::glyph const single_lined_bottom_right_corner("\U00002518");

static constexpr terminalpp::glyph const single_lined_horizontal_beam("\U00002500");
static constexpr terminalpp::glyph const single_lined_vertical_beam("\U00002502");
static constexpr terminalpp::glyph const single_lined_top_tee("\U0000252C");
static constexpr terminalpp::glyph const single_lined_left_tee("\U0000251C");
static constexpr terminalpp::glyph const single_lined_bottom_tee("\U00002534");
static constexpr terminalpp::glyph const single_lined_right_tee("\U00002524");
static constexpr terminalpp::glyph const single_lined_cross("\U0000253C");

// Double-lined box drawing
static constexpr terminalpp::glyph const double_lined_top_left_corner("\U00002554");
static constexpr terminalpp::glyph const double_lined_top_right_corner("\U00002557");
static constexpr terminalpp::glyph const double_lined_bottom_left_corner("\U0000255A");
static constexpr terminalpp::glyph const double_lined_bottom_right_corner("\U0000255D");
static constexpr terminalpp::glyph const double_lined_horizontal_beam("\U00002550");
static constexpr terminalpp::glyph const double_lined_vertical_beam("\U00002551");
static constexpr terminalpp::glyph const double_lined_top_tee("\U00002566");
static constexpr terminalpp::glyph const double_lined_left_tee("\U00002560");
static constexpr terminalpp::glyph const double_lined_bottom_tee("\U00002569");
static constexpr terminalpp::glyph const double_lined_right_tee("\U00002563");
static constexpr terminalpp::glyph const double_lined_cross("\U0000256C");

// Mix-lined box drawing.  v and h describe whether the single-line component
// is horizontal or vertical
static constexpr terminalpp::glyph const mix_lined_vright_tee("\U00002561");
static constexpr terminalpp::glyph const mix_lined_hright_tee("\U00002562");
static constexpr terminalpp::glyph const mix_lined_vtop_right_corner("\U00002555");
static constexpr terminalpp::glyph const mix_lined_htop_right_corner("\U00002556");
static constexpr terminalpp::glyph const mix_lined_vbottom_right_corner("\U0000255B");
static constexpr terminalpp::glyph const mix_lined_hbottom_right_corner("\U0000255C");
static constexpr terminalpp::glyph const mix_lined_vleft_tee("\U0000255E");
static constexpr terminalpp::glyph const mix_lined_hleft_tee("\U0000255F");
static constexpr terminalpp::glyph const mix_lined_vbottom_tee("\U00002567");
static constexpr terminalpp::glyph const mix_lined_hbottom_tee("\U00002568");
static constexpr terminalpp::glyph const mix_lined_vtop_tee("\U00002564");
static constexpr terminalpp::glyph const mix_lined_htop_tee("\U00002565");
static constexpr terminalpp::glyph const mix_lined_vbottom_left_corner("\U00002559");
static constexpr terminalpp::glyph const mix_lined_hbottom_left_corner("\U0000255A");
static constexpr terminalpp::glyph const mix_lined_vtop_left_corner("\U00002552");
static constexpr terminalpp::glyph const mix_lined_htop_left_corner("\U00002553");
static constexpr terminalpp::glyph const mix_lined_hcross("\U0000256B");
static constexpr terminalpp::glyph const mix_lined_vcross("\U0000256A");

// Other drawing characters.
static constexpr terminalpp::glyph const left_guillemet("\U000000AB");
static constexpr terminalpp::glyph const right_guillemet("\U000000BB");

}}

#endif

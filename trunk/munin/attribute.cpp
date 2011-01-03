// ==========================================================================
// Munin ANSI Attribute Constants.
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
#include "munin/attribute.hpp"
#include "odin/ansi/protocol.hpp"
#include <iostream>

namespace munin {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
attribute::attribute()
    : foreground_colour(odin::ansi::graphics::COLOUR_DEFAULT)
    , background_colour(odin::ansi::graphics::COLOUR_DEFAULT)
    , intensity(odin::ansi::graphics::INTENSITY_NORMAL)
    , underlining(odin::ansi::graphics::UNDERLINING_NOT_UNDERLINED)
    , polarity(odin::ansi::graphics::POLARITY_POSITIVE)
    , character_set(odin::ansi::character_set::CHARACTER_SET_G0)
    , locale(odin::ansi::character_set::LOCALE_US_ASCII)
{
}

bool operator==(attribute const &lhs, attribute const &rhs)
{
    return lhs.foreground_colour == rhs.foreground_colour
        && lhs.background_colour == rhs.background_colour
        && lhs.intensity         == rhs.intensity
        && lhs.underlining       == rhs.underlining
        && lhs.polarity          == rhs.polarity
        && lhs.locale            == rhs.locale
        && lhs.character_set     == rhs.character_set;
}

std::ostream &operator<<(std::ostream &out, attribute const &attr)
{
    out << "attr[";

    // TODO: add attributes.

    out << "]";

    return out;
}

}


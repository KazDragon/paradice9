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
#include <boost/typeof/typeof.hpp>
#include <iostream>

using namespace boost;

namespace munin {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
attribute::attribute()
    : foreground_colour_(odin::ansi::graphics::colour::default_)
    , background_colour_(odin::ansi::graphics::colour::default_)
    , intensity_(odin::ansi::graphics::intensity::normal)
    , underlining_(odin::ansi::graphics::underlining::not_underlined)
    , polarity_(odin::ansi::graphics::polarity::positive)
    , blinking_(odin::ansi::graphics::blinking::steady)
{
}

// ==========================================================================
// EQUALITY OPERATOR
// ==========================================================================
bool operator==(attribute const &lhs, attribute const &rhs)
{
    return lhs.foreground_colour_ == rhs.foreground_colour_
        && lhs.background_colour_ == rhs.background_colour_
        && lhs.intensity_         == rhs.intensity_
        && lhs.underlining_       == rhs.underlining_
        && lhs.polarity_          == rhs.polarity_
        && lhs.blinking_          == rhs.blinking_;
}

// ==========================================================================
// EQUALITY_OPERATOR
// ==========================================================================
bool operator==(attribute::colour const &lhs, attribute::colour const &rhs)
{
    return lhs.value_ == rhs.value_;
}

// ==========================================================================
// INEQUALITY_OPERATOR
// ==========================================================================
bool operator!=(attribute::colour const &lhs, attribute::colour const &rhs)
{
    return !(lhs == rhs);
}

// ==========================================================================
// EQUALITY_OPERATOR
// ==========================================================================
bool operator==(
    attribute::low_colour const &lhs
  , attribute::low_colour const &rhs)
{
    return lhs.value_ == rhs.value_;
}

// ==========================================================================
// EQUALITY_OPERATOR
// ==========================================================================
bool operator==(
    attribute::high_colour const &lhs
  , attribute::high_colour const &rhs)
{
    return lhs.red_   == rhs.red_
        && lhs.green_ == rhs.green_
        && lhs.blue_  == rhs.blue_;
}

// ==========================================================================
// EQUALITY_OPERATOR
// ==========================================================================
bool operator==(
    attribute::greyscale_colour const &lhs
  , attribute::greyscale_colour const &rhs)
{
    return lhs.shade_ == rhs.shade_;
}

// ==========================================================================
// STREAM OPERATOR
// ==========================================================================
std::ostream &operator<<(std::ostream &out, attribute const &attr)
{
    out << "attr[";

    // TODO: add attributes.

    out << "]";

    return out;
}

}


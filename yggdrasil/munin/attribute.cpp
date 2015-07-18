// ==========================================================================
// Munin Attribute.
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
#include "yggdrasil/munin/attribute.hpp"
#include "yggdrasil/bragi/ansi/protocol.hpp"
#include <iostream>
#include <utility>

namespace yggdrasil { namespace munin {

// ==========================================================================
// DEFAULT CONSTRUCTOR
// ==========================================================================
attribute::attribute()
  : attribute(yggdrasil::graphics::colour::default_)
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
attribute::attribute(const attribute::colour& foreground_colour)
  : attribute(foreground_colour, yggdrasil::graphics::colour::default_)
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
attribute::attribute(const yggdrasil::graphics::colour& foreground_colour)
  : attribute(foreground_colour, yggdrasil::graphics::colour::default_)
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
attribute::attribute(
    const yggdrasil::graphics::colour& foreground_colour, 
    const yggdrasil::graphics::colour& background_colour)
  : attribute(
        attribute::colour(foreground_colour), 
        attribute::colour(background_colour))
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
attribute::attribute(
    const attribute::colour& foreground_colour, 
    const attribute::colour& background_colour)
  : foreground_colour_(foreground_colour),
    background_colour_(background_colour),
    intensity_(yggdrasil::graphics::intensity::normal),
    underlining_(yggdrasil::graphics::underlining::not_underlined),
    polarity_(yggdrasil::graphics::polarity::positive),
    blinking_(yggdrasil::graphics::blinking::steady)
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
// INEQUALITY OPERATOR
// ==========================================================================
bool operator!=(attribute const &lhs, attribute const &rhs)
{
    return !(lhs == rhs);
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
// STREAM OPERATOR(attribute)
// ==========================================================================
std::ostream &operator<<(std::ostream &out, attribute const &attr)
{
    out << "attr["
        << "foreground_colour=" << attr.foreground_colour_ << ","
        << "background_colour=" << attr.background_colour_ << ","
        << "intensity=" << attr.intensity_ << ","
        << "polarity=" << attr.polarity_ << ","
        << "blinking=" << attr.blinking_
        << "]";
    
    return out;
}

// ==========================================================================
// STREAM OPERATOR(colour)
// ==========================================================================
std::ostream &operator<<(std::ostream &out, attribute::colour const &col)
{
    out << "colour[" << col.value_ << "]";
    
    return out;
}

// ==========================================================================
// STREAM OPERATOR(high_colour)
// ==========================================================================
std::ostream &operator<<(std::ostream &out, attribute::high_colour const &hcol)
{
    out << "high_colour["
        << "r:" << int(hcol.red_) << "|"
        << "g:" << int(hcol.green_) << "|"
        << "b:" << int(hcol.blue_) << "]";

    return out;
}

// ==========================================================================
// STREAM OPERATOR(low_colour)
// ==========================================================================
std::ostream &operator<<(std::ostream &out, attribute::low_colour const &lcol)
{
    out << "low_colour[" << lcol.value_ << "]";
    return out;
}

// ==========================================================================
// STREAM_OPERATOR(greyscale_colour)
// ==========================================================================
std::ostream &operator<<(
    std::ostream &out, 
    attribute::greyscale_colour const &gcol)
{
    out << "greyscale_colour[" << int(gcol.shade_) << "]";
    return out;
}

}}


// ==========================================================================
// Munin ANSI Attribute.
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
#ifndef MUNIN_ANSI_ATTRIBUTE_HPP_
#define MUNIN_ANSI_ATTRIBUTE_HPP_

#include "munin/export.hpp"
#include "odin/core.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/variant.hpp>
#include <iosfwd>

namespace munin {

//* =========================================================================
/// \brief A structure that carries around the presentation attributes of
/// an ANSI element.
//* =========================================================================
struct MUNIN_EXPORT attribute
{
    // Structure representing a normal ANSI 16-colour value
    struct low_colour
    {
        low_colour()
            : low_colour(odin::ansi::graphics::colour::white)
        {
        }

        low_colour(char value)
          : low_colour(odin::ansi::graphics::colour(value))
        {
        }

        low_colour(odin::ansi::graphics::colour colour)
          : value_(colour)
        {
        };

        odin::ansi::graphics::colour value_;
    };

    // Structure representing the central 216 colours of a 256-colour palette
    struct high_colour
    {
        high_colour()
            : red_(255)
            , green_(255)
            , blue_(255)
        {
        }

        high_colour(odin::u8 red, odin::u8 green, odin::u8 blue)
            : red_(red)
            , green_(green)
            , blue_(blue)
        {
        }

        odin::u8 red_;
        odin::u8 green_;
        odin::u8 blue_;
    };

    // Structure representing the 24 greyscale tones of a 256-colour palette
    struct greyscale_colour
    {
        greyscale_colour()
            : shade_(23)
        {
        }

        explicit greyscale_colour(odin::u8 shade)
            : shade_(shade)
        {
        }

        odin::u8 shade_;
    };

    // A holder for the colours with a default initialisation.
    struct colour
    {
        typedef boost::variant
        <
            low_colour, high_colour, greyscale_colour
        > colour_variant;

        colour()
            : value_(low_colour(odin::ansi::graphics::colour::white))
        {
        }

        colour(odin::ansi::graphics::colour const &value)
            : value_(low_colour(value))
        {
        }

        colour(colour const &other)
            : value_(other.value_)
        {
        }

        colour(colour_variant const &value)
            : value_(value)
        {
        }

        colour(high_colour const &col)
            : value_(colour_variant(col))
        {
        }

        colour(greyscale_colour const &col)
            : value_(colour_variant(col))
        {
        }

        colour_variant value_;
    };

    //* =====================================================================
    /// \brief Initialises the attribute to having the default colour,
    /// no intensity, no underlining, and normal polarity.
    //* =====================================================================
    attribute();

    // Graphics Attributes
    colour                            foreground_colour_;
    colour                            background_colour_;
    odin::ansi::graphics::intensity   intensity_;
    odin::ansi::graphics::underlining underlining_;
    odin::ansi::graphics::polarity    polarity_;
    odin::ansi::graphics::blinking    blinking_;
};

bool operator==(attribute const &lhs, attribute const &rhs);
bool operator==(attribute::colour const &lhs, attribute::colour const &rhs);
bool operator!=(attribute::colour const &lhs, attribute::colour const &rhs);
bool operator==(
    attribute::low_colour const &lhs
  , attribute::low_colour const &rhs);
bool operator==(
    attribute::high_colour const &lhs
  , attribute::high_colour const &rhs);
bool operator==(
    attribute::greyscale_colour const &lhs
  , attribute::greyscale_colour const &rhs);

std::ostream &operator<<(std::ostream &out, attribute const &attr);

}

#endif

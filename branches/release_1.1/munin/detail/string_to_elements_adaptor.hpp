// ==========================================================================
// Munin string-to-elements Parser Adaptation Structures.
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
#ifndef MUNIN_DETAIL_STRING_TO_ELEMENTS_ADAPTOR_HPP_
#define MUNIN_DETAIL_STRING_TO_ELEMENTS_ADAPTOR_HPP_

#include "odin/ansi/protocol.hpp"
#include "munin/attribute.hpp"
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant.hpp>
#include <string>

namespace munin { namespace detail {
     
// Because most of the attribute modifiers share types with each other in
// one way or another, it's necessary to make some structs to differentiate
// them in the variant.
struct default_directive
{
    // The default directive doesn't have a value, but this is required
    // in order for it to be parsed.
    char dummy_;
};

struct character_set_directive
{
    char character_set_;
};
    
struct locale_directive
{
    char locale_;
};

struct intensity_directive
{
    char intensity_;
};

struct polarity_directive
{
    char polarity_;
};

struct underlining_directive
{
    char underlining_;
};

struct foreground_colour_directive
{
    munin::attribute::colour foreground_colour_;
};

struct background_colour_directive
{
    munin::attribute::colour background_colour_;
};

// This variant is a list of all the things the parser can tell us to do.
typedef boost::variant<
    char           
  , default_directive
  , character_set_directive
  , locale_directive
  , intensity_directive
  , polarity_directive
  , underlining_directive
  , foreground_colour_directive
  , background_colour_directive
> string_to_elements_directive;
    
}}

BOOST_FUSION_ADAPT_STRUCT(
    munin::detail::default_directive
  , (char, dummy_)
)

BOOST_FUSION_ADAPT_STRUCT(
    munin::detail::character_set_directive
  , (char, character_set_)
)

BOOST_FUSION_ADAPT_STRUCT(
    munin::detail::locale_directive
  , (char, locale_)
)

BOOST_FUSION_ADAPT_STRUCT(
    munin::detail::intensity_directive
  , (char, intensity_)
)

BOOST_FUSION_ADAPT_STRUCT(
    munin::detail::polarity_directive
  , (char, polarity_)
)

BOOST_FUSION_ADAPT_STRUCT(
    munin::detail::underlining_directive
  , (char, underlining_)
)

BOOST_FUSION_ADAPT_STRUCT(
    munin::attribute::low_colour
  , (char, value_)
)

BOOST_FUSION_ADAPT_STRUCT(
    munin::attribute::high_colour
  , (odin::u8, red_)
    (odin::u8, green_)
    (odin::u8, blue_)
)

BOOST_FUSION_ADAPT_STRUCT(
    munin::attribute::greyscale_colour
  , (odin::u8, shade_)
)

BOOST_FUSION_ADAPT_STRUCT(
    munin::attribute::colour
  , (munin::attribute::colour::colour_variant, value_)
)

BOOST_FUSION_ADAPT_STRUCT(
    munin::detail::foreground_colour_directive
  , (munin::attribute::colour, foreground_colour_)
)

BOOST_FUSION_ADAPT_STRUCT(
    munin::detail::background_colour_directive
  , (munin::attribute::colour, background_colour_)
)

#endif


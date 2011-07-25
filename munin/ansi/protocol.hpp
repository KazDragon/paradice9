// ==========================================================================
// Munin ANSI Protocol Constants.
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
#ifndef MUNIN_ANSI_PROTOCOL_HPP_
#define MUNIN_ANSI_PROTOCOL_HPP_

#include "munin/types.hpp"
#include "odin/runtime_array.hpp"
#include <string>

namespace munin { namespace ansi {

//* =========================================================================
/// \brief Creates an array of array of elements from an array of strings.
//* =========================================================================
odin::runtime_array< odin::runtime_array<element_type> > 
    elements_from_strings(odin::runtime_array<std::string> const &strings
  , attribute const &attr = attribute());

//* =========================================================================
/// \brief Converts a string into a line of runtime_array<> of ANSI elements.
/// \param source_line a string of text to be converted
/// \param attr the default attribute given to each element.
//* =========================================================================
odin::runtime_array<munin::element_type> elements_from_string(
    std::string const &source_line
  , attribute const &attr = attribute());

//* =========================================================================
/// \brief Converts a runtime_array<> of ANSI elements into a string.
//* =========================================================================
std::string string_from_elements(
    odin::runtime_array<munin::element_type> const &elements);

//* =========================================================================
/// \brief Enables mouse tracking.
//* =========================================================================
std::string enable_mouse_tracking();

//* =========================================================================
/// \brief Hides the cursor.
//* =========================================================================
std::string hide_cursor();

//* =========================================================================
/// \brief Shows the cursor.
//* =========================================================================
std::string show_cursor();

//* =========================================================================
/// \brief Constructs a string that is used to move to a particular
/// co-ordinate.  The passed in point is expected to be 0-based, while the
/// ANSI result is 1-based.
//* =========================================================================
std::string cursor_position(munin::point const &position);

//* =========================================================================
/// \brief Constructs a string that is used to set the window title to the 
/// specified text.
//* =========================================================================
std::string set_window_title(std::string const &text);

//* =========================================================================
/// \brief Sets the 'normal cursor keys' mode.
//* =========================================================================
std::string set_normal_cursor_keys();

//* =========================================================================
/// \brief Returns a colour value for a high-colour RGB value.
//* =========================================================================
std::string colour_string(munin::attribute::high_colour const &colour);

//* =========================================================================
/// \brief Returns a colour value for a greyscale value.
//* =========================================================================
std::string colour_string(munin::attribute::greyscale_colour const &colour);

}}

#endif


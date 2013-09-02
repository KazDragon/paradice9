// ==========================================================================
// Munin ANSI Protocol.
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
#include "munin/ansi/protocol.hpp"
#include "odin/ansi/protocol.hpp"
#include "odin/ascii/protocol.hpp"
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <algorithm>
#include <cstdio>

using namespace std;
using namespace boost;
using namespace odin;

namespace munin { namespace ansi {
    
// ==========================================================================
// ELEMENTS_FROM_STRINGS
// ==========================================================================
vector< vector<munin::element_type> > 
    elements_from_strings(vector<string> const &strings
  , attribute const &attr)
{
    vector< vector<munin::element_type> > elements(strings.size());

    transform(
        strings.begin(), strings.end()
      , elements.begin()
      , bind(elements_from_string, _1, attr));
    
    return elements;
}

// ==========================================================================
// ELEMENTS_FROM_STRING
// ==========================================================================
vector<element_type> elements_from_string(
    string const    &source_line
  , attribute const &attr)
{
    vector<munin::element_type> dest_line(source_line.size());
    
    for (u32 index = 0; index < source_line.size(); ++index)
    {
        dest_line[index] = munin::element_type(source_line[index], attr);
    }
    
    return dest_line;
}

// ==========================================================================
// STRING_FROM_ELEMENTS
// ==========================================================================
string string_from_elements(
    vector<munin::element_type> const &elements)
{
    string text(elements.size(), ' ');
    
    for (u32 index = 0; index < elements.size(); ++index)
    {
        text[index] = elements[index].glyph_.character_;
    }
    
    return text;
}

// ==========================================================================
// ENABLE_MOUSE_TRACKING
// ==========================================================================
string enable_mouse_tracking()
{
    return string()
        + odin::ansi::ESCAPE
        + odin::ansi::CONTROL_SEQUENCE_INTRODUCER
        + odin::ansi::PRIVATE_MODE_SET
        + "1000"
        + odin::ascii::LOWERCASE_H;
}

// ==========================================================================
// HIDE_CURSOR
// ==========================================================================
string hide_cursor()
{
    return string()
        + odin::ansi::ESCAPE
        + odin::ansi::CONTROL_SEQUENCE_INTRODUCER
        + odin::ansi::PRIVATE_MODE_SET
        + "25"
        + odin::ascii::LOWERCASE_L;
}

// ==========================================================================
// SHOW_CURSOR
// ==========================================================================
string show_cursor()
{
    return string()
        + odin::ansi::ESCAPE
        + odin::ansi::CONTROL_SEQUENCE_INTRODUCER
        + odin::ansi::PRIVATE_MODE_SET
        + "25"
        + odin::ascii::LOWERCASE_H;
}

// ==========================================================================
// CURSOR_POSITION
// ==========================================================================    
string cursor_position(munin::point const &position)
{
    // We would like to take advantage of some of the space-saving features
    // of ANSI here, but Ubuntu Telnet chokes on them.  So we always use the
    // long-hand version.

    // TODO: replace with a Spirit variant for safety.
    char buffer[2+10+1+10+1+1];
    sprintf(
        buffer
      , "%c%c%d;%d%c"
      , odin::ansi::ESCAPE
      , odin::ansi::CONTROL_SEQUENCE_INTRODUCER
      , (position.y + 1)
      , (position.x + 1)
      , odin::ansi::CURSOR_POSITION);

    return buffer;
}

// ==========================================================================
// SET_WINDOW_TITLE
// ==========================================================================
string set_window_title(string const &text)
{
    return str(format("%c%c0;%s%c")
        % odin::ansi::ESCAPE
        % odin::ansi::OPERATING_SYSTEM_COMMAND
        % text
        % odin::ansi::BEL);
}

// ==========================================================================
// SET_NORMAL_CURSOR_KEYS
// ==========================================================================
string set_normal_cursor_keys()
{
    return str(format("%c%c%c1%c")
         % odin::ansi::ESCAPE
         % odin::ansi::CONTROL_SEQUENCE_INTRODUCER
         % odin::ansi::PRIVATE_MODE_SET
         % odin::ansi::DECRST);
}

// ==========================================================================
// COLOUR_STRING
// ==========================================================================
string colour_string(attribute::high_colour const &colour)
{
    return str(format("%d")
        % ((colour.red_ * 36) + (colour.green_ * 6) + colour.blue_ + 16)); 
}

// ==========================================================================
// COLOUR_STRING
// ==========================================================================
string colour_string(attribute::greyscale_colour const &colour)
{
    return str(format("%d")% (colour.shade_ + 232));
}

// ==========================================================================
// CLEAR_SCREEN
// ==========================================================================
string clear_screen()
{
    return str(format("%c%c2%c")
        % odin::ansi::ESCAPE
        % odin::ansi::CONTROL_SEQUENCE_INTRODUCER
        % odin::ansi::ERASE_DATA);
}

}}


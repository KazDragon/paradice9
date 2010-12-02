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
#include <boost/format.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace odin;

namespace munin { namespace ansi {
    
// ==========================================================================
// ELEMENTS_FROM_STRING
// ==========================================================================
runtime_array<element_type> elements_from_string(
    string const &source_line
  , attribute     attr)
{
    runtime_array<munin::ansi::element_type> dest_line(source_line.size());
    
    for (u32 index = 0; index < source_line.size(); ++index)
    {
        dest_line[index] = munin::ansi::element_type(
            source_line[index]
          , attr);
    }
    
    return dest_line;
}

// ==========================================================================
// STRING_FROM_ELEMENTS
// ==========================================================================
string string_from_elements(
    runtime_array<munin::ansi::element_type> const &elements)
{
    string text(elements.size(), ' ');
    
    for (u32 index = 0; index < elements.size(); ++index)
    {
        text[index] = elements[index].first;
    }
    
    return text;
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
    // First work out the co-ordinate representations.  These are a bit funky 
    // because of three things:
    // 1) ANSI co-ordinates are 1-based, so a point of (1,0) is actually (2,1)
    // 2) A co-ordinate position of 1 (ANSI) can be omitted.
    // 3) If the second co-ordinate position can be omitted, then so can the
    //    semi-colon that separates the arguments.
    
    // In the interest of brevity, we should take advantage of this.

    string x_coordinate = 
        position.x == 0 
      ? string("")
      : str(format("%s") % (position.x + 1));
      
    string y_coordinate = 
        position.y == 0 
      ? string("")
      : str(format("%s") % (position.y + 1));
                    
    string separator =
        (position.x == 0)
      ? ""
      : ";";
      
    return str(format("%c%c%s%s%s%c")
        % odin::ansi::ESCAPE
        % odin::ansi::CONTROL_SEQUENCE_INTRODUCER
        % y_coordinate
        % separator
        % x_coordinate
        % odin::ansi::CURSOR_POSITION);
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

}}


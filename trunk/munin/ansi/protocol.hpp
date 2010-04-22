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

#include "odin/types.hpp"
#include <boost/utility.hpp>
#include <string>

namespace munin { namespace ansi {

    // The generic formula for an ANSI escape code is:
    //   ESCAPE '[' [arg [;arg...]] COMMAND
    // where ESCAPE is character 27, the arguments are usually numeric and
    // separated by semi-colons, and COMMAND is a single letter that states
    // the purpose of the sequence.
    
    static char const ESCAPE        = '\x1B';
    static char const ANSI_SEQUENCE = '[';
    
    // Move the cursor up N rows.
    static char const CURSOR_UP                        = 'A';
    // Move the cursor down N rows.
    static char const CURSOR_DOWN                      = 'B';
    // Move the cursor right N columns.
    static char const CURSOR_FORWARD                   = 'C';
    // Move the cursor left N columns.
    static char const CURSOR_BACKWARD                  = 'D';
    
    // Move the cursor to the beginning of the line that is N lines down.
    static char const CURSOR_NEXT_LINE                 = 'E';
    // Move the cursor to the beginning of the line that is N lines up.
    static char const CURSOR_PREVIOUS_LINE             = 'F';
    
    // Move to a specific column.
    static char const CURSOR_HORIZONTAL_ABSOLUTE       = 'G';
    // Move to a specific cursor position (row,column).
    static char const CURSOR_POSITION                  = 'H';

    // Erase data.  
    //  0 - (Default) Clear from the cursor to the end of the screen.
    //  1 - Clear from the cursor to the beginning of the screen.
    //  2 - Clear the entire screen (cursor position is indeterminate).
    static char const ERASE_DATA                       = 'J';
    
    // Erase line
    //  0 - (Default) Clear from the cursor to the end of the line.
    //  1 - Clear from the cursor to the beginning of the line.
    //  2 - Clear entire line (cursor position remains the same).
    static char const ERASE_LINE                       = 'K';
    
    // Scroll up by N lines
    static char const SCROLL_UP                        = 'S';
    
    // Scroll down by N lines
    static char const SCROLL_DOWN                      = 'T';
    
    // Horizontal and Vertical position - as Cursor Position
    static char const HORIZONTAL_AND_VERTICAL_POSITION = 'f';

    // Select graphics rendition - see graphics namespace
    static char const SELECT_GRAPHICS_RENDITION        = 'm';
    
    // Device status report - request requires parameter of 6, response
    // requires parameters of row,column.
    static char const DEVICE_STATUS_REPORT             = 'n';
    static char const DEVICE_STATUS_REPORT_REPLY       = 'R';
    
    // Save cursor position
    static char const SAVE_CURSOR_POSITION             = 's';
    
    // Restore (unsave) cursor
    static char const RESTORE_CURSOR_POSITION          = 'u';
    
// Namespace for Select Graphics Rendition parameters
namespace graphics {

    static char const NO_ATTRIBUTES          = 0;
    
    typedef char intensity;
    static char const INTENSITY_BOLD         = 1;
    static char const INTENSITY_FAINT        = 2;
    static char const INTENSITY_NORMAL       = 13;

    static char const FOREGROUND_COLOUR_BASE = 30;
    static char const BACKGROUND_COLOUR_BASE = 40;
    
    typedef char colour;
    static colour const COLOUR_BLACK         = 0;
    static colour const COLOUR_RED           = 1;
    static colour const COLOUR_GREEN         = 2;
    static colour const COLOUR_YELLOW        = 3;
    static colour const COLOUR_BLUE          = 4;
    static colour const COLOUR_MAGENTA       = 5;
    static colour const COLOUR_CYAN          = 6;
    static colour const COLOUR_WHITE         = 7;
    static colour const COLOUR_DEFAULT       = 9;
}

}}

#endif

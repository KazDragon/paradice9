// ==========================================================================
// Odin ANSI Protocol.
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
#ifndef ODIN_ANSI_PROTOCOL_HPP_
#define ODIN_ANSI_PROTOCOL_HPP_

#include "odin/types.hpp"
#include "odin/ascii/protocol.hpp"
#include <boost/utility.hpp>
#include <string>

namespace odin { namespace ansi {
    static char const BEL                              = 0x07;

    // The generic formula for an ANSI escape code is:
    //   ESCAPE '[' [arg [;arg...]] COMMAND
    // where ESCAPE is character 27, the arguments are usually numeric and
    // separated by semi-colons, and COMMAND is a single letter that states
    // the purpose of the sequence.
    
    static char const ESCAPE                           = odin::ascii::ESC;
    static char const CONTROL_SEQUENCE_INTRODUCER      = odin::ascii::OPEN_SQUARE_BRACKET; 
    static char const OPERATING_SYSTEM_COMMAND         = odin::ascii::CLOSE_SQUARE_BRACKET;
    static char const PARAMETER_SEPARATOR              = odin::ascii::SEMI_COLON;
    
    // Move the cursor up N rows.
    static char const CURSOR_UP                        = odin::ascii::UPPERCASE_A;
    // Move the cursor down N rows.
    static char const CURSOR_DOWN                      = odin::ascii::UPPERCASE_B;
    // Move the cursor right N columns.
    static char const CURSOR_FORWARD                   = odin::ascii::UPPERCASE_C;
    // Move the cursor left N columns.
    static char const CURSOR_BACKWARD                  = odin::ascii::UPPERCASE_D;
    
    // Move the cursor to the beginning of the line that is N lines down.
    static char const CURSOR_NEXT_LINE                 = odin::ascii::UPPERCASE_E;
    // Move the cursor to the beginning of the line that is N lines up.
    static char const CURSOR_PREVIOUS_LINE             = odin::ascii::UPPERCASE_F;
    
    // Move to a specific column.
    static char const CURSOR_HORIZONTAL_ABSOLUTE       = odin::ascii::UPPERCASE_G;
    // Move to a specific cursor position (row,column).
    static char const CURSOR_POSITION                  = odin::ascii::UPPERCASE_H;

    // Erase data.  
    //  0 - (Default) Clear from the cursor to the end of the screen.
    //  1 - Clear from the cursor to the beginning of the screen.
    //  2 - Clear the entire screen (cursor position is indeterminate).
    static char const ERASE_DATA                       = odin::ascii::UPPERCASE_J;
    
    // Erase line
    //  0 - (Default) Clear from the cursor to the end of the line.
    //  1 - Clear from the cursor to the beginning of the line.
    //  2 - Clear entire line (cursor position remains the same).
    static char const ERASE_LINE                       = odin::ascii::UPPERCASE_K;
    
    // Scroll up by N lines
    static char const SCROLL_UP                        = odin::ascii::UPPERCASE_S;
    
    // Scroll down by N lines
    static char const SCROLL_DOWN                      = odin::ascii::UPPERCASE_T;
    
    // Horizontal and Vertical position - as Cursor Position
    static char const HORIZONTAL_AND_VERTICAL_POSITION = odin::ascii::LOWERCASE_F;

    // Select graphics rendition - see graphics namespace
    static char const SELECT_GRAPHICS_RENDITION        = odin::ascii::LOWERCASE_M;
    
    // Device status report - request requires parameter of 6, response
    // requires parameters of row,column.
    static char const DEVICE_STATUS_REPORT             = odin::ascii::LOWERCASE_N;
    static char const DEVICE_STATUS_REPORT_REPLY       = odin::ascii::UPPERCASE_R;
    
    // Save cursor position
    static char const SAVE_CURSOR_POSITION             = odin::ascii::LOWERCASE_S;
    
    // Restore (unsave) cursor
    static char const RESTORE_CURSOR_POSITION          = odin::ascii::LOWERCASE_U;
    
    // Erase Characters
    static char const ERASE_CHARACTER                  = odin::ascii::UPPERCASE_X;
    
    // Tab backwards
    static char const CURSOR_BACKWARD_TABULATION       = odin::ascii::UPPERCASE_Z;
    
    // A number of undocumented extensions exist for PuTTY and other terminals
    static char const PUTTY_EXTENSION                  = odin::ascii::TILDE;    

// Namespace for Select Graphics Rendition parameters
namespace graphics {

    static char const NO_ATTRIBUTES          = 0;
    
    typedef char intensity;
    static char const INTENSITY_BOLD         = 1;
    static char const INTENSITY_FAINT        = 2;
    static char const INTENSITY_NORMAL       = 22;

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

namespace character_set {
    static char const LOCALE_DEC                   = odin::ascii::ZERO;
    static char const LOCALE_UK                    = odin::ascii::UPPERCASE_A;
    static char const LOCALE_US_ASCII              = odin::ascii::UPPERCASE_B;
    
    static char const CHARACTER_SET_G0             = odin::ascii::OPEN_BRACKET;
    static char const CHARACTER_SET_G1             = odin::ascii::CLOSE_BRACKET;
}

// For ANSI sequences of the format: ESC[x;y;zC (e.g. "ESC[10;2H"
// In the above example, "[" is the initiator, "H" is the command, and
// "10;2" is the arguments.
struct control_sequence
{
    std::string arguments_;
    bool        meta_;      
    char        initiator_;
    char        command_;
};

bool operator==(control_sequence const& lhs, control_sequence const& rhs);

}}

#endif


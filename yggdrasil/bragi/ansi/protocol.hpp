// ==========================================================================
// Yggdrasil Bragi ANSI Protocol.
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
#ifndef YGGDRASIL_BRAGI_ANSI_PROTOCOL_HPP_
#define YGGDRASIL_BRAGI_ANSI_PROTOCOL_HPP_

#include "yggdrasil/types.hpp"
#include "yggdrasil/bragi/ascii/protocol.hpp"
#include <iosfwd>
#include <string>

namespace yggdrasil { inline namespace bragi { inline namespace ansi {
    static char const BEL                              = 0x07;

    // The generic formula for an ANSI escape code is:
    //   ESCAPE '[' [arg [;arg...]] COMMAND
    // where ESCAPE is character 27, the arguments are usually numeric and
    // separated by semi-colons, and COMMAND is a single letter that states
    // the purpose of the sequence.

    static char const ESCAPE                           = yggdrasil::ESC;
    static char const CONTROL_SEQUENCE_INTRODUCER      = yggdrasil::OPEN_SQUARE_BRACKET;
    static char const OPERATING_SYSTEM_COMMAND         = yggdrasil::CLOSE_SQUARE_BRACKET;
    static char const PRIVATE_MODE_SET                 = yggdrasil::QUESTION_MARK;
    static char const PARAMETER_SEPARATOR              = yggdrasil::SEMI_COLON;
    static char const SINGLE_SHIFT_SELECT_G2           = yggdrasil::UPPERCASE_N;
    static char const SINGLE_SHIFT_SELECT_G3           = yggdrasil::UPPERCASE_O;

    // Move the cursor up N rows.
    static char const CURSOR_UP                        = yggdrasil::UPPERCASE_A;
    // Move the cursor down N rows.
    static char const CURSOR_DOWN                      = yggdrasil::UPPERCASE_B;
    // Move the cursor right N columns.
    static char const CURSOR_FORWARD                   = yggdrasil::UPPERCASE_C;
    // Move the cursor left N columns.
    static char const CURSOR_BACKWARD                  = yggdrasil::UPPERCASE_D;

    // Move the cursor to the beginning of the line that is N lines down.
    static char const CURSOR_NEXT_LINE                 = yggdrasil::UPPERCASE_E;
    // Move the cursor to the beginning of the line that is N lines up.
    static char const CURSOR_PREVIOUS_LINE             = yggdrasil::UPPERCASE_F;

    // Move to a specific column.
    static char const CURSOR_HORIZONTAL_ABSOLUTE       = yggdrasil::UPPERCASE_G;
    // Move to a specific cursor position (row,column).
    static char const CURSOR_POSITION                  = yggdrasil::UPPERCASE_H;

    // Erase data.
    //  0 - (Default) Clear from the cursor to the end of the screen.
    //  1 - Clear from the cursor to the beginning of the screen.
    //  2 - Clear the entire screen (cursor position is indeterminate).
    static char const ERASE_DATA                       = yggdrasil::UPPERCASE_J;

    // Erase line
    //  0 - (Default) Clear from the cursor to the end of the line.
    //  1 - Clear from the cursor to the beginning of the line.
    //  2 - Clear entire line (cursor position remains the same).
    static char const ERASE_LINE                       = yggdrasil::UPPERCASE_K;

    // Scroll up by N lines
    static char const SCROLL_UP                        = yggdrasil::UPPERCASE_S;

    // Scroll down by N lines
    static char const SCROLL_DOWN                      = yggdrasil::UPPERCASE_T;

    // Horizontal and Vertical position - as Cursor Position
    static char const HORIZONTAL_AND_VERTICAL_POSITION = yggdrasil::LOWERCASE_F;

    // Select graphics rendition - see graphics namespace
    static char const SELECT_GRAPHICS_RENDITION        = yggdrasil::LOWERCASE_M;

    // Device status report - request requires parameter of 6, response
    // requires parameters of row,column.
    static char const DEVICE_STATUS_REPORT             = yggdrasil::LOWERCASE_N;
    static char const DEVICE_STATUS_REPORT_REPLY       = yggdrasil::UPPERCASE_R;

    // Save cursor position
    static char const SAVE_CURSOR_POSITION             = yggdrasil::LOWERCASE_S;

    // Restore (unsave) cursor
    static char const RESTORE_CURSOR_POSITION          = yggdrasil::LOWERCASE_U;

    // Erase Characters
    static char const ERASE_CHARACTER                  = yggdrasil::UPPERCASE_X;

    // Tab backwards
    static char const CURSOR_BACKWARD_TABULATION       = yggdrasil::UPPERCASE_Z;

    // Keypad buttons / Function keys
    static char const KEYPAD_FUNCTION                  = yggdrasil::TILDE;

    // DEC Private Mode Set
    static char const DECSET                           = yggdrasil::LOWERCASE_H;

    // DEC Private Mode Reset
    static char const DECRST                           = yggdrasil::LOWERCASE_L;

// Namespace for Select Graphics Rendition parameters
namespace graphics {

    static char const NO_ATTRIBUTES                     = 0;

    enum class intensity : char
    {
        bold                                            = 1,
        faint                                           = 2,
        normal                                          = 22,
    };

    std::ostream &operator<<(std::ostream &out, intensity const &in);
    
    enum class underlining : char
    {
        underlined                                      = 4,
        not_underlined                                  = 24,
    };

    std::ostream &operator<<(std::ostream &out, underlining const &un);
    
    enum class blinking : char
    {
        blink                                           = 5,
        steady                                          = 25,
    };

    std::ostream &operator<<(std::ostream &out, blinking const &blink);
    
    static char const FOREGROUND_COLOUR_BASE            = 30;
    static char const BACKGROUND_COLOUR_BASE            = 40;

    enum class colour : char
    {
        black                                           = 0,
        red                                             = 1,
        green                                           = 2,
        yellow                                          = 3,
        blue                                            = 4,
        magenta                                         = 5,
        cyan                                            = 6,
        white                                           = 7,
        default_                                        = 9,
    };
    
    std::ostream &operator<<(std::ostream &out, colour const &col);

    enum class polarity : char
    {
        negative                                        = 7,
        positive                                        = 27,
    };
    
    std::ostream &operator<<(std::ostream &out, polarity const &pol);
}

namespace character_set {
    // Note: For extended ASCII, both PuTTY and TeraTerm support
    // SCO+G0.
    static char const LOCALE_DEC                   = yggdrasil::ZERO;
    static char const LOCALE_UK                    = yggdrasil::UPPERCASE_A;
    static char const LOCALE_US_ASCII              = yggdrasil::UPPERCASE_B;
    static char const LOCALE_SCO                   = yggdrasil::UPPERCASE_U;

    static char const CHARACTER_SET_G0             = yggdrasil::OPEN_BRACKET;
    static char const CHARACTER_SET_G1             = yggdrasil::CLOSE_BRACKET;
}

namespace ss3 {
    // specific control codes for Single Shift Select of character set G3
    static char const HOME                         = yggdrasil::UPPERCASE_H;
    static char const END                          = yggdrasil::UPPERCASE_F;
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

// Mouse
struct mouse_report
{
    static yggdrasil::u8 const LEFT_BUTTON_DOWN   = yggdrasil::u8(0);
    static yggdrasil::u8 const MIDDLE_BUTTON_DOWN = yggdrasil::u8(1);
    static yggdrasil::u8 const RIGHT_BUTTON_DOWN  = yggdrasil::u8(2);
    static yggdrasil::u8 const BUTTON_UP          = yggdrasil::u8(3);
    static yggdrasil::u8 const SCROLLWHEEL_UP     = yggdrasil::u8(32);
    static yggdrasil::u8 const SCROLLWHEEL_DOWN   = yggdrasil::u8(33);

    yggdrasil::u8  button_;
    yggdrasil::s32 x_position_;
    yggdrasil::s32 y_position_;
};

// What on earth are these doing here and why is it static?  Because Visual
// Studio 2008 has trouble recognising source files in the same project that
// have the same name.  odin/ansi/protocol.cpp, odin/telnet/protocol.cpp, etc.
inline bool operator==(control_sequence const& lhs, control_sequence const& rhs)
{
    return lhs.meta_      == rhs.meta_
        && lhs.command_   == rhs.command_
        && lhs.initiator_ == rhs.initiator_
        && lhs.arguments_ == rhs.arguments_;
}

inline bool operator==(mouse_report const &lhs, mouse_report const &rhs)
{
    return lhs.button_     == rhs.button_
        && lhs.x_position_ == rhs.x_position_
        && lhs.y_position_ == rhs.y_position_;
}

}}}


#endif


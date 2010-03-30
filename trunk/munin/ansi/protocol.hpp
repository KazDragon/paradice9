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

    static std::string const ESCAPE    = "\x1B";
    
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

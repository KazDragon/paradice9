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
#include "yggdrasil/bragi/ansi/protocol.hpp"
#include <ostream>
#include <cassert>

namespace yggdrasil { inline namespace bragi { inline namespace ansi {
    
namespace graphics {

std::ostream &operator<<(std::ostream &out, intensity const &in)
{
    switch (in)
    {
        case intensity::bold :
            out << "intensity[bold]";
            break;
            
        case intensity::faint :
            out << "intensity[faint]";
            break;
            
        case intensity::normal :
            out << "intensity[normal]";
            break;
            
        default :
            assert(!"invalid intensity");
            break;
    }
    
    return out;
}

std::ostream &operator<<(std::ostream &out, underlining const &un)
{
    switch (un)
    {
        case underlining::underlined :
            out << "underlining[underlined]";
            break;
            
        case underlining::not_underlined :
            out << "underlining[not underlined]";
            break;
            
        default :
            assert("!invalid underlining");
            break;
    }
    
    return out;
}

std::ostream &operator<<(std::ostream &out, blinking const &blink)
{
    switch (blink)
    {
        case blinking::blink :
            out << "blinking[blink]";
            break;
            
        case blinking::steady :
            out << "blinking[steady]";
            break;
            
        default :
            assert("!invalid blinking");
            break;
    }
    
    return out;
}

std::ostream &operator<<(std::ostream &out, colour const &col)
{
    static char const *colour_names[] = {
        "black",
        "red",
        "green",
        "yellow",
        "blue",
        "magenta",
        "cyan",
        "white",
        "UNUSED",
        "default",
    };
    
    char const value = static_cast<char>(col);
    
    // The value should be an array index.
    assert(value < (sizeof(colour_names) / sizeof(*colour_names)));
    
    // The value should not be the UNUSED value.
    assert(value != 8);
    
    out << "raw_colour[" << colour_names[value] << "]";
    
    return out;
}

std::ostream &operator<<(std::ostream &out, polarity const &pol)
{
    switch (pol)
    {
        case polarity::negative :
            out << "polarity[negative]";
            break;
            
        case polarity::positive :
            out << "polarity[positive]";
            break;
            
        default :
            assert(!"invalid polarity");
            break;
    }
    
    return out;
}

}

}}}

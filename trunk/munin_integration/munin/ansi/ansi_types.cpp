// ==========================================================================
// Munin ANSI Types.
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
#include "munin/ansi/ansi_types.hpp"
#include <boost/format.hpp>
#include <iostream>
#include <cctype>

namespace munin { namespace ansi {

bool operator==(element_type const &lhs, element_type const &rhs)
{
    if (lhs.first != rhs.first)
    {
        return false;
    }
    
    if (!lhs.second.is_initialized() && !rhs.second.is_initialized())
    {
        return true;
    }
    
    if (lhs.second.is_initialized() != rhs.second.is_initialized())
    {
        return false;
    }
    
    return lhs.second.get() == rhs.second.get();
}

std::ostream &operator<<(std::ostream &out, element_type const &element)
{
    out << "element['";
    
    if (std::isprint(element.first))
    {
        out << element.first;
    }
    else
    {
        out << boost::format("0x%02X") % int(element.first);
    }
    
    out << "', ";
    
    if (element.second.is_initialized())
    {
        out << element.second.get();
    }
    else
    {
        out << "(default)";
    }
    
    out << "]";
    
    return out;
}

}}

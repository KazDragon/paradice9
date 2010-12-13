// ==========================================================================
// Munin ANSI Basic Container.
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
#include "munin/ansi/basic_container.hpp"
#include "munin/canvas.hpp"
#include <boost/typeof/typeof.hpp>

namespace munin { namespace ansi {

// ==========================================================================
// DO_INITIALISE_REGION
// ==========================================================================
void basic_container::do_initialise_region(
    canvas<element_type> &cvs
  , rectangle const      &region)
{
    BOOST_AUTO(position, get_position());
    
    for (odin::s32 row = 0; row < region.size.height; ++row)
    {
        for (odin::s32 column = 0; column < region.size.width; ++column)
        {
            cvs[position.x + region.origin.x + column]
               [position.y + region.origin.y + row   ] 
                    = element_type(' ', attribute());
        }
    }
}

}}


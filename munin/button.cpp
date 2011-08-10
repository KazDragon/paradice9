// ==========================================================================
// Munin Button.
//
// Copyright (C) 2011 Matthew Chaplain, All Rights Reserved.
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
#include "munin/button.hpp"
#include "munin/basic_container.hpp"
#include "munin/framed_component.hpp"
#include "munin/grid_layout.hpp"
#include "munin/image.hpp"
#include "munin/solid_frame.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace odin;
using namespace boost;
using namespace std;

namespace munin {
    
// ==========================================================================
// BUTTON::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct button::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl()
    {
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
button::button(runtime_array<element_type> const &caption)
    : pimpl_(make_shared<impl>())
{
    BOOST_AUTO(img, make_shared<image>(caption));
    img->set_can_focus(true);
    
    get_container()->set_layout(
        make_shared<grid_layout>(1, 1));
    
    get_container()->add_component(
        make_shared<framed_component>(
            make_shared<solid_frame>()
          , img));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
button::~button()
{
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void button::do_event(any const &event)
{
    BOOST_AUTO(ch, any_cast<char>(&event));
    
    if (ch != NULL)
    {
        if (*ch == ' ' || *ch == '\n' || *ch == '\r')
        {
            on_click();
        }
    }

    BOOST_AUTO(report, any_cast<odin::ansi::mouse_report>(&event));
        
    if (report != NULL)
    {
        if (report->button_ == 0)
        {
            on_click();
        }
    }
}

}


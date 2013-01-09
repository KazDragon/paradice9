// ==========================================================================
// GuiBuilder UI
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
#include "ui.hpp"
#include "sized_block.hpp"
#include "munin/algorithm.hpp"
#include "munin/ansi/protocol.hpp"
#include "munin/compass_layout.hpp"
#include "munin/edit.hpp"
#include "munin/filled_box.hpp"
#include "munin/framed_component.hpp"
#include "munin/solid_frame.hpp"
#include "munin/tabbed_frame.hpp"
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace munin;
using namespace munin::ansi;
using namespace odin;
using namespace boost;
using namespace std;

namespace guibuilder {

ui::ui()
{
    BOOST_AUTO(container, get_container());
    container->set_layout(make_shared<compass_layout>());

    // Central - tabbed control.
    
    container->add_component(
        make_shared<framed_component>(
            make_shared<tabbed_frame>()
          , make_shared<filled_box>(element_type('x')))
      , COMPASS_LAYOUT_CENTRE);

    // Bottommost - edit control.
    BOOST_AUTO(input_field, make_shared<edit>());
    container->add_component(
        make_shared<framed_component>(
            make_shared<solid_frame>()
          , input_field)
      , COMPASS_LAYOUT_SOUTH);
}

void ui::do_event(any const &ev)
{
    char const *ch = any_cast<char>(&ev);
    
    if (ch && *ch == '\t')
    {
        focus_next();
        
        if (!has_focus())
        {
            focus_next();
        }
    }
    else
    {
        odin::ansi::control_sequence const *control_sequence = 
            any_cast<odin::ansi::control_sequence>(&ev);

        if (control_sequence)
        {
            if (control_sequence->initiator_ == odin::ansi::CONTROL_SEQUENCE_INTRODUCER
             && control_sequence->command_   == odin::ansi::CURSOR_BACKWARD_TABULATION)
            {
                focus_previous();

                if (!has_focus())
                {
                    focus_previous();
                }
            }
        }

        composite_component::do_event(ev);
    }
}

}

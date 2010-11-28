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
#include "munin/compass_layout.hpp"
#include "munin/composite_component.hpp"
#include "munin/grid_layout.hpp"
#include "munin/ansi/basic_container.hpp"
#include "munin/ansi/edit.hpp"
#include "munin/ansi/framed_component.hpp"
#include "munin/ansi/frame.hpp"
#include "munin/ansi/text_area.hpp"
#include "munin/ansi/frame.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace boost;

namespace guibuilder {

ui::ui()
    : munin::composite_component<munin::ansi::element_type>(
           make_shared<munin::ansi::basic_container>())
{
    BOOST_AUTO(container, get_container());
    
    container->set_layout(
        make_shared< munin::compass_layout<munin::ansi::element_type> >());

    container->add_component(
        make_shared<munin::ansi::framed_component>(
            make_shared<munin::ansi::frame>()
          , make_shared<munin::ansi::edit>())
      , munin::COMPASS_LAYOUT_NORTH);
    
    BOOST_AUTO(inner_container, make_shared<munin::ansi::basic_container>());
    inner_container->set_layout(
        make_shared< munin::grid_layout<munin::ansi::element_type> >(2, 1));
    
    inner_container->add_component(
        make_shared<munin::ansi::framed_component>(
            make_shared<munin::ansi::frame>()
          , make_shared<munin::ansi::text_area>()));

    inner_container->add_component(
        make_shared<munin::ansi::framed_component>(
            make_shared<munin::ansi::frame>()
          , make_shared<munin::ansi::text_area>()));

    container->add_component(
        inner_container
      , munin::COMPASS_LAYOUT_CENTRE);

    container->add_component(
        make_shared<munin::ansi::framed_component>(
            make_shared<munin::ansi::frame>()
          , make_shared<munin::ansi::edit>())
      , munin::COMPASS_LAYOUT_SOUTH);
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

        munin::composite_component<munin::ansi::element_type>::do_event(ev);
    }
}

}

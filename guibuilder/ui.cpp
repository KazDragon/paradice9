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
#include "munin/aligned_layout.hpp"
#include "munin/ansi/protocol.hpp"
#include "munin/basic_container.hpp"
#include "munin/compass_layout.hpp"
#include "munin/composite_component.hpp"
#include "munin/dropdown_list.hpp"
#include "munin/grid_layout.hpp"
#include "munin/horizontal_scroll_bar.hpp"
#include "munin/image.hpp"
#include "munin/edit.hpp"
#include "munin/framed_component.hpp"
#include "munin/list.hpp"
#include "munin/scroll_pane.hpp"
#include "munin/solid_frame.hpp"
#include "munin/text_area.hpp"
#include "munin/vertical_scroll_bar.hpp"
#include "odin/ansi/protocol.hpp"
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
    //*
    BOOST_AUTO(text_area, make_shared<munin::text_area>());
    BOOST_AUTO(scroller, make_shared<munin::scroll_pane>(text_area));

    container->set_layout(make_shared<compass_layout>());
    container->add_component(scroller, COMPASS_LAYOUT_CENTRE);

    string txt;

    txt += "G9,SCO\n";

    for (int i = 0; i < 256; ++i)
    {
        txt += str(format("0x%02X: [\\c1\\lU\\C%03d]\n") % i % i);
    }

    BOOST_AUTO(doc, text_area->get_document());
    doc->insert_text(string_to_elements(txt));

    /*/
    container->set_layout(make_shared<compass_layout>());

    vector<string> names(20);
    for (size_t index = 0; index < names.size(); ++index)
    {
        names[index] = str(format("Item #%d") % index);
    }
    
    BOOST_AUTO(list, make_shared<munin::list>());
    BOOST_AUTO(scroller, make_shared<scroll_pane>(list));

    list->set_items(elements_from_strings(names));
    list->set_item_index(0);
    
    BOOST_AUTO(dropdown_list, make_shared<munin::dropdown_list>());
    dropdown_list->set_items(elements_from_strings(names));
    dropdown_list->set_item_index(0);

    container->add_component(dropdown_list, COMPASS_LAYOUT_NORTH);
    container->add_component(scroller, COMPASS_LAYOUT_CENTRE);
    //*/
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

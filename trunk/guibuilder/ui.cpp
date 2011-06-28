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
#include "munin/aligned_layout.hpp"
#include "munin/basic_container.hpp"
#include "munin/compass_layout.hpp"
#include "munin/composite_component.hpp"
#include "munin/grid_layout.hpp"
#include "munin/horizontal_scroll_bar.hpp"
#include "munin/image.hpp"
#include "munin/edit.hpp"
#include "munin/framed_component.hpp"
#include "munin/solid_frame.hpp"
#include "munin/text_area.hpp"
#include "munin/vertical_scroll_bar.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace munin;
using namespace odin;
using namespace boost;
using namespace std;

namespace guibuilder {

static string main_image[] = {
    
    "             _"
  , "    |/ _._  | \\.__. _  _ ._/ _"
  , "    |\\(_|/_ |_/|(_|(_|(_)| |_>"
  , "                    _|"
  , " "
  , "                   ___                   ___           ___"
  , "                  / _ \\___ ________ ____/ (_)______   / _ \\"
  , "                 / ___/ _ `/ __/ _ `/ _  / / __/ -_)  \\_, /"
  , "                /_/   \\_,_/_/  \\_,_/\\_,_/_/\\__/\\__/  /___/"
  , "                                                           v1.0"
  , "                                                               "
};

BOOST_STATIC_CONSTANT(u8, SLIDER_STEP = 10);

void on_left(weak_ptr<horizontal_scroll_bar> weak_scroll_bar)
{
    BOOST_AUTO(scroll_bar, weak_scroll_bar.lock());

    if (scroll_bar != NULL)
    {
        BOOST_AUTO(slider_position, scroll_bar->get_slider_position().get());

        scroll_bar->set_slider_position(
            slider_position < SLIDER_STEP
          ? 0
          : slider_position - SLIDER_STEP);
    }

    printf("Left\n");
}

void on_right(weak_ptr<horizontal_scroll_bar> weak_scroll_bar)
{
    BOOST_AUTO(scroll_bar, weak_scroll_bar.lock());

    if (scroll_bar != NULL)
    {
        BOOST_AUTO(slider_position, scroll_bar->get_slider_position().get());

        scroll_bar->set_slider_position(
            slider_position > (100 - SLIDER_STEP)
          ? 100
          : slider_position + SLIDER_STEP);
    }

    printf("Right\n");
}

void on_down(weak_ptr<vertical_scroll_bar> weak_scroll_bar)
{
    BOOST_AUTO(scroll_bar, weak_scroll_bar.lock());

    if (scroll_bar != NULL)
    {
        BOOST_AUTO(slider_position, scroll_bar->get_slider_position().get());

        scroll_bar->set_slider_position(
            slider_position > (100 - SLIDER_STEP)
          ? 100
          : slider_position + SLIDER_STEP);
    }
    
    printf("Down\n");
}

void on_up(weak_ptr<vertical_scroll_bar> weak_scroll_bar)
{
    BOOST_AUTO(scroll_bar, weak_scroll_bar.lock());

    if (scroll_bar != NULL)
    {
        BOOST_AUTO(slider_position, scroll_bar->get_slider_position().get());

        scroll_bar->set_slider_position(
            slider_position < SLIDER_STEP
          ? 0
          : slider_position - SLIDER_STEP);
    }
    
    printf("Up\n");
}

ui::ui()
    : composite_component(make_shared<basic_container>())
{
    BOOST_AUTO(container, get_container());
    container->set_layout(make_shared<compass_layout>());

    BOOST_AUTO(hscrollbar, make_shared<horizontal_scroll_bar>());
    hscrollbar->on_page_left.connect(
        bind(on_left, weak_ptr<horizontal_scroll_bar>(hscrollbar)));
    hscrollbar->on_page_right.connect(
        bind(on_right, weak_ptr<horizontal_scroll_bar>(hscrollbar)));
    
    attribute pen;
    pen.character_set = odin::ansi::character_set::CHARACTER_SET_G0;
    pen.locale        = odin::ansi::character_set::LOCALE_SCO;
    element_type tlelement(char(201), pen);
    element_type tlelements[] = { tlelement };
    BOOST_AUTO(tlcorner, make_shared<image>(tlelements));
    
    element_type trelement(char(187), pen);
    element_type trelements[] = { trelement };
    BOOST_AUTO(trcorner, make_shared<image>(trelements));
    
    BOOST_AUTO(top_panel, make_shared<basic_container>());
    top_panel->set_layout(make_shared<compass_layout>());
    top_panel->add_component(
        tlcorner
      , COMPASS_LAYOUT_WEST);
    top_panel->add_component(
        hscrollbar
      , COMPASS_LAYOUT_CENTRE);
    top_panel->add_component(
        trcorner
      , COMPASS_LAYOUT_EAST);
    
    container->add_component(
        top_panel
      , COMPASS_LAYOUT_NORTH);
    
    BOOST_AUTO(vscrollbar, make_shared<vertical_scroll_bar>());
    vscrollbar->on_page_up.connect(
        bind(on_up, weak_ptr<vertical_scroll_bar>(vscrollbar)));
    vscrollbar->on_page_down.connect(
        bind(on_down, weak_ptr<vertical_scroll_bar>(vscrollbar)));
    
    container->add_component(
        vscrollbar
      , COMPASS_LAYOUT_WEST);
    
    
   
    /*
 
    container->set_layout(make_shared<compass_layout>());
    
    BOOST_AUTO(input_container, make_shared<basic_container>());
    input_container->set_layout(make_shared<compass_layout>());
    
    BOOST_AUTO(labels_container, make_shared<basic_container>());
    labels_container->set_layout(make_shared<grid_layout>(2, 1));
    
    BOOST_AUTO(fields_container, make_shared<basic_container>());
    fields_container->set_layout(make_shared<grid_layout>(2, 1));
    fields_container->add_component(
        make_shared<framed_component>(
            make_shared<solid_frame>(), make_shared<edit>()));
    
    BOOST_AUTO(password_field, make_shared<edit>());
    element_type password_element;
    password_element.first = '*';
    password_element.second.foreground_colour =
        odin::ansi::graphics::COLOUR_RED;
    password_field->set_attribute(
        EDIT_PASSWORD_ELEMENT
      , password_element);
    
    fields_container->add_component(make_shared<framed_component>(
        make_shared<solid_frame>(), password_field));
          
    alignment_data alignment = {
        HORIZONTAL_ALIGNMENT_RIGHT
      , VERTICAL_ALIGNMENT_CENTRE
    };
    
    BOOST_AUTO(name_container, make_shared<basic_container>());
    name_container->set_layout(make_shared<aligned_layout>());
    
    string name_image[] = { "name:" };
    name_container->add_component(
        make_shared<image>(image_from_text(name_image))
      , alignment);
    
    BOOST_AUTO(password_container, make_shared<basic_container>());
    password_container->set_layout(make_shared<aligned_layout>());
    
    string password_image[] = { "password:" };
    password_container->add_component(
        make_shared<image>(image_from_text(password_image))
      , alignment);
    
    labels_container->add_component(name_container);
    labels_container->add_component(password_container);
    
    input_container->add_component(
        labels_container, COMPASS_LAYOUT_WEST);
    input_container->add_component(
        fields_container, COMPASS_LAYOUT_CENTRE);
    
    BOOST_AUTO(image_container, make_shared<basic_container>());
    image_container->set_layout(make_shared<aligned_layout>());
    
    alignment.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTRE;
    alignment.vertical_alignment   = VERTICAL_ALIGNMENT_CENTRE;
    image_container->add_component(
        make_shared<image>(image_from_text(main_image))
      , alignment);
    
    container->add_component(input_container, COMPASS_LAYOUT_SOUTH);
    container->add_component(image_container, COMPASS_LAYOUT_CENTRE);
    */
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

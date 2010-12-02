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
#include "munin/compass_layout.hpp"
#include "munin/composite_component.hpp"
#include "munin/grid_layout.hpp"
#include "munin/ansi/basic_container.hpp"
#include "munin/ansi/edit.hpp"
#include "munin/ansi/framed_component.hpp"
#include "munin/ansi/frame.hpp"
#include "munin/ansi/image.hpp"
#include "munin/ansi/text_area.hpp"
#include "munin/ansi/frame.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

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

ui::ui()
    : munin::composite_component<munin::ansi::element_type>(
           make_shared<munin::ansi::basic_container>())
{
    typedef munin::ansi::basic_container                     basic_container;
    typedef munin::ansi::framed_component                    framed_component;
    typedef munin::ansi::edit                                edit;
    typedef munin::ansi::frame                               frame;
    typedef munin::ansi::image                               image;
    typedef munin::aligned_layout<munin::ansi::element_type> aligned_layout;
    typedef munin::compass_layout<munin::ansi::element_type> compass_layout;
    typedef munin::grid_layout<munin::ansi::element_type>    grid_layout;
    
    BOOST_AUTO(container, get_container());
 
    container->set_layout(make_shared<compass_layout>());
    
    BOOST_AUTO(input_container, make_shared<basic_container>());
    input_container->set_layout(make_shared<compass_layout>());
    
    BOOST_AUTO(labels_container, make_shared<basic_container>());
    labels_container->set_layout(make_shared<grid_layout>(2, 1));
    
    BOOST_AUTO(fields_container, make_shared<basic_container>());
    fields_container->set_layout(make_shared<grid_layout>(2, 1));
    fields_container->add_component(
        make_shared<framed_component>(
            make_shared<frame>(), make_shared<edit>()));
    
    BOOST_AUTO(password_field, make_shared<edit>());
    munin::ansi::element_type password_element;
    password_element.first = '*';
    password_element.second.foreground_colour =
        odin::ansi::graphics::COLOUR_RED;
    password_field->set_attribute(
        munin::ansi::EDIT_PASSWORD_ELEMENT
      , password_element);
    
    fields_container->add_component(
        make_shared<framed_component>(make_shared<frame>(), password_field));
          
    munin::alignment_data alignment = {
        munin::HORIZONTAL_ALIGNMENT_RIGHT
      , munin::VERTICAL_ALIGNMENT_CENTRE
    };
    
    BOOST_AUTO(name_container, make_shared<basic_container>());
    name_container->set_layout(make_shared<aligned_layout>());
    
    string name_image[] = { "name:" };
    name_container->add_component(
        make_shared<image>(munin::ansi::image_from_text(name_image))
      , alignment);
    
    BOOST_AUTO(password_container, make_shared<basic_container>());
    password_container->set_layout(make_shared<aligned_layout>());
    
    string password_image[] = { "password:" };
    password_container->add_component(
        make_shared<image>(munin::ansi::image_from_text(password_image))
      , alignment);
    
    labels_container->add_component(name_container);
    labels_container->add_component(password_container);
    
    input_container->add_component(
        labels_container, munin::COMPASS_LAYOUT_WEST);
    input_container->add_component(
        fields_container, munin::COMPASS_LAYOUT_CENTRE);
    
    BOOST_AUTO(image_container, make_shared<basic_container>());
    image_container->set_layout(make_shared<aligned_layout>());
    
    alignment.horizontal_alignment = munin::HORIZONTAL_ALIGNMENT_CENTRE;
    alignment.vertical_alignment   = munin::VERTICAL_ALIGNMENT_CENTRE;
    image_container->add_component(
        make_shared<image>(munin::ansi::image_from_text(main_image))
      , alignment);
    
    container->add_component(input_container, munin::COMPASS_LAYOUT_SOUTH);
    container->add_component(image_container, munin::COMPASS_LAYOUT_CENTRE);
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

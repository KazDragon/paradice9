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
#include "user_interface.hpp"
#include "munin/ansi/basic_container.hpp"
#include "munin/ansi/edit.hpp"
#include "munin/ansi/frame.hpp"
#include "munin/ansi/framed_component.hpp"
#include "munin/ansi/image.hpp"
#include "munin/ansi/protocol.hpp"
#include "munin/ansi/text_area.hpp"
#include "munin/aligned_layout.hpp"
#include "munin/grid_layout.hpp"
#include "munin/card.hpp"
#include "munin/compass_layout.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace odin;
using namespace boost;
using namespace std;

typedef munin::component<munin::ansi::element_type>       component;
typedef munin::container<munin::ansi::element_type>       container;
typedef munin::card<munin::ansi::element_type>            card;
typedef munin::aligned_layout<munin::ansi::element_type>  aligned_layout;
typedef munin::compass_layout<munin::ansi::element_type>  compass_layout;
typedef munin::grid_layout<munin::ansi::element_type>     grid_layout;
typedef munin::ansi::basic_container                      basic_container;
typedef munin::ansi::edit                                 edit;
typedef munin::ansi::image                                image;
typedef munin::ansi::text_area                            text_area;
typedef munin::ansi::frame                                frame;
typedef munin::ansi::framed_component                     framed_component;

namespace hugin {

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
  , "                                                     v1.0"
};

// ==========================================================================
// USER_INTERFACE::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct user_interface::impl
{
    boost::shared_ptr<component> create_intro_screen()
    {
        BOOST_AUTO(content, make_shared<basic_container>());
        content->set_layout(make_shared<compass_layout>());

        BOOST_AUTO(image_container, make_shared<basic_container>());
        image_container->set_layout(make_shared<aligned_layout>());

        BOOST_AUTO(
            greetings_image
          , make_shared<image>(munin::ansi::image_from_text(main_image)));
        image_container->add_component(greetings_image);

        content->add_component(
            image_container
          , munin::COMPASS_LAYOUT_CENTRE);

        std::string name_label_elements[] = { "Name: " };

        BOOST_AUTO(
            name_label
          , make_shared<image>(
                munin::ansi::image_from_text(name_label_elements)));

        BOOST_AUTO(name_container, make_shared<basic_container>());
        name_container->set_layout(make_shared<aligned_layout>());

        munin::alignment_data alignment;
        alignment.horizontal_alignment = munin::HORIZONTAL_ALIGNMENT_RIGHT;
        alignment.vertical_alignment   = munin::VERTICAL_ALIGNMENT_CENTRE;

        name_container->add_component(name_label, alignment);

        intro_name_field_ = make_shared<edit>();

        BOOST_AUTO(bottom_container, make_shared<basic_container>());
        bottom_container->set_layout(make_shared<compass_layout>());
        bottom_container->add_component(
            name_container
          , munin::COMPASS_LAYOUT_WEST);
        bottom_container->add_component(
            make_shared<framed_component>(
                make_shared<frame>()
              , intro_name_field_)
          , munin::COMPASS_LAYOUT_CENTRE);

        content->add_component(
            bottom_container
          , munin::COMPASS_LAYOUT_SOUTH);

        return content;
    }

    void on_username_entered()
    {
        if (on_username_entered_)
        {
            BOOST_AUTO(document, intro_name_field_->get_document());
            BOOST_AUTO(elements, document->get_text_line(0));

            std::string username;

            BOOST_FOREACH(munin::ansi::element_type element, elements)
            {
                username += element.first;
            }

            on_username_entered_(username);
        }
    }

    std::string                    face_name_;
    boost::shared_ptr<card>        active_screen_;
    boost::shared_ptr<edit>        intro_name_field_;
    boost::function<void (string)> on_username_entered_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
user_interface::user_interface()
    : munin::composite_component<munin::ansi::element_type>(
           make_shared<munin::ansi::basic_container>())
    , pimpl_(new impl)
{
    pimpl_->active_screen_ = make_shared<card>();

    
    pimpl_->face_name_ = "intro";
    pimpl_->active_screen_->add_face(
        pimpl_->create_intro_screen(), pimpl_->face_name_);
    pimpl_->active_screen_->select_face(pimpl_->face_name_);

    BOOST_AUTO(container, get_container());
    container->set_layout(make_shared<grid_layout>(1, 1));
    container->add_component(pimpl_->active_screen_);
}

// ==========================================================================
// ON_USERNAME_ENTERED
// ==========================================================================
void user_interface::on_username_entered(function<void (string)> callback)
{
    pimpl_->on_username_entered_ = callback;
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void user_interface::do_event(any const &ev)
{
    char const *ch = any_cast<char>(&ev);
    odin::ansi::control_sequence const *control_sequence = 
        any_cast<odin::ansi::control_sequence>(&ev);

    if (pimpl_->face_name_ == "intro" 
     && pimpl_->intro_name_field_->has_focus()
     && ch != NULL)
    {
        if (*ch == '\n')
        {
            pimpl_->on_username_entered();
        }
        else if (*ch == ' ')
        {
            // Do nothing - we don't allow spaces in names.
        }
        else
        {
            munin::composite_component<munin::ansi::element_type>::do_event(ev);
        }
    }
    else
    {
        munin::composite_component<munin::ansi::element_type>::do_event(ev);
    }
}

}

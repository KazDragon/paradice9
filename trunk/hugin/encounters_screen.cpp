// ==========================================================================
// Hugin Encounters Screen
//
// Copyright (C) 2013 Matthew Chaplain, All Rights Reserved.
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
#include "hugin/encounters_screen.hpp"
#include "hugin/beast_editor.hpp"
#include "hugin/bestiary_page.hpp"
#include "hugin/encounters_page.hpp"
#include "munin/algorithm.hpp"
#include "munin/basic_container.hpp"
#include "munin/button.hpp"
#include "munin/card.hpp"
#include "munin/compass_layout.hpp"
#include "munin/filled_box.hpp"
#include "munin/tabbed_panel.hpp"
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>
#include <vector>

using namespace munin;
using namespace boost;
using namespace std;

namespace hugin {

namespace {
    BOOST_STATIC_CONSTANT(string, bestiary_face = "BESTIARY");
    BOOST_STATIC_CONSTANT(string, beast_editor_face = "BEAST_EDITOR");
}

// ==========================================================================
// ENCOUNTERS_SCREEN::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct encounters_screen::impl
{
    shared_ptr<tabbed_panel>    tabbed_panel_;
    shared_ptr<card>            bestiary_tab_card_;
    shared_ptr<bestiary_page>   bestiary_page_;
    shared_ptr<beast_editor>    beast_editor_;
    shared_ptr<encounters_page> encounters_page_;
    shared_ptr<button>          back_button_;

    shared_ptr<paradice::beast> current_beast_;

    void on_new_beast()
    {
        current_beast_ = make_shared<paradice::beast>();

        beast_editor_->set_beast_name("New Beast");
        beast_editor_->set_beast_description("");

        bestiary_tab_card_->select_face(beast_editor_face);
    }

    void on_save_beast()
    {
        current_beast_->set_name(beast_editor_->get_beast_name());
        current_beast_->set_description(beast_editor_->get_beast_description());

        // If it's not in the beast list, then add it and make it selected.
        BOOST_AUTO(beasts, bestiary_page_->get_beasts());
        bool found = false;

        BOOST_FOREACH(shared_ptr<paradice::beast> beast, beasts)
        {
            if (beast == current_beast_)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            beasts.push_back(current_beast_);
        }

        bestiary_page_->set_beasts(beasts);
        bestiary_tab_card_->select_face(bestiary_face);
    }

    void on_revert_beast()
    {
        bestiary_tab_card_->select_face(bestiary_face);
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
encounters_screen::encounters_screen()
    : pimpl_(make_shared<impl>())
{
    pimpl_->bestiary_page_     = make_shared<bestiary_page>();
    pimpl_->beast_editor_      = make_shared<beast_editor>();
    pimpl_->encounters_page_   = make_shared<encounters_page>();

    pimpl_->bestiary_tab_card_ = make_shared<card>();
    pimpl_->bestiary_tab_card_->add_face(pimpl_->bestiary_page_, bestiary_face);
    pimpl_->bestiary_tab_card_->add_face(pimpl_->beast_editor_, beast_editor_face);
    pimpl_->bestiary_tab_card_->select_face(bestiary_face);

    pimpl_->bestiary_page_->on_new.connect(bind(
        &impl::on_new_beast
      , pimpl_.get()));

    pimpl_->beast_editor_->on_save.connect(bind(
        &impl::on_save_beast
      , pimpl_.get()));

    pimpl_->beast_editor_->on_revert.connect(bind(
        &impl::on_revert_beast
      , pimpl_.get()));
        
    pimpl_->back_button_ = make_shared<button>(
        string_to_elements("Back"));
    pimpl_->back_button_->on_click.connect(bind(ref(on_back)));

    pimpl_->tabbed_panel_ = make_shared<tabbed_panel>();
    pimpl_->tabbed_panel_->insert_tab("Bestiary", pimpl_->bestiary_tab_card_);
    pimpl_->tabbed_panel_->insert_tab("Encounters", pimpl_->encounters_page_);

    BOOST_AUTO(buttons_panel, make_shared<basic_container>());
    buttons_panel->set_layout(make_shared<compass_layout>());
    buttons_panel->add_component(pimpl_->back_button_, COMPASS_LAYOUT_WEST);
    buttons_panel->add_component(
        make_shared<filled_box>(element_type(' '))
      , COMPASS_LAYOUT_CENTRE);

    BOOST_AUTO(content, get_container());
    content->set_layout(make_shared<compass_layout>());
    content->add_component(pimpl_->tabbed_panel_, COMPASS_LAYOUT_CENTRE);
    content->add_component(buttons_panel, COMPASS_LAYOUT_SOUTH);
}
    
// ==========================================================================
// DESTRUCTOR
// ==========================================================================
encounters_screen::~encounters_screen()
{
}
    
// ==========================================================================
// DO_EVENT
// ==========================================================================
void encounters_screen::do_event(any const &ev)
{
    bool handled = false;

    char const *ch = any_cast<char>(&ev);
    odin::ansi::control_sequence const *control_sequence = 
        any_cast<odin::ansi::control_sequence>(&ev);

    if (ch != NULL && *ch == '\t')
    {
        focus_next();
        
        if (!has_focus())
        {
            focus_next();
        }
        
        handled = true;
    }
    else if (control_sequence != NULL
          && control_sequence->initiator_ == odin::ansi::CONTROL_SEQUENCE_INTRODUCER
          && control_sequence->command_   == odin::ansi::CURSOR_BACKWARD_TABULATION)
    {
        focus_previous();

        if (!has_focus())
        {
            focus_previous();
        }
        
        handled = true;
    }
    
    if (!handled)
    {
        composite_component::do_event(ev);
    }
}

}

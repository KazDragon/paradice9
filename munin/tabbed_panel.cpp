// ==========================================================================
// Munin Tabbed Panel.
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
#include "munin/tabbed_panel.hpp"
#include "munin/card.hpp"
#include "munin/framed_component.hpp"
#include "munin/grid_layout.hpp"
#include "munin/tabbed_frame.hpp"
#include <boost/bind.hpp>
#include <boost/typeof/typeof.hpp>

using namespace odin;
using namespace boost;
using namespace std;

namespace munin {

// ==========================================================================
// TABBED_PANEL::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct tabbed_panel::impl
{
    shared_ptr<card>         card_;
    shared_ptr<tabbed_frame> frame_;

    // ======================================================================
    // TAB_SELECTED_THUNK
    // ======================================================================
    static void tab_selected_thunk(
        weak_ptr<impl> weak_this
      , string const &text)
    {
        BOOST_AUTO(strong_this, weak_this.lock());

        if (strong_this)
        {
            strong_this->tab_selected(text);
        }
    }

    // ======================================================================
    // TAB_SELECTED
    // ======================================================================
    void tab_selected(string const &text)
    {
        card_->select_face(text);
    }

    // ======================================================================
    // FOCUS_CHANGE_THUNK
    // ======================================================================
    static void focus_change_thunk(weak_ptr<impl> weak_this)
    {
        BOOST_AUTO(strong_this, weak_this.lock());

        if (strong_this)
        {
            strong_this->focus_change();
        }
    }

    // ======================================================================
    // FOCUS_CHANGE
    // ======================================================================
    void focus_change()
    {
        frame_->set_highlight(card_->has_focus() || frame_->has_focus());
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
tabbed_panel::tabbed_panel()
    : pimpl_(new impl)
{
    pimpl_->card_  = make_shared<card>();
    pimpl_->frame_ = make_shared<tabbed_frame>();

    BOOST_AUTO(inner, make_shared<framed_component>(
        pimpl_->frame_, pimpl_->card_));

    BOOST_AUTO(content, get_container());
    content->set_layout(make_shared<grid_layout>(1, 1));
    content->add_component(inner);

    pimpl_->frame_->on_tab_selected.connect(bind(
        &impl::tab_selected_thunk
      , weak_ptr<impl>(pimpl_)
      , _1));

    BOOST_AUTO(focus_callback, bind(
        &impl::focus_change_thunk
      , weak_ptr<impl>(pimpl_)));

    pimpl_->frame_->on_focus_set.connect(focus_callback);
    pimpl_->frame_->on_focus_lost.connect(focus_callback);
    pimpl_->card_->on_focus_set.connect(focus_callback);
    pimpl_->card_->on_focus_lost.connect(focus_callback);
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
tabbed_panel::~tabbed_panel()
{
}

// ==========================================================================
// INSERT_TAB
// ==========================================================================
void tabbed_panel::insert_tab(
    string const          &text, 
    shared_ptr<component>  comp,
    optional<u32>          index /*= optional<u32>()*/)
{
    pimpl_->card_->add_face(comp, text);
    pimpl_->frame_->insert_tab(text, index);

    if (pimpl_->card_->get_number_of_faces() == 1)
    {
        pimpl_->card_->select_face(text);
    }
}

// ==========================================================================
// REMOVE_TAB
// ==========================================================================
void tabbed_panel::remove_tab(u32 index)
{
    pimpl_->frame_->remove_tab(index);
}

}

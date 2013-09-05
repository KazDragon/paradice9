// ==========================================================================
// Hugin Encounters Page
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
#include "hugin/encounters_page.hpp"
#include "paradice/encounter.hpp"
#include "munin/algorithm.hpp"
#include "munin/basic_container.hpp"
#include "munin/button.hpp"
#include "munin/compass_layout.hpp"
#include "munin/filled_box.hpp"
#include "munin/grid_layout.hpp"
#include "munin/vertical_squeeze_layout.hpp"
#include "munin/vertical_strip_layout.hpp"
#include "munin/list.hpp"
#include "munin/scroll_pane.hpp"
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace paradice;
using namespace munin;
using namespace odin;
using namespace boost;
using namespace std;

namespace hugin {

// ==========================================================================
// ENCOUNTERS_PAGE::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct encounters_page::impl
{
    encounters_page                    &self_;
    shared_ptr<munin::list>             encounters_list_;
    shared_ptr<munin::list>             beasts_list_;
    shared_ptr<button>                  new_button_;
    shared_ptr<button>                  edit_button_;
    shared_ptr<button>                  clone_button_;
    shared_ptr<button>                  delete_button_;

    shared_ptr<container>               split_container_;
    shared_ptr<container>               details_container_;

    vector< shared_ptr<encounter> >     encounters_;

    vector<boost::signals::connection>  connections_;


    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(encounters_page &self)
        : self_(self)
    {
    }

    // ======================================================================
    // ON_ITEM_CHANGED
    // ======================================================================
    void on_item_changed(s32 from)
    {
        split_container_->remove_component(details_container_);

        vector< vector<element_type> > beast_names;

        BOOST_AUTO(index, encounters_list_->get_item_index());

        if (index >= 0 && size_t(index) < encounters_.size())
        {
            BOOST_AUTO(beasts, encounters_[index]->get_beasts());

            BOOST_FOREACH(shared_ptr<beast> const &beast, beasts)
            {
                beast_names.push_back(
                    string_to_elements(beast->get_name()));
            }

            beasts_list_->set_items(beast_names);
            split_container_->add_component(details_container_);
        }
    }

    // ======================================================================
    // ON_NEW
    // ======================================================================
    void on_new()
    {
        self_.on_new();
    }

    // ======================================================================
    // ON_CLONE
    // ======================================================================
    void on_clone()
    {
        BOOST_AUTO(index, encounters_list_->get_item_index());

        if (index != -1)
        {
            self_.on_clone(encounters_[index]);
        }
    }

    // ======================================================================
    // ON_EDIT
    // ======================================================================
    void on_edit()
    {
        BOOST_AUTO(index, encounters_list_->get_item_index());

        if (index != -1)
        {
            self_.on_edit(encounters_[index]);
        }
    }

    // ======================================================================
    // ON_DELETE
    // ======================================================================
    void on_delete()
    {
        BOOST_AUTO(index, encounters_list_->get_item_index());

        if (index != -1)
        {
            self_.on_delete(encounters_[index]);
        }
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
encounters_page::encounters_page()
{
    pimpl_ = make_shared<impl>(ref(*this));
    pimpl_->encounters_list_ = make_shared<munin::list>();
    pimpl_->beasts_list_     = make_shared<munin::list>();

    pimpl_->new_button_       = make_shared<button>(
        string_to_elements("New"));
    pimpl_->clone_button_     = make_shared<button>(
        string_to_elements("Clone"));
    pimpl_->edit_button_      = make_shared<button>(
        string_to_elements("Edit"));
    pimpl_->delete_button_    = make_shared<button>(
        string_to_elements("Delete"));

    pimpl_->connections_.push_back(
        pimpl_->encounters_list_->on_item_changed.connect(bind(
            &impl::on_item_changed
          , pimpl_
          , _1)));

    pimpl_->connections_.push_back(
        pimpl_->new_button_->on_click.connect(bind(
            &impl::on_new
          , pimpl_)));

    pimpl_->connections_.push_back(
        pimpl_->clone_button_->on_click.connect(bind(
            &impl::on_clone
          , pimpl_)));

    pimpl_->connections_.push_back(
        pimpl_->edit_button_->on_click.connect(bind(
            &impl::on_edit
          , pimpl_)));

    pimpl_->connections_.push_back(
        pimpl_->delete_button_->on_click.connect(bind(
            &impl::on_delete
          , pimpl_)));

    // The split container will house both the encounter list and the details
    // for the currently selected encounter.  However, the details panel will
    // remain hidden until there are items in the list and one is selected.
    pimpl_->split_container_ = make_shared<basic_container>();
    pimpl_->split_container_->set_layout(
        make_shared<vertical_squeeze_layout>());
    pimpl_->split_container_->add_component(
        make_shared<scroll_pane>(pimpl_->encounters_list_));

    // The details container will be pre-made so that it can be just 
    // added/removed to/from the split container as appropriate.
    pimpl_->details_container_ = make_shared<basic_container>();
    pimpl_->details_container_->set_layout(make_shared<grid_layout>(1, 1));
    pimpl_->details_container_->add_component(
        make_shared<scroll_pane>(pimpl_->beasts_list_));

    // The New, Clone, and Edit buttons sit together on the left; a group of
    // "safe" buttons.  At the far right is the dangerous Delete button.
    BOOST_AUTO(safe_buttons_container, make_shared<basic_container>());
    safe_buttons_container->set_layout(make_shared<vertical_strip_layout>());
    safe_buttons_container->add_component(pimpl_->edit_button_);
    safe_buttons_container->add_component(pimpl_->new_button_);
    safe_buttons_container->add_component(pimpl_->clone_button_);

    BOOST_AUTO(dangerous_buttons_container, make_shared<basic_container>());
    dangerous_buttons_container->set_layout(make_shared<vertical_strip_layout>());
    dangerous_buttons_container->add_component(pimpl_->delete_button_);

    BOOST_AUTO(buttons_container, make_shared<basic_container>());
    buttons_container->set_layout(make_shared<compass_layout>());
    buttons_container->add_component(
        safe_buttons_container, COMPASS_LAYOUT_WEST);
    buttons_container->add_component(
        make_shared<filled_box>(element_type(' ')), COMPASS_LAYOUT_CENTRE);
    buttons_container->add_component(
        dangerous_buttons_container, COMPASS_LAYOUT_EAST);

    BOOST_AUTO(content, get_container());
    content->set_layout(make_shared<compass_layout>());
    content->add_component(pimpl_->split_container_, COMPASS_LAYOUT_CENTRE);
    content->add_component(buttons_container, COMPASS_LAYOUT_SOUTH);
}
    
// ==========================================================================
// DESTRUCTOR
// ==========================================================================
encounters_page::~encounters_page()
{
    BOOST_FOREACH(boost::signals::connection &cnx, pimpl_->connections_)
    {
        cnx.disconnect();
    }
}

// ==========================================================================
// SET_ENCOUNTERS
// ==========================================================================
void encounters_page::set_encounters(
    vector< shared_ptr<encounter> > encounters)
{
    pimpl_->encounters_ = encounters;

    vector< vector<element_type> > encounter_names;

    BOOST_FOREACH(shared_ptr<encounter> const &enc, pimpl_->encounters_)
    {
        encounter_names.push_back(
            string_to_elements(enc->get_name()));
    }

    pimpl_->encounters_list_->set_items(encounter_names);
    pimpl_->encounters_list_->set_item_index(-1);
}

// ==========================================================================
// GET_ENCOUNTERS
// ==========================================================================
vector< shared_ptr<encounter> > encounters_page::get_encounters() const
{
    return pimpl_->encounters_;
}

// ==========================================================================
// GET_SELECTED_ENCOUNTER
// ==========================================================================
shared_ptr<encounter> encounters_page::get_selected_encounter() const
{
    BOOST_AUTO(selected_index, pimpl_->encounters_list_->get_item_index());

    return selected_index == -1
        ? shared_ptr<encounter>()
        : pimpl_->encounters_[selected_index];
}

}

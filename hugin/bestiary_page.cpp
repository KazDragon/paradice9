// ==========================================================================
// Hugin Bestiary Page
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
#include "hugin/bestiary_page.hpp"
#include "munin/algorithm.hpp"
#include "munin/basic_container.hpp"
#include "munin/button.hpp"
#include "munin/compass_layout.hpp"
#include "munin/edit.hpp"
#include "munin/filled_box.hpp"
#include "munin/framed_component.hpp"
#include "munin/grid_layout.hpp"
#include "munin/list.hpp"
#include "munin/scroll_pane.hpp"
#include "munin/solid_frame.hpp"
#include "munin/text_area.hpp"
#include "munin/vertical_squeeze_layout.hpp"
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/uuid/uuid_generators.hpp>

using namespace paradice;
using namespace munin;
using namespace boost;

namespace hugin {

// ==========================================================================
// BESTIARY_PAGE::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct bestiary_page::impl
{
    shared_ptr<list>      beast_list_;
    shared_ptr<edit>      name_field_;
    shared_ptr<text_area> description_area_;
    shared_ptr<button>    new_button_;
    shared_ptr<button>    delete_button_;
    shared_ptr<button>    back_button_;

    shared_ptr<container> split_container_;
    shared_ptr<container> details_container_;

    std::vector< shared_ptr<beast> >         beasts_;
    std::vector< std::vector<element_type> > names_;

    // ======================================================================
    // ON_ITEM_CHANGED_THUNK
    // ======================================================================
    static void on_item_changed_thunk(weak_ptr<impl> weak_this)
    {
        BOOST_AUTO(strong_this, weak_this.lock());

        if (strong_this)
        {
            strong_this->on_item_changed();
        }
    }

    // ======================================================================
    // ON_ITEM_CHANGED
    // ======================================================================
    void on_item_changed()
    {
        split_container_->remove_component(details_container_);

        BOOST_AUTO(index, beast_list_->get_item_index());

        if (index != -1)
        {
            split_container_->add_component(details_container_);
            name_field_->get_document()->set_text(names_[index]);
        }
    }

    // ======================================================================
    // ON_NEW_THUNK
    // ======================================================================
    static void on_new_thunk(weak_ptr<impl> weak_this)
    {
        BOOST_AUTO(strong_this, weak_this.lock());

        if (strong_this)
        {
            strong_this->on_new();
        }
    }

    // ======================================================================
    // ON_NEW
    // ======================================================================
    void on_new()
    {
        // Create a new beast, add it to the vectors, add it to the ui list,
        // select the item.
        BOOST_AUTO(new_beast, make_shared<beast>());
        new_beast->set_id(boost::uuids::random_generator()());
        new_beast->set_name("New beast");

        beasts_.push_back(new_beast);
        names_.push_back(string_to_elements(new_beast->get_name()));

        beast_list_->set_items(names_);
        beast_list_->set_item_index(names_.size() - 1);
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
bestiary_page::bestiary_page()
    : pimpl_(make_shared<impl>())
{
    pimpl_->beast_list_       = make_shared<list>();
    pimpl_->name_field_       = make_shared<edit>();
    pimpl_->description_area_ = make_shared<text_area>();
    pimpl_->new_button_       = make_shared<button>(
        string_to_elements("New"));
    pimpl_->delete_button_    = make_shared<button>(
        string_to_elements("Delete"));

    // Set up event callbacks.
    pimpl_->beast_list_->on_item_changed.connect(bind(
        &impl::on_item_changed_thunk
      , weak_ptr<impl>(pimpl_)));

    pimpl_->new_button_->on_click.connect(bind(
        &impl::on_new_thunk
      , weak_ptr<impl>(pimpl_)));

    // The split container will house both the beast list and the
    // details for the currently selected beast.  However, the details
    // panel will remain hidden until there are items in the list and
    // one is selected.
    pimpl_->split_container_ = make_shared<basic_container>();
    pimpl_->split_container_->set_layout(
        make_shared<vertical_squeeze_layout>());
    pimpl_->split_container_->add_component(
        make_shared<scroll_pane>(pimpl_->beast_list_));

    // The details container will be pre-made so that it can be just 
    // added/removed to/from the split container as appropriate.
    pimpl_->details_container_ = make_shared<basic_container>();
    pimpl_->details_container_->set_layout(make_shared<compass_layout>());
    pimpl_->details_container_->add_component(
        make_shared<framed_component>(
            make_shared<solid_frame>()
          , pimpl_->name_field_)
      , COMPASS_LAYOUT_NORTH);
    pimpl_->details_container_->add_component(
        make_shared<scroll_pane>(pimpl_->description_area_)
      , COMPASS_LAYOUT_CENTRE);

    // The button layout sits on the bottom and is comprised of the new button
    // on the left and the delete button on the right.  A spacer sits in the
    // middle to keep them apart.
    BOOST_AUTO(buttons_container, make_shared<basic_container>());
    buttons_container->set_layout(make_shared<compass_layout>());
    buttons_container->add_component(
        pimpl_->new_button_, COMPASS_LAYOUT_WEST);
    buttons_container->add_component(
        make_shared<filled_box>(element_type(' ')), COMPASS_LAYOUT_CENTRE);
    buttons_container->add_component(
        pimpl_->delete_button_, COMPASS_LAYOUT_EAST);

    // Arrange the upper and lower parts on the screen.
    BOOST_AUTO(content, get_container());
    content->set_layout(make_shared<compass_layout>());
    content->add_component(pimpl_->split_container_, COMPASS_LAYOUT_CENTRE);
    content->add_component(buttons_container, COMPASS_LAYOUT_SOUTH);
}
    
// ==========================================================================
// DESTRUCTOR
// ==========================================================================
bestiary_page::~bestiary_page()
{
}

// ==========================================================================
// SET_BEASTS
// ==========================================================================
void bestiary_page::set_beasts(std::vector< shared_ptr<beast> > const &beasts)
{
    pimpl_->beasts_ = beasts;

    // rebuild the vector of ansi strings to match the names of the beasts.
    pimpl_->names_.clear();

    BOOST_FOREACH(shared_ptr<beast> current_beast, beasts)
    {
        pimpl_->names_.push_back(
            string_to_elements(current_beast->get_name()));
    }

    // Set this as the current list of beasts and ensure it is de-selected.
    pimpl_->beast_list_->set_items(pimpl_->names_);
    pimpl_->beast_list_->set_item_index(-1);
}

// ==========================================================================
// GET_BEASTS
// ==========================================================================
std::vector< shared_ptr<beast> > bestiary_page::get_beasts() const
{
    return pimpl_->beasts_;
}

}

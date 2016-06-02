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
#include <munin/algorithm.hpp>
#include <munin/background_fill.hpp>
#include <munin/button.hpp>
#include <munin/compass_layout.hpp>
#include <munin/edit.hpp>
#include <munin/filled_box.hpp>
#include <munin/framed_component.hpp>
#include <munin/grid_layout.hpp>
#include <munin/list.hpp>
#include <munin/scroll_pane.hpp>
#include <munin/solid_frame.hpp>
#include <munin/text_area.hpp>
#include <munin/vertical_squeeze_layout.hpp>
#include <munin/vertical_strip_layout.hpp>
#include <munin/view.hpp>
#include <terminalpp/string.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <vector>

namespace hugin {

// ==========================================================================
// BESTIARY_PAGE::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct bestiary_page::impl
{
    bestiary_page                    &self_;
    std::shared_ptr<munin::list>      beast_list_;
    std::shared_ptr<munin::edit>      name_field_;
    std::shared_ptr<munin::text_area> description_area_;
    std::shared_ptr<munin::button>    new_button_;
    std::shared_ptr<munin::button>    clone_button_;
    std::shared_ptr<munin::button>    edit_button_;
    std::shared_ptr<munin::button>    fight_button_;
    std::shared_ptr<munin::button>    delete_button_;
    std::shared_ptr<munin::button>    back_button_;

    std::shared_ptr<munin::container> split_container_;
    std::shared_ptr<munin::container> details_container_;

    std::vector<std::shared_ptr<paradice::beast>> beasts_;
    std::vector<terminalpp::string>   names_;

    std::vector<boost::signals::connection> connections_;

    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(bestiary_page &self)
        : self_(self)
    {
    }

    // ======================================================================
    // ON_ITEM_CHANGED
    // ======================================================================
    void on_item_changed(odin::s32 from)
    {
        split_container_->remove_component(details_container_);

        auto index = beast_list_->get_item_index();

        if (index != -1)
        {
            split_container_->add_component(details_container_);
            name_field_->get_document()->set_text(names_[index]);

            auto description = beasts_[index]->get_description();
            description_area_->get_document()->set_text(description);
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
        auto index = beast_list_->get_item_index();

        if (index != -1)
        {
            self_.on_clone(beasts_[index]);
        }
    }

    // ======================================================================
    // ON_CLONE
    // ======================================================================
    void on_fight()
    {
        auto index = beast_list_->get_item_index();

        if (index != -1)
        {
            self_.on_fight(beasts_[index]);
        }
    }

    // ======================================================================
    // ON_EDIT
    // ======================================================================
    void on_edit()
    {
        auto index = beast_list_->get_item_index();

        if (index != -1)
        {
            self_.on_edit(beasts_[index]);
        }
    }

    // ======================================================================
    // ON_DELETE
    // ======================================================================
    void on_delete()
    {
        auto index = beast_list_->get_item_index();

        if (index != -1)
        {
            self_.on_delete(beasts_[index]);
        }
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
bestiary_page::bestiary_page()
{
    pimpl_ = std::make_shared<impl>(boost::ref(*this));

    pimpl_->beast_list_       = munin::make_list();
    pimpl_->name_field_       = munin::make_edit();
    pimpl_->description_area_ = munin::make_text_area();
    pimpl_->new_button_       = munin::make_button("New");
    pimpl_->clone_button_     = munin::make_button("Clone");
    pimpl_->edit_button_      = munin::make_button("Edit");
    pimpl_->fight_button_     = munin::make_button("Fight!");
    pimpl_->delete_button_    = munin::make_button("Delete");

    // Set up event callbacks.
    pimpl_->connections_.push_back(
        pimpl_->beast_list_->on_item_changed.connect(
            [this](auto idx){pimpl_->on_item_changed(idx);}));

    pimpl_->connections_.push_back(
        pimpl_->new_button_->on_click.connect(
            [this]{pimpl_->on_new();}));
            
    pimpl_->connections_.push_back(
        pimpl_->clone_button_->on_click.connect(
            [this]{pimpl_->on_clone();}));

    pimpl_->connections_.push_back(
        pimpl_->fight_button_->on_click.connect(
            [this]{pimpl_->on_fight();}));

    pimpl_->connections_.push_back(
        pimpl_->edit_button_->on_click.connect(
            [this]{pimpl_->on_edit();}));

    pimpl_->connections_.push_back(
        pimpl_->delete_button_->on_click.connect(
            [this]{pimpl_->on_delete();}));

    // The split container will house both the beast list and the
    // details for the currently selected beast.  However, the details
    // panel will remain hidden until there are items in the list and
    // one is selected.
    pimpl_->split_container_ = munin::view(
        munin::make_vertical_squeeze_layout(),
        munin::make_scroll_pane(pimpl_->beast_list_));

    // The details container will be pre-made so that it can be just 
    // added/removed to/from the split container as appropriate.
    pimpl_->details_container_ = munin::view(
        munin::make_compass_layout(),
        munin::make_framed_component(
            munin::make_solid_frame(),
            pimpl_->name_field_
        ), munin::COMPASS_LAYOUT_NORTH,
        munin::make_scroll_pane(
            pimpl_->description_area_
        ), munin::COMPASS_LAYOUT_CENTRE);
    
    auto buttons_container = munin::view(
        munin::make_compass_layout(),
        // "Safe" buttons
        munin::view(
            munin::make_vertical_strip_layout(),
            pimpl_->edit_button_,
            pimpl_->new_button_,
            pimpl_->clone_button_,
            pimpl_->fight_button_
        ), munin::COMPASS_LAYOUT_WEST,
        // Spacer
        munin::make_background_fill(), munin::COMPASS_LAYOUT_CENTRE,
        // "Dangerous" buttons
        munin::view(
            munin::make_vertical_strip_layout(),
            pimpl_->delete_button_
        ), munin::COMPASS_LAYOUT_EAST);
        
    // Arrange the upper and lower parts on the screen.
    auto content = get_container();
    content->set_layout(munin::make_compass_layout());
    content->add_component(pimpl_->split_container_, munin::COMPASS_LAYOUT_CENTRE);
    content->add_component(buttons_container, munin::COMPASS_LAYOUT_SOUTH);
}
    
// ==========================================================================
// DESTRUCTOR
// ==========================================================================
bestiary_page::~bestiary_page()
{
    for (auto &cnx : pimpl_->connections_)
    {
        cnx.disconnect();
    }
}

// ==========================================================================
// SET_BEASTS
// ==========================================================================
void bestiary_page::set_beasts(
    std::vector<std::shared_ptr<paradice::beast>> const &beasts)
{
    pimpl_->beasts_ = beasts;

    // rebuild the vector of ansi strings to match the names of the beasts.
    pimpl_->names_.clear();

    for (auto &current_beast : beasts)
    {
        pimpl_->names_.push_back(current_beast->get_name());
    }

    // Set this as the current list of beasts and ensure it is de-selected.
    pimpl_->beast_list_->set_items(pimpl_->names_);
    pimpl_->beast_list_->set_item_index(-1);
}

// ==========================================================================
// GET_BEASTS
// ==========================================================================
std::vector<std::shared_ptr<paradice::beast>> bestiary_page::get_beasts() const
{
    return pimpl_->beasts_;
}

// ==========================================================================
// GET_SELECTED_BEAST
// ==========================================================================
std::shared_ptr<paradice::beast> bestiary_page::get_selected_beast() const
{
    auto selected_index = pimpl_->beast_list_->get_item_index();

    return selected_index == -1
      ? std::shared_ptr<paradice::beast>()
      : pimpl_->beasts_[selected_index];
}

}

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
#include <terminalpp/string.hpp>

namespace hugin {

// ==========================================================================
// ENCOUNTERS_PAGE::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct encounters_page::impl
{
    encounters_page                   &self_;
    std::shared_ptr<munin::list>       encounters_list_;
    std::shared_ptr<munin::list>       beasts_list_;
    std::shared_ptr<munin::button>     new_button_;
    std::shared_ptr<munin::button>     edit_button_;
    std::shared_ptr<munin::button>     clone_button_;
    std::shared_ptr<munin::button>     fight_button_;
    std::shared_ptr<munin::button>     delete_button_;

    std::shared_ptr<munin::container>  split_container_;
    std::shared_ptr<munin::container>  details_container_;

    std::vector<std::shared_ptr<paradice::encounter>> encounters_;

    std::vector<boost::signals::connection> connections_;


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
    void on_item_changed(odin::s32 from)
    {
        split_container_->remove_component(details_container_);

        std::vector<terminalpp::string> beast_names;

        auto index = encounters_list_->get_item_index();

        if (index >= 0 && size_t(index) < encounters_.size())
        {
            auto beasts = encounters_[index]->get_beasts();

            for (auto const &beast : beasts)
            {
                beast_names.push_back(beast->get_name());
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
        auto index = encounters_list_->get_item_index();

        if (index != -1)
        {
            self_.on_clone(encounters_[index]);
        }
    }

    // ======================================================================
    // ON_FIGHT
    // ======================================================================
    void on_fight()
    {
        auto index = encounters_list_->get_item_index();

        if (index != -1)
        {
            self_.on_fight(encounters_[index]);
        }
    }

    // ======================================================================
    // ON_EDIT
    // ======================================================================
    void on_edit()
    {
        auto index = encounters_list_->get_item_index();

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
        auto index = encounters_list_->get_item_index();

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
    using namespace terminalpp::literals;

    pimpl_ = std::make_shared<impl>(std::ref(*this));
    pimpl_->encounters_list_ = std::make_shared<munin::list>();
    pimpl_->beasts_list_     = std::make_shared<munin::list>();

    pimpl_->new_button_       = std::make_shared<munin::button>("New"_ts);
    pimpl_->clone_button_     = std::make_shared<munin::button>("Clone"_ts);
    pimpl_->edit_button_      = std::make_shared<munin::button>("Edit"_ts);
    pimpl_->fight_button_     = std::make_shared<munin::button>("Fight!"_ts);
    pimpl_->delete_button_    = std::make_shared<munin::button>("Delete"_ts);

    pimpl_->connections_.push_back(
        pimpl_->encounters_list_->on_item_changed.connect(
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

    // The split container will house both the encounter list and the details
    // for the currently selected encounter.  However, the details panel will
    // remain hidden until there are items in the list and one is selected.
    pimpl_->split_container_ = std::make_shared<munin::basic_container>();
    pimpl_->split_container_->set_layout(
        std:: make_shared<munin::vertical_squeeze_layout>());
    pimpl_->split_container_->add_component(
        std::make_shared<munin::scroll_pane>(pimpl_->encounters_list_));

    // The details container will be pre-made so that it can be just 
    // added/removed to/from the split container as appropriate.
    pimpl_->details_container_ = std::make_shared<munin::basic_container>();
    pimpl_->details_container_->set_layout(std::make_shared<munin::grid_layout>(1, 1));
    pimpl_->details_container_->add_component(
        std::make_shared<munin::scroll_pane>(pimpl_->beasts_list_));

    // The New, Clone, and Edit buttons sit together on the left; a group of
    // "safe" buttons.  At the far right is the dangerous Delete button.
    auto safe_buttons_container = std::make_shared<munin::basic_container>();
    safe_buttons_container->set_layout(std::make_shared<munin::vertical_strip_layout>());
    safe_buttons_container->add_component(pimpl_->edit_button_);
    safe_buttons_container->add_component(pimpl_->new_button_);
    safe_buttons_container->add_component(pimpl_->clone_button_);
    safe_buttons_container->add_component(pimpl_->fight_button_);

    auto dangerous_buttons_container = std::make_shared<munin::basic_container>();
    dangerous_buttons_container->set_layout(std::make_shared<munin::vertical_strip_layout>());
    dangerous_buttons_container->add_component(pimpl_->delete_button_);

    auto buttons_container = std::make_shared<munin::basic_container>();
    buttons_container->set_layout(std::make_shared<munin::compass_layout>());
    buttons_container->add_component(
        safe_buttons_container, munin::COMPASS_LAYOUT_WEST);
    buttons_container->add_component(
        std::make_shared<munin::filled_box>(' '), munin::COMPASS_LAYOUT_CENTRE);
    buttons_container->add_component(
        dangerous_buttons_container, munin::COMPASS_LAYOUT_EAST);

    auto content = get_container();
    content->set_layout(std::make_shared<munin::compass_layout>());
    content->add_component(pimpl_->split_container_, munin::COMPASS_LAYOUT_CENTRE);
    content->add_component(buttons_container, munin::COMPASS_LAYOUT_SOUTH);
}
    
// ==========================================================================
// DESTRUCTOR
// ==========================================================================
encounters_page::~encounters_page()
{
    for (auto &cnx : pimpl_->connections_)
    {
        cnx.disconnect();
    }
}

// ==========================================================================
// SET_ENCOUNTERS
// ==========================================================================
void encounters_page::set_encounters(
    std::vector<std::shared_ptr<paradice::encounter>> const &encounters)
{
    pimpl_->encounters_ = encounters;

    std::vector<terminalpp::string> encounter_names;

    for (auto const &enc : pimpl_->encounters_)
    {
        encounter_names.push_back(enc->get_name());
    }

    pimpl_->encounters_list_->set_items(encounter_names);
    pimpl_->encounters_list_->set_item_index(-1);
}

// ==========================================================================
// GET_ENCOUNTERS
// ==========================================================================
std::vector<std::shared_ptr<paradice::encounter>> 
    encounters_page::get_encounters() const
{
    return pimpl_->encounters_;
}

// ==========================================================================
// GET_SELECTED_ENCOUNTER
// ==========================================================================
std::shared_ptr<paradice::encounter> encounters_page::get_selected_encounter() const
{
    auto selected_index = pimpl_->encounters_list_->get_item_index();

    return selected_index == -1
      ? std::shared_ptr<paradice::encounter>()
      : pimpl_->encounters_[selected_index];
}

}

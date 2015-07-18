// ==========================================================================
// Hugin GM Tools Screen
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
#include "hugin/gm_tools_screen.hpp"
#include "hugin/beast_editor.hpp"
#include "hugin/bestiary_page.hpp"
#include "hugin/encounter_editor.hpp"
#include "hugin/encounters_page.hpp"
#include "hugin/delete_confirmation_dialog.hpp"
#include "munin/algorithm.hpp"
#include "munin/basic_container.hpp"
#include "munin/button.hpp"
#include "munin/card.hpp"
#include "munin/compass_layout.hpp"
#include "munin/filled_box.hpp"
#include "munin/tabbed_panel.hpp"
#include <vector>

namespace hugin {

namespace {
    BOOST_STATIC_CONSTANT(std::string, bestiary_face = "BESTIARY");
    BOOST_STATIC_CONSTANT(std::string, beast_editor_face = "BEAST_EDITOR");
    BOOST_STATIC_CONSTANT(std::string, delete_beast_face = "DELETE_BEAST");

    BOOST_STATIC_CONSTANT(std::string, encounters_face = "ENCOUNTERS");
    BOOST_STATIC_CONSTANT(std::string, encounter_editor_face = "ENCOUNTER_EDITOR");
    BOOST_STATIC_CONSTANT(std::string, delete_encounter_face = "DELETE_ENCOUNTER");
}

// ==========================================================================
// GM_TOOLS_SCREEN::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct gm_tools_screen::impl
{
    std::shared_ptr<munin::tabbed_panel>        tabbed_panel_;
    std::shared_ptr<munin::card>                bestiary_tab_card_;
    std::shared_ptr<bestiary_page>              bestiary_page_;
    std::shared_ptr<beast_editor>               beast_editor_;
    std::shared_ptr<munin::card>                encounter_tab_card_;
    std::shared_ptr<encounters_page>            encounters_page_;
    std::shared_ptr<encounter_editor>           encounter_editor_;
    std::shared_ptr<delete_confirmation_dialog> delete_beast_dialog_;
    std::shared_ptr<delete_confirmation_dialog> delete_encounter_dialog_;
    std::shared_ptr<munin::button>              back_button_;

    std::shared_ptr<paradice::beast>            current_beast_;
    std::shared_ptr<paradice::encounter>        current_encounter_;

    void on_edit_beast()
    {
        auto selected_beast = bestiary_page_->get_selected_beast();

        if (selected_beast)
        {
            current_beast_ = selected_beast;
            beast_editor_->set_beast_name(
                selected_beast->get_name());
            beast_editor_->set_beast_description(
                selected_beast->get_description());

            bestiary_tab_card_->select_face(beast_editor_face);
            bestiary_tab_card_->set_focus();
        }
    }

    void on_new_beast()
    {
        current_beast_ = std::make_shared<paradice::beast>();

        beast_editor_->set_beast_name("New Beast");
        beast_editor_->set_beast_description("");

        bestiary_tab_card_->select_face(beast_editor_face);
        bestiary_tab_card_->set_focus();
    }

    void on_clone_beast()
    {
        auto selected_beast = bestiary_page_->get_selected_beast();

        if (selected_beast)
        {
            current_beast_ = std::make_shared<paradice::beast>();
            beast_editor_->set_beast_name(
                "Clone of " + selected_beast->get_name());
            beast_editor_->set_beast_description(
                selected_beast->get_description());

            bestiary_tab_card_->select_face(beast_editor_face);
            bestiary_tab_card_->set_focus();
        }
    }

    void on_save_beast()
    {
        current_beast_->set_name(beast_editor_->get_beast_name());
        current_beast_->set_description(beast_editor_->get_beast_description());

        // If it's not in the beast list, then add it and make it selected.
        auto beasts = bestiary_page_->get_beasts();
        bool found = false;

        for (auto const &beast : beasts)
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
        bestiary_tab_card_->set_focus();
    }

    void on_revert_beast()
    {
        bestiary_tab_card_->select_face(bestiary_face);
        bestiary_tab_card_->set_focus();
    }

    void on_new_encounter()
    {
        current_encounter_ = std::make_shared<paradice::encounter>();

        encounter_editor_->set_encounter_name("New Encounter");
        encounter_editor_->set_bestiary(bestiary_page_->get_beasts());
        encounter_editor_->set_encounter_beasts(
            current_encounter_->get_beasts());

        encounter_tab_card_->select_face(encounter_editor_face);
        encounter_tab_card_->set_focus();
    }

    void on_edit_encounter()
    {
        auto selected_encounter = encounters_page_->get_selected_encounter();

        if (selected_encounter)
        {
            current_encounter_ = selected_encounter;
            encounter_editor_->set_encounter_name(
                current_encounter_->get_name());
            encounter_editor_->set_encounter_beasts(
                current_encounter_->get_beasts());
            encounter_editor_->set_bestiary(
                bestiary_page_->get_beasts());

            encounter_tab_card_->select_face(encounter_editor_face);
            encounter_tab_card_->set_focus();
        }
    }

    void on_clone_encounter()
    {
        auto selected_encounter = encounters_page_->get_selected_encounter();

        if (selected_encounter)
        {
            current_encounter_ = std::make_shared<paradice::encounter>();
            encounter_editor_->set_encounter_name(
                "Clone of " + selected_encounter->get_name());

            // Deep copy the beasts.
            std::vector<std::shared_ptr<paradice::beast>> beasts;

            for (auto const &original_beast : selected_encounter->get_beasts())
            {
                auto new_beast = std::make_shared<paradice::beast>();

                new_beast->set_name(original_beast->get_name());
                new_beast->set_description(original_beast->get_description());
                beasts.push_back(new_beast);
            }

            encounter_editor_->set_encounter_beasts(beasts);
            encounter_editor_->set_bestiary(bestiary_page_->get_beasts());

            encounter_tab_card_->select_face(encounter_editor_face);
            encounter_tab_card_->set_focus();
        }
    }

    void on_delete_encounter()
    {
        auto selected_encounter = encounters_page_->get_selected_encounter();

        if (selected_encounter)
        {
            delete_encounter_dialog_->set_deletion_target_text(
                selected_encounter->get_name());
            encounter_tab_card_->select_face(delete_encounter_face);
            encounter_tab_card_->set_focus();
        }
    }

    void on_save_encounter()
    {
        // Copy encounter attributess to current_encounter_
        current_encounter_->set_name(encounter_editor_->get_encounter_name());
        current_encounter_->set_beasts(
            encounter_editor_->get_encounter_beasts());

        // If it's not in the encounter list, then add it and make it selected.
        auto encounters = encounters_page_->get_encounters();

        if (std::find(encounters.begin(), encounters.end(), current_encounter_)
            == encounters.end())
        {
            encounters.push_back(current_encounter_);
        }

        encounters_page_->set_encounters(encounters);
        encounter_tab_card_->select_face(encounters_face);
        encounter_tab_card_->set_focus();
    }

    void on_revert_encounter()
    {
        encounter_tab_card_->select_face(encounters_face);
        encounter_tab_card_->set_focus();
    }

    void on_delete_beast()
    {
        auto selected_beast = bestiary_page_->get_selected_beast();

        if (selected_beast)
        {
            delete_beast_dialog_->set_deletion_target_text(
                selected_beast->get_name());
            bestiary_tab_card_->select_face(delete_beast_face);
            bestiary_tab_card_->set_focus();
        }
    }

    void on_delete_beast_confirmation()
    {
        auto selected_beast = bestiary_page_->get_selected_beast();

        if (selected_beast)
        {
            auto beasts = bestiary_page_->get_beasts();
            beasts.erase(std::remove(
                beasts.begin()
              , beasts.end()
              , selected_beast));
            bestiary_page_->set_beasts(beasts);
        }

        bestiary_tab_card_->select_face(bestiary_face);
        bestiary_tab_card_->set_focus();
    }

    void on_delete_beast_rejection()
    {
        bestiary_tab_card_->select_face(bestiary_face);
        bestiary_tab_card_->set_focus();
    }

    void on_delete_encounter_confirmation()
    {
        auto selected_encounter = encounters_page_->get_selected_encounter();

        if (selected_encounter)
        {
            auto encounters = encounters_page_->get_encounters();
            encounters.erase(remove(
                encounters.begin()
              , encounters.end()
              , selected_encounter));
            encounters_page_->set_encounters(encounters);
        }

        encounter_tab_card_->select_face(encounters_face);
        encounter_tab_card_->set_focus();
    }

    void on_delete_encounter_rejection()
    {
        encounter_tab_card_->select_face(encounters_face);
        encounter_tab_card_->set_focus();
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
gm_tools_screen::gm_tools_screen()
    : pimpl_(std::make_shared<impl>())
{
    pimpl_->bestiary_page_           = std::make_shared<bestiary_page>();
    pimpl_->beast_editor_            = std::make_shared<beast_editor>();
    pimpl_->encounters_page_         = std::make_shared<encounters_page>();
    pimpl_->encounter_editor_        = std::make_shared<encounter_editor>();
    pimpl_->delete_beast_dialog_     = std::make_shared<delete_confirmation_dialog>();
    pimpl_->delete_encounter_dialog_ = std::make_shared<delete_confirmation_dialog>();

    pimpl_->bestiary_tab_card_   = std::make_shared<munin::card>();
    pimpl_->bestiary_tab_card_->add_face(pimpl_->bestiary_page_, bestiary_face);
    pimpl_->bestiary_tab_card_->add_face(pimpl_->beast_editor_, beast_editor_face);
    pimpl_->bestiary_tab_card_->add_face(pimpl_->delete_beast_dialog_, delete_beast_face);
    pimpl_->bestiary_tab_card_->select_face(bestiary_face);

    pimpl_->encounter_tab_card_  = std::make_shared<munin::card>();
    pimpl_->encounter_tab_card_->add_face(pimpl_->encounters_page_, encounters_face);
    pimpl_->encounter_tab_card_->add_face(pimpl_->encounter_editor_, encounter_editor_face);
    pimpl_->encounter_tab_card_->add_face(pimpl_->delete_encounter_dialog_, delete_encounter_face);
    pimpl_->encounter_tab_card_->select_face(encounters_face);

    pimpl_->bestiary_page_->on_edit.connect(
        [this](auto beast){pimpl_->on_edit_beast();});
    pimpl_->bestiary_page_->on_new.connect([this]{pimpl_->on_new_beast();});
    pimpl_->bestiary_page_->on_clone.connect(
        [this](auto beast){pimpl_->on_clone_beast();});
    pimpl_->bestiary_page_->on_fight.connect(on_fight_beast);
    pimpl_->bestiary_page_->on_delete.connect(
        [this](auto beast){pimpl_->on_delete_beast();});
    
    pimpl_->beast_editor_->on_save.connect([this]{pimpl_->on_save_beast();});
    pimpl_->beast_editor_->on_revert.connect([this]{pimpl_->on_revert_beast();});
    
    pimpl_->encounters_page_->on_new.connect([this]{pimpl_->on_new_encounter();});
    pimpl_->encounters_page_->on_edit.connect(
        [this](auto encounter){pimpl_->on_edit_encounter();});
    pimpl_->encounters_page_->on_clone.connect(
        [this](auto encounter){pimpl_->on_clone_encounter();});
    pimpl_->encounters_page_->on_fight.connect(on_fight_encounter);
    pimpl_->encounters_page_->on_delete.connect(
        [this](auto encounter){pimpl_->on_delete_encounter();});
    
    pimpl_->encounter_editor_->on_save.connect(
        [this]{pimpl_->on_save_encounter();});
    pimpl_->encounter_editor_->on_revert.connect(
        [this]{pimpl_->on_revert_encounter();});
    
    pimpl_->delete_beast_dialog_->on_delete_confirmation.connect(
        [this]{pimpl_->on_delete_beast_confirmation();});
    pimpl_->delete_beast_dialog_->on_delete_rejection.connect(
        [this]{pimpl_->on_delete_beast_rejection();});
    
    pimpl_->delete_encounter_dialog_->on_delete_confirmation.connect(
        [this]{pimpl_->on_delete_encounter_confirmation();});
    pimpl_->delete_encounter_dialog_->on_delete_rejection.connect(
        [this]{pimpl_->on_delete_encounter_rejection();});
    
    pimpl_->back_button_ = std::make_shared<munin::button>(
        munin::string_to_elements("Back"));
    pimpl_->back_button_->on_click.connect(on_back);

    pimpl_->tabbed_panel_ = std::make_shared<munin::tabbed_panel>();
    pimpl_->tabbed_panel_->insert_tab("Bestiary", pimpl_->bestiary_tab_card_);
    pimpl_->tabbed_panel_->insert_tab("Encounters", pimpl_->encounter_tab_card_);

    auto buttons_panel = std::make_shared<munin::basic_container>();
    buttons_panel->set_layout(std::make_shared<munin::compass_layout>());
    buttons_panel->add_component(pimpl_->back_button_, munin::COMPASS_LAYOUT_WEST);
    buttons_panel->add_component(
        std::make_shared<munin::filled_box>(munin::element_type(' '))
        , munin::COMPASS_LAYOUT_CENTRE);

    auto content = get_container();
    content->set_layout(std::make_shared<munin::compass_layout>());
    content->add_component(pimpl_->tabbed_panel_, munin::COMPASS_LAYOUT_CENTRE);
    content->add_component(buttons_panel, munin::COMPASS_LAYOUT_SOUTH);
}
    
// ==========================================================================
// DESTRUCTOR
// ==========================================================================
gm_tools_screen::~gm_tools_screen()
{
}

// ==========================================================================
// SET_BEASTS
// ==========================================================================
void gm_tools_screen::set_beasts(
    std::vector<std::shared_ptr<paradice::beast>> const &beasts)
{
    pimpl_->bestiary_page_->set_beasts(beasts);
}

// ==========================================================================
// GET_BEASTS
// ==========================================================================
std::vector< std::shared_ptr<paradice::beast> > gm_tools_screen::get_beasts() const
{
    return pimpl_->bestiary_page_->get_beasts();
}

// ==========================================================================
// SET_ENCOUNTERS
// ==========================================================================
void gm_tools_screen::set_encounters(
    std::vector<std::shared_ptr<paradice::encounter>> const &encounters)
{
    pimpl_->encounters_page_->set_encounters(encounters);
}

// ==========================================================================
// GET_BEASTS
// ==========================================================================
std::vector< std::shared_ptr<paradice::encounter> > gm_tools_screen::get_encounters() const
{
    return pimpl_->encounters_page_->get_encounters();
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void gm_tools_screen::do_event(boost::any const &ev)
{
    bool handled = false;

    auto const *ch = boost::any_cast<char>(&ev);
    auto const *control_sequence = 
        boost::any_cast<odin::ansi::control_sequence>(&ev);

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

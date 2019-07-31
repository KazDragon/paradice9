// ==========================================================================
// Hugin Active Encounter view
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
#include "hugin/active_encounter_view.hpp"
#include <munin/algorithm.hpp>
#include <munin/container.hpp>
#include <munin/grid_layout.hpp>
#include <munin/list.hpp>
#include <munin/scroll_pane.hpp>
#include <paradice/beast.hpp>
#include <paradice/character.hpp>
#include <terminalpp/encoder.hpp>
#include <boost/format.hpp>
#include <numeric>

namespace hugin {

// ==========================================================================
// ACTIVE_ENCOUNTER_VIEW::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct active_encounter_view::impl
{
    std::shared_ptr<paradice::active_encounter> encounter_;
    bool                                        gm_mode_;

    std::shared_ptr<munin::list>                participant_list_;

    struct active_encounter_entry_visitor : boost::static_visitor<std::string>
    {
        std::string operator()(paradice::active_encounter::player const &ply) const
        {
            std::shared_ptr<paradice::character> ch = ply.character_.lock();

            if (ch)
            {
                return ch->get_name();
            }
            else
            {
                return ply.name_;
            }
        }

        std::string operator()(std::shared_ptr<paradice::beast> const &beast) const
        {
            return beast->get_name();
        }
    };
    
    void update()
    {
        if (!encounter_)
        {
            participant_list_->set_items({});
            return;
        }

        auto selected_index = participant_list_->get_item_index();

        active_encounter_entry_visitor entry_visitor;
        std::vector<terminalpp::string> list_data;

        for (auto const &entry : encounter_->entries_)
        {
            std::string name = boost::apply_visitor(entry_visitor, entry.participant_);
            std::string text = boost::str(boost::format("(%d)") % entry.id_)
                 + " "
                 + name;

            if (!entry.annotation_.empty())
            {
                text += boost::str(boost::format(" [%s]") % entry.annotation_);
            }

            boost::optional<paradice::dice_result> const &last_roll =
                entry.roll_data_.empty()
              ? boost::optional<paradice::dice_result>()
              : entry.roll_data_.back();

            if (last_roll)
            {
                std::int32_t total_score = 0;

                for (auto const &repetition : last_roll->results_)
                {
                    using std::begin;
                    using std::end;
                    
                    total_score += std::accumulate(
                        begin(repetition),
                        end(repetition),
                        last_roll->roll_.bonus_);
                }

                text += boost::str(
                    boost::format(" | %s -> %d")
                        % describe_dice(last_roll->roll_) 
                        % total_score);
            }

            list_data.push_back(terminalpp::encode(text));
        }

        participant_list_->set_items(list_data);
        participant_list_->set_item_index(selected_index);
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
active_encounter_view::active_encounter_view()
    : pimpl_(std::make_shared<impl>())
{
    pimpl_->gm_mode_ = false;
    pimpl_->participant_list_ = munin::make_list();

    auto content = get_container();
    content->set_layout(munin::make_grid_layout(1, 1));
    content->add_component(munin::make_scroll_pane(pimpl_->participant_list_));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
active_encounter_view::~active_encounter_view()
{
}

// ==========================================================================
// SET_GM_MODE
// ==========================================================================
void active_encounter_view::set_gm_mode(bool mode)
{
    pimpl_->gm_mode_ = mode;
    pimpl_->update();
}

// ==========================================================================
// SET_ENCOUNTER
// ==========================================================================
void active_encounter_view::set_encounter(
    std::shared_ptr<paradice::active_encounter> const &encounter)
{
    pimpl_->encounter_ = encounter;
    pimpl_->update();
}

}

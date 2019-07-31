// ==========================================================================
// Paradice Active Encounter
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
#include "paradice/active_encounter.hpp"
#include "paradice/beast.hpp"
#include <algorithm>

namespace paradice {

// ==========================================================================
// ADD_PARTICIPANT
// ==========================================================================
void add_participant(
    std::shared_ptr<active_encounter> const &enc
  , active_encounter::participant const &part)
{
    // First, find the maximum ID currently used in the encounter.
    std::uint32_t max_id = 0;

    for (auto const &ent : enc->entries_)
    {
        max_id = (std::max)(ent.id_, max_id);
    }

    // The next ID should be one higher than the maximum ID.
    active_encounter::entry new_entry;
    new_entry.id_ = max_id + 1;
    new_entry.participant_ = part;

    enc->entries_.push_back(new_entry);
}

// ==========================================================================
// ADD_CHARACTER
// ==========================================================================
void add_character(
    std::shared_ptr<active_encounter> const &enc,
    std::shared_ptr<character> const &ch)
{
    // Create a player from the character.
    active_encounter::player ply;
    ply.character_ = ch;
    ply.name_ = ch->get_name();

    // Add the player as a participant.
    add_participant(enc, ply);
}

// ==========================================================================
// ADD_BEAST
// ==========================================================================
void add_beast(
    std::shared_ptr<active_encounter> const &enc,
    std::shared_ptr<paradice::beast> const &beast)
{
    auto cloned_beast = std::make_shared<paradice::beast>();
    cloned_beast->set_name(beast->get_name());
    cloned_beast->set_description(beast->get_description());

    add_participant(enc, cloned_beast);
}

}
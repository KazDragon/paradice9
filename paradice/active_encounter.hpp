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
#ifndef PARADICE_ACTIVE_ENCOUNTER_HPP_
#define PARADICE_ACTIVE_ENCOUNTER_HPP_

#include "paradice/beast.hpp"
#include "paradice/character.hpp"
#include <boost/variant.hpp>
#include <boost/weak_ptr.hpp>
#include <string>
#include <vector>

namespace paradice {

//* =========================================================================
/// \brief A structure to represent an active in-game encounter.
//* =========================================================================
struct active_encounter
{
    struct player
    {
        boost::weak_ptr<character> character_;
        std::string name_;
    };

    typedef boost::variant<
        player,
        boost::shared_ptr<beast>
    > participant;

    struct entry
    {
        participant participant_;
        std::string last_roll_;
        odin::u32   id_;
    };

    std::vector<entry> entries_;
};

//* =========================================================================
/// \brief Add a new participant to the encounter.
//* =========================================================================
void add_participant(
    boost::shared_ptr<active_encounter> enc
  , active_encounter::participant part);

//* =========================================================================
/// \brief Adds a character to the encounter.
//* =========================================================================
void add_character(
    boost::shared_ptr<active_encounter> enc
  , boost::shared_ptr<character> ch);

//* =========================================================================
/// \brief Adds a beast (clones it first) to the encounter.
//* =========================================================================
void add_beast(
    boost::shared_ptr<active_encounter> enc
  , boost::shared_ptr<paradice::beast> beast);

}

#endif


// ==========================================================================
// Paradice Character
//
// Copyright (C) 2011 Matthew Chaplain, All Rights Reserved.
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
#include "paradice/character.hpp"

namespace paradice {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
character::character()
  : gm_level_(0)
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
character::~character()
{
}

// ==========================================================================
// SET_NAME
// ==========================================================================
void character::set_name(std::string const &name)
{
    name_ = name;
}

// ==========================================================================
// GET_NAME
// ==========================================================================
std::string character::get_name() const
{
    return name_;
}

// ==========================================================================
// SET_PREFIX
// ==========================================================================
void character::set_prefix(std::string const &prefix)
{
    prefix_ = prefix;
}

// ==========================================================================
// GET_PREFIX
// ==========================================================================
std::string character::get_prefix() const
{
    return prefix_;
}

// ==========================================================================
// SET_SUFFIX
// ==========================================================================
void character::set_suffix(std::string const &suffix)
{
    suffix_ = suffix;
}

// ==========================================================================
// GET_SUFFIX
// ==========================================================================
std::string character::get_suffix() const
{
    return suffix_;
}

// ==========================================================================
// SET_GM_LEVEL
// ==========================================================================
void character::set_gm_level(odin::u32 gm_level)
{
    gm_level_ = gm_level;
}

// ==========================================================================
// GET_GM_LEVEL
// ==========================================================================
odin::u32 character::get_gm_level() const
{
    return gm_level_;
}

// ==========================================================================
// SET_BEASTS
// ==========================================================================
void character::set_beasts(std::vector<std::shared_ptr<beast>> const &beasts)
{
    beasts_ = std::move(beasts);
}

// ==========================================================================
// GET_BEASTS
// ==========================================================================
std::vector<std::shared_ptr<beast>> character::get_beasts() const
{
    return beasts_;
}

// ==========================================================================
// SET_ENCOUNTERS
// ==========================================================================
void character::set_encounters(std::vector<std::shared_ptr<encounter>> const &encounters)
{
    encounters_ = std::move(encounters);
}

// ==========================================================================
// GET_ENCOUNTERS
// ==========================================================================
std::vector<std::shared_ptr<encounter>> character::get_encounters() const
{
    return encounters_;
}

}

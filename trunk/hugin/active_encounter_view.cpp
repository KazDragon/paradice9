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
#include <boost/make_shared.hpp>

using namespace boost;

namespace hugin {

// ==========================================================================
// ACTIVE_ENCOUNTER_VIEW::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct active_encounter_view::impl
{
    shared_ptr<paradice::encounter> encounter_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
active_encounter_view::active_encounter_view()
    : pimpl_(make_shared<impl>())
{
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
}

// ==========================================================================
// SET_ENCOUNTER
// ==========================================================================
void active_encounter_view::set_encounter(
    shared_ptr<paradice::encounter> encounter)
{
}

}

// ==========================================================================
// Hugin Encounters Screen
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
#include "hugin/encounters_screen.hpp"
#include "hugin/bestiary_page.hpp"
#include "hugin/encounters_page.hpp"
#include "munin/grid_layout.hpp"
#include "munin/tabbed_panel.hpp"
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace munin;
using namespace boost;

namespace hugin {

// ==========================================================================
// ENCOUNTERS_SCREEN::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct encounters_screen::impl
{
    shared_ptr<tabbed_panel>    tabbed_panel_;
    shared_ptr<bestiary_page>   bestiary_page_;
    shared_ptr<encounters_page> encounters_page_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
encounters_screen::encounters_screen()
    : pimpl_(make_shared<impl>())
{
    pimpl_->bestiary_page_   = make_shared<bestiary_page>();
    pimpl_->encounters_page_ = make_shared<encounters_page>();

    pimpl_->tabbed_panel_ = make_shared<tabbed_panel>();
    pimpl_->tabbed_panel_->insert_tab("Bestiary", pimpl_->bestiary_page_);
    pimpl_->tabbed_panel_->insert_tab("Encounters", pimpl_->encounters_page_);

    BOOST_AUTO(content, get_container());
    content->set_layout(make_shared<grid_layout>(1, 1));
    content->add_component(pimpl_->tabbed_panel_);
}
    
// ==========================================================================
// DESTRUCTOR
// ==========================================================================
encounters_screen::~encounters_screen()
{
}
    
}

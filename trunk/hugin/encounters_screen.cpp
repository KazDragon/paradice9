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
#include "munin/algorithm.hpp"
#include "munin/basic_container.hpp"
#include "munin/button.hpp"
#include "munin/compass_layout.hpp"
#include "munin/filled_box.hpp"
#include "munin/tabbed_panel.hpp"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>
#include <vector>

using namespace munin;
using namespace boost;
using namespace std;

namespace hugin {

// ==========================================================================
// ENCOUNTERS_SCREEN::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct encounters_screen::impl
{
    shared_ptr<tabbed_panel>    tabbed_panel_;
    shared_ptr<bestiary_page>   bestiary_page_;
    shared_ptr<encounters_page> encounters_page_;
    shared_ptr<button>          back_button_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
encounters_screen::encounters_screen()
    : pimpl_(make_shared<impl>())
{
    pimpl_->bestiary_page_   = make_shared<bestiary_page>();
    pimpl_->encounters_page_ = make_shared<encounters_page>();

    pimpl_->back_button_ = make_shared<button>(
        string_to_elements("Back"));
    pimpl_->back_button_->on_click.connect(bind(ref(on_back)));

    pimpl_->tabbed_panel_ = make_shared<tabbed_panel>();
    pimpl_->tabbed_panel_->insert_tab("Bestiary", pimpl_->bestiary_page_);
    pimpl_->tabbed_panel_->insert_tab("Encounters", pimpl_->encounters_page_);

    BOOST_AUTO(buttons_panel, make_shared<basic_container>());
    buttons_panel->set_layout(make_shared<compass_layout>());
    buttons_panel->add_component(pimpl_->back_button_, COMPASS_LAYOUT_WEST);
    buttons_panel->add_component(
        make_shared<filled_box>(element_type(' '))
      , COMPASS_LAYOUT_CENTRE);

    BOOST_AUTO(content, get_container());
    content->set_layout(make_shared<compass_layout>());
    content->add_component(pimpl_->tabbed_panel_, COMPASS_LAYOUT_CENTRE);
    content->add_component(buttons_panel, COMPASS_LAYOUT_SOUTH);
}
    
// ==========================================================================
// DESTRUCTOR
// ==========================================================================
encounters_screen::~encounters_screen()
{
}
    
}

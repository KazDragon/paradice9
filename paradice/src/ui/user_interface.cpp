// ==========================================================================
// Paradice User Interface
//
// Copyright (C) 2020 Matthew Chaplain, All Rights Reserved.
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
#include "paradice/ui/user_interface.hpp"
#include "paradice/ui/account_creation_page.hpp"
#include "paradice/ui/title_page.hpp"
#include <terminalpp/virtual_key.hpp>
#include <munin/brush.hpp>
#include <munin/grid_layout.hpp>
#include <boost/make_unique.hpp>

namespace paradice { namespace ui {

// ==========================================================================
// USER_INTERFACE::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct user_interface::impl
{
    impl(user_interface &self)
      : self_(self)
    {
        content_->set_layout(munin::make_grid_layout({1, 1}));
    }

    // ======================================================================
    // DROP_CONTENT
    // ======================================================================
    void drop_content()
    {
        content_->lose_focus();
        content_->remove_component(last_content_);
    }

    // ======================================================================
    // GO_TO_TITLE_PAGE
    // ======================================================================
    void go_to_title_page()
    {
        drop_content();
        auto new_page = std::make_shared<title_page>();
        new_page->on_new_account.connect(
            [this]{go_to_account_creation_page();});

        content_->add_component(new_page);
        last_content_ = new_page;

        content_->set_focus();
        self_.on_redraw({{{}, self_.get_size()}});
    }

    // ======================================================================
    // GO_TO_ACCOUNT_CREATION_PAGE
    // ======================================================================
    void go_to_account_creation_page()
    {
        drop_content();
        auto new_page = std::make_shared<account_creation_page>();
        new_page->on_return.connect([this]{go_to_title_page();});

        content_->add_component(new_page);
        last_content_ = new_page;
        content_->set_focus();
        self_.on_redraw({{{}, self_.get_size()}});
    }

    // ======================================================================
    // HANDLE_MOUSE_EVENT
    // ======================================================================
    bool handle_mouse_event(terminalpp::ansi::mouse::report const &report)
    {
        return false;
    }

    // ======================================================================
    // HANDLE_VIRTUAL_KEY_EVENT
    // ======================================================================
    bool handle_virtual_key_event(terminalpp::virtual_key const &vk)
    {
        switch (vk.key)
        {
            case terminalpp::vk::ht:
                self_.focus_next();

                while (!self_.has_focus())
                {
                    self_.focus_next();
                }

                return true;

            case terminalpp::vk::bt:
                self_.focus_previous();

                while (!self_.has_focus())
                {
                    self_.focus_previous();
                }

                return true;

            default:
                return false;
        }
    }

    user_interface                    &self_;
    std::shared_ptr<munin::container>  content_{munin::make_container()};
    std::shared_ptr<munin::component>  last_content_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
user_interface::user_interface()
  : pimpl_(boost::make_unique<impl>(*this))
{
    using namespace terminalpp::literals;

    set_layout(munin::make_grid_layout({1, 1}));
    add_component(pimpl_->content_);
    pimpl_->go_to_title_page();
};

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
user_interface::~user_interface() = default;

// ==========================================================================
// EVENT
// ==========================================================================
void user_interface::do_event(boost::any const &event)
{
    bool handled = false;

    if (!handled)
    {
        auto const *vk_event = 
            boost::any_cast<terminalpp::virtual_key>(&event);
        handled = vk_event != nullptr 
              && pimpl_->handle_virtual_key_event(*vk_event);
    }

    if (!handled)
    {
        auto const *mouse_event = 
            boost::any_cast<terminalpp::ansi::mouse::report>(&event);
        handled = mouse_event != nullptr
              && pimpl_->handle_mouse_event(*mouse_event);
    }

    if (!handled)
    {
        composite_component::do_event(event);
    }
}

}}

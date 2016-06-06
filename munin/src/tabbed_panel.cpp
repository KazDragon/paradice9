// ==========================================================================
// Munin Tabbed Panel.
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
#include "munin/tabbed_panel.hpp"
#include "munin/card.hpp"
#include "munin/container.hpp"
#include "munin/framed_component.hpp"
#include "munin/grid_layout.hpp"
#include "munin/tabbed_frame.hpp"

namespace munin {

// ==========================================================================
// TABBED_PANEL::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct tabbed_panel::impl
{
    std::shared_ptr<card>         card_;
    std::shared_ptr<tabbed_frame> frame_;

    // ======================================================================
    // TAB_SELECTED
    // ======================================================================
    void tab_selected(std::string const &text)
    {
        card_->select_face(text);
    }

    // ======================================================================
    // FOCUS_CHANGE
    // ======================================================================
    void focus_change()
    {
        frame_->set_highlight(card_->has_focus() || frame_->has_focus());
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
tabbed_panel::tabbed_panel()
    : pimpl_(new impl)
{
    pimpl_->card_  = make_card();
    pimpl_->frame_ = make_tabbed_frame();

    auto inner = make_framed_component(pimpl_->frame_, pimpl_->card_);

    auto content = get_container();
    content->set_layout(make_grid_layout(1, 1));
    content->add_component(inner);

    pimpl_->frame_->on_tab_selected.connect(
        [wpthis=std::weak_ptr<impl>(pimpl_)]
        (auto idx)
        {
            auto pthis = wpthis.lock();

            if (pthis)
            {
                pthis->tab_selected(idx);
            }
        });

    auto focus_callback =
        [wpthis=std::weak_ptr<impl>(pimpl_)]
        {
            auto pthis = wpthis.lock();

            if (pthis)
            {
                pthis->focus_change();
            }
        };

    pimpl_->frame_->on_focus_set.connect(focus_callback);
    pimpl_->frame_->on_focus_lost.connect(focus_callback);
    pimpl_->card_->on_focus_set.connect(focus_callback);
    pimpl_->card_->on_focus_lost.connect(focus_callback);
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
tabbed_panel::~tabbed_panel()
{
}

// ==========================================================================
// INSERT_TAB
// ==========================================================================
void tabbed_panel::insert_tab(
    std::string                const &text,
    std::shared_ptr<component> const &comp,
    boost::optional<odin::u32>        index /*= optional<u32>()*/)
{
    pimpl_->card_->add_face(comp, text);
    pimpl_->frame_->insert_tab(text, index);

    if (pimpl_->card_->get_number_of_faces() == 1)
    {
        pimpl_->card_->select_face(text);
    }
}

// ==========================================================================
// REMOVE_TAB
// ==========================================================================
void tabbed_panel::remove_tab(odin::u32 index)
{
    pimpl_->frame_->remove_tab(index);
}

// ==========================================================================
// MAKE_TABBED_PANEL
// ==========================================================================
std::shared_ptr<tabbed_panel> make_tabbed_panel()
{
    return std::make_shared<tabbed_panel>();
}

}

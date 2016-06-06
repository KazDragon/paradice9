// ==========================================================================
// Munin Drop-Down List Component.
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
#include "munin/dropdown_list.hpp"
#include "munin/background_fill.hpp"
#include "munin/compass_layout.hpp"
#include "munin/filled_box.hpp"
#include "munin/grid_layout.hpp"
#include "munin/horizontal_strip_layout.hpp"
#include "munin/horizontal_squeeze_layout.hpp"
#include "munin/image.hpp"
#include "munin/list.hpp"
#include "munin/sco_glyphs.hpp"
#include "munin/scroll_pane.hpp"
#include "munin/vertical_squeeze_layout.hpp"
#include "munin/view.hpp"
#include <terminalpp/ansi/mouse.hpp>
#include <terminalpp/string.hpp>
#include <terminalpp/virtual_key.hpp>

namespace munin {

namespace {

// ==========================================================================
// FIXED_HEIGHT_LAYOUT
// ==========================================================================
class fixed_height_layout : public layout
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    fixed_height_layout(odin::u32 preferred_height)
        : preferred_height_(preferred_height)
    {
    }

protected :
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to retrieve the preferred size of the layout
    /// in a custom manner.
    //* =====================================================================
    virtual terminalpp::extent do_get_preferred_size(
        std::vector<std::shared_ptr<component>> const &components,
        std::vector<boost::any>                 const &hints) const
    {
        // The preferred width is the width of the first component.  There
        // should only be one component in this anyway.
        auto preferred_size =
            components.empty()
          ? terminalpp::extent{}
          : terminalpp::extent(components[0]->get_preferred_size().width
                             , preferred_height_);

        return preferred_size;
    }

    //* =====================================================================
    /// \brief Called by operator().  Derived classes must override this
    /// function in order to lay a container's components out in a custom
    /// manner.
    //* =====================================================================
    virtual void do_layout(
        std::vector<std::shared_ptr<component>> const &components,
        std::vector<boost::any>                 const &hints,
        terminalpp::extent                             size)
    {
        for (auto current_component : components)
        {
            current_component->set_position({});
            current_component->set_size(size);
        }
    }

    odin::u32 preferred_height_;
};

}

// ==========================================================================
// DROPDOWN_LIST::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct dropdown_list::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(dropdown_list &self)
        : self_(self)
        , dropdown_open_(false)
        , closing_dropdown_(false)
    {
        focussed_pen_.foreground_colour_ = terminalpp::high_colour(1, 4, 5);
    }

    // ======================================================================
    // ON_ITEM_CHANGED
    // ======================================================================
    void on_item_changed(odin::s32)
    {
        selected_text_->set_image(self_.get_item());
    }

    // ======================================================================
    // OPEN_DROPDOWN
    // ======================================================================
    void open_dropdown()
    {
        if (!dropdown_open_)
        {
            dropdown_open_ = true;
            self_.get_container()->add_component(dropdown_);

            bottom_left_corner_->set_attribute(
                ATTRIBUTE_GLYPH
              , double_lined_left_tee);

            bottom_tee_->set_attribute(
                ATTRIBUTE_GLYPH
              , double_lined_cross);

            list_->set_focus();
            self_.on_layout_change();
        }
    }

    // ======================================================================
    // CLOSE_DROPDOWN
    // ======================================================================
    void close_dropdown()
    {
        if (dropdown_open_)
        {
            closing_dropdown_ = true;
            dropdown_open_ = false;
            dropdown_button_->set_focus();
            self_.get_container()->remove_component(dropdown_);

            bottom_left_corner_->set_attribute(
                ATTRIBUTE_GLYPH
              , double_lined_bottom_left_corner);

            bottom_tee_->set_attribute(
                ATTRIBUTE_GLYPH
              , double_lined_bottom_tee);
            closing_dropdown_ = false;

            self_.on_layout_change();
        }
    }

    // ======================================================================
    // UPDATE_PEN
    // ======================================================================
    void update_pen()
    {
        self_.set_attribute(
            ATTRIBUTE_PEN
          , self_.has_focus()
                ? focussed_pen_
                : unfocussed_pen_);
    }

    // ======================================================================
    // TOGGLE_DROPDOWN
    // ======================================================================
    void toggle_dropdown()
    {
        if (dropdown_open_)
        {
            close_dropdown();
        }
        else
        {
            open_dropdown();
        }
    }

    // ======================================================================
    // DO_CURSOR_DOWN_KEY_EVENT
    // ======================================================================
    bool do_cursor_down_key_event(odin::u32 times)
    {
        if (!dropdown_open_)
        {
            open_dropdown();
            return true;
        }
        else
        {
            return false;
        }
    }

    // ======================================================================
    // DO_ANSI_CONTROL_SEQUENCE_EVENT
    // ======================================================================
    bool do_vk_event(terminalpp::virtual_key const &vk)
    {
        switch (vk.key)
        {
            case terminalpp::vk::enter :
                toggle_dropdown();
                return true;
                
            case terminalpp::vk::cursor_down :
                return do_cursor_down_key_event(vk.repeat_count);
                
            default :
                // Do nothing.
                break;
        }
        
        return false;
    }

    // ======================================================================
    // DO_MOUSE_EVENT
    // ======================================================================
    bool do_mouse_event(terminalpp::ansi::mouse::report const &report)
    {
        // Detect if the click occurred within the dropdown-button area.
        // Otherwise, we don't care.
        if (report.x_position_ >= self_.get_size().width - 3
         && report.y_position_ < 3)
        {
            if (report.button_ == terminalpp::ansi::mouse::report::BUTTON_UP)
            {
                toggle_dropdown();
            }

            return true;
        }

        return false;
    }

    dropdown_list             &self_;
    terminalpp::attribute      focussed_pen_;
    terminalpp::attribute      unfocussed_pen_;
    std::shared_ptr<image>     selected_text_;
    std::shared_ptr<image>     dropdown_button_;
    std::shared_ptr<component> bottom_left_corner_;
    std::shared_ptr<component> bottom_tee_;
    std::shared_ptr<component> bottom_right_corner_;
    std::shared_ptr<list>      list_;
    std::shared_ptr<component> dropdown_;
    bool                       dropdown_open_;
    bool                       closing_dropdown_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
dropdown_list::dropdown_list()
{
    pimpl_ = std::make_shared<impl>(std::ref(*this));

    auto content = get_container();
    on_focus_set.connect([this]{pimpl_->update_pen();});
    on_focus_lost.connect([this]{pimpl_->update_pen();});

    // Construct the top bar +----+-+
    auto top_row = view(
        make_compass_layout(),
        make_fill(double_lined_top_left_corner), COMPASS_LAYOUT_WEST,
        make_fill(double_lined_horizontal_beam), COMPASS_LAYOUT_CENTRE,
        view(
            make_compass_layout(),
            make_fill(double_lined_top_tee),          COMPASS_LAYOUT_WEST,
            make_fill(double_lined_horizontal_beam),  COMPASS_LAYOUT_CENTRE,
            make_fill(double_lined_top_right_corner), COMPASS_LAYOUT_EAST
        ), COMPASS_LAYOUT_EAST);

    // Construct the centre bar |   |V|
    pimpl_->dropdown_button_ = make_image("V");
    pimpl_->dropdown_button_->set_can_focus(true);
    pimpl_->selected_text_ = make_image(" ");

    auto centre_row = view(
        make_compass_layout(),
        make_fill(double_lined_vertical_beam), COMPASS_LAYOUT_WEST,
        pimpl_->selected_text_,                COMPASS_LAYOUT_CENTRE,
        view(
            make_compass_layout(),
            make_fill(double_lined_vertical_beam), COMPASS_LAYOUT_WEST,
            pimpl_->dropdown_button_,              COMPASS_LAYOUT_CENTRE,
            make_fill(double_lined_vertical_beam), COMPASS_LAYOUT_EAST
        ), COMPASS_LAYOUT_EAST);
        
    // Construct the bottom bar +-----+-+
    pimpl_->bottom_left_corner_  = make_fill(double_lined_bottom_left_corner);
    pimpl_->bottom_tee_          = make_fill(double_lined_bottom_tee);
    pimpl_->bottom_right_corner_ = make_fill(double_lined_bottom_right_corner);

    auto bottom_row = view(
        make_compass_layout(),
        pimpl_->bottom_left_corner_, COMPASS_LAYOUT_WEST,
        make_fill(double_lined_horizontal_beam), COMPASS_LAYOUT_CENTRE,
        view(
            make_compass_layout(),
            pimpl_->bottom_tee_, COMPASS_LAYOUT_WEST,
            make_fill(double_lined_horizontal_beam), COMPASS_LAYOUT_CENTRE,
            pimpl_->bottom_right_corner_, COMPASS_LAYOUT_EAST
        ), COMPASS_LAYOUT_EAST);

    // ======================================================================
    // Construct the dropdown list.
    // ======================================================================
    pimpl_->list_ = std::make_shared<list>();
    pimpl_->list_->on_item_changed.connect(
        [this](auto idx)
        {
            pimpl_->on_item_changed(idx);
        });
    auto scroller = std::make_shared<scroll_pane>(pimpl_->list_, false);

    // Now put them together.
    pimpl_->dropdown_ = view(
        make_compass_layout(),
        // Put the scrollpane in a fixed-height layout so that it doesn't 
        // prefer to take up the entire screen.
        view(
            std::unique_ptr<fixed_height_layout>(new fixed_height_layout(7)),
            scroller
        ), COMPASS_LAYOUT_CENTRE,
        // Create a spacer to the right, so that the list is offset by the
        // width of the dropdown button.
        view(
            make_vertical_squeeze_layout(),
            make_background_fill(),
            make_background_fill()
        ), COMPASS_LAYOUT_EAST);

    /// ...

    auto top_box = view(
        make_horizontal_squeeze_layout(),
        top_row,
        centre_row,
        bottom_row);

    content->set_layout(make_horizontal_strip_layout());
    content->add_component(top_box);
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
dropdown_list::~dropdown_list()
{
}

// ==========================================================================
// SET_ITEMS
// ==========================================================================
void dropdown_list::set_items(std::vector<terminalpp::string> const &items)
{
    pimpl_->list_->set_items(items);
}

// ==========================================================================
// SET_ITEM_INDEX
// ==========================================================================
void dropdown_list::set_item_index(odin::s32 index)
{
    pimpl_->list_->set_item_index(index);
}

// ==========================================================================
// GET_ITEM_INDEX
// ==========================================================================
odin::s32 dropdown_list::get_item_index() const
{
    return pimpl_->list_->get_item_index();
}

// ==========================================================================
// GET_ITEM
// ==========================================================================
terminalpp::string dropdown_list::get_item() const
{
    return pimpl_->list_->get_item();
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void dropdown_list::do_event(boost::any const &event)
{
    bool handled = false;

    auto vk = boost::any_cast<terminalpp::virtual_key>(&event);
    
    if (vk)
    {
        handled = pimpl_->do_vk_event(*vk);
    }

    auto report =
        boost::any_cast<terminalpp::ansi::mouse::report>(&event);

    if (report)
    {
        handled = pimpl_->do_mouse_event(*report);
    }

    if (!handled)
    {
        composite_component::do_event(event);
    }
}

// ==========================================================================
// MAKE_DROPDOWN_LIST
// ==========================================================================
std::shared_ptr<dropdown_list> make_dropdown_list()
{
    return std::make_shared<dropdown_list>();

}

}

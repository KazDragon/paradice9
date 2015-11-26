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
#include "munin/basic_container.hpp"
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
#include "terminalpp/string.hpp"

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
    // DO_CHARACTER_EVENT
    // ======================================================================
    bool do_character_event(char ch)
    {
        if (ch == '\n')
        {
            toggle_dropdown();
            return true;
        }

        return false;
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
    /* @@ TODO:
    bool do_ansi_control_sequence_event(
        odin::ansi::control_sequence const &sequence)
    {
        if (sequence.initiator_ == odin::ansi::CONTROL_SEQUENCE_INTRODUCER)
        {
            // Check for the down arrow key
            if (sequence.command_ == odin::ansi::CURSOR_DOWN)
            {
                odin::u32 times = sequence.arguments_.empty()
                          ? 1
                          : atoi(sequence.arguments_.c_str());

                return do_cursor_down_key_event(times);
            }
        }

        return false;
    }
    */

    // ======================================================================
    // DO_ANSI_MOUSE_REPORT_EVENT
    // ======================================================================
    /* @@ TODO:
    bool do_ansi_mouse_report_event(
        odin::ansi::mouse_report const &mouse_report)
    {
        // printf("button = %d", mouse_report.button_);

        // Detect if the click occurred within the dropdown-button area.
        // Otherwise, we don't care.
        if (mouse_report.x_position_ >= self_.get_size().width - 3
         && mouse_report.y_position_ < 3)
        {
            if (mouse_report.button_ == odin::ansi::mouse_report::BUTTON_UP)
            {
                toggle_dropdown();
            }

            return true;
        }

        return false;
    }
    */

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

    // Construct the top right part of the dropdown +-+
    auto dropdown_top_left = std::make_shared<filled_box>(
        double_lined_top_tee);
    auto dropdown_top_centre = std::make_shared<filled_box>(
        double_lined_horizontal_beam);
    auto dropdown_top_right = std::make_shared<filled_box>(
        double_lined_top_right_corner);

    auto dropdown_top_row = std::make_shared<basic_container>();
    dropdown_top_row->set_layout(std::make_shared<compass_layout>());
    dropdown_top_row->add_component(dropdown_top_left,   COMPASS_LAYOUT_WEST);
    dropdown_top_row->add_component(dropdown_top_centre, COMPASS_LAYOUT_CENTRE);
    dropdown_top_row->add_component(dropdown_top_right,  COMPASS_LAYOUT_EAST);

    // Construct the top bar +----+-+
    auto top_left = std::make_shared<filled_box>(
        double_lined_top_left_corner);
    auto top_centre = std::make_shared<filled_box>(
        double_lined_horizontal_beam);

    auto top_row = std::make_shared<basic_container>();
    top_row->set_layout(std::make_shared<compass_layout>());
    top_row->add_component(top_left,   COMPASS_LAYOUT_WEST);
    top_row->add_component(top_centre, COMPASS_LAYOUT_CENTRE);
    top_row->add_component(dropdown_top_row,  COMPASS_LAYOUT_EAST);

    // Construct the middle-right part of the dropdown |V|
    auto dropdown_centre_left = std::make_shared<filled_box>(
        double_lined_vertical_beam);
    pimpl_->dropdown_button_ = std::make_shared<image>("V");
    pimpl_->dropdown_button_->set_can_focus(true);
    auto dropdown_centre_right = std::make_shared<filled_box>(
        double_lined_vertical_beam);

    auto dropdown_centre_row = std::make_shared<basic_container>();
    dropdown_centre_row->set_layout(std::make_shared<compass_layout>());
    dropdown_centre_row->add_component(dropdown_centre_left  ,   COMPASS_LAYOUT_WEST);
    dropdown_centre_row->add_component(pimpl_->dropdown_button_, COMPASS_LAYOUT_CENTRE);
    dropdown_centre_row->add_component(dropdown_centre_right,    COMPASS_LAYOUT_EAST);

    // Construct the centre bar |   |V|
    auto centre_left = std::make_shared<filled_box>(
        double_lined_vertical_beam);
    pimpl_->selected_text_ = std::make_shared<image>("");

    auto centre_row = std::make_shared<basic_container>();
    centre_row->set_layout(std::make_shared<compass_layout>());
    centre_row->add_component(centre_left,   COMPASS_LAYOUT_WEST);
    centre_row->add_component(pimpl_->selected_text_, COMPASS_LAYOUT_CENTRE);
    centre_row->add_component(dropdown_centre_row,  COMPASS_LAYOUT_EAST);

    // Construct the bottom-right part of the dropdown +-+
    pimpl_->bottom_tee_ = std::make_shared<filled_box>(
        double_lined_bottom_tee);
    auto dropdown_bottom_centre = std::make_shared<filled_box>(
        double_lined_horizontal_beam);
    pimpl_->bottom_right_corner_ = std::make_shared<filled_box>(
        double_lined_bottom_right_corner);

    auto dropdown_bottom_row = std::make_shared<basic_container>();
    dropdown_bottom_row->set_layout(std::make_shared<compass_layout>());
    dropdown_bottom_row->add_component(pimpl_->bottom_tee_,   COMPASS_LAYOUT_WEST);
    dropdown_bottom_row->add_component(dropdown_bottom_centre, COMPASS_LAYOUT_CENTRE);
    dropdown_bottom_row->add_component(pimpl_->bottom_right_corner_, COMPASS_LAYOUT_EAST);

    // Construct the bottom bar +-----+
    pimpl_->bottom_left_corner_ = std::make_shared<filled_box>(
        double_lined_bottom_left_corner);
    auto bottom_centre = std::make_shared<filled_box>(
        double_lined_horizontal_beam);

    auto bottom_row = std::make_shared<basic_container>();
    bottom_row->set_layout(std::make_shared<compass_layout>());
    bottom_row->add_component(pimpl_->bottom_left_corner_,   COMPASS_LAYOUT_WEST);
    bottom_row->add_component(bottom_centre, COMPASS_LAYOUT_CENTRE);
    bottom_row->add_component(dropdown_bottom_row,  COMPASS_LAYOUT_EAST);

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

    // Put the scrollpane in a fixed-height layout so that it doesn't prefer to
    // take up the entire screen.
    auto scroll_container = std::make_shared<basic_container>();
    scroll_container->set_layout(std::make_shared<fixed_height_layout>(7));
    scroll_container->add_component(scroller);

    // Create a spacer to the right, so that the list is offset by the
    // width of the dropdown button.
    auto spacer0 = std::make_shared<filled_box>(terminalpp::glyph(' '));
    auto spacer1 = std::make_shared<filled_box>(terminalpp::glyph(' '));
    auto spacer =  std::make_shared<basic_container>();
    spacer->set_layout(std::make_shared<vertical_squeeze_layout>());
    spacer->add_component(spacer0);
    spacer->add_component(spacer1);

    // Now put them together.
    auto dropdown = std::make_shared<basic_container>();
    dropdown->set_layout(std::make_shared<compass_layout>());
    dropdown->add_component(scroll_container, COMPASS_LAYOUT_CENTRE);
    dropdown->add_component(spacer, COMPASS_LAYOUT_EAST);
    pimpl_->dropdown_ = dropdown;

    /// ...

    auto top_box = std::make_shared<basic_container>();
    top_box->set_layout(std::make_shared<horizontal_squeeze_layout>());
    top_box->add_component(top_row);
    top_box->add_component(centre_row);
    top_box->add_component(bottom_row);

    content->set_layout(std::make_shared<horizontal_strip_layout>());
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
    /* @@ TODO: 
    bool handled = false;

    char const *ch = boost::any_cast<char>(&event);

    if (ch != nullptr)
    {
        handled = pimpl_->do_character_event(*ch);
    }

    odin::ansi::control_sequence const *sequence =
        boost::any_cast<odin::ansi::control_sequence>(&event);

    if (sequence != nullptr)
    {
        handled = pimpl_->do_ansi_control_sequence_event(*sequence);
    }

    odin::ansi::mouse_report const *report =
        boost::any_cast<odin::ansi::mouse_report>(&event);

    if (report != nullptr)
    {
        handled = pimpl_->do_ansi_mouse_report_event(*report);
    }

    if (!handled)
    {
        composite_component::do_event(event);
    }
    */
}

}

// ==========================================================================
// Munin Tabbed Frame.
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
#include "munin/tabbed_frame.hpp"
#include "munin/background_fill.hpp"
#include "munin/basic_container.hpp"
#include "munin/compass_layout.hpp"
#include "munin/context.hpp"
#include "munin/filled_box.hpp"
#include "munin/grid_layout.hpp"
#include "munin/image.hpp"
#include "munin/sco_glyphs.hpp"
#include "munin/vertical_strip_layout.hpp"
#include "munin/view.hpp"
#include <terminalpp/canvas.hpp>
#include <terminalpp/string.hpp>
#include <terminalpp/virtual_key.hpp>
#include <algorithm>

namespace munin {

namespace {

// ==========================================================================
// TABBED_FRAME_HEADER_RIVET
// ==========================================================================
class tabbed_frame_header_rivet : public composite_component
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    tabbed_frame_header_rivet()
      : top_element_(make_background_fill()),
        middle_element_(make_background_fill()),
        bottom_element_(make_background_fill())
    {
        auto content = get_container();
        content->set_layout(make_grid_layout(3, 1));
        content->add_component(top_element_);
        content->add_component(middle_element_);
        content->add_component(bottom_element_);
    };

    // ======================================================================
    // SET_TOP_ELEMENT
    // ======================================================================
    void set_top_element(terminalpp::element const &top_element)
    {
        top_element_->set_fill(top_element);
    }

    // ======================================================================
    // SET_MIDDLE_ELEMENT
    // ======================================================================
    void set_middle_element(terminalpp::element const &middle_element)
    {
        middle_element_->set_fill(middle_element);
    }

    // ======================================================================
    // SET_BOTTOM_ELEMENT
    // ======================================================================
    void set_bottom_element(terminalpp::element const &bottom_element)
    {
        bottom_element_->set_fill(bottom_element);
    }

private :
    std::shared_ptr<filled_box> top_element_;
    std::shared_ptr<filled_box> middle_element_;
    std::shared_ptr<filled_box> bottom_element_;
};

// ==========================================================================
// TABBED_FRAME_HEADER_LABEL
// ==========================================================================
class tabbed_frame_header_label : public composite_component
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    tabbed_frame_header_label(terminalpp::string const &label)
      : top_box_(make_background_fill()),
        label_(make_image(label)),
        bottom_box_(make_background_fill())
    {
        auto content = get_container();
        content->set_layout(make_compass_layout());
        content->add_component(top_box_, COMPASS_LAYOUT_NORTH);
        content->add_component(label_, COMPASS_LAYOUT_CENTRE);
        content->add_component(bottom_box_, COMPASS_LAYOUT_SOUTH);

        label_->set_can_focus(true);
    }

    // ======================================================================
    // SET_TOP_ELEMENT
    // ======================================================================
    void set_top_element(terminalpp::element const &element)
    {
        top_box_->set_fill(element);
    }

    // ======================================================================
    // SET_LABEL
    // ======================================================================
    void set_label(terminalpp::string const &text)
    {
        label_->set_image(text);
    }

    // ======================================================================
    // SET_BOTTOM_ELEMENT
    // ======================================================================
    void set_bottom_element(terminalpp::element const &element)
    {
        bottom_box_->set_fill(element);
    }

    boost::signal<void ()> on_click;

protected :
    // ======================================================================
    // DO_EVENT
    // ======================================================================
    void do_event(boost::any const &event)
    {
        auto *report = 
            boost::any_cast<terminalpp::ansi::mouse::report>(&event);
            
        if (report
         && report->button_ != terminalpp::ansi::mouse::report::BUTTON_UP)
        {
            
            on_click();
        }
    }

private :
    std::shared_ptr<filled_box> top_box_;
    std::shared_ptr<image>      label_;
    std::shared_ptr<filled_box> bottom_box_;
};

// ==========================================================================
// TABBED_FRAME_HEADER
// ==========================================================================
class tabbed_frame_header
  : public composite_component,
    public std::enable_shared_from_this<tabbed_frame_header>
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    tabbed_frame_header()
      : selected_(0)
      , highlight_(false)
    {
        get_container()->set_layout(make_grid_layout(1, 1));
        assemble_default();
    }

    // ======================================================================
    // INSERT_TAB
    // ======================================================================
    void insert_tab(std::string const &text, boost::optional<odin::u32> index)
    {
        // If index is not initialised, then place the new tab at the end
        // of the current list of tabs.  Otherwise, place it at the index
        // specified.
        auto actual_index = index.is_initialized()
                          ? index.get()
                          : tabs_.size();

        // If this isn't the first tab, and we're inserting before the selected
        // tab, then we need to increment that value in order to keep the same
        // tab selected.
        if (!tabs_.empty() && actual_index <= selected_)
        {
            ++selected_;
        }

        tabs_.insert(tabs_.begin() + actual_index, text);


        // Remove any existing content and rebuild.
        if (content_)
        {
            get_container()->remove_component(content_);
        }

        assemble();
    }

    // ======================================================================
    // REMOVE_TAB
    // ======================================================================
    void remove_tab(odin::u32 index)
    {
        tabs_.erase(tabs_.begin() + index);

        // If the selected tab is no longer valid, ensure it is.
        if (selected_ >= tabs_.size() && !tabs_.empty())
        {
            selected_ = tabs_.size() - 1;
        }

        // Remove any existing content and rebuild.
        if (content_)
        {
            get_container()->remove_component(content_);
        }

        assemble();
    }

    // ======================================================================
    // SET_HIGHLIGHT
    // ======================================================================
    void set_highlight(bool highlight)
    {
        highlight_ = highlight;
        update_highlights();
    }

    boost::signal<void (std::string)> on_tab_selected;

protected :
    // ======================================================================
    // DO_EVENT
    // ======================================================================
    void do_event(boost::any const &event)
    {
        bool handled = false;
        auto *vk = boost::any_cast<terminalpp::virtual_key>(&event);

        if (vk)
        {
            switch (vk->key)
            {
                case terminalpp::vk::cursor_right :
                    if (selected_ + 1 < tabs_.size())
                    {
                        ++selected_;
                        update_highlights();
                        on_tab_selected(tabs_[selected_]);
                        handled = true;
                    }
                    break;
                    
                case terminalpp::vk::cursor_left :
                    if (selected_ != 0)
                    {
                        --selected_;
                        update_highlights();
                        on_tab_selected(tabs_[selected_]);
                        handled = true;
                    }
                    break;
                    
                default :
                    // Do nothing.
                    break;
            }
        }

        if (!handled)
        {
            composite_component::do_event(event);
        }
    }

private :
    // ======================================================================
    // ON_LABEL_CLICK
    // ======================================================================
    void on_label_click(
        std::shared_ptr<tabbed_frame_header_label> const &label)
    {
        // Find the index of this label and then fire the on_tab_selected
        // signal using the string identifier of the tab of that index.
        auto it = std::find(labels_.begin(), labels_.end(), label);

        if (it != labels_.end())
        {
            auto index = std::distance(labels_.begin(), it);
            selected_ = index;
            update_highlights();
            on_tab_selected(tabs_[selected_]);
        }
    }

    // ======================================================================
    // ASSEMBLE
    // ======================================================================
    void assemble()
    {
        rivets_.clear();
        labels_.clear();

        // Special case: if there are no tabs, then assemble a default empty
        // tab.
        if (tabs_.empty())
        {
            assemble_default();
            return;
        }

        // Left and right rivets are always present, along with filler.
        auto left_rivet = std::make_shared<tabbed_frame_header_rivet>();
        rivets_.push_back(left_rivet);

        filler_ = make_fill(double_lined_horizontal_beam);
        auto filler = view(
            make_compass_layout(),
            make_background_fill(), COMPASS_LAYOUT_NORTH,
            make_background_fill(), COMPASS_LAYOUT_CENTRE,
            filler_, COMPASS_LAYOUT_SOUTH);
            
        // To create the middle section, use a vertical strip layout and
        // alternate label/rivet until complete.
        auto tab_section = view(
            make_vertical_strip_layout(),
            left_rivet);

        for (std::vector<std::string>::size_type index = 0;
             index < tabs_.size();
             ++index)
        {
            auto label = 
                std::make_shared<tabbed_frame_header_label>(tabs_[index]);
            labels_.push_back(label);

            label->on_click.connect(
                [wpthis=std::weak_ptr<tabbed_frame_header>(shared_from_this()),
                 wplabel=std::weak_ptr<tabbed_frame_header_label>(label)]
                {
                    auto pthis = wpthis.lock();
                    auto plabel = wplabel.lock();

                    if (pthis && plabel)
                    {
                        pthis->on_label_click(plabel);
                    }
                });

            tab_section->add_component(label);

            auto rivet = std::make_shared<tabbed_frame_header_rivet>();
            tab_section->add_component(rivet);
            rivets_.push_back(rivet);
        }

        auto right_rivet = std::make_shared<tabbed_frame_header_rivet>();
        rivets_.push_back(right_rivet);

        content_ = view(
            make_compass_layout(),
            tab_section, COMPASS_LAYOUT_WEST,
            filler, COMPASS_LAYOUT_CENTRE,
            right_rivet, COMPASS_LAYOUT_EAST);

        get_container()->add_component(content_);

        update_highlights();
    }

    // ======================================================================
    // UPDATE_HIGHLIGHTS
    // ======================================================================
    void update_highlights()
    {
        // Updates the rivets and labels to show where the selected tab is
        // and also whether the framed component is focussed (todo).
        odin::u32 index = 0;

        terminalpp::attribute selected_item_pen;

        if (highlight_)
        {
            selected_item_pen.foreground_colour_ =
                terminalpp::high_colour(1, 4, 5);
        }

        for (auto &rivet : rivets_)
        {
            // We want a highlight if this wraps the selected item.
            auto pen = index == selected_ || index == selected_ + 1
                     ? selected_item_pen
                     : terminalpp::attribute();

            if (index <= selected_)
            {
                rivet->set_top_element({double_lined_top_left_corner, pen});
                rivet->set_middle_element({double_lined_vertical_beam, pen});

            }
            else if (index == rivets_.size() - 1)
            {
                rivet->set_top_element({' '});
                rivet->set_middle_element({' '});
            }
            else
            {
                rivet->set_top_element({double_lined_top_right_corner, pen});

                rivet->set_middle_element({double_lined_vertical_beam, pen});
            }

            if (index == 0)
            {
                rivet->set_bottom_element({
                    selected_ == 0
                      ? double_lined_left_tee
                      : double_lined_top_left_corner
                  , selected_item_pen});
            }
            else if (index == rivets_.size() - 1)
            {
                rivet->set_bottom_element(
                    {double_lined_top_right_corner, selected_item_pen});
            }
            else
            {
                if (index == selected_ || index == selected_ + 1)
                {
                    rivet->set_bottom_element(
                        {double_lined_bottom_tee, selected_item_pen});
                }
                else
                {
                    rivet->set_bottom_element(
                        {double_lined_horizontal_beam, selected_item_pen});
                }
            }

            ++index;
        }

        index = 0;

        for (auto &label : labels_)
        {
            label->set_top_element({
                double_lined_horizontal_beam
              , index == selected_ ? selected_item_pen : terminalpp::attribute()});
            label->set_bottom_element(
                {double_lined_horizontal_beam, selected_item_pen});

            ++index;
        }

        if (filler_)
        {
            filler_->set_fill({filler_->get_fill().glyph_, selected_item_pen});
        }
    }

    // ======================================================================
    // ASSEMBLE_DEFAULT
    // ======================================================================
    void assemble_default()
    {
        auto left_rivet = std::make_shared<tabbed_frame_header_rivet>();
        auto label = std::make_shared<tabbed_frame_header_label>("");
        auto right_rivet = std::make_shared<tabbed_frame_header_rivet>();

        auto filler = view(
            make_compass_layout(),
            make_background_fill(), COMPASS_LAYOUT_NORTH,
            make_background_fill(), COMPASS_LAYOUT_CENTRE,
            make_fill(double_lined_horizontal_beam), COMPASS_LAYOUT_SOUTH);

        auto rightmost = std::make_shared<tabbed_frame_header_rivet>();

        auto tabs = view(
            make_vertical_strip_layout(),
            left_rivet,
            label,
            right_rivet);
        
        content_ = view(
            make_compass_layout(),
            tabs, COMPASS_LAYOUT_WEST,
            filler, COMPASS_LAYOUT_CENTRE,
            right_rivet, COMPASS_LAYOUT_EAST);
        
        get_container()->add_component(content_);

        update_highlights();
    }

private :
    std::vector<std::string>                                tabs_;
    std::shared_ptr<container>                              content_;
    std::shared_ptr<filled_box>                             filler_;
    std::vector<std::shared_ptr<tabbed_frame_header_rivet>> rivets_;
    std::vector<std::shared_ptr<tabbed_frame_header_label>> labels_;
    odin::u32                                               selected_;
    bool                                                    highlight_;
};

}

// ==========================================================================
// TABBED_FRAME::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct tabbed_frame::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(tabbed_frame &self)
      : self_(self)
    {
    }

    // ======================================================================
    // ON_TAB_SELECTED
    // ======================================================================
    void on_tab_selected(std::string const &text)
    {
        self_.on_tab_selected(text);
    }

    // ======================================================================
    // SET_HIGHLIGHT
    // ======================================================================
    void set_highlight(bool highlight)
    {
        terminalpp::attribute pen;

        if (highlight)
        {
            pen.foreground_colour_ = terminalpp::high_colour(1, 4, 5);
        }

        header_->set_highlight(highlight);
        left_border_->set_fill(
            {left_border_->get_fill().glyph_, pen});
        bottom_left_corner_->set_fill(
            {bottom_left_corner_->get_fill().glyph_, pen});
        bottom_border_->set_fill(
            {bottom_border_->get_fill().glyph_, pen});
        bottom_right_corner_->set_fill(
            {bottom_right_corner_->get_fill().glyph_, pen});
        right_border_->set_fill(
            {right_border_->get_fill().glyph_, pen});
    }

    tabbed_frame                         &self_;
    std::shared_ptr<tabbed_frame_header>  header_;
    std::shared_ptr<filled_box>           left_border_;
    std::shared_ptr<filled_box>           bottom_left_corner_;
    std::shared_ptr<filled_box>           bottom_border_;
    std::shared_ptr<filled_box>           bottom_right_corner_;
    std::shared_ptr<filled_box>           right_border_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
tabbed_frame::tabbed_frame()
{
    pimpl_ = std::make_shared<impl>(std::ref(*this));

    // North - Header full of tabs.
    pimpl_->header_ = std::make_shared<tabbed_frame_header>();
    pimpl_->header_->on_tab_selected.connect(
        [wpthis=std::weak_ptr<impl>(pimpl_)]
        (std::string const &text)
        {
            auto pthis = wpthis.lock();

            if (pthis)
            {
                pthis->on_tab_selected(text);
            }
        });

    pimpl_->left_border_         = make_fill(double_lined_vertical_beam);
    pimpl_->right_border_        = make_fill(double_lined_vertical_beam);
    pimpl_->bottom_left_corner_  = make_fill(double_lined_bottom_left_corner);
    pimpl_->bottom_border_       = make_fill(double_lined_horizontal_beam);
    pimpl_->bottom_right_corner_ = make_fill(double_lined_bottom_right_corner);
        
    auto north = view(
        make_grid_layout(1, 1),
        pimpl_->header_);

    // West Container
    auto west = view(
        make_grid_layout(1, 1),
        pimpl_->left_border_);
    
    // East Container
    auto east = view(
        make_grid_layout(1, 1),
        pimpl_->right_border_);

    // South Container
    auto south = view(
        make_compass_layout(),
        pimpl_->bottom_left_corner_, COMPASS_LAYOUT_WEST,
        pimpl_->bottom_border_, COMPASS_LAYOUT_CENTRE,
        pimpl_->bottom_right_corner_, COMPASS_LAYOUT_EAST);

    // All together.
    auto content = get_container();
    content->set_layout(make_compass_layout());
    content->add_component(north, COMPASS_LAYOUT_NORTH);
    content->add_component(south, COMPASS_LAYOUT_SOUTH);
    content->add_component(west, COMPASS_LAYOUT_WEST);
    content->add_component(east, COMPASS_LAYOUT_EAST);
}

// ==========================================================================
// INSERT_TAB
// ==========================================================================
void tabbed_frame::insert_tab(
    std::string const &text,
    boost::optional<odin::u32> index /* = optional<u32>() */)
{
    pimpl_->header_->insert_tab(text, index);
}

// ==========================================================================
// SET_HIGHLIGHT
// ==========================================================================
void tabbed_frame::set_highlight(bool highlight)
{
    do_set_highlight(highlight);
}

// ==========================================================================
// REMOVE_TAB
// ==========================================================================
void tabbed_frame::remove_tab(odin::u32 index)
{
    pimpl_->header_->remove_tab(index);
}

// ==========================================================================
// DO_GET_TOP_BORDER_HEIGHT
// ==========================================================================
odin::s32 tabbed_frame::do_get_top_border_height() const
{
    return 3;
}

// ==========================================================================
// DO_GET_BOTTOM_BORDER_HEIGHT
// ==========================================================================
odin::s32 tabbed_frame::do_get_bottom_border_height() const
{
    return 1;
}

// ==========================================================================
// DO_GET_LEFT_BORDER_WIDTH
// ==========================================================================
odin::s32 tabbed_frame::do_get_left_border_width() const
{
    return 1;
}

// ==========================================================================
// DO_GET_RIGHT_BORDER_WIDTH
// ==========================================================================
odin::s32 tabbed_frame::do_get_right_border_width() const
{
    return 1;
}

// ==========================================================================
// DO_SET_HIGHLIGHT
// ==========================================================================
void tabbed_frame::do_set_highlight(bool highlight)
{
    pimpl_->set_highlight(highlight);
}

// ==========================================================================
// MAKE_TABBED_FRAME
// ==========================================================================
std::shared_ptr<tabbed_frame> make_tabbed_frame()
{
    return std::make_shared<tabbed_frame>();
}

}

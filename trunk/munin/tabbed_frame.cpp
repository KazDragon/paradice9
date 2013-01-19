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
#include "munin/ansi/protocol.hpp"
#include "munin/basic_container.hpp"
#include "munin/canvas.hpp"
#include "munin/compass_layout.hpp"
#include "munin/context.hpp"
#include "munin/filled_box.hpp"
#include "munin/grid_layout.hpp"
#include "munin/image.hpp"
#include "munin/vertical_strip_layout.hpp"
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>
#include <algorithm>

using namespace munin::ansi;
using namespace odin;
using namespace boost;
using namespace std;

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
      : top_element_(make_shared<filled_box>(element_type(' ')))
      , middle_element_(make_shared<filled_box>(element_type(' ')))
      , bottom_element_(make_shared<filled_box>(element_type(' ')))
    {
        BOOST_AUTO(content, get_container());
        content->set_layout(make_shared<grid_layout>(3, 1));
        content->add_component(top_element_);
        content->add_component(middle_element_);
        content->add_component(bottom_element_);
    };

    // ======================================================================
    // SET_TOP_ELEMENT
    // ======================================================================
    void set_top_element(element_type const &top_element)
    {
        top_element_->set_fill(top_element);
    }

    // ======================================================================
    // SET_MIDDLE_ELEMENT
    // ======================================================================
    void set_middle_element(element_type const &middle_element)
    {
        middle_element_->set_fill(middle_element);
    }

    // ======================================================================
    // SET_BOTTOM_ELEMENT
    // ======================================================================
    void set_bottom_element(element_type const &bottom_element)
    {
        bottom_element_->set_fill(bottom_element);
    }

private :
    shared_ptr<filled_box> top_element_;
    shared_ptr<filled_box> middle_element_;
    shared_ptr<filled_box> bottom_element_;
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
    tabbed_frame_header_label(vector<element_type> const &label)
      : top_box_(make_shared<filled_box>(element_type(' ')))
      , label_(make_shared<image>(label))
      , bottom_box_(make_shared<filled_box>(element_type(' ')))
    {
        BOOST_AUTO(content, get_container());
        content->set_layout(make_shared<compass_layout>());
        content->add_component(top_box_, COMPASS_LAYOUT_NORTH);
        content->add_component(label_, COMPASS_LAYOUT_CENTRE);
        content->add_component(bottom_box_, COMPASS_LAYOUT_SOUTH);
        
        label_->set_can_focus(true);
    }

    // ======================================================================
    // SET_TOP_ELEMENT
    // ======================================================================
    void set_top_element(element_type const &element)
    {
        top_box_->set_fill(element);
    }

    // ======================================================================
    // SET_LABEL
    // ======================================================================
    void set_label(vector<element_type> const &text)
    {
        label_->set_image(text);
    }

    // ======================================================================
    // SET_BOTTOM_ELEMENT
    // ======================================================================
    void set_bottom_element(element_type const &element)
    {
        bottom_box_->set_fill(element);
    }

    signal<void ()> on_click;

protected :
    // ======================================================================
    // DO_EVENT
    // ======================================================================
    void do_event(any const &event)
    {
        BOOST_AUTO(mouse, any_cast<odin::ansi::mouse_report>(&event));

        if (mouse)
        {
            on_click();
        }
    }

private :
    shared_ptr<filled_box> top_box_;
    shared_ptr<image>      label_;
    shared_ptr<filled_box> bottom_box_;
};

// ==========================================================================
// TABBED_FRAME_HEADER
// ==========================================================================
class tabbed_frame_header
  : public composite_component
  , public enable_shared_from_this<tabbed_frame_header>
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    tabbed_frame_header()
      : selected_(0)
      , highlight_(false)
    {
        get_container()->set_layout(make_shared<grid_layout>(1, 1));
        assemble_default();
    }

    // ======================================================================
    // INSERT_TAB
    // ======================================================================
    void insert_tab(string const &text, optional<u32> index)
    {
        // If index is not initialised, then place the new tab at the end
        // of the current list of tabs.  Otherwise, place it at the index
        // specified.
        BOOST_AUTO(
            actual_index
          , index.is_initialized() ? index.get() : tabs_.size());

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
    void remove_tab(u32 index)
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

    signal<void (string)> on_tab_selected;

protected :
    // ======================================================================
    // DO_EVENT
    // ======================================================================
    void do_event(boost::any const &event)
    {
        BOOST_AUTO(sequence, any_cast<odin::ansi::control_sequence>(&event));

        bool handled = false;

        if (sequence)
        {
            if (sequence->command_ == odin::ansi::CURSOR_FORWARD)
            {
                if (selected_ + 1 < tabs_.size())
                {
                    ++selected_;
                    update_highlights();
                    on_tab_selected(tabs_[selected_]);
                    handled = true;
                }
            }
            else if (sequence->command_ == odin::ansi::CURSOR_BACKWARD)
            {
                if (selected_ != 0)
                {
                    --selected_;
                    update_highlights();
                    on_tab_selected(tabs_[selected_]);
                    handled = true;
                }
            }
        }

        if (!handled)
        {
            composite_component::do_event(event);
        }
    }

private :
    // ======================================================================
    // ON_LABEL_CLICK_THUNK
    // ======================================================================
    static void on_label_click_thunk(
        weak_ptr<tabbed_frame_header>       weak_this
      , weak_ptr<tabbed_frame_header_label> weak_label)
    {
        shared_ptr<tabbed_frame_header> strong_this(weak_this.lock());
        shared_ptr<tabbed_frame_header_label> strong_label(weak_label.lock());

        if (strong_this && strong_label)
        {
            strong_this->on_label_click(strong_label);
        }
    }

    // ======================================================================
    // ON_LABEL_CLICK
    // ======================================================================
    void on_label_click(shared_ptr<tabbed_frame_header_label> label)
    {
        // Find the index of this label and then fire the on_tab_selected
        // signal using the string identifier of the tab of that index.
        BOOST_AUTO(it, find(labels_.begin(), labels_.end(), label));

        if (it != labels_.end())
        {
            BOOST_AUTO(index, distance(labels_.begin(), it));
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
        BOOST_AUTO(left_rivet, make_shared<tabbed_frame_header_rivet>());
        rivets_.push_back(left_rivet);

        BOOST_AUTO(right_rivet, make_shared<tabbed_frame_header_rivet>());

        BOOST_AUTO(filler, make_shared<basic_container>());
        filler->set_layout(make_shared<compass_layout>());
        filler->add_component(
            make_shared<filled_box>(element_type(' '))
          , COMPASS_LAYOUT_NORTH);
        filler->add_component(
            make_shared<filled_box>(element_type(' '))
          , COMPASS_LAYOUT_CENTRE);
        filler_ = make_shared<filled_box>(element_type(double_lined_horizontal_beam));
        filler->add_component(filler_, COMPASS_LAYOUT_SOUTH);

        // To create the middle section, use a vertical strip layout and
        // alternate label/rivet until complete.
        BOOST_AUTO(tab_section, make_shared<basic_container>());
        tab_section->set_layout(make_shared<vertical_strip_layout>());
        tab_section->add_component(left_rivet);

        for (vector<string>::size_type index = 0;
             index < tabs_.size();
             ++index)
        {
            BOOST_AUTO(label, make_shared<tabbed_frame_header_label>(
                elements_from_string(tabs_[index])));
            labels_.push_back(label);

            label->on_click.connect(bind(
                &tabbed_frame_header::on_label_click_thunk
              , weak_ptr<tabbed_frame_header>(shared_from_this())
              , weak_ptr<tabbed_frame_header_label>(label)));

            tab_section->add_component(label);

            BOOST_AUTO(rivet, make_shared<tabbed_frame_header_rivet>());
            tab_section->add_component(rivet);
            rivets_.push_back(rivet);
        }

        rivets_.push_back(right_rivet);

        content_ = make_shared<basic_container>();
        content_->set_layout(make_shared<compass_layout>());
        content_->add_component(tab_section, COMPASS_LAYOUT_WEST);
        content_->add_component(filler, COMPASS_LAYOUT_CENTRE);
        content_->add_component(right_rivet, COMPASS_LAYOUT_EAST);
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
        u32 index = 0;
        
        attribute selected_item_pen;
        
        if (highlight_)
        {
            selected_item_pen.foreground_colour_ = 
                attribute::high_colour(1, 4, 5);
        }

        BOOST_FOREACH(shared_ptr<tabbed_frame_header_rivet> rivet, rivets_)
        {
            // We want a highlight if this wraps the selected item.
            attribute pen = index == selected_ || index == selected_ + 1
                          ? selected_item_pen
                          : attribute();

            if (index <= selected_)
            {
                rivet->set_top_element(element_type(
                    double_lined_top_left_corner, pen));

                rivet->set_middle_element(element_type(
                    double_lined_vertical_beam, pen));

            }
            else if (index == rivets_.size() - 1)
            {
                rivet->set_top_element(element_type(' '));
                rivet->set_middle_element(element_type(' '));
            }
            else
            {
                rivet->set_top_element(element_type(
                    double_lined_top_right_corner, pen));

                rivet->set_middle_element(element_type(
                    double_lined_vertical_beam, pen));
            }

            if (index == 0)
            {
                rivet->set_bottom_element(element_type(
                    selected_ == 0 
                      ? double_lined_left_tee
                      : double_lined_top_left_corner
                  , selected_item_pen));
            }
            else if (index == rivets_.size() - 1)
            {
                rivet->set_bottom_element(element_type(
                    double_lined_top_right_corner, selected_item_pen));
            }
            else
            {
                if (index == selected_ || index == selected_ + 1)
                {
                    rivet->set_bottom_element(element_type(
                        double_lined_bottom_tee, selected_item_pen));
                }
                else
                {
                    rivet->set_bottom_element(element_type(
                        double_lined_horizontal_beam, selected_item_pen));
                }
            }

            ++index;
        }

        index = 0;

        BOOST_FOREACH(shared_ptr<tabbed_frame_header_label> label, labels_)
        {
            label->set_top_element(element_type(
                double_lined_horizontal_beam
              , index == selected_ ? selected_item_pen : attribute()));
            label->set_bottom_element(element_type(
                double_lined_horizontal_beam, selected_item_pen));

            ++index;
        }

        if (filler_)
        {
            filler_->set_fill(
                element_type(filler_->get_fill().glyph_, selected_item_pen));
        }
    }

    // ======================================================================
    // ASSEMBLE_DEFAULT
    // ======================================================================
    void assemble_default()
    {
        BOOST_AUTO(left_rivet, make_shared<tabbed_frame_header_rivet>());
        BOOST_AUTO(
            label
          , make_shared<tabbed_frame_header_label>(elements_from_string("")));

        BOOST_AUTO(right_rivet, make_shared<tabbed_frame_header_rivet>());

        BOOST_AUTO(filler, make_shared<basic_container>());
        filler->set_layout(make_shared<compass_layout>());
        filler->add_component(
            make_shared<filled_box>(element_type(' '))
          , COMPASS_LAYOUT_NORTH);
        filler->add_component(
            make_shared<filled_box>(element_type(' '))
          , COMPASS_LAYOUT_CENTRE);
        filler->add_component(
            make_shared<filled_box>(element_type(single_lined_horizontal_beam))
          , COMPASS_LAYOUT_SOUTH);

        BOOST_AUTO(rightmost, make_shared<tabbed_frame_header_rivet>());

        BOOST_AUTO(tabs, make_shared<basic_container>());
        tabs->set_layout(make_shared<vertical_strip_layout>());
        tabs->add_component(left_rivet);
        tabs->add_component(label);
        tabs->add_component(right_rivet);

        content_ = make_shared<basic_container>();
        content_->set_layout(make_shared<compass_layout>());
        content_->add_component(tabs, COMPASS_LAYOUT_WEST);
        content_->add_component(filler, COMPASS_LAYOUT_CENTRE);
        content_->add_component(right_rivet, COMPASS_LAYOUT_EAST);
        get_container()->add_component(content_);

        update_highlights();
    }

private :
    vector<string>                                  tabs_;
    shared_ptr<basic_container>                     content_;
    shared_ptr<filled_box>                          filler_;
    vector< shared_ptr<tabbed_frame_header_rivet> > rivets_;
    vector< shared_ptr<tabbed_frame_header_label> > labels_;
    u32                                             selected_;
    bool                                            highlight_;
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
    // ON_TAB_SELECTED_THUNK
    // ======================================================================
    static void on_tab_selected_thunk(
        weak_ptr<impl>  weak_this
      , string const   &text)
    {
        shared_ptr<impl> strong_this(weak_this.lock());

        if (strong_this)
        {
            strong_this->on_tab_selected(text);
        }
    }

    // ======================================================================
    // ON_TAB_SELECTED
    // ======================================================================
    void on_tab_selected(string const &text)
    {
        self_.on_tab_selected(text);
    }

    // ======================================================================
    // SET_HIGHLIGHT
    // ======================================================================
    void set_highlight(bool highlight)
    {
        attribute pen;

        if (highlight)
        {
            pen.foreground_colour_ = attribute::high_colour(1, 4, 5);
        }

        header_->set_highlight(highlight);
        left_border_->set_fill(
            element_type(left_border_->get_fill().glyph_, pen));
        bottom_left_corner_->set_fill(
            element_type(bottom_left_corner_->get_fill().glyph_, pen));
        bottom_border_->set_fill(
            element_type(bottom_border_->get_fill().glyph_, pen));
        bottom_right_corner_->set_fill(
            element_type(bottom_right_corner_->get_fill().glyph_, pen));
        right_border_->set_fill(
            element_type(right_border_->get_fill().glyph_, pen));
    }

    tabbed_frame                    &self_;
    shared_ptr<tabbed_frame_header>  header_;
    shared_ptr<filled_box>           left_border_;
    shared_ptr<filled_box>           bottom_left_corner_;
    shared_ptr<filled_box>           bottom_border_;
    shared_ptr<filled_box>           bottom_right_corner_;
    shared_ptr<filled_box>           right_border_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
tabbed_frame::tabbed_frame()
{
    pimpl_ = make_shared<impl>(ref(*this));

    // North - Header full of tabs.
    pimpl_->header_ = make_shared<tabbed_frame_header>();
    pimpl_->header_->on_tab_selected.connect(bind(
        &impl::on_tab_selected_thunk
      , weak_ptr<impl>(pimpl_)
      , _1));

    BOOST_AUTO(north, make_shared<basic_container>());
    north->set_layout(make_shared<grid_layout>(1, 1));
    north->add_component(pimpl_->header_);

    // West Container
    BOOST_AUTO(west, make_shared<basic_container>());
    west->set_layout(make_shared<grid_layout>(1, 1));
    pimpl_->left_border_ = make_shared<filled_box>(
        element_type(double_lined_vertical_beam));
    west->add_component(pimpl_->left_border_);

    // East Container
    BOOST_AUTO(east, make_shared<basic_container>());
    east->set_layout(make_shared<grid_layout>(1, 1));
    pimpl_->right_border_ = make_shared<filled_box>(
        element_type(double_lined_vertical_beam));
    east->add_component(pimpl_->right_border_);

    // South Container
    BOOST_AUTO(south, make_shared<basic_container>());
    south->set_layout(make_shared<compass_layout>());
    pimpl_->bottom_left_corner_ = make_shared<filled_box>(
        element_type(double_lined_bottom_left_corner));
    pimpl_->bottom_border_ = make_shared<filled_box>(
        element_type(double_lined_horizontal_beam));
    pimpl_->bottom_right_corner_ = make_shared<filled_box>(
        element_type(double_lined_bottom_right_corner));

    south->add_component(pimpl_->bottom_left_corner_, COMPASS_LAYOUT_WEST);
    south->add_component(pimpl_->bottom_border_, COMPASS_LAYOUT_CENTRE);
    south->add_component(pimpl_->bottom_right_corner_, COMPASS_LAYOUT_EAST);

    // All together.
    BOOST_AUTO(content, get_container());
    content->set_layout(make_shared<compass_layout>());
    content->add_component(north, COMPASS_LAYOUT_NORTH);
    content->add_component(south, COMPASS_LAYOUT_SOUTH);
    content->add_component(west, COMPASS_LAYOUT_WEST);
    content->add_component(east, COMPASS_LAYOUT_EAST);
}

// ==========================================================================
// INSERT_TAB
// ==========================================================================
void tabbed_frame::insert_tab(
    string const &text
  , optional<u32> index /* = optional<u32>() */)
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
void tabbed_frame::remove_tab(u32 index)
{
    pimpl_->header_->remove_tab(index);
}

// ==========================================================================
// DO_GET_TOP_BORDER_HEIGHT
// ==========================================================================
s32 tabbed_frame::do_get_top_border_height() const
{
    return 3;
}

// ==========================================================================
// DO_GET_BOTTOM_BORDER_HEIGHT
// ==========================================================================
s32 tabbed_frame::do_get_bottom_border_height() const
{
    return 1;
}

// ==========================================================================
// DO_GET_LEFT_BORDER_WIDTH
// ==========================================================================
s32 tabbed_frame::do_get_left_border_width() const
{
    return 1;
}

// ==========================================================================
// DO_GET_RIGHT_BORDER_WIDTH
// ==========================================================================
s32 tabbed_frame::do_get_right_border_width() const
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

}

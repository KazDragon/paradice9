// ==========================================================================
// Munin Scroll Pane
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
#include "munin/scroll_pane.hpp"
#include "munin/basic_frame.hpp"
#include "munin/container.hpp"
#include "munin/horizontal_scroll_bar.hpp"
#include "munin/filled_box.hpp"
#include "munin/framed_component.hpp"
#include "munin/grid_layout.hpp"
#include "munin/sco_glyphs.hpp"
#include "munin/vertical_scroll_bar.hpp"
#include "munin/viewport.hpp"
#include "terminalpp/element.hpp"
#include <algorithm>

namespace munin {

namespace {

class scroll_frame : public basic_frame
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    scroll_frame(
        std::shared_ptr<horizontal_scroll_bar> const &hscroll_bar,
        std::shared_ptr<vertical_scroll_bar>   const &vscroll_bar,
        bool                                   top_border)
      : basic_frame(
            top_border
          ? make_fill(double_lined_top_left_corner)
          : std::shared_ptr<filled_box>(),
            top_border
          ? make_fill(double_lined_horizontal_beam)
          : std::shared_ptr<filled_box>(),
            top_border
          ? make_fill(double_lined_top_right_corner)
          : std::shared_ptr<filled_box>(),
            make_fill(double_lined_vertical_beam),
            vscroll_bar,
            make_fill(double_lined_bottom_left_corner),
            hscroll_bar,
            make_fill(double_lined_bottom_right_corner))
    {
    }

    // ======================================================================
    // DESTRUCTOR
    // ======================================================================
    ~scroll_frame()
    {
    }
};

}

// ==========================================================================
// SCROLL_PANE::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct scroll_pane::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(scroll_pane &self)
        : self_(self)
    {
    }

    scroll_pane                            &self_;
    std::shared_ptr<component>              underlying_component_;
    std::shared_ptr<viewport>               viewport_;
    std::shared_ptr<horizontal_scroll_bar>  horizontal_scroll_bar_;
    std::shared_ptr<vertical_scroll_bar>    vertical_scroll_bar_;
    std::shared_ptr<scroll_frame>           scroll_frame_;

    // ======================================================================
    // ON_PAGE_LEFT
    // ======================================================================
    void on_page_left()
    {
        terminalpp::point new_origin;
        auto origin = viewport_->get_origin();

        new_origin = origin;

        // TODO: Tab a page, not a line.
        if (new_origin.x != 0)
        {
            --new_origin.x;
        }

        viewport_->set_origin(new_origin);
        calculate_scrollbars();
    }

    // ======================================================================
    // ON_PAGE_RIGHT
    // ======================================================================
    void on_page_right()
    {
        terminalpp::point new_origin;
        auto origin = viewport_->get_origin();
        auto size = self_.get_size();
        auto underlying_component_size = underlying_component_->get_size();

        // Find out how far over to the right the maximum origin should be.
        // If the underlying component has a width smaller than this, then
        // the max is 0, because we can't scroll to the right.  Otherwise,
        // it is the width of the underlying component - the width of the
        // viewport.
        auto max_right =
            (underlying_component_size.width < size.width
          ? 0
          : underlying_component_size.width - size.width);

        // Same for the bottom, with height.
        auto max_bottom =
            (underlying_component_size.height < size.height
          ? 0
          : underlying_component_size.height - size.height);

        // Now, move the origin over to the right by one page, up to the
        // maximum to the right.
        // new_origin.x = (min)(origin.x + size.width, max_right);
        new_origin.x = (std::min)(origin.x + 1, max_right);
        new_origin.y = (std::min)(origin.y, max_bottom);

        viewport_->set_origin(new_origin);
        calculate_scrollbars();
    }

    // ======================================================================
    // ON_PAGE_UP
    // ======================================================================
    void on_page_up()
    {
        auto origin = viewport_->get_origin();
        auto size = viewport_->get_size();

        // If we would tab over the top of the viewport, then move to the
        // top instead.
        if (size.height >= origin.y)
        {
            origin.y = 0;
        }
        // Otherwise, move up by a page.
        else
        {
            origin.y -= size.height;
        }

        viewport_->set_origin(origin);
    }

    // ======================================================================
    // ON_PAGE_DOWN
    // ======================================================================
    void on_page_down()
    {
        terminalpp::point new_origin;
        auto origin = viewport_->get_origin();
        auto size = viewport_->get_size();
        auto underlying_component_size = underlying_component_->get_size();

        // Find out how far over to the right the maximum origin should be.
        // If the underlying component has a width smaller than this, then
        // the max is 0, because we can't scroll to the right.  Otherwise,
        // it is the width of the underlying component - the width of the
        // viewport.
        auto max_right =
            (underlying_component_size.width < size.width
          ? 0
          : underlying_component_size.width - size.width);

        // Same for the bottom, with height.
        auto max_bottom =
            (underlying_component_size.height < size.height
          ? 0
          : underlying_component_size.height - size.height);

        // Now, move the origin over to the right by one page, up to the
        // maximum to the right.
        new_origin.x = (std::min)(origin.x, max_right);
        new_origin.y = (std::min)(origin.y + size.height, max_bottom);

        viewport_->set_origin(new_origin);
    }

    // ======================================================================
    // CALCULATE_HORIZONTAL_SCROLLBAR
    // ======================================================================
    boost::optional<odin::u8> calculate_horizontal_scrollbar()
    {
        auto origin = viewport_->get_origin();
        auto size = viewport_->get_size();
        auto underlying_component_size = underlying_component_->get_size();

        odin::u8 slider_position = 0;

        if (underlying_component_size.width <= size.width)
        {
            return {};
        }

        if (origin.x != 0)
        {
            auto max_right = underlying_component_size.width - size.width;

            if (origin.x == max_right)
            {
                slider_position = 100;
            }
            else
            {
                slider_position = odin::u8((origin.x * 100) / max_right);

                if (slider_position == 0)
                {
                    slider_position = 1;
                }

                if (slider_position == 100)
                {
                    slider_position = 99;
                }
            }
        }

        return slider_position;
    }

    // ======================================================================
    // CALCULATE_VERTICAL_SCROLLBAR
    // ======================================================================
    boost::optional<odin::u8> calculate_vertical_scrollbar()
    {
        auto origin = viewport_->get_origin();
        auto size = viewport_->get_size();
        auto underlying_component_size = underlying_component_->get_size();

        odin::u8 slider_position = 0;

        if (underlying_component_size.height <= size.height)
        {
            return {};
        }

        if (origin.y != 0)
        {
            auto max_bottom = underlying_component_size.height - size.height;

            if (origin.y == max_bottom)
            {
                slider_position = 100;
            }
            else
            {
                slider_position = odin::u8((origin.y * 100) / max_bottom);

                if (slider_position == 0)
                {
                    slider_position = 1;
                }

                if (slider_position == 100)
                {
                    slider_position = 99;
                }
            }
        }

        return slider_position;
    }

    // ======================================================================
    // CALCULATE_SCROLLBARS
    // ======================================================================
    void calculate_scrollbars()
    {
        // Fix the scrollbars to be at the correct percentages.
        auto horizontal_slider_position = calculate_horizontal_scrollbar();
        auto vertical_slider_position = calculate_vertical_scrollbar();

        horizontal_scroll_bar_->set_slider_position(horizontal_slider_position);
        vertical_scroll_bar_->set_slider_position(vertical_slider_position);
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
scroll_pane::scroll_pane(
    std::shared_ptr<component> const &underlying_component
  , bool                              top_border)
{
    pimpl_ = std::make_shared<impl>(std::ref(*this));

    pimpl_->underlying_component_ = underlying_component;

    pimpl_->viewport_ = make_viewport(pimpl_->underlying_component_);
    pimpl_->viewport_->on_size_changed.connect(
        [this]{pimpl_->calculate_scrollbars();});
    pimpl_->viewport_->on_subcomponent_size_changed.connect(
        [this]{pimpl_->calculate_scrollbars();});
    pimpl_->viewport_->on_origin_changed.connect(
        [this]{pimpl_->calculate_scrollbars();});

    pimpl_->horizontal_scroll_bar_ = make_horizontal_scroll_bar();
    pimpl_->horizontal_scroll_bar_->on_page_left.connect(
        [this]{pimpl_->on_page_left();});
    pimpl_->horizontal_scroll_bar_->on_page_right.connect(
        [this]{pimpl_->on_page_right();});

    pimpl_->vertical_scroll_bar_ = make_vertical_scroll_bar();
    pimpl_->vertical_scroll_bar_->on_page_up.connect(
        [this]{pimpl_->on_page_up();});
    pimpl_->vertical_scroll_bar_->on_page_down.connect(
        [this]{pimpl_->on_page_down();});

    pimpl_->scroll_frame_ = std::make_shared<scroll_frame>(
        pimpl_->horizontal_scroll_bar_
      , pimpl_->vertical_scroll_bar_
      , top_border);

    auto content = get_container();
    content->set_layout(make_grid_layout(1, 1));

    content->add_component(make_framed_component(
        pimpl_->scroll_frame_
      , pimpl_->viewport_));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
scroll_pane::~scroll_pane()
{
}

// ==========================================================================
// MAKE_SCROLL_PANE
// ==========================================================================
std::shared_ptr<component> make_scroll_pane(
    std::shared_ptr<component> const &underlying_component, 
    bool                              top_border)
{
    return std::make_shared<scroll_pane>(underlying_component, top_border);
}

}

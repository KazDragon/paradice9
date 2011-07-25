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
#include "munin/container.hpp"
#include "munin/horizontal_scroll_bar.hpp"
#include "munin/filled_box.hpp"
#include "munin/frame.hpp"
#include "munin/framed_component.hpp"
#include "munin/grid_layout.hpp"
#include "munin/vertical_scroll_bar.hpp"
#include "munin/viewport.hpp"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>
#include <algorithm>

using namespace odin;
using namespace boost;
using namespace std;

namespace munin {

namespace {

class scroll_frame : public composite_component
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    scroll_frame(bool top_border)
    {
        element_type top_left_element(double_lined_top_left_corner, attribute());
        element_type top_element(double_lined_horizontal_beam, attribute());
        element_type top_right_element(double_lined_top_right_corner, attribute());
        element_type left_element(double_lined_vertical_beam, attribute());
        element_type bottom_left_element(double_lined_bottom_left_corner, attribute());
        element_type bottom_right_element(double_lined_bottom_right_corner, attribute());

        vertical_scroll_bar_ = make_shared<vertical_scroll_bar>();
        horizontal_scroll_bar_ = make_shared<horizontal_scroll_bar>();

        shared_ptr<filled_box> top_left;
        shared_ptr<filled_box> top;
        shared_ptr<filled_box> top_right;

        if (top_border)
        {
            top_left  = make_shared<filled_box>(top_left_element);
            top       = make_shared<filled_box>(top_element);
            top_right = make_shared<filled_box>(top_right_element);
        }

        BOOST_AUTO(left,         make_shared<filled_box>(left_element));
        BOOST_AUTO(bottom_left,  make_shared<filled_box>(bottom_left_element));
        BOOST_AUTO(bottom_right, make_shared<filled_box>(bottom_right_element));

        BOOST_AUTO(content, get_container());
        content->set_layout(make_shared<grid_layout>(1, 1));

        content->add_component(make_shared<frame>(
            top_left, top, top_right
          , left, vertical_scroll_bar_
          , bottom_left, horizontal_scroll_bar_, bottom_right));
    }

    // ======================================================================
    // DESTRUCTOR
    // ======================================================================
    ~scroll_frame()
    {
    }

    shared_ptr<horizontal_scroll_bar> horizontal_scroll_bar_;
    shared_ptr<vertical_scroll_bar>   vertical_scroll_bar_;
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

    scroll_pane              &self_;
    shared_ptr<scroll_frame>  scroll_frame_;
    shared_ptr<viewport>      viewport_;
    shared_ptr<component>     underlying_component_;

    // ======================================================================
    // ON_PAGE_LEFT
    // ======================================================================
    void on_page_left()
    {
        point new_origin;
        BOOST_AUTO(origin, viewport_->get_origin());
        BOOST_AUTO(size, self_.get_size());
        BOOST_AUTO(underlying_component_size, underlying_component_->get_size());

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
        point new_origin;
        BOOST_AUTO(origin, viewport_->get_origin());
        BOOST_AUTO(size, self_.get_size());
        BOOST_AUTO(underlying_component_size, underlying_component_->get_size());

        // Find out how far over to the right the maximum origin should be.
        // If the underlying component has a width smaller than this, then
        // the max is 0, because we can't scroll to the right.  Otherwise,
        // it is the width of the underlying component - the width of the
        // viewport.
        BOOST_AUTO(max_right,
            (underlying_component_size.width < size.width
          ? 0
          : underlying_component_size.width - size.width));

        // Same for the bottom, with height.
        BOOST_AUTO(max_bottom,
            (underlying_component_size.height < size.height
          ? 0
          : underlying_component_size.height - size.height));

        // Now, move the origin over to the right by one page, up to the
        // maximum to the right.
        // new_origin.x = (min)(origin.x + size.width, max_right);
        new_origin.x = (min)(origin.x + 1, max_right);
        new_origin.y = (min)(origin.y, max_bottom);

        viewport_->set_origin(new_origin);
        calculate_scrollbars();
    }

    // ======================================================================
    // ON_PAGE_UP
    // ======================================================================
    void on_page_up()
    {
        BOOST_AUTO(origin, viewport_->get_origin());
        BOOST_AUTO(size, viewport_->get_size());

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
        point new_origin;
        BOOST_AUTO(origin, viewport_->get_origin());
        BOOST_AUTO(size, viewport_->get_size());
        BOOST_AUTO(underlying_component_size, underlying_component_->get_size());

        // Find out how far over to the right the maximum origin should be.
        // If the underlying component has a width smaller than this, then
        // the max is 0, because we can't scroll to the right.  Otherwise,
        // it is the width of the underlying component - the width of the
        // viewport.
        BOOST_AUTO(max_right,
            (underlying_component_size.width < size.width
          ? 0
          : underlying_component_size.width - size.width));

        // Same for the bottom, with height.
        BOOST_AUTO(max_bottom,
            (underlying_component_size.height < size.height
          ? 0
          : underlying_component_size.height - size.height));

        // Now, move the origin over to the right by one page, up to the
        // maximum to the right.
        new_origin.x = (min)(origin.x, max_right);
        new_origin.y = (min)(origin.y + size.height, max_bottom);

        viewport_->set_origin(new_origin);
    }

    // ======================================================================
    // CALCULATE_HORIZONTAL_SCROLLBAR
    // ======================================================================
    optional<u8> calculate_horizontal_scrollbar()
    {
        BOOST_AUTO(origin, viewport_->get_origin());
        BOOST_AUTO(size, viewport_->get_size());
        BOOST_AUTO(underlying_component_size, underlying_component_->get_size());

        u8 slider_position = 0;

        if (underlying_component_size.width <= size.width)
        {
            return optional<u8>();
        }

        if (origin.x != 0)
        {
            BOOST_AUTO(
                max_right
              , underlying_component_size.width - size.width);

            if (origin.x == max_right)
            {
                slider_position = 100;
            }
            else
            {
                slider_position = u8((origin.x * 100) / max_right);

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
    optional<u8> calculate_vertical_scrollbar()
    {
        BOOST_AUTO(origin, viewport_->get_origin());
        BOOST_AUTO(size, viewport_->get_size());
        BOOST_AUTO(underlying_component_size, underlying_component_->get_size());

        u8 slider_position = 0;

        if (underlying_component_size.height <= size.height)
        {
            return optional<u8>();
        }

        if (origin.y != 0)
        {
            BOOST_AUTO(
                max_bottom
              , underlying_component_size.height - size.height);

            if (origin.y == max_bottom)
            {
                slider_position = 100;
            }
            else
            {
                slider_position = u8((origin.y * 100) / max_bottom);

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
        BOOST_AUTO(
            horizontal_slider_position
          , calculate_horizontal_scrollbar());

        BOOST_AUTO(
            vertical_slider_position
          , calculate_vertical_scrollbar());

        scroll_frame_->horizontal_scroll_bar_->set_slider_position(
            horizontal_slider_position);
        scroll_frame_->vertical_scroll_bar_->set_slider_position(
            vertical_slider_position);
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
scroll_pane::scroll_pane(
    shared_ptr<component> underlying_component
  , bool                  top_border)
{
    pimpl_ = make_shared<impl>(ref(*this));

    pimpl_->viewport_ = make_shared<viewport>(underlying_component);
    pimpl_->scroll_frame_ = make_shared<scroll_frame>(top_border);
    pimpl_->underlying_component_ = underlying_component;

    pimpl_->viewport_->on_size_changed.connect(
        bind(&impl::calculate_scrollbars, pimpl_.get()));

    pimpl_->viewport_->on_subcomponent_size_changed.connect(
        bind(&impl::calculate_scrollbars, pimpl_.get()));

    pimpl_->viewport_->on_origin_changed.connect(
        bind(&impl::calculate_scrollbars, pimpl_.get()));

    pimpl_->scroll_frame_->horizontal_scroll_bar_->on_page_left.connect(
        bind(&impl::on_page_left, pimpl_.get()));

    pimpl_->scroll_frame_->horizontal_scroll_bar_->on_page_right.connect(
        bind(&impl::on_page_right, pimpl_.get()));

    pimpl_->scroll_frame_->vertical_scroll_bar_->on_page_up.connect(
        bind(&impl::on_page_up, pimpl_.get()));

    pimpl_->scroll_frame_->vertical_scroll_bar_->on_page_down.connect(
        bind(&impl::on_page_down, pimpl_.get()));

    BOOST_AUTO(content, get_container());
    content->set_layout(make_shared<grid_layout>(1, 1));

    content->add_component(make_shared<framed_component>(
        pimpl_->scroll_frame_
      , pimpl_->viewport_));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
scroll_pane::~scroll_pane()
{
}

}

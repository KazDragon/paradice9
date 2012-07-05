// ==========================================================================
// Munin Vertical Scroll Bar
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
#include "munin/vertical_scroll_bar.hpp"
#include "munin/canvas.hpp"
#include "munin/algorithm.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/assign/list_of.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>
#include <boost/typeof/typeof.hpp>
#include <utility>

using namespace munin;
using namespace odin;
using namespace boost;
using namespace boost::assign;
using namespace std;

namespace munin {
    
// ==========================================================================
// VERTICAL_SCROLL_BAR::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct vertical_scroll_bar::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(vertical_scroll_bar &self)
        : self_(self)
        , slider_position_(0)
    {
    }

    // ======================================================================
    // DO_DRAW
    // ======================================================================
    void do_draw(
        canvas          &cvs
      , rectangle const &region)
    {
        draw_beam(cvs, region);

        // Only draw the slider if the percentage is initialized
        if (percentage_.is_initialized())
        {
            draw_slider(cvs, region);
        }
    }

    // ======================================================================
    // DRAW_BEAM
    // ======================================================================
    void draw_beam(
        canvas          &cvs
      , rectangle const &region)
    {
        // Draws the beam on which the slider is placed.
        for (s32 y_coord = region.origin.y;
             y_coord < region.origin.y + region.size.height;
             ++y_coord)
        {
            for (s32 x_coord = region.origin.x;
                 x_coord < region.origin.x + region.size.width;
                 ++x_coord)
            {
                cvs[x_coord][y_coord] = 
                    element_type(double_lined_vertical_beam, pen_);
            }
        }
    }
    
    // ======================================================================
    // CALCULATE_SLIDER_POSITION
    // ======================================================================
    void calculate_slider_position()
    {
        // If there is no percentage, then leave this undrawn.  This will
        // then look like a normal beam.
        if (!percentage_.is_initialized())
        {
            return;
        }
        
        BOOST_AUTO(percentage, percentage_.get());
        
        // Work out the position of the slider on the scroll bar.
        BOOST_AUTO(size, self_.get_size());
        
        // The slider position would be at a particular percentage of the
        // height.  So, if the slider is 80% the way through a bar of height
        // 40, then it should be at position 40 * (80 / 100) = 32.  However,
        // we're playing with integer arithmetic -- 80/100 is always 0.
        // Therefore, we make sure to do (40 * 80) / 100, which has the
        // correct result.
        slider_position_ = (size.height * percentage) / 100;
        
        // Some special cases: the slider is at the topmost position only if
        // the percentage is 0.  Likewise, the slider is at the bottommost
        // position only if the percentage is 100.  Otherwise, the slider
        // is slightly inwards.
        slider_position_ = (percentage == 0) 
                         ? 0
                         : (std::max)(slider_position_, s32(1));
                        
        slider_position_ = (percentage == 100)
                         ? (size.height - 1)
                         : (std::min)(slider_position_, s32(size.height - 2)); 

    }

    // ======================================================================
    // DRAW_SLIDER
    // ======================================================================
    void draw_slider(
        canvas          &cvs
      , rectangle const &region)
    {
        // Now that we know precisely where the slider is, check that it is
        // within the redraw region.
        if (intersection(
            region
          , rectangle(point(0, slider_position_), extent(1, 1))))
        {
            cvs[0][slider_position_] = element_type(mix_lined_hcross, pen_);
        }
    }
    
    // ======================================================================
    // ON_MOUSE_EVENT
    // ======================================================================
    void on_mouse_event(odin::ansi::mouse_report const &report)
    {
        if (report.button_ == odin::ansi::mouse_report::LEFT_BUTTON_DOWN)
        {
            if (report.y_position_ < slider_position_)
            {
                self_.on_page_up();
            }
            else if (report.y_position_ > slider_position_)
            {
                self_.on_page_down();
            }
        }
    }
    
    vertical_scroll_bar &self_;
    attribute              pen_;
    s32                    slider_position_;
    optional<u8>           percentage_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
vertical_scroll_bar::vertical_scroll_bar()
{
    pimpl_ = make_shared<impl>(ref(*this));
    set_can_focus(false);
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
vertical_scroll_bar::~vertical_scroll_bar()
{
}

// ==========================================================================
// GET_SLIDER_POSITION
// ==========================================================================
optional<u8> vertical_scroll_bar::get_slider_position() const
{
    return pimpl_->percentage_;
}

// ==========================================================================
// SET_SLIDER_POSITION
// ==========================================================================
void vertical_scroll_bar::set_slider_position(optional<u8> percentage)
{
    BOOST_AUTO(old_slider_position, pimpl_->slider_position_);
    BOOST_AUTO(old_percentage, pimpl_->percentage_);

    pimpl_->percentage_ = percentage;
    pimpl_->calculate_slider_position();

    if (old_slider_position != pimpl_->slider_position_)
    {
        // The new percentage has caused the slider position to move.
        // Therefore, we need to redraw both where the slider was, and
        // where the slider now is.
        on_redraw(list_of
            (rectangle(point(0, old_slider_position), extent(1, 1)))
            (rectangle(point(0, pimpl_->slider_position_), extent(1, 1))));
    }
    else if (old_percentage.is_initialized() == percentage.is_initialized())
    {
        // The slider has been turned either on or off.  Redraw it.
        on_redraw(list_of
            (rectangle(point(pimpl_->slider_position_, 0), extent(1, 1))));
    }
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
extent vertical_scroll_bar::do_get_preferred_size() const
{
    return extent(1, get_size().height);
}

// ==========================================================================
// DO_SET_SIZE
// ==========================================================================
void vertical_scroll_bar::do_set_size(extent const &size)
{
    basic_component::do_set_size(size);
    pimpl_->calculate_slider_position();
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void vertical_scroll_bar::do_draw(
    canvas          &cvs
  , rectangle const &region)
{
    pimpl_->do_draw(cvs, region);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void vertical_scroll_bar::do_event(any const &event)
{
    odin::ansi::mouse_report const *report =
        any_cast<odin::ansi::mouse_report>(&event);
    
    if (report != NULL)
    {
        pimpl_->on_mouse_event(*report);
    }
}

// ==========================================================================
// DO_SET_ATTRIBUTE
// ==========================================================================
void vertical_scroll_bar::do_set_attribute(string const &name, any const &attr)
{
    if (name == ATTRIBUTE_PEN)
    {
        BOOST_AUTO(pen, any_cast<attribute>(&attr));

        if (pen != NULL)
        {
            pimpl_->pen_ = *pen;
        }

        on_redraw(list_of(rectangle(point(), get_size())));
    }
}

}


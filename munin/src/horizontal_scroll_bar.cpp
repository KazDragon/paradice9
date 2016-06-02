// ==========================================================================
// Munin Horizontal Scroll Bar
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
#include "munin/horizontal_scroll_bar.hpp"
#include "munin/context.hpp"
#include "munin/algorithm.hpp"
#include "munin/sco_glyphs.hpp"
#include <terminalpp/canvas_view.hpp>
#include <utility>

namespace munin {

// ==========================================================================
// HORIZONTAL_SCROLL_BAR::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct horizontal_scroll_bar::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(horizontal_scroll_bar &self)
        : self_(self)
        , slider_position_(0)
    {
    }

    // ======================================================================
    // DO_DRAW
    // ======================================================================
    void do_draw(
        terminalpp::canvas_view &cvs
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
        terminalpp::canvas_view &cvs
      , rectangle const &region)
    {
        // Draws the beam on which the slider is placed.
        for (odin::s32 y_coord = region.origin.y;
             y_coord < region.origin.y + region.size.height;
             ++y_coord)
        {
            for (odin::s32 x_coord = region.origin.x;
                 x_coord < region.origin.x + region.size.width;
                 ++x_coord)
            {
                cvs[x_coord][y_coord] =
                    terminalpp::element(double_lined_horizontal_beam, pen_);
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

        auto percentage = percentage_.get();

        // Work out the position of the slider on the scroll bar.
        auto size = self_.get_size();

        // The slider position would be at a particular percentage of the
        // width.  So, if the slider is 80% the way through a bar of width
        // 40, then it should be at position 40 * (80 / 100) = 32.  However,
        // we're playing with integer arithmetic -- 80/100 is always 0.
        // Therefore, we make sure to do (40 * 80) / 100, which has the
        // correct result.
        slider_position_ = (size.width * percentage) / 100;

        // Some special cases: the slider is at the leftmost position only if
        // the percentage is 0.  Likewise, the slider is at the rightmost
        // position only if the percentage is 100.  Otherwise, the slider
        // is slightly inwards.
        slider_position_ = (percentage == 0)
                         ? 0
                         : (std::max)(slider_position_, odin::s32(1));

        slider_position_ = (percentage == 100)
                         ? (size.width - 1)
                         : (std::min)(slider_position_, odin::s32(size.width - 2));

    }

    // ======================================================================
    // DRAW_SLIDER
    // ======================================================================
    void draw_slider(
        terminalpp::canvas_view &cvs
      , rectangle const &region)
    {
        // Now that we know precisely where the slider is, check that it is
        // within the redraw region.
        if (intersection(
            region
          , rectangle(
                terminalpp::point(slider_position_, 0)
              , terminalpp::extent(1, 1))))
        {
            cvs[slider_position_][0] = terminalpp::element(mix_lined_vcross, pen_);
        }
    }

    // ======================================================================
    // ON_MOUSE_EVENT
    // ======================================================================
    void on_mouse_event(terminalpp::ansi::mouse::report const &report)
    {
        if (report.button_ == terminalpp::ansi::mouse::report::LEFT_BUTTON_DOWN)
        {
            if (report.x_position_ < slider_position_)
            {
                self_.on_page_left();
            }
            else if (report.x_position_ > slider_position_)
            {
                self_.on_page_right();
            }
        }
    }

    horizontal_scroll_bar     &self_;
    terminalpp::attribute      pen_;
    odin::s32                  slider_position_;
    boost::optional<odin::u8>  percentage_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
horizontal_scroll_bar::horizontal_scroll_bar()
{
    pimpl_ = std::make_shared<impl>(std::ref(*this));
    set_can_focus(false);
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
horizontal_scroll_bar::~horizontal_scroll_bar()
{
}

// ==========================================================================
// GET_SLIDER_POSITION
// ==========================================================================
boost::optional<odin::u8> horizontal_scroll_bar::get_slider_position() const
{
    return pimpl_->percentage_;
}

// ==========================================================================
// SET_SLIDER_POSITION
// ==========================================================================
void horizontal_scroll_bar::set_slider_position(boost::optional<odin::u8> percentage)
{
    auto old_slider_position = pimpl_->slider_position_;
    auto old_percentage = pimpl_->percentage_;

    pimpl_->percentage_ = percentage;
    pimpl_->calculate_slider_position();

    if (old_slider_position != pimpl_->slider_position_)
    {
        // The new percentage has caused the slider position to move.
        // Therefore, we need to redraw both where the slider was, and
        // where the slider now is.
        on_redraw({
            rectangle(
                terminalpp::point(old_slider_position, 0)
              , terminalpp::extent(1, 1))
          , rectangle(
                terminalpp::point(pimpl_->slider_position_, 0)
              , terminalpp::extent(1, 1))});
    }
    else if (old_percentage.is_initialized() == percentage.is_initialized())
    {
        // The slider has been turned either on or off.  Redraw it.
        on_redraw({
            rectangle(
                terminalpp::point(pimpl_->slider_position_, 0)
              , terminalpp::extent(1, 1))});
    }
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
terminalpp::extent horizontal_scroll_bar::do_get_preferred_size() const
{
    return terminalpp::extent(get_size().width, 1);
}

// ==========================================================================
// DO_SET_SIZE
// ==========================================================================
void horizontal_scroll_bar::do_set_size(terminalpp::extent const &size)
{
    basic_component::do_set_size(size);
    pimpl_->calculate_slider_position();
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void horizontal_scroll_bar::do_draw(
    context         &ctx
  , rectangle const &region)
{
    pimpl_->do_draw(ctx.get_canvas(), region);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void horizontal_scroll_bar::do_event(boost::any const &event)
{
    auto report =
        boost::any_cast<terminalpp::ansi::mouse::report>(&event);

    if (report)
    {
        pimpl_->on_mouse_event(*report);
    }
}

// ==========================================================================
// DO_SET_ATTRIBUTE
// ==========================================================================
void horizontal_scroll_bar::do_set_attribute(std::string const &name, boost::any const &attr)
{
    if (name == ATTRIBUTE_PEN)
    {
        auto pen = boost::any_cast<terminalpp::attribute>(&attr);

        if (pen != nullptr)
        {
            pimpl_->pen_ = *pen;
        }

        on_redraw({rectangle({}, get_size())});
    }
}

// ==========================================================================
// MAKE_HORIZONTAL_SCROLL_BAR
// ==========================================================================
std::shared_ptr<horizontal_scroll_bar> make_horizontal_scroll_bar()
{
    return std::make_shared<horizontal_scroll_bar>();
}


}


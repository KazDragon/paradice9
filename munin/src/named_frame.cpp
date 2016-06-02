// ==========================================================================
// Munin Named Frame.
//
// Copyright (C) 2010 Matthew Chaplain, All Rights Reserved.
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
#include "munin/named_frame.hpp"
#include "munin/algorithm.hpp"
#include "munin/context.hpp"
#include "munin/filled_box.hpp"
#include "munin/frame.hpp"
#include "munin/grid_layout.hpp"
#include "munin/sco_glyphs.hpp"
#include <terminalpp/canvas_view.hpp>
#include <utility>

namespace munin {

// ==========================================================================
// TITLE_BAR
// ==========================================================================
class title_bar : public basic_component
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    title_bar(
        terminalpp::string const &title
      , terminalpp::element const &filler_element)
        : title_(title)
        , filler_element_(filler_element)
    {
        set_can_focus(false);
    }

    // ======================================================================
    // SET_TITLE
    // ======================================================================
    void set_title(terminalpp::string const &title)
    {
        title_ = title;
        on_redraw({rectangle({}, get_size())});
    }

    // ======================================================================
    // DO_GET_PREFERRED_SIZE
    // ======================================================================
    terminalpp::extent do_get_preferred_size() const
    {
        // The preferred width is the title's width, plus two blank either
        // side, plus at least one filler either side for an additional 2;
        // and a height of 1.
        return terminalpp::extent(title_.size() + 4, 1);
    }

    // ======================================================================
    // DO_DRAW
    // ======================================================================
    void do_draw(
        context         &ctx
      , rectangle const &region)
    {
        draw_filler(ctx.get_canvas(), region);
        draw_title(ctx.get_canvas(), region);
    }

protected :
    // ======================================================================
    // DO_SET_ATTRIBUTE
    // ======================================================================
    void do_set_attribute(std::string const &name, boost::any const &attr)
    {
        if (name == ATTRIBUTE_PEN)
        {
            auto pen = boost::any_cast<terminalpp::attribute>(&attr);

            if (pen != nullptr)
            {
                filler_element_.attribute_ = *pen;
            }
        }

        on_redraw({rectangle({}, get_size())});
    }

private :
    // ======================================================================
    // DRAW_FILLER
    // ======================================================================
    void draw_filler(
        terminalpp::canvas_view &cvs
      , rectangle const &region)
    {
        for (odin::s32 y_coord = region.origin.y;
             y_coord < region.origin.y + region.size.height;
             ++y_coord)
        {
            for (odin::s32 x_coord = region.origin.x;
                 x_coord < region.origin.x + region.size.width;
                 ++x_coord)
            {
                cvs[x_coord][y_coord] = filler_element_;
            }
        }
    }

    // ======================================================================
    // DRAW_TITLE
    // ======================================================================
    void draw_title(
        terminalpp::canvas_view &cvs
      , rectangle const &region)
    {
        static terminalpp::element const default_element(' ');

        for (odin::s32 y_coord = region.origin.y;
             y_coord < region.origin.y + region.size.height;
             ++y_coord)
        {
            for (odin::s32 x_coord = region.origin.x;
                 x_coord < region.origin.x + region.size.width;
                 ++x_coord)
            {
                auto index = x_coord - 2;

                if (index == -1 || index == odin::s32(title_.size()))
                {
                    cvs[x_coord][y_coord] = default_element;
                }
                else if (index >= 0 && index < odin::s32(title_.size()))
                {
                    cvs[x_coord][y_coord] = title_[index];
                }
            }
        }
    }

    terminalpp::string  title_;
    terminalpp::element filler_element_;
};

// ==========================================================================
// FRAME IMPLEMENTATION STRUCTURE
// ==========================================================================
struct named_frame::impl
{
public :
    impl(named_frame &self)
        : self_(self)
        , closeable_(false)
    {
    }

    // ======================================================================
    // SET_NAME
    // ======================================================================
    void set_name(terminalpp::string const &name)
    {
        title_bar_->set_title(name);
    }

    // ======================================================================
    // SET_CLOSEABLE
    // ======================================================================
    void set_closeable(bool closeable)
    {
        closeable_ = closeable;

        if (closeable_)
        {
            terminalpp::attribute pen;
            pen.foreground_colour_ = terminalpp::ansi::graphics::colour::red;
            pen.intensity_ = terminalpp::ansi::graphics::intensity::bold;

            top_right_->set_attribute(ATTRIBUTE_LOCK,  false);
            top_right_->set_attribute(ATTRIBUTE_GLYPH, single_lined_top_right_corner);
            top_right_->set_attribute(ATTRIBUTE_PEN,   pen);
            top_right_->set_attribute(ATTRIBUTE_LOCK,  true);
        }
        else
        {
            top_right_->set_attribute(ATTRIBUTE_LOCK,  false);
            top_right_->set_attribute(ATTRIBUTE_GLYPH, double_lined_top_right_corner);
            top_right_->set_attribute(ATTRIBUTE_PEN,   terminalpp::attribute());
        }
    }

    // ======================================================================
    // HANDLE_MOUSE_EVENT
    // ======================================================================
    bool handle_mouse_event(terminalpp::ansi::mouse::report const &report)
    {
        bool handled = false;

        // If this is not a mouse click, then ignore the event.
        if (report.button_ != terminalpp::ansi::mouse::report::LEFT_BUTTON_DOWN
         && report.button_ != terminalpp::ansi::mouse::report::RIGHT_BUTTON_DOWN)
        {
            return false;
        }
        
        // If we're closeable, then check to see if the close button has been
        // pressed.  If so, fire the close signal.
        if (closeable_)
        {
            auto position = self_.get_position();
            auto size =     self_.get_size();

            terminalpp::point close_button((position.x + size.width) - 1, 0);

            if (report.x_position_ == close_button.x
             && report.y_position_ == close_button.y)
            {
                self_.on_close();
                handled = true;
            }
        }

        return handled;
    }

    named_frame                &self_;
    std::shared_ptr<title_bar>  title_bar_;
    std::shared_ptr<component>  top_right_;
    bool                        closeable_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
named_frame::named_frame()
  : basic_frame(
        std::make_shared<filled_box>(double_lined_top_left_corner),
        std::make_shared<title_bar>("", double_lined_horizontal_beam),
        std::make_shared<filled_box>(double_lined_top_right_corner),
        std::make_shared<filled_box>(double_lined_vertical_beam),
        std::make_shared<filled_box>(double_lined_vertical_beam),
        std::make_shared<filled_box>(double_lined_bottom_left_corner),
        std::make_shared<filled_box>(double_lined_horizontal_beam),
        std::make_shared<filled_box>(double_lined_bottom_right_corner))
{
    pimpl_ = std::make_shared<impl>(std::ref(*this));
    pimpl_->title_bar_ = std::dynamic_pointer_cast<title_bar>(get_top_component());
    pimpl_->top_right_ = get_top_right_component();
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
named_frame::~named_frame()
{
}

// ==========================================================================
// SET_NAME
// ==========================================================================
void named_frame::set_name(terminalpp::string const &name)
{
    pimpl_->set_name(name);
    on_redraw({rectangle({}, terminalpp::extent(get_size().width, 1))});
}

// ==========================================================================
// SET_CLOSEABLE
// ==========================================================================
void named_frame::set_closeable(bool closeable)
{
    pimpl_->set_closeable(closeable);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void named_frame::do_event(boost::any const &event)
{
    bool handled = false;

    auto report =
        boost::any_cast<terminalpp::ansi::mouse::report>(&event);

    if (report)
    {
        handled = pimpl_->handle_mouse_event(*report);
    }

    if (!handled)
    {
        composite_component::do_event(event);
    }
}

// ==========================================================================
// NAMED_FRAME
// ==========================================================================
std::shared_ptr<named_frame> make_named_frame(terminalpp::string const &name)
{
    auto frame = std::make_shared<named_frame>();
    frame->set_name(name);
    return frame;
}

}

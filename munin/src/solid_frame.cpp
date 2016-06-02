// ==========================================================================
// Munin Solid Frame.
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
#include "munin/solid_frame.hpp"
#include "munin/algorithm.hpp"
#include "munin/filled_box.hpp"
#include "munin/frame.hpp"
#include "munin/grid_layout.hpp"
#include "munin/sco_glyphs.hpp"
#include "terminalpp/canvas.hpp"

namespace munin {

// ==========================================================================
// FRAME IMPLEMENTATION STRUCTURE
// ==========================================================================
struct solid_frame::impl
{
public :
    impl(solid_frame &self, std::shared_ptr<component> top_right)
        : self_(self)
        , top_right_(top_right)
        , closeable_(false)
    {
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

    solid_frame                &self_;
    std::shared_ptr<component>  top_right_;
    bool                        closeable_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
solid_frame::solid_frame()
  : basic_frame(
        std::make_shared<filled_box>(double_lined_top_left_corner),
        std::make_shared<filled_box>(double_lined_horizontal_beam),
        std::make_shared<filled_box>(double_lined_top_right_corner),
        std::make_shared<filled_box>(double_lined_vertical_beam),
        std::make_shared<filled_box>(double_lined_vertical_beam),
        std::make_shared<filled_box>(double_lined_bottom_left_corner),
        std::make_shared<filled_box>(double_lined_horizontal_beam),
        std::make_shared<filled_box>(double_lined_bottom_right_corner))
{
    pimpl_ = std::make_shared<impl>(std::ref(*this), get_top_right_component());
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
solid_frame::~solid_frame()
{
}

// ==========================================================================
// SET_CLOSEABLE
// ==========================================================================
void solid_frame::set_closeable(bool closeable)
{
    pimpl_->set_closeable(closeable);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void solid_frame::do_event(boost::any const &event)
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
// MAKE_SOLID_FRAME
// ==========================================================================
std::shared_ptr<frame> make_solid_frame()
{
    return std::make_shared<solid_frame>();
}

}


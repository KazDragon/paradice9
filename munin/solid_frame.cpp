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
#include "munin/canvas.hpp"
#include "munin/filled_box.hpp"
#include "munin/frame.hpp"
#include "munin/grid_layout.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/assign/list_of.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace odin;
using namespace boost;
using namespace boost::assign;
using namespace std;

namespace munin {

// ==========================================================================
// FRAME IMPLEMENTATION STRUCTURE
// ==========================================================================
struct solid_frame::impl
{
public :
    impl(solid_frame &self)
        : self_(self)
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
            attribute pen;
            pen.foreground_colour_ = odin::ansi::graphics::COLOUR_RED;
            pen.intensity_ = odin::ansi::graphics::INTENSITY_BOLD;

            top_right_->set_attribute(ATTRIBUTE_LOCK,  false);
            top_right_->set_attribute(ATTRIBUTE_GLYPH, single_lined_top_right_corner);
            top_right_->set_attribute(ATTRIBUTE_PEN,   pen);
            top_right_->set_attribute(ATTRIBUTE_LOCK,  true);
        }
        else
        {
            top_right_->set_attribute(ATTRIBUTE_LOCK,  false);
            top_right_->set_attribute(ATTRIBUTE_GLYPH, double_lined_top_right_corner);
            top_right_->set_attribute(ATTRIBUTE_PEN,   attribute());
        }
    }
    
    // ======================================================================
    // HANDLE_MOUSE_CLICK
    // ======================================================================
    bool handle_mouse_click(odin::ansi::mouse_report const *report)
    {
        bool handled = false;
        
        // If we're closeable, then check to see if the close button has been
        // pressed.  If so, fire the close signal.
        if (closeable_)
        {
            BOOST_AUTO(position, self_.get_position());
            BOOST_AUTO(size,     self_.get_size());
            
            point close_button((position.x + size.width) - 1, 0);
            
            if (report->x_position_ == close_button.x
             && report->y_position_ == close_button.y)
            {
                self_.on_close();
                handled = true;
            }
        }
        
        return handled;
    }
    
    solid_frame           &self_;
    shared_ptr<component>  top_right_;
    bool                   closeable_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
solid_frame::solid_frame()
{
    pimpl_ = make_shared<impl>(ref(*this));

    element_type top_left_element(double_lined_top_left_corner, attribute());
    element_type top_element(double_lined_horizontal_beam, attribute());
    element_type top_right_element(double_lined_top_right_corner, attribute());
    element_type left_element(double_lined_vertical_beam, attribute());
    element_type right_element(double_lined_vertical_beam, attribute());
    element_type bottom_left_element(double_lined_bottom_left_corner, attribute());
    element_type bottom_element(double_lined_horizontal_beam, attribute());
    element_type bottom_right_element(double_lined_bottom_right_corner, attribute());

    BOOST_AUTO(top_left,     make_shared<filled_box>(top_left_element));
    BOOST_AUTO(top,          make_shared<filled_box>(top_element));
    pimpl_->top_right_ =     make_shared<filled_box>(top_right_element);
    BOOST_AUTO(left,         make_shared<filled_box>(left_element));
    BOOST_AUTO(right,        make_shared<filled_box>(right_element));
    BOOST_AUTO(bottom_left,  make_shared<filled_box>(bottom_left_element));
    BOOST_AUTO(bottom,       make_shared<filled_box>(bottom_element));
    BOOST_AUTO(bottom_right, make_shared<filled_box>(bottom_right_element));

    BOOST_AUTO(content, get_container());
    content->set_layout(make_shared<grid_layout>(1, 1));

    content->add_component(make_shared<frame>(
        top_left, top, pimpl_->top_right_
      , left, right
      , bottom_left, bottom, bottom_right));
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
void solid_frame::do_event(any const &event)
{
    bool handled = false;
    
    odin::ansi::mouse_report const *report =
        any_cast<odin::ansi::mouse_report>(&event);
     
    if (report != NULL)
    {
        handled = pimpl_->handle_mouse_click(report);
    }
    
    if (!handled)
    {
        composite_component::do_event(event);
    }
}

}


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
    impl(solid_frame &self, shared_ptr<component> top_right)
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
    : basic_frame(
          make_shared<filled_box>(element_type(double_lined_top_left_corner))
        , make_shared<filled_box>(element_type(double_lined_horizontal_beam))
        , make_shared<filled_box>(element_type(double_lined_top_right_corner))
        , make_shared<filled_box>(element_type(double_lined_vertical_beam))
        , make_shared<filled_box>(element_type(double_lined_vertical_beam))
        , make_shared<filled_box>(element_type(double_lined_bottom_left_corner))
        , make_shared<filled_box>(element_type(double_lined_horizontal_beam))
        , make_shared<filled_box>(element_type(double_lined_bottom_right_corner)))
{
    pimpl_ = make_shared<impl>(ref(*this), get_top_right_component());
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


// ==========================================================================
// Munin ANSI Window.
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
#include "munin/ansi/window.hpp"
#include "munin/ansi/container.hpp"
#include "munin/ansi/basic_container.hpp"
#include "munin/ansi/ansi_canvas.hpp"
#include "munin/ansi/protocol.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/typeof/typeof.hpp>

using namespace boost;
using namespace std;
using namespace odin;

namespace {


//* =========================================================================
/// \brief Returns true if the specified region is identical between the
/// two canvases, false otherwise.
//* =========================================================================
bool canvas_region_compare(
    munin::rectangle                         const &region
  , munin::canvas<munin::ansi::element_type>       &lhs
  , munin::canvas<munin::ansi::element_type>       &rhs)
{
    bool compare_equal = true;
    
    for (s32 row = 0;
         row < region.size.height && compare_equal; 
         ++row)
    {
        for (s32 column = 0;
             column < region.size.width && compare_equal;
             ++column)
        {
            munin::ansi::element_type const &lhs_element =
                lhs[row + region.origin.y][column + region.origin.x];
            munin::ansi::element_type const &rhs_element =
                rhs[row + region.origin.y][column + region.origin.x];
                
            compare_equal = lhs_element == rhs_element;
        }
    }
    
    return compare_equal;
}

//* =========================================================================
/// \brief Returns a string which would paint the specified region on a
/// canvas.
//* =========================================================================
string canvas_region_string(
    munin::rectangle const &region
  , munin::canvas<munin::ansi::element_type> &cvs)
{
    string output;
    
    for (s32 row = 0; row < region.size.height; ++row)
    {
        // Place the cursor at the start of this row.
        output += munin::ansi::cursor_position(
            munin::point(region.origin.x, region.origin.y + row));
        
        for (s32 column = 0; column < region.size.width; ++column)
        {
            munin::ansi::element_type const &element = 
                cvs[column + region.origin.x]
                   [row    + region.origin.y];
            
            // TODO: apply graphics attributes here.
            
            // With the attributes set, output the actual character.
            output.push_back(element.first);
        }
    }
    
    return output;
}

}

namespace munin { namespace ansi {

// ==========================================================================
// WINDOW IMPLEMENTATION STRUCTURE
// ==========================================================================
class window::impl
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(window &self, boost::asio::io_service &io_service)
        : self_(self)
        , io_service_(io_service)
        , content_(new basic_container)
        , last_cursor_position_(point(0,0))
        , last_cursor_state_(false)
        , repaint_scheduled_(false)
    {
        content_->on_redraw.connect(
            bind(&impl::redraw_handler, this, _1));
        
        content_->on_cursor_state_changed.connect(
            bind(&impl::schedule_repaint, this));
        
        content_->on_cursor_position_changed.connect(
            bind(&impl::schedule_repaint, this));
    }
    
    // ======================================================================
    // GET_CONTENT
    // ======================================================================
    boost::shared_ptr<container> get_content()
    {
        return content_;
    }

    // ======================================================================
    // EVENT
    // ======================================================================
    void event(boost::any const &event)
    {
        content_->event(event);
    }
    
private :
    // ======================================================================
    // SCHEDULE_REPAINT
    // ======================================================================
    void schedule_repaint()
    {
        // Schedules a repaint only if one has not already been scheduled.
        if (!repaint_scheduled_)
        {
            io_service_.post(bind(&impl::do_repaint, this));
            repaint_scheduled_ = true;
        }
    }
    
    // ======================================================================
    // REDRAW_HANDLER
    // ======================================================================
    void redraw_handler(vector<rectangle> const &regions)
    {
        // Coalesce all redraw events into a single repaint.  That way,
        // there is only one major repaint if 100 components decide they
        // need drawing at once.
        copy(regions.begin(), regions.end(), back_inserter(redraw_regions_));
        schedule_repaint();
    }
    
    // ======================================================================
    // DO_REPAINT
    // ======================================================================
    void do_repaint()
    {
        // First, prune out any regions with dimensions zero.
        vector<rectangle> pruned_regions;
        
        BOOST_FOREACH(rectangle const &region, redraw_regions_)
        {
            if (region.size.width != 0 && region.size.height != 0)
            {
                pruned_regions.push_back(region);
            }
        }

        // Prepare a string that is a collection of the ANSI data required
        // to update the window.         
        string output;
        
        // Next, cut the regions into horizontal slices.
        vector<rectangle> slices = rectangular_slice(pruned_regions);
        
        // Ensure that our canvas is the correct size.
        extent size = content_->get_size();
        canvas_.set_size(size);
        
        // Take a copy of the canvas.  We will want to check against this
        // after the draw operations to see if anything has changed.
        ansi_canvas canvas_clone = canvas_;
        
        // Draw each slice on the canvas.
        BOOST_FOREACH(rectangle const &region, slices)
        {
            content_->draw(canvas_, point(0,0), region);
        }
        
        // For each slice, see if it has changed between the canvas that was
        // updated and the clone of the original.
        BOOST_FOREACH(rectangle const &slice, slices)
        {
            if (!canvas_region_compare(slice, canvas_, canvas_clone))
            {
                output += canvas_region_string(slice, canvas_);
            }
        }
        
        // Finally, deal with the cursor.
        bool cursor_state = content_->get_cursor_state();
        
        if (cursor_state)
        {
            point current_cursor_position = content_->get_cursor_position();
            
            // The cursor is enabled.  If there has been any output, or if
            // it has been moved, then it must be put in position.
            if (!output.empty() 
             || current_cursor_position != last_cursor_position_)
            {
                output += cursor_position(current_cursor_position);
                last_cursor_position_ = current_cursor_position;
            }
        }
        else
        {
            // There is no cursor.  Therefore, we drop it in the bottom right
            // corner, but only if there has been any output.
            if (!output.empty())
            {
                output += cursor_position(
                    point(size.width - 1, size.height - 1));
            }
        }
        
        // Finally, only if anything has been repainted, send the notification 
        // to any observers, telling them how it is done.
        if (!output.empty())
        {
            self_.on_repaint(output);
        }
        
        redraw_regions_.clear();
        repaint_scheduled_ = false;
    }

    window                       &self_;    
    boost::asio::io_service      &io_service_;
    boost::shared_ptr<container>  content_;
    ansi_canvas                   canvas_;
    
    point                         last_cursor_position_;
    bool                          last_cursor_state_;
    
    vector<rectangle>             redraw_regions_;
    bool                          repaint_scheduled_;
};
    
// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
window::window(boost::asio::io_service &io_service)
{
    pimpl_.reset(new impl(*this, io_service));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
window::~window()
{
}

// ==========================================================================
// GET_CONTENT
// ==========================================================================
shared_ptr<container> window::get_content()
{
    return pimpl_->get_content();
}

// ==========================================================================
// EVENT
// ==========================================================================
void window::event(any const &event)
{
    pimpl_->event(event);
}

}}

// ==========================================================================
// Munin Window.
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
#include "munin/window.hpp"
#include "munin/algorithm.hpp"
#include "munin/container.hpp"
#include "munin/basic_container.hpp"
#include "munin/canvas.hpp"
#include "munin/ansi/protocol.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace boost;
using namespace std;
using namespace odin;

namespace {

//* =========================================================================
/// \brief Returns true if the specified region is identical between the
/// two canvases, false otherwise.
//* =========================================================================
static bool canvas_region_compare(
    munin::rectangle const &region
  , munin::canvas          &lhs
  , munin::canvas          &rhs)
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
            munin::element_type const &lhs_element =
                lhs[column + region.origin.x][row + region.origin.y];
            munin::element_type const &rhs_element =
                rhs[column + region.origin.x][row + region.origin.y];
                
            compare_equal = lhs_element == rhs_element;
        }
    }
    
    return compare_equal;
}

//* =========================================================================
/// \brief Returns a string that is an ANSI sequence that will change the
/// output style from 'from' to 'to'.  Assigns the latter to the former.  
//* =========================================================================
static string change_attribute(munin::attribute &from, munin::attribute to)
{
    // Some terminal clients are particularly bad at handling colour changes
    // using the 'default', and leave the attribute as whatever it was last.  
    // We can work around this by using the 'reset attributes' key in advance.
    // However, this will also mean that we need to send all other attributes
    // afterwards.  We can do that by making the 'from' attribute default in
    // all cases.
    string default_sequence;
    
    if ((from.foreground_colour_ != to.foreground_colour_
      && to.foreground_colour_ == odin::ansi::graphics::COLOUR_DEFAULT)
     || (from.background_colour_ != to.background_colour_
      && to.background_colour_ == odin::ansi::graphics::COLOUR_DEFAULT))
    {
        default_sequence = string()
            + odin::ansi::ESCAPE
            + odin::ansi::CONTROL_SEQUENCE_INTRODUCER
            + str(format("%s") % int(odin::ansi::graphics::NO_ATTRIBUTES))
            + odin::ansi::SELECT_GRAPHICS_RENDITION;
            
        // Clear the attribute so that it can be rewritten entirely.
        from = munin::attribute();
    }

    string graphics_sequence;

    if (from.foreground_colour_ != to.foreground_colour_)
    {
        graphics_sequence += string()
                          + odin::ansi::ESCAPE
                          + odin::ansi::CONTROL_SEQUENCE_INTRODUCER;
                       
        graphics_sequence += str(format("%s")
            % int(odin::ansi::graphics::FOREGROUND_COLOUR_BASE
                + to.foreground_colour_));
        
        from.foreground_colour_ = to.foreground_colour_;
    }
    
    if (from.background_colour_ != to.background_colour_)
    {
        if (graphics_sequence.empty())
        {
            graphics_sequence += string()
                              + odin::ansi::ESCAPE
                              + odin::ansi::CONTROL_SEQUENCE_INTRODUCER;
        }
        else
        {
            graphics_sequence += odin::ansi::PARAMETER_SEPARATOR;
        }
        
        graphics_sequence += str(format("%s")
            % int(odin::ansi::graphics::BACKGROUND_COLOUR_BASE
                + to.background_colour_));
        
        from.background_colour_ = to.background_colour_;
    }
    
    if (from.intensity_ != to.intensity_)
    {
        if (graphics_sequence.empty())
        {
            graphics_sequence += string()
                              + odin::ansi::ESCAPE
                              + odin::ansi::CONTROL_SEQUENCE_INTRODUCER;
        }
        else
        {
            graphics_sequence += odin::ansi::PARAMETER_SEPARATOR;
        }
        
        graphics_sequence += str(format("%s") % int(to.intensity_));
        
        from.intensity_ = to.intensity_;
    }
    
    if (from.polarity_ != to.polarity_)
    {
        if (graphics_sequence.empty())
        {
            graphics_sequence += string()
                              + odin::ansi::ESCAPE
                              + odin::ansi::CONTROL_SEQUENCE_INTRODUCER;
        }
        else
        {
            graphics_sequence += odin::ansi::PARAMETER_SEPARATOR;
        }
        
        graphics_sequence += str(format("%s") % int(to.polarity_));
        
        from.polarity_ = to.polarity_;
    }

    if (from.underlining_ != to.underlining_)
    {
        if (graphics_sequence.empty())
        {
            graphics_sequence += string()
                              + odin::ansi::ESCAPE
                              + odin::ansi::CONTROL_SEQUENCE_INTRODUCER;
        }
        else
        {
            graphics_sequence += odin::ansi::PARAMETER_SEPARATOR;
        }
        
        graphics_sequence += str(format("%s") % int(to.underlining_));
        
        from.underlining_ = to.underlining_;
    }
    
    if (!graphics_sequence.empty())
    {
        graphics_sequence += odin::ansi::SELECT_GRAPHICS_RENDITION;
    }

    /*
    string charset_sequence;
    
    if (from.locale != to.locale || from.character_set != to.character_set)
    {
        charset_sequence += string()
                          + odin::ansi::ESCAPE
                          + to.character_set
                          + to.locale;
                          
        from.locale        = to.locale;
        from.character_set = to.character_set;
    }
    */
    return default_sequence + graphics_sequence;// + charset_sequence;
}

//* =========================================================================
/// \brief Returns a string that is an ANSI sequence of commands required
/// to draw the selected glyph.  Character set and locale are only output
/// if they have changed.
//* =========================================================================
static string write_glyph(
    munin::glyph &last_glyph
  , munin::glyph  current_glyph)
{
    string charset_sequence;
    
    if (last_glyph.locale_        != current_glyph.locale_
     || last_glyph.character_set_ != current_glyph.character_set_)
    {
        last_glyph.locale_        = current_glyph.locale_;
        last_glyph.character_set_ = current_glyph.character_set_;
        
        charset_sequence += string()
                          + odin::ansi::ESCAPE
                          + current_glyph.character_set_
                          + current_glyph.locale_;
                          
    }
    
    last_glyph.character_ = current_glyph.character_;
    
    return charset_sequence + current_glyph.character_;
}

//* =========================================================================
/// \brief Returns a string which would paint the specified region on a
/// canvas.
//* =========================================================================
static string canvas_region_string(
    munin::glyph           &last_glyph
  , munin::attribute       &last_attribute
  , munin::rectangle const &region
  , munin::canvas          &cvs)
{
    string output;
    
    for (s32 row = 0; row < region.size.height; ++row)
    {
        // Place the cursor at the start of this row.
        output += munin::ansi::cursor_position(
            munin::point(region.origin.x, region.origin.y + row));
        
        for (s32 column = 0; column < region.size.width; ++column)
        {
            munin::element_type const &element = 
                cvs[column + region.origin.x]
                   [row    + region.origin.y];

            // Apply any attribute changes that are required.
            if (!(last_attribute == element.attribute_))
            {
                output += change_attribute(last_attribute, element.attribute_);
            }
            
            output += write_glyph(last_glyph, element.glyph_);
        }
    }
    
    return output;
}

}

namespace munin {

// ==========================================================================
// WINDOW IMPLEMENTATION STRUCTURE
// ==========================================================================
class window::impl
    : public enable_shared_from_this<window::impl>
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(window &self, boost::asio::io_service &io_service)
        : self_(self)
        , self_valid_(true)
        , io_service_(io_service)
        , content_(make_shared<basic_container>())
        , last_cursor_position_(point(0,0))
        , last_cursor_state_(false)
        , repaint_scheduled_(false)
    {
        connections_.push_back(content_->on_redraw.connect(
            bind(&impl::redraw_handler, this, _1)));
        
        connections_.push_back(content_->on_cursor_state_changed.connect(
            bind(&impl::schedule_repaint, this)));
        
        connections_.push_back(content_->on_cursor_position_changed.connect(
            bind(&impl::schedule_repaint, this)));
    }
    
    // ======================================================================
    // DESTRUCTOR
    // ======================================================================
    ~impl()
    {
        BOOST_FOREACH(boost::signals::connection& cnx, connections_)
        {
            cnx.disconnect();
        }
    }
    
    // ======================================================================
    // INVALIDATE_SELF
    // ======================================================================
    void invalidate_self()
    {
        self_valid_ = false;
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
        // Once a repaint has been scheduled, there is no need to schedule
        // any more until the repaint request has been fulfilled.  Ignore
        // any further repaint requests.
        if (!repaint_scheduled_)
        {
            io_service_.post(bind(&impl::do_repaint, shared_from_this()));
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
    // CREATE_REPAINT_SLICES
    // ======================================================================
    vector<rectangle> create_repaint_slices()
    {
        // In this function, we take the redraw regions and work out what
        // portions of the screen need to be sent to the client.  In
        // addition, we try and optimise this so that the smallest amount
        // of data will be sent.
        
        // First, clip all of the regions so that they are bound by the size
        // of our content - no point in drawing anything that's off-screen.
        BOOST_AUTO(
            clipped_regions
          , clip_regions(redraw_regions_, content_->get_size()));
        
        // Next, prune out any regions with 0-sized dimensions, since they
        // don't exist.
        BOOST_AUTO(pruned_regions, prune_regions(clipped_regions));
        
        // Next, redistribute the regions into slices that have a height
        // of only one.
        BOOST_AUTO(slices, rectangular_slice(pruned_regions));
        
        return slices;
    }
    
    // ======================================================================
    // DO_REPAINT
    // ======================================================================
    void do_repaint()
    {
        BOOST_AUTO(size, content_->get_size());

        // If the canvas has changed size, then many things can happen.
        // If it's shrunk, then there's no way to tell if the client has
        // clipped or scrolled or whatever.  If it's grown, then the new
        // regions of the screen may contain junk and need to be overwritten.
        // Therefore, we forego detection of whether a region is similar
        // to what it used to be and instead just repaint everything.
        bool repaint_all = canvas_.get_size().width  != size.width
                        || canvas_.get_size().height != size.height;
        
        // Ensure that our canvas is the correct size for the content that we
        // are going to paint.
        canvas_.set_size(size);

        // Create the slices that we aim to repaint.
        // Since we are going to repaint as little as possible, we first
        // clip all the redraw regions to the size of the content, prune out
        // any regions whose dimensions are zero, then cut those regions
        // into horizontal slices.
        BOOST_AUTO(slices, create_repaint_slices());
        
        // Take a copy of the canvas.  We will want to check against this
        // after the draw operations to see if anything has changed.
        canvas canvas_clone = canvas_;

        // Draw each slice on the canvas.
        BOOST_FOREACH(rectangle region, slices)
        {
            content_->draw(canvas_, region);
        }
        
        // Prepare a string that is a collection of the ANSI data required
        // to update the window.         
        string output;
        
        // For each slice, see if it has changed between the canvas that was
        // updated and the clone of the original.
        BOOST_FOREACH(rectangle slice, slices)
        {
            if (repaint_all
             || !canvas_region_compare(slice, canvas_, canvas_clone))
            {
                // This would only output the differences in the two regions
                output += canvas_region_string(
                    glyph_, attribute_, slice, canvas_);
            }
        }

        // Finally, deal with the cursor.
        bool cursor_state = content_->get_cursor_state();
        
        if (cursor_state != last_cursor_state_)
        {
            output += cursor_state
                    ? munin::ansi::show_cursor()
                    : munin::ansi::hide_cursor();
        }
        
        if (cursor_state)
        {
            point current_cursor_position = content_->get_cursor_position();
            
            // The cursor is enabled.  If there has been any output, or if
            // it has been moved, or the cursor has just been turned on, then
            // it must be put in position.
            if (!output.empty() 
             || current_cursor_position != last_cursor_position_
             || cursor_state != last_cursor_state_)
            {
                output += munin::ansi::cursor_position(current_cursor_position);
                last_cursor_position_ = current_cursor_position;
            }
        }
        else
        {
            // There is no cursor.  Therefore, we drop it in the bottom right
            // corner, but only if there has been any output.
            if (!output.empty())
            {
                output += munin::ansi::cursor_position(
                    point(size.width - 1, size.height - 1));
            }
        }
        
        last_cursor_state_ = cursor_state;
        
        // Finally, only if anything has been repainted, send the notification 
        // to any observers, telling them how it is done.
        if (!output.empty() && self_valid_)
        {
            self_.on_repaint(output);
        }
        
        redraw_regions_.clear();
        
        // We are once again interested in repaint requests.
        repaint_scheduled_ = false;
    }

    window                       &self_;
    bool                          self_valid_;
    
    boost::asio::io_service      &io_service_;
    boost::shared_ptr<container>  content_;
    canvas                        canvas_;
    glyph                         glyph_;
    attribute                     attribute_;
    
    point                         last_cursor_position_;
    bool                          last_cursor_state_;
    
    vector<rectangle>             redraw_regions_;
    bool                          repaint_scheduled_;
    
    vector<boost::signals::connection> connections_;
};
    
// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
window::window(boost::asio::io_service &io_service)
{
    pimpl_ = make_shared<impl>(ref(*this), ref(io_service));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
window::~window()
{
    pimpl_->invalidate_self();
}

// ==========================================================================
// SET_TITLE
// ==========================================================================
void window::set_title(string const &title)
{
    on_repaint(munin::ansi::set_window_title(title));
}

// ==========================================================================
// ENABLE_MOUSE_TRACKING
// ==========================================================================
void window::enable_mouse_tracking()
{
    on_repaint(munin::ansi::enable_mouse_tracking());
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

}

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
#include <boost/typeof/typeof.hpp>

using namespace boost;
using namespace std;
using namespace odin;

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
        , repaint_scheduled_(false)
    {
        content_->on_redraw.connect(
            bind(&impl::redraw_handler, this, _1));
    }
    
    // ======================================================================
    // GET_CONTENT
    // ======================================================================
    boost::shared_ptr<container> get_content()
    {
        return content_;
    }

private :
    // ======================================================================
    // REDRAW_HANDLER
    // ======================================================================
    void redraw_handler(vector<rectangle> const &regions)
    {
        // Coalesce all redraw events into a single repaint.  That way,
        // there is only one major repaint if 100 components decide they
        // need drawing at once.
        copy(
            regions.begin()
          , regions.end()
          , back_inserter(redraw_regions_));
        
        if (!repaint_scheduled_)
        {
            io_service_.post(bind(&impl::do_repaint, this));
            repaint_scheduled_ = true;
        }
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
        
        if (!pruned_regions.empty())
        {
            extent size = content_->get_size();
            canvas_.set_size(size);
            
            point origin;
            origin.x = 0;
            origin.y = 0;
            
            // Take a copy of the canvas.  We will want to check against this
            // after the draw operations to see if anything has changed.
            ansi_canvas canvas_clone = canvas_;
            
            BOOST_FOREACH(rectangle const &region, pruned_regions)
            {
                content_->draw(canvas_, origin, region);
            }
            
            if (!(canvas_ == canvas_clone))
            {
                vector<char> output;
                output.push_back(ESCAPE);
                output.push_back(ANSI_SEQUENCE);
                output.push_back(SAVE_CURSOR_POSITION);
                
                for (u32 column = 0; column < size.width; ++column)
                {
                    BOOST_AUTO(current_column, canvas_[column]);
                    
                    for (u32 row = 0; row < size.height; ++row)
                    {
                         element_type const &element = current_column[row];
                         output.push_back(element.first);
                    }
                }

                output.push_back(ESCAPE);
                output.push_back(ANSI_SEQUENCE);
                output.push_back(RESTORE_CURSOR_POSITION);
    
                self_.on_repaint(output);
            }
        }
        
        redraw_regions_.clear();
        repaint_scheduled_ = false;
    }

    window                       &self_;    
    boost::asio::io_service      &io_service_;
    boost::shared_ptr<container>  content_;
    ansi_canvas                   canvas_;
    
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

}}

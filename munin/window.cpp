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
#include "munin/context.hpp"
#include <terminalpp/terminalpp.hpp>
#include <boost/format.hpp>
#include <iostream>

namespace munin {
    
namespace {

// ==========================================================================
// UNPACKAGE_VISITOR
// ==========================================================================
struct unpackage_visitor : boost::static_visitor<boost::any>
{
    // Simply package up any input into a Boost.Any and return it.
    template <class Packaged>
    boost::any operator()(Packaged &&pack)
    {
        std::cout << "EVENT: " << pack << std::endl;
        return pack;
    }
};

}

// ==========================================================================
// WINDOW IMPLEMENTATION STRUCTURE
// ==========================================================================
class window::impl
    : public std::enable_shared_from_this<window::impl>
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(
        window                  &self
      , boost::asio::strand     &strand)
        : self_(self)
        , self_valid_(true)
        , strand_(strand)
        , content_(std::make_shared<basic_container>())
        , canvas_({80, 24})
        , screen_()
        , last_cursor_position_({})
        , last_cursor_state_(false)
        , last_window_size_({0, 0})
        , repaint_scheduled_(false)
        , layout_scheduled_(false)
        , handling_newline_(false)
        , newline_char_('\0')
    {
        connections_.push_back(content_->on_redraw.connect(
            [this](auto const &regions){redraw_handler(regions);}));

        connections_.push_back(content_->on_cursor_state_changed.connect(
            [this](auto const &){schedule_repaint();}));

        connections_.push_back(content_->on_cursor_position_changed.connect(
            [this](auto const &){schedule_repaint();}));

        connections_.push_back(content_->on_preferred_size_changed.connect(
            [this]{preferred_size_change_handler();}));

        connections_.push_back(content_->on_layout_change.connect(
            [this]{schedule_layout();}));
    }

    // ======================================================================
    // DESTRUCTOR
    // ======================================================================
    ~impl()
    {
        for (auto &cnx : connections_)
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
    // SET_TITLE
    // ======================================================================
    void set_title(std::string const &title)
    {
        self_.on_repaint(terminal_.set_window_title(title));
    }

    // ======================================================================
    // ENABLE_MOUSE_TRACKING
    // ======================================================================
    void enable_mouse_tracking()
    {
        self_.on_repaint(terminal_.enable_mouse());
    }

    // ======================================================================
    // GET_CONTENT
    // ======================================================================
    std::shared_ptr<container> get_content()
    {
        return content_;
    }

    // ======================================================================
    // GET_CONTENT
    // ======================================================================
    std::shared_ptr<container const> get_content() const
    {
        return content_;
    }

    // ======================================================================
    // DATA
    // ======================================================================
    void data(std::string const &data)
    {
        static unpackage_visitor visitor;

        auto tokens = terminal_.read(data);
        
        for (auto const &token : tokens)
        {
            content_->event(boost::apply_visitor(visitor, token));
        }
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
            strand_.post([sp=shared_from_this()]{sp->do_repaint();});
            repaint_scheduled_ = true;
        }
    }

    // ======================================================================
    // SCHEDULE_LAYOUT
    // ======================================================================
    void schedule_layout()
    {
        // As with schedule_repaint, we don't need to schedule a layout
        // if there's one already scheduled.
        if (!layout_scheduled_)
        {
            strand_.post([sp=shared_from_this()]{sp->do_layout();});
            layout_scheduled_ = true;
        }
    }

    // ======================================================================
    // REDRAW_HANDLER
    // ======================================================================
    void redraw_handler(std::vector<rectangle> const &regions)
    {
        // Coalesce all redraw events into a single repaint.  That way,
        // there is only one major repaint if 100 components decide they
        // need drawing at once.
        redraw_regions_.insert(
            redraw_regions_.end()
          , regions.begin()
          , regions.end());
        schedule_repaint();
    }

    // ======================================================================
    // PREFERRED_SIZE_CHANGE_HANDLER
    // ======================================================================
    void preferred_size_change_handler()
    {
        // Force the content to re-lay itself out.
        content_->set_size(content_->get_size());
    }

    // ======================================================================
    // CREATE_REPAINT_SLICES
    // ======================================================================
    std::vector<rectangle> create_repaint_slices()
    {
        // In this function, we take the redraw regions and work out what
        // portions of the screen need to be sent to the client.  In
        // addition, we try and optimise this so that the smallest amount
        // of data will be sent.

        // First, clip all of the regions so that they are bound by the size
        // of our content - no point in drawing anything that's off-screen.
        // Next, prune out any regions with 0-sized dimensions, since they
        // don't exist.
        // Finally, redistribute the regions into slices that have a height
        // of only one.
        return rectangular_slice(
            prune_regions(
                clip_regions(redraw_regions_, content_->get_size())));
    }

    // ======================================================================
    // DO_REPAINT
    // ======================================================================
    void do_repaint()
    {
        auto size = content_->get_size();
        
        // If the canvas has changed size, then many things can happen.
        // If it's shrunk, then there's no way to tell if the client has
        // clipped or scrolled or whatever.  If it's grown, then the new
        // regions of the screen may contain junk and need to be overwritten.
        // Therefore, we forego detection of whether a region is similar
        // to what it used to be and instead just repaint everything.
        bool size_changed = size != last_window_size_;
        
        // Ensure that our canvas is the correct size for the content that we
        // are going to paint.
        if (size_changed)
        {
            this->redraw_regions_.push_back({{}, size});
            canvas_ = terminalpp::canvas(size);
        }

        // Create the slices that we aim to repaint.
        // Since we are going to repaint as little as possible, we first
        // clip all the redraw regions to the size of the content, prune out
        // any regions whose dimensions are zero, then cut those regions
        // into horizontal slices.
        auto slices = create_repaint_slices();

        context ctx(canvas_, strand_);
        
        // Draw each slice on the canvas.
        for (auto const &region : slices)
        {
            content_->draw(ctx, region);
        }
        
        // And draw the screen onto the terminal.
        self_.on_repaint(screen_.draw(terminal_, canvas_));
        
/*
        // Prepare a string that is a collection of the ANSI data required
        // to update the window.
        std::string output;

        // First, if we are repainting everything, then clear the screen to
        // ensure that nothing is left from the previous data.
        if (repaint_all)
        {
            output += munin::ansi::clear_screen();
        }

        // For each slice, see if it has changed between the canvas that was
        // updated and the clone of the original.
        for (auto const &slice : slices)
        {
            std::vector<rectangle> subslices;

            if (repaint_all)
            {
                subslices.push_back(slice);
            }
            else
            {
                subslices = find_different_subslices(
                    slice, canvas_, canvas_clone_);
            }

            for (auto const &subslice : subslices)
            {
                output += canvas_region_string(
                    glyph_, attribute_, subslice, canvas_);
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
            auto current_cursor_position = content_->get_cursor_position();

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
        */
    }

    // ======================================================================
    // DO_LAYOUT
    // ======================================================================
    void do_layout()
    {
        content_->layout();
        layout_scheduled_ = false;

        redraw_handler({
            rectangle(content_->get_position(), content_->get_size())});
    }

    window                       &self_;
    bool                          self_valid_;

    boost::asio::strand          &strand_;
    
    terminalpp::terminal          terminal_;
    std::shared_ptr<container>    content_;
    terminalpp::screen            screen_;
    terminalpp::canvas            canvas_;
    terminalpp::glyph             glyph_;
    terminalpp::attribute         attribute_;

    terminalpp::point             last_cursor_position_;
    bool                          last_cursor_state_;
    terminalpp::extent            last_window_size_;

    std::vector<rectangle>        redraw_regions_;
    bool                          repaint_scheduled_;
    bool                          layout_scheduled_;

    bool                          handling_newline_;
    char                          newline_char_;

    std::vector<boost::signals::connection> connections_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
window::window(boost::asio::strand &strand)
{
    pimpl_ = std::make_shared<impl>(std::ref(*this), std::ref(strand));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
window::~window()
{
    pimpl_->invalidate_self();
}

// ==========================================================================
// GET_SIZE
// ==========================================================================
terminalpp::extent window::get_size() const
{
    return pimpl_->get_content()->get_size();
}

// ==========================================================================
// SET_SIZE
// ==========================================================================
void window::set_size(terminalpp::extent size)
{
    get_content()->set_size(size);
    get_content()->on_layout_change();
}

// ==========================================================================
// SET_TITLE
// ==========================================================================
void window::set_title(std::string const &title)
{
    pimpl_->set_title(title);
}

// ==========================================================================
// ENABLE_MOUSE_TRACKING
// ==========================================================================
void window::enable_mouse_tracking()
{
    pimpl_->enable_mouse_tracking();
}

// ==========================================================================
// GET_CONTENT
// ==========================================================================
std::shared_ptr<container> window::get_content()
{
    return pimpl_->get_content();
}

// ==========================================================================
// EVENT
// ==========================================================================
void window::data(std::string const &data)
{
    pimpl_->data(data);
}

}


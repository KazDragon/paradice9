// ==========================================================================
// Munin ANSI Text Area Component.
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
#include "munin/ansi/text_area.hpp"
#include "munin/ansi/basic_container.hpp"
#include "munin/ansi/frame.hpp"
#include "munin/ansi/text/default_multiline_document.hpp"
#include "munin/algorithm.hpp"
#include "munin/canvas.hpp"
#include "odin/ansi/protocol.hpp"
#include "odin/ascii/protocol.hpp"
#include <boost/make_shared.hpp>
#include <cctype>
#include <vector>

using namespace odin;
using namespace boost;
using namespace std;

namespace munin { namespace ansi {

// ==========================================================================
// TEXT_AREA::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct text_area::impl
{
    impl(text_area &self)
        : self_(self)
        , document_(
              make_shared<munin::ansi::text::default_multiline_document>())
        , cursor_state_(true)
        , document_base_(0)
    {
        document_->on_redraw.connect(
            bind(&impl::on_document_changed, this, _1));
        
        document_->on_caret_position_changed.connect(
            bind(&impl::on_caret_position_changed, this));
    }

    //* =====================================================================
    /// \brief Retrieves the document
    //* =====================================================================
    shared_ptr< munin::text::document<element_type> > get_document()
    {
        return document_;
    }
    
    //* =====================================================================
    /// \brief Sets the size of the document
    //* =====================================================================
    void set_size(extent const &size)
    {
        document_->set_size(size);
    }
    
    //* =====================================================================
    /// \brief Gets the preferred size of the document.
    //* =====================================================================
    extent get_preferred_size() const
    {
        return document_->get_size();
    }
    
    //* =====================================================================
    /// \brief Returns the cursor state.
    //* =====================================================================
    bool get_cursor_state() const
    {
        return cursor_state_;
    }

    //* =====================================================================
    /// \brief Returns the cursor position.
    //* =====================================================================
    point get_cursor_position() const
    {
        return cursor_position_;
    }
    
    //* =====================================================================
    /// \brief Draws the document onto the canvas. 
    //* =====================================================================
    void draw(
        canvas<element_type> &cvs
      , point const          &offset
      , rectangle const      &region)
    {
        BOOST_AUTO(position, self_.get_position());

        for (u32 row_index = region.origin.y;
             row_index < u32(region.origin.y + region.size.height);
             ++row_index)
        {
            BOOST_AUTO(
                line
              , document_->get_text_line(document_base_ + row_index));
            
            u32 column_index = region.origin.x;
            
            // Write the characters that are required by the document.
            for (;
                 column_index < u32(region.origin.x + region.size.width)
              && column_index < u32(line.size());
                 ++column_index)
            {
                cvs[position.x + column_index + offset.x]
                   [position.y + row_index    + offset.y] = line[column_index];
            }
            
            // Pad the rest with blanks
            for (;
                column_index < u32(region.size.width);
                ++column_index)
            {
                cvs[position.x + column_index + offset.x]
                   [position.y + row_index    + offset.y] =
                    element_type(' ', attribute());
            }
        }
    }
    
    //* =====================================================================
    /// \brief Processes events.
    //* =====================================================================
    void do_event(any const &event)
    {
        char const *ch = any_cast<char>(&event);
        
        if (ch != NULL)
        {
            do_character_event(*ch);
        }
        
        odin::ansi::control_sequence const *sequence = 
            any_cast<odin::ansi::control_sequence>(&event);
            
        if (sequence != NULL)
        {
            do_ansi_control_sequence_event(*sequence);
        }
    }
    
private :
    //* =====================================================================
    /// \brief Called when the underlying document changes.
    //* =====================================================================
    void on_document_changed(vector<rectangle> regions)
    {
        render();
    }
    
    //* =====================================================================
    /// \brief Called when the position of the caret in the underlying 
    /// document changes.
    //* =====================================================================
    void on_caret_position_changed()
    {
        BOOST_AUTO(new_position, document_->get_caret_position());
        
        if (u32(new_position.y) < document_base_)
        {
            // The caret has gone off the top of our document base.  Rebase
            // ourselves and repaint.  Also set the cursor position to the
            // base.
            document_base_ = new_position.y;
            render();
            cursor_position_ = point(new_position.x, 0);
            self_.on_cursor_position_changed(cursor_position_);
        }
        else if (
            u32(new_position.y + 1) > document_base_ + self_.get_size().height)
        {
            // The caret has gone off the bottom of our component.  Rebase
            // ourselves to that the caret will be at the bottom, and
            // repaint.
            document_base_ = (new_position.y + 1) - self_.get_size().height;
            render();
            cursor_position_ = point(
                new_position.x
              , new_position.y - document_base_);
            self_.on_cursor_position_changed(cursor_position_);
        }
        else
        {
            // Simply set the cursor position.
            cursor_position_ = point(
                new_position.x
              , new_position.y - document_base_);
            self_.on_cursor_position_changed(cursor_position_);
        }
    }
    
    //* =====================================================================
    /// \brief Called when we want to render the document.
    //* =====================================================================
    void render()
    {
        rectangle redraw_region(point(), self_.get_size());
        vector<rectangle> redraw_regions;
        redraw_regions.push_back(redraw_region);
        self_.on_redraw(redraw_regions);
    }
    
    //* =====================================================================
    /// \brief Called by do_ansi_control_sequence_event when the up arrow
    /// key has been pressed.
    //* =====================================================================
    void do_cursor_up_key_event(u32 times)
    {
        BOOST_AUTO(position, document_->get_caret_position());
        
        if (times > u32(position.y))
        {
            times = position.y;
        }
        
        document_->set_caret_position(point(position.x, position.y - times));
    }
    
    //* =====================================================================
    /// \brief Called by do_ansi_control_sequence_event when the down arrow
    /// key has been pressed.
    //* =====================================================================
    void do_cursor_down_key_event(u32 times)
    {
        BOOST_AUTO(position, document_->get_caret_position());
        
        document_->set_caret_position(point(
            position.x
          , position.y + times));
    }
    
    //* =====================================================================
    /// \brief Called by do_ansi_control_sequence_event when the <- key
    /// has been pressed.
    //* =====================================================================
    void do_cursor_backward_key_event(u32 times)
    {
        BOOST_AUTO(index, document_->get_caret_index());
        BOOST_AUTO(new_index, (index < times ? 0 : index - times));
        
        document_->set_caret_index(new_index);
    }
    
    //* =====================================================================
    /// \brief Called by do_ansi_control_sequence_event when the -> key
    /// has been pressed.
    //* =====================================================================
    void do_cursor_forward_key_event(u32 times)
    {
        document_->set_caret_index(document_->get_caret_index() + times);
    }
    
    //* =====================================================================
    /// \brief Called by do_ansi_control_sequence_event when the PGUP key
    /// has been pressed.
    //* =====================================================================
    void do_pgup_key_event()
    {
        // PgUp - shift the document base one page up, then shift the
        // cursor one page up.  Then render.
        BOOST_AUTO(size, self_.get_size());
        
        if (document_base_ < u32(size.height))
        {
            document_base_ = 0;
        }
        else
        {
            document_base_ -= size.height;
        }
        
        BOOST_AUTO(caret_position, document_->get_caret_position());
        
        if (caret_position.y < size.height)
        {
            caret_position.y = 0;
        }
        else
        {
            caret_position.y -= size.height;
        }
        
        document_->set_caret_position(caret_position);
        
        render();
    }
    
    //* =====================================================================
    /// \brief Called by do_ansi_control_sequence_event when the PGDN key
    /// has been pressed.
    //* =====================================================================
    void do_pgdn_key_event()
    {
        // PgDn - shift the document base one page down, then shift the
        // cursor one page down.  Then render.
        BOOST_AUTO(size, self_.get_size());
        BOOST_AUTO(lines, document_->get_number_of_lines());
        
        // We don't want the document base to be more than one page before
        // the end.
        u32 maximum_base = lines <= u32(size.height)
                         ? 0
                         : lines - size.height;
                         
        document_base_ = 
            (min)(u32(document_base_ + size.height), maximum_base);
        
        BOOST_AUTO(caret_position, document_->get_caret_position());
        caret_position.y += size.height;
        
        document_->set_caret_position(caret_position);
        
        render();
    }
    
    //* =====================================================================
    /// \brief Called by do_ansi_control_sequence_event when the DEL key
    /// has been pressed.
    //* =====================================================================
    void do_del_key_event()
    {
        BOOST_AUTO(caret_index, document_->get_caret_index());
        document_->delete_text(
            make_pair(caret_index, caret_index + 1));
    }

    //* =====================================================================
    /// \brief Called by do_ansi_control_sequence_event when the META+HOME
    /// key combination has been pressed.
    //* =====================================================================
    void do_meta_home_key_event()
    {
        // The META+HOME key combination will move the caret to the beginning
        // of the document.
        // Events called from this action should ensure that the view and
        // the cursor position are correctly placed.
        document_->set_caret_index(0); 
    }
    
    //* =====================================================================
    /// \brief Called by do_ansi_control_sequence_event when the HOME key
    /// has been pressed.
    //* =====================================================================
    void do_home_key_event()
    {
        // The HOME key goes to the first character of the current line.
        BOOST_AUTO(position, document_->get_caret_position());
        document_->set_caret_position(point(0, position.y));
    }

    //* =====================================================================
    /// \brief Called by do_ansi_control_sequence_event when the META+END
    /// key combination has been pressed.
    //* =====================================================================
    void do_meta_end_key_event()
    {
        // The META+END key combination will move the caret to the end of the
        // document.
        // Events called from this action should ensure that the view and
        // the cursor position are correctly placed.
        BOOST_AUTO(document_size, document_->get_size());
        document_->set_caret_index(
            document_size.width
          * document_size.height);
    }
    
    //* =====================================================================
    /// \brief Called by do_ansi_control_sequence_event when the END key
    /// has been pressed.
    //* =====================================================================
    void do_end_key_event()
    {
        // The END key goes to the last character of the current line.
        BOOST_AUTO(position, document_->get_caret_position());
        BOOST_AUTO(document_size, document_->get_size());

        document_->set_caret_position(point(
            document_size.width - 1
          , position.y));
    }
    
    //* =====================================================================
    /// \brief Called by do_event when an ANSI control sequence has been
    /// received.
    //* =====================================================================
    void do_ansi_control_sequence_event(
        odin::ansi::control_sequence const &sequence)
    {
        if (sequence.initiator_ == odin::ansi::CONTROL_SEQUENCE_INTRODUCER)
        {
            // Check for the <- key
            if (sequence.command_ == odin::ansi::CURSOR_BACKWARD)
            {
                u32 times = sequence.arguments_.empty() 
                          ? 1 
                          : atoi(sequence.arguments_.c_str());
                          
                do_cursor_backward_key_event(times);
            }
            // Check for the -> key
            else if (sequence.command_ == odin::ansi::CURSOR_FORWARD)
            {
                u32 times = sequence.arguments_.empty() 
                          ? 1 
                          : atoi(sequence.arguments_.c_str());
                          
                do_cursor_forward_key_event(times);
            }
            // Check for the up arrow key
            else if (sequence.command_ == odin::ansi::CURSOR_UP)
            {
                u32 times = sequence.arguments_.empty()
                          ? 1
                          : atoi(sequence.arguments_.c_str());
                          
                do_cursor_up_key_event(times);
            }
            // Check for the up arrow key
            else if (sequence.command_ == odin::ansi::CURSOR_DOWN)
            {
                u32 times = sequence.arguments_.empty()
                          ? 1
                          : atoi(sequence.arguments_.c_str());
                          
                do_cursor_down_key_event(times);
            }
            else if (sequence.command_ == odin::ansi::PUTTY_EXTENSION)
            {
                // Check for the HOME key
                if (sequence.arguments_.size() == 1
                 && sequence.arguments_[0] == '1')
                {
                    if (sequence.meta_)
                    {
                        do_meta_home_key_event();
                    }
                    else
                    {
                        do_home_key_event();
                    }
                }
                // Check for the END key
                if (sequence.arguments_.size() == 1
                 && sequence.arguments_[0] == '4')
                {
                    if (sequence.meta_)
                    {
                        do_meta_end_key_event();
                    }
                    else
                    {
                        do_end_key_event();
                    }
                }
                // Check for the PGUP key
                if (sequence.arguments_.size() == 1
                 && sequence.arguments_[0] == '5')
                {
                    do_pgup_key_event();
                }
                // Check for the PGDN key
                if (sequence.arguments_.size() == 1
                 && sequence.arguments_[0] == '6')
                {
                    do_pgdn_key_event();
                }
                // Check for the DEL key
                if (sequence.arguments_.size() == 1
                 && sequence.arguments_[0] == '3')
                {
                    do_del_key_event();
                }
            }
        }
    }
    
    //* =====================================================================
    /// \brief Called by do_event when a character event has occurred.
    //* =====================================================================
    void do_character_event(char ch)
    {
        if (self_.is_enabled())
        {
            if (ch == odin::ascii::BS || ch == odin::ascii::DEL)
            {
                BOOST_AUTO(caret_index, document_->get_caret_index());
                
                if (caret_index != 0)
                {
                    document_->delete_text(
                        make_pair(caret_index - 1, caret_index));
                }
            }
            else if (isprint(ch) || ch == '\n')
            {
                element_type data[] = { make_pair(ch, attribute()) };
                document_->insert_text(data, optional<u32>());
            }
        }
    }
    
    text_area                                         &self_;
    shared_ptr< munin::text::document<element_type> >  document_;
    point                                              cursor_position_;
    bool                                               cursor_state_;
    u32                                                document_base_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
text_area::text_area()
{
    pimpl_.reset(new impl(*this));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
text_area::~text_area()
{
}

// ==========================================================================
// GET_DOCUMENT
// ==========================================================================
shared_ptr< munin::text::document<element_type> > text_area::get_document()
{
    return pimpl_->get_document();
}

// ==========================================================================
// DO_SET_SIZE
// ==========================================================================
void text_area::do_set_size(extent const &size)
{
    // On top of setting the size of this component, we also want to set
    // the size of the document so that it can be rendered at the new
    // size.
    basic_component<element_type>::do_set_size(size);
    pimpl_->set_size(size);
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
extent text_area::do_get_preferred_size() const
{
    return pimpl_->get_preferred_size();
}
    
// ==========================================================================
// DO_GET_CURSOR_STATE
// ==========================================================================
bool text_area::do_get_cursor_state() const
{
    return pimpl_->get_cursor_state();
}
    
// ==========================================================================
// DO_GET_CURSOR_POSITION
// ==========================================================================
point text_area::do_get_cursor_position() const
{
    return pimpl_->get_cursor_position();
}
    
// ==========================================================================
// DO_DRAW
// ==========================================================================
void text_area::do_draw(
    canvas<element_type> &cvs
  , point const          &offset
  , rectangle const      &region)
{
    pimpl_->draw(cvs, offset, region);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void text_area::do_event(any const &event)
{
    pimpl_->do_event(event);
}

}}

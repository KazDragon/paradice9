// ==========================================================================
// Munin Text Area Component.
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
#include "munin/text_area.hpp"
#include "munin/text/default_multiline_document.hpp"
#include "munin/algorithm.hpp"
#include "munin/canvas.hpp"
#include "odin/ansi/protocol.hpp"
#include "odin/ascii/protocol.hpp"
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>
#include <cctype>
#include <vector>

using namespace odin;
using namespace boost;
using namespace boost::assign;
using namespace std;

namespace munin {

// ==========================================================================
// TEXT_AREA::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct text_area::impl
{
    impl(text_area &self)
        : self_(self)
        , document_(
              make_shared<munin::text::default_multiline_document>())
        , cursor_state_(true)
    {
        document_->on_redraw.connect(
            bind(&impl::on_document_changed, this, _1));
        
        document_->on_caret_position_changed.connect(
            bind(&impl::on_caret_position_changed, this));
    }

    //* =====================================================================
    /// \brief Retrieves the document
    //* =====================================================================
    shared_ptr<munin::text::document> get_document()
    {
        return document_;
    }
    
    //* =====================================================================
    /// \brief Gets the size of the document.
    //* =====================================================================
    extent get_size() const
    {
        return document_->get_size();
    }

    //* =====================================================================
    /// \brief Sets the size of the document.
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
        return document_->get_caret_position();
    }

    //* =====================================================================
    /// \brief Sets the cursor position.
    //* =====================================================================
    void set_cursor_position(point const &position)
    {
        document_->set_caret_position(position);
    }

    //* =====================================================================
    /// \brief Draws the document onto the canvas. 
    //* =====================================================================
    void draw(
        canvas          &cvs
      , rectangle const &region)
    {
        for (u32 row_index = region.origin.y;
             row_index < u32(region.origin.y + region.size.height);
             ++row_index)
        {
            BOOST_AUTO(
                line
              , document_->get_line(row_index));
            
            u32 column_index = region.origin.x;
            
            // Write the characters that are required by the document.
            for (;
                 column_index < u32(region.origin.x + region.size.width)
              && column_index < u32(line.size());
                 ++column_index)
            {
                cvs[column_index][row_index] = line[column_index];
            }
            
            // Pad the rest with blanks
            static element_type const default_element(' ');
            for (;
                column_index < u32(region.size.width);
                ++column_index)
            {
                cvs[column_index][row_index] = default_element;
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

        odin::ansi::mouse_report const *report =
            boost::any_cast<odin::ansi::mouse_report>(&event);
        
        if (report != NULL)
        {
            do_ansi_mouse_report_event(*report);
        }
    }
    
private :
    //* =====================================================================
    /// \brief Called when the underlying document changes.
    //* =====================================================================
    void on_document_changed(vector<rectangle> regions)
    {
        self_.on_preferred_size_changed();
        repaint();
    }
    
    //* =====================================================================
    /// \brief Called when the position of the caret in the underlying 
    /// document changes.
    //* =====================================================================
    void on_caret_position_changed()
    {
        self_.on_cursor_position_changed(document_->get_caret_position());
    }
    
    //* =====================================================================
    /// \brief Called when we want to repaint the document.
    //* =====================================================================
    void repaint()
    {
        self_.on_redraw(list_of(rectangle(point(), self_.get_size())));
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
    /// \brief Called by do_ansi_control_sequence_event when the DEL key
    /// has been pressed.
    //* =====================================================================
    void do_del_key_event()
    {
        if (self_.is_enabled())
        {
            BOOST_AUTO(caret_index, document_->get_caret_index());
            document_->delete_text(
                make_pair(caret_index, caret_index + 1));
        }
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
            document_size.width * document_size.height);
    }
    
    //* =====================================================================
    /// \brief Called by do_ansi_control_sequence_event when the END key
    /// has been pressed.
    //* =====================================================================
    void do_end_key_event()
    {
        // The END key goes to the last character of the current line.
        BOOST_AUTO(position, document_->get_caret_position());
        document_->set_caret_position(point(
            document_->get_size().width - 1
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
            // Check for the down arrow key
            else if (sequence.command_ == odin::ansi::CURSOR_DOWN)
            {
                u32 times = sequence.arguments_.empty()
                          ? 1
                          : atoi(sequence.arguments_.c_str());
                          
                do_cursor_down_key_event(times);
            }
            else if (sequence.command_ == odin::ansi::KEYPAD_FUNCTION)
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
                // Check for the DEL key
                if (sequence.arguments_.size() == 1
                 && sequence.arguments_[0] == '3')
                {
                    do_del_key_event();
                }
            }
        }
        else if (sequence.initiator_ == odin::ansi::SINGLE_SHIFT_SELECT_G3)
        {
            // Check for the alternative HOME key
            if (sequence.command_ == odin::ansi::ss3::HOME)
            {
                do_home_key_event();
            }
            // Check for the alternative END key
            if (sequence.command_ == odin::ansi::ss3::END)
            {
                do_end_key_event();
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
                document_->insert_text(list_of(element_type(ch)));
            }
        }
    }
    
    //* =====================================================================
    /// \brief Called by do_event when an ANSI mouse report has been
    /// received.
    //* =====================================================================
    void do_ansi_mouse_report_event(odin::ansi::mouse_report const &report)
    {
        if (report.button_ == 0)
        {
            if (self_.can_focus())
            {
                if (!self_.has_focus())
                {
                    self_.set_focus();
                }
                
                self_.set_cursor_position(point(
                    report.x_position_
                  , report.y_position_));
            }
        }
    }
    
    text_area                         &self_;
    shared_ptr<munin::text::document>  document_;
    bool                               cursor_state_;
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
shared_ptr<munin::text::document> text_area::get_document()
{
    return pimpl_->get_document();
}

// ==========================================================================
// DO_GET_SIZE
// ==========================================================================
extent text_area::do_get_size() const
{
    return pimpl_->get_size();
}

// ==========================================================================
// DO_SET_SIZE
// ==========================================================================
void text_area::do_set_size(extent const &size)
{
    // The size is a property owned by the document, not this area.
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
// DO_SET_CURSOR_POSITION
// ==========================================================================
void text_area::do_set_cursor_position(point const &position)
{
    pimpl_->set_cursor_position(position);
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void text_area::do_draw(
    canvas          &cvs
  , rectangle const &region)
{
    pimpl_->draw(cvs, region);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void text_area::do_event(any const &event)
{
    pimpl_->do_event(event);
}

}


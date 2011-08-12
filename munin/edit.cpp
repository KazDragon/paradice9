// ==========================================================================
// Munin Edit Component.
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
#include "munin/edit.hpp"
#include "munin/text/default_singleline_document.hpp"
#include "munin/algorithm.hpp"
#include "munin/canvas.hpp"
#include "odin/ansi/protocol.hpp"
#include "odin/ascii/protocol.hpp"
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>
#include <algorithm>
#include <cctype>
#include <vector>

using namespace odin;
using namespace boost;
using namespace boost::assign;
using namespace std;

namespace munin {

// ==========================================================================
// EDIT::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct edit::impl
{
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    impl(edit &self)
        : self_(self)
        , document_(
              make_shared<munin::text::default_singleline_document>())
        , cursor_state_(true)
        , document_base_(0)
    {
        document_->on_redraw.connect(
            bind(&impl::on_document_changed, this, _1));
        
        document_->on_caret_position_changed.connect(
            bind(&impl::on_caret_position_changed, this));
    }

    //* =====================================================================
    /// \brief Returns the document associated with this component.
    //* =====================================================================
    shared_ptr<munin::text::document> get_document()
    {
        return document_;
    }
    
    //* =====================================================================
    /// \brief Returns the preferred size of this component.
    //* =====================================================================
    extent get_preferred_size() const
    {
        return document_->get_size();
    }
    
    //* =====================================================================
    /// \brief Returns the cursor state
    //* =====================================================================
    bool get_cursor_state() const
    {
        return cursor_state_;
    }

    //* =====================================================================
    /// \brief Returns the cursor position
    //* =====================================================================
    point get_cursor_position() const
    {
        return cursor_position_;
    }
    
    //* =====================================================================
    /// \brief Draws the component
    //* =====================================================================
    void draw(
        canvas          &cvs
      , rectangle const &region)
    {
        BOOST_AUTO(document_size, document_->get_size());
        BOOST_AUTO(characters, document_size.width - document_base_);
        BOOST_AUTO(text, document_->get_line(0));
        
        s32 index = 0;

        // Write whatever characters are required.
        if (password_element_.is_initialized())
        {
            BOOST_AUTO(element, password_element_.get());
            
            for (;
                 (index + 1) < region.size.width
              && u32(region.origin.x + index) < characters;
                 ++index)
            {
                cvs[index][0] = element;
            }
        }
        else
        {
            for (;
                 (index + 1) < region.size.width
              && u32(region.origin.x + index) < characters;
                 ++index)
            {
                cvs[index][0] = text[document_base_ + index];
            }
        }
        
        // Pad the rest with blanks.
        static element_type const blank_element(' ');
        for (;
             index < region.size.width;
             ++index)
        {
            cvs[index][0] = blank_element;
        }
    }

    //* =====================================================================
    /// \brief Called by event().  Derived classes must override this 
    /// function in order to handle events in a custom manner.
    //* =====================================================================
    void do_event(boost::any const &event)
    {
        char const *ch = boost::any_cast<char>(&event);
        
        if (ch != NULL)
        {
            do_character_event(*ch);
        }
        
        odin::ansi::control_sequence const *sequence = 
            boost::any_cast<odin::ansi::control_sequence>(&event);
            
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

    //* =====================================================================
    /// \brief Sets the edit field to password mode, where every character
    /// is replaced by the specified element.
    //* =====================================================================
    void set_password_element(element_type const &element)
    {
        password_element_ = element;
        
        // Changing the password element will completely change how this
        // component is drawn.  Therefore, we will need to redraw the
        // entire component.
        redraw();
    }
    
private :
    //* =====================================================================
    /// \brief Called when the underlying document changes.
    //* =====================================================================
    void on_document_changed(vector<rectangle> regions)
    {
        // Search the regions, looking for regions that overlap with the
        // text that we are displaying.
        vector<rectangle> overlapping_changes;
        
        // Determine the rectangle of the document that we are displaying.
        rectangle display_rectangle(
            point(document_base_, 0)
          , extent(self_.get_size().width, 1));
        
        BOOST_FOREACH(rectangle region, regions)
        {
            optional<rectangle> overlap = intersection(
                region, display_rectangle);
            
            if (overlap)
            {
                overlapping_changes.push_back(overlap.get());
            }
        }

        // If we have any overlapping regions, then we need to redraw those
        // portions of the document.
        
        // First naive implementation: redraw the entire component.
        if (!overlapping_changes.empty())
        {
            redraw();
        }
    }

    //* =====================================================================
    /// \brief Called when the position of the caret in the underlying 
    /// document changes.
    //* =====================================================================
    void on_caret_position_changed()
    {
        BOOST_AUTO(new_index, document_->get_caret_index());
        
        if  (new_index < document_base_)
        {
            // The caret has gone to the left of the document base.  Rebase
            // ourselves, and redraw.  Also set the cursor position to
            // the base.
            document_base_ = new_index;
            redraw();
            cursor_position_ = point();
            self_.on_cursor_position_changed(cursor_position_);
        }
        else if (new_index + 1 > (document_base_ + self_.get_size().width))
        {
            // The caret has gone beyond the right of the component.  Rebase
            // ourselves so that the caret will be rightmost, and redraw.
            // Also set the cursor position to the rightmost.
            document_base_ = (new_index + 1) - self_.get_size().width;
            redraw();
            cursor_position_ = point(self_.get_size().width - 1, 0);
            self_.on_cursor_position_changed(cursor_position_);
        }
        else
        {
            // Simply set the caret position.
            cursor_position_ = point(new_index - document_base_, 0);
            self_.on_cursor_position_changed(cursor_position_);
        }
    }

    //* =====================================================================
    /// \brief Called when we want to redraw the document.
    //* =====================================================================
    void redraw()
    {
        self_.on_redraw(list_of(rectangle(point(), self_.get_size())));
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
    /// \brief Called by do_ansi_control_sequence_event when the HOME key
    /// has been pressed.
    //* =====================================================================
    void do_home_key_event()
    {
        // The HOME key will move the caret to the beginning of the document.
        // Events called from this action should ensure that the view and
        // the cursor position are correctly placed.
        document_->set_caret_index(0); 
    }

    //* =====================================================================
    /// \brief Called by do_ansi_control_sequence_event when the END key
    /// has been pressed.
    //* =====================================================================
    void do_end_key_event()
    {
        // The END key will move the caret to the end of the document.
        // Events called from this action should ensure that the view and
        // the cursor position are correctly placed.
        BOOST_AUTO(document_size, document_->get_size());
        document_->set_caret_index(document_size.width * document_size.height);
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
            else if (sequence.command_ == odin::ansi::KEYPAD_FUNCTION)
            {
                // Check for the HOME key
                if (sequence.arguments_.size() == 1
                 && sequence.arguments_[0] == '1')
                {
                    do_home_key_event();
                }
                // Check for the END key
                if (sequence.arguments_.size() == 1
                 && sequence.arguments_[0] == '4')
                {
                    do_end_key_event();
                }
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
                
                self_.get_document()->set_caret_position(point(
                    report.x_position_
                  , report.y_position_));
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
            else if (isprint(ch))
            {
                element_type data[] = { element_type(ch, attribute()) };
                document_->insert_text(data, optional<u32>());
            }
        }
    }
    
    edit                              &self_;
    shared_ptr<munin::text::document>  document_;
    point                              cursor_position_;
    bool                               cursor_state_;
    
    u32                                document_base_;
    
    boost::optional<element_type>      password_element_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
edit::edit()
{
    pimpl_ = make_shared<impl>(ref(*this));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
edit::~edit()
{
}

// ==========================================================================
// GET_DOCUMENT
// ==========================================================================
shared_ptr<munin::text::document> edit::get_document()
{
    return pimpl_->get_document();
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
extent edit::do_get_preferred_size() const
{
    return pimpl_->get_preferred_size();
}

// ==========================================================================
// DO_GET_CURSOR_STATE
// ==========================================================================
bool edit::do_get_cursor_state() const
{
    return pimpl_->get_cursor_state();
}


// ==========================================================================
// DO_GET_CURSOR_POSITION
// ==========================================================================
point edit::do_get_cursor_position() const
{
    return pimpl_->get_cursor_position();
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void edit::do_draw(
    canvas          &cvs
  , rectangle const &region)
{
    pimpl_->draw(cvs, region);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void edit::do_event(any const &event)
{
    pimpl_->do_event(event);
}

// ==========================================================================
// DO_SET_ATTRIBUTE
// ==========================================================================
void edit::do_set_attribute(string const &name, any const &attr)
{
    if (name == EDIT_PASSWORD_ELEMENT)
    {
        element_type const *element = any_cast<element_type>(&attr);
        
        if (element != NULL)
        {
            pimpl_->set_password_element(*element);
        }
    }
}

}


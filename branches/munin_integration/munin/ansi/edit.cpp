// ==========================================================================
// Munin ANSI Edit Component.
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
#include "munin/ansi/edit.hpp"
#include "munin/ansi/text/default_singleline_document.hpp"
#include "munin/algorithm.hpp"
#include "munin/basic_layout.hpp"
#include "munin/canvas.hpp"
#include "odin/ansi/protocol.hpp"
#include "odin/ascii/protocol.hpp"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>
#include <algorithm>
#include <cctype>
#include <vector>

using namespace odin;
using namespace boost;
using namespace std;

namespace munin { namespace ansi {

namespace {
    typedef munin::ansi::element_type             character_type;
    typedef munin::text::document<character_type> document_type;
}

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
              make_shared<munin::ansi::text::default_singleline_document>())
        , cursor_state_(true)
        , document_base_(0)
    {
        document_->on_redraw.connect(
            bind(&impl::on_document_changed, this, _1));
        
        document_->on_caret_position_changed.connect(
            bind(&impl::on_caret_position_changed, this));
    }
    
    //* =====================================================================
    /// \brief Returns the preferred size of this component.
    //* =====================================================================
    munin::extent get_preferred_size() const
    {
        return munin::extent(document_->get_width(), 1);
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
    munin::point get_cursor_position() const
    {
        return cursor_position_;
    }
    
    //* =====================================================================
    /// \brief Draws the component
    //* =====================================================================
    void draw(
        canvas<element_type> &cvs
      , point const          &offset
      , rectangle const      &region)
    {
        munin::point position = self_.get_position();
    
        // TODO: Fix this so that only a certain number of password field
        // characters are shown in password mode.
        BOOST_AUTO(characters, 
            (min)(u32(document_view_.size()), u32(document_->get_width())));
        
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
                cvs[position.x + index + offset.x]
                   [position.y + 0     + offset.y] = element;
            }
        }
        else
        {
            for (;
                 (index + 1) < region.size.width
              && u32(region.origin.x + index) < characters;
                 ++index)
            {
                cvs[position.x + index + offset.x]
                   [position.y + 0     + offset.y] = document_view_[index];
            }
        }
        
        // Pad the rest with blanks.
        for (;
             index < region.size.width;
             ++index)
        {
            cvs[position.x + index + offset.x]
               [position.y + 0     + offset.y] =
                   munin::ansi::element_type(
                       ' '
                     , munin::ansi::attribute());
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
    }

    //* =====================================================================
    /// \brief Sets the edit field to password mode, where every character
    /// is replaced by the specified element.
    //* =====================================================================
    void set_password_element(element_type const &element)
    {
        password_element_ = element;
        
        // Changing the password element will completely change how this
        // component is rendered.  Therefore, we will need to redraw the
        // entire component.
        vector<rectangle> regions;
        regions.push_back(rectangle(point(), self_.get_size()));
        self_.on_redraw(regions);
    }
    
private :
    //* =====================================================================
    /// \brief Called when the underlying document changes.
    //* =====================================================================
    void on_document_changed(vector<munin::rectangle> regions)
    {
        // Search the regions, looking for regions that overlap with the
        // text that we are displaying.
        vector<munin::rectangle> overlapping_changes;
        
        // Determine the rectangle of the document that we are displaying.
        munin::rectangle display_rectangle(
            munin::point(document_base_, 0)
          , munin::extent(self_.get_size().width, 1));
        
        BOOST_FOREACH(munin::rectangle rectangle, regions)
        {
            optional<munin::rectangle> overlap = munin::intersection(
                rectangle, display_rectangle);
            
            if (overlap)
            {
                overlapping_changes.push_back(overlap.get());
            }
        }

        // If we have any overlapping regions, then we need to redraw those
        // portions of the document, and also schedule redraws for this
        // renderer.
        
        // First naive implementation: redraw the entire component.
        if (!overlapping_changes.empty())
        {
            render();
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
            // ourselves, and repaint.  Also set the cursor position to
            // the base.
            document_base_ = new_index;
            render();
            cursor_position_ = munin::point(0, 0);
            self_.on_cursor_position_changed(cursor_position_);
        }
        else if (new_index + 1 > (document_base_ + self_.get_size().width))
        {
            // The caret has gone beyond the right of the component.  Rebase
            // ourselves so that the caret will be rightmost, and repaint.
            // Also set the cursor position to the rightmost.
            document_base_ = (new_index + 1) - self_.get_size().width;
            render();
            cursor_position_ = munin::point(self_.get_size().width - 1, 0);
            self_.on_cursor_position_changed(cursor_position_);
        }
        else
        {
            // Simply set the caret position.
            cursor_position_ = munin::point(new_index - document_base_, 0);
            self_.on_cursor_position_changed(cursor_position_);
        }
    }

    //* =====================================================================
    /// \brief Called when we want to render the document.
    //* =====================================================================
    void render()
    {
        document_view_.resize(self_.get_size().width);
        fill(document_view_.begin()
           , document_view_.end()
           , munin::ansi::element_type(' ', munin::ansi::attribute()));
        
        // Find the segment of the document that we will display
        odin::runtime_array<document_type::character_type> line =
            document_->get_text_line(0);
        
        BOOST_AUTO(
            start_index
          , (min)(u32(line.size()), document_base_));
        
        BOOST_AUTO(
            end_index
          , (min)(u32(line.size()), u32(document_base_ + document_view_.size())));
        
        copy(
            line.begin() + start_index
          , line.begin() + end_index
          , document_view_.begin());
        
        munin::rectangle redraw_region(
            munin::point()
          , self_.get_size());
        vector<munin::rectangle> redraw_regions;
        redraw_regions.push_back(redraw_region);
        self_.on_redraw(redraw_regions);
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
        document_->set_caret_index(
            document_->get_width() 
          * document_->get_height());
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
            else if (sequence.command_ == odin::ansi::PUTTY_EXTENSION)
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
                munin::ansi::element_type data[] = {
                    make_pair(ch, munin::ansi::attribute())
                };
            
                document_->insert_text(data, optional<u32>());
            }
        }
    }
    
    edit                                              &self_;
    shared_ptr< munin::text::document<element_type> >  document_;
    munin::point                                       cursor_position_;
    bool                                               cursor_state_;
    
    u32                                                document_base_;
    vector<element_type>                               document_view_;
    
    boost::optional<element_type>                      password_element_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
edit::edit()
{
    pimpl_.reset(new impl(*this));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
edit::~edit()
{
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
munin::extent edit::do_get_preferred_size() const
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
munin::point edit::do_get_cursor_position() const
{
    return pimpl_->get_cursor_position();
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void edit::do_draw(
    canvas<element_type> &cvs
  , point const          &offset
  , rectangle const      &region)
{
    pimpl_->draw(cvs, offset, region);
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

}}


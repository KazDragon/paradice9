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
#include "munin/context.hpp"
#include <terminalpp/canvas_view.hpp>
#include <terminalpp/string.hpp>
#include <terminalpp/virtual_key.hpp>
#include <algorithm>
#include <cctype>
#include <vector>

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
              std::make_shared<munin::text::default_singleline_document>())
        , cursor_state_(true)
        , document_base_(0)
    {
        document_->on_redraw.connect(
            [this](auto const &region)
            {
                this->on_document_changed(region);
            });

        document_->on_caret_position_changed.connect(
            [this]()
            {
                this->on_caret_position_changed();
            });
    }

    //* =====================================================================
    /// \brief Returns the document associated with this component.
    //* =====================================================================
    std::shared_ptr<munin::text::document> get_document()
    {
        return document_;
    }

    //* =====================================================================
    /// \brief Returns the preferred size of this component.
    //* =====================================================================
    terminalpp::extent get_preferred_size() const
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
    terminalpp::point get_cursor_position() const
    {
        return cursor_position_;
    }

    //* =====================================================================
    /// \brief Draws the component
    //* =====================================================================
    void draw(
        terminalpp::canvas_view &cvs
      , rectangle const &region)
    {
        auto document_size = document_->get_size();
        auto characters = document_size.width - document_base_;
        auto text = document_->get_line(0);

        odin::s32 index = 0;

        // Write whatever characters are required.
        if (password_element_.is_initialized())
        {
            auto element = password_element_.get();

            for (;
                 (index + 1) < region.size.width
              && odin::u32(region.origin.x + index) < characters;
                 ++index)
            {
                cvs[index][0] = element;
            }
        }
        else
        {
            for (;
                 (index + 1) < region.size.width
              && odin::u32(region.origin.x + index) < characters;
                 ++index)
            {
                cvs[index][0] = text[document_base_ + index];
            }
        }

        // Pad the rest with blanks.
        static terminalpp::element const blank_element(' ');
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
        auto const *vk = boost::any_cast<terminalpp::virtual_key>(&event);
        
        if (vk)
        {
            do_vk_event(*vk);
        }
        
        auto const *report = 
            boost::any_cast<terminalpp::ansi::mouse::report>(&event);
            
        if (report)
        {
            do_mouse_event(*report);
        }
        
    }

    //* =====================================================================
    /// \brief Sets the edit field to password mode, where every character
    /// is replaced by the specified element.
    //* =====================================================================
    void set_password_element(terminalpp::element const &element)
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
    void on_document_changed(std::vector<rectangle> const &regions)
    {
        // Search the regions, looking for regions that overlap with the
        // text that we are displaying.
        std::vector<rectangle> overlapping_changes;

        // Determine the rectangle of the document that we are displaying.
        rectangle display_rectangle(
            terminalpp::point(document_base_, 0)
          , terminalpp::extent(self_.get_size().width, 1));

        for (auto const &region : regions)
        {
            boost::optional<rectangle> overlap = intersection(
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
        auto new_index = document_->get_caret_index();

        if  (new_index < document_base_)
        {
            // The caret has gone to the left of the document base.  Rebase
            // ourselves, and redraw.  Also set the cursor position to
            // the base.
            document_base_ = new_index;
            redraw();
            cursor_position_ = {};
            self_.on_cursor_position_changed(cursor_position_);
        }
        else if (new_index + 1 > (document_base_ + self_.get_size().width))
        {
            // The caret has gone beyond the right of the component.  Rebase
            // ourselves so that the caret will be rightmost, and redraw.
            // Also set the cursor position to the rightmost.
            document_base_ = (new_index + 1) - self_.get_size().width;
            redraw();
            cursor_position_ = terminalpp::point(self_.get_size().width - 1, 0);
            self_.on_cursor_position_changed(cursor_position_);
        }
        else
        {
            // Simply set the caret position.
            cursor_position_ = terminalpp::point(new_index - document_base_, 0);
            self_.on_cursor_position_changed(cursor_position_);
        }
    }

    //* =====================================================================
    /// \brief Called when we want to redraw the document.
    //* =====================================================================
    void redraw()
    {
        self_.on_redraw({rectangle({}, self_.get_size())});
    }

    //* =====================================================================
    /// \brief Called by do_ansi_control_sequence_event when the <- key
    /// has been pressed.
    //* =====================================================================
    void do_cursor_backward_key_event(odin::u32 times)
    {
        auto index = document_->get_caret_index();
        auto new_index = index < times ? 0 : index - times;

        document_->set_caret_index(new_index);
    }

    //* =====================================================================
    /// \brief Called by do_ansi_control_sequence_event when the -> key
    /// has been pressed.
    //* =====================================================================
    void do_cursor_forward_key_event(odin::u32 times)
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
        auto document_size = document_->get_size();
        document_->set_caret_index(document_size.width * document_size.height);
    }

    //* =====================================================================
    /// \brief Called by do_event when an ANSI control sequence has been
    /// received.
    //* =====================================================================
    void do_vk_event(terminalpp::virtual_key const &vk)
    {
        auto amount = vk.repeat_count;
        
        switch (vk.key)
        {
            case terminalpp::vk::cursor_left :
                do_cursor_backward_key_event(amount);
                break;
                
            case terminalpp::vk::cursor_right :
                do_cursor_forward_key_event(amount);
                break;
                
            case terminalpp::vk::home :
                do_home_key_event();
                break;
                
            case terminalpp::vk::end :
                do_end_key_event();
                break;
                
            case terminalpp::vk::f12 :
                if (self_.is_enabled())
                {
                    get_document()->delete_text(
                        std::make_pair(0, get_document()->get_text_size()));
                }
                break;

            case terminalpp::vk::bs : // fall-through
            case terminalpp::vk::del :
                if (self_.is_enabled())
                {
                    auto caret_index = document_->get_caret_index();
    
                    if (caret_index != 0)
                    {
                        document_->delete_text(
                            std::make_pair(caret_index - 1, caret_index));
                    }
                }
                break;
            
            default :
                if (self_.is_enabled())
                {
                    terminalpp::glyph gly(char(vk.key));
    
                    if (is_printable(gly))
                    {
                        document_->insert_text({gly});
                    }
                }
        }
    }

    //* =====================================================================
    /// \brief Called by do_event when an ANSI mouse report has been
    /// received.
    //* =====================================================================
    void do_mouse_event(terminalpp::ansi::mouse::report const &report)
    {
        if (report.button_ == terminalpp::ansi::mouse::report::LEFT_BUTTON_DOWN)
        {
            if (self_.can_focus())
            {
                if (!self_.has_focus())
                {
                    self_.set_focus();
                }

                self_.get_document()->set_caret_position(terminalpp::point(
                    report.x_position_
                  , report.y_position_));
            }
        }
    }

    edit                                   &self_;
    std::shared_ptr<munin::text::document>  document_;
    terminalpp::point                       cursor_position_;
    bool                                    cursor_state_;

    odin::u32                               document_base_;

    boost::optional<terminalpp::element>    password_element_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
edit::edit()
{
    pimpl_ = std::make_shared<impl>(std::ref(*this));
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
std::shared_ptr<munin::text::document> edit::get_document()
{
    return pimpl_->get_document();
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
terminalpp::extent edit::do_get_preferred_size() const
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
terminalpp::point edit::do_get_cursor_position() const
{
    return pimpl_->get_cursor_position();
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void edit::do_draw(
    context         &ctx
  , rectangle const &region)
{
    pimpl_->draw(ctx.get_canvas(), region);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void edit::do_event(boost::any const &event)
{
    pimpl_->do_event(event);
}

// ==========================================================================
// DO_SET_ATTRIBUTE
// ==========================================================================
void edit::do_set_attribute(std::string const &name, boost::any const &attr)
{
    if (name == EDIT_PASSWORD_ELEMENT)
    {
        terminalpp::element const *element = 
            boost::any_cast<terminalpp::element>(&attr);

        if (element != nullptr)
        {
            pimpl_->set_password_element(*element);
        }
    }
}

// ==========================================================================
// MAKE_EDIT
// ==========================================================================
std::shared_ptr<edit> make_edit()
{
    return std::make_shared<edit>();

}

}

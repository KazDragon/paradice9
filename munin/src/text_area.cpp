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
#include "munin/context.hpp"
#include <terminalpp/ansi/mouse.hpp>
#include <terminalpp/canvas_view.hpp>
#include <terminalpp/string.hpp>
#include <terminalpp/virtual_key.hpp>
#include <cctype>
#include <vector>

namespace munin {

// ==========================================================================
// TEXT_AREA::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct text_area::impl
{
    impl(text_area &self)
        : self_(self)
        , document_(
              std::make_shared<munin::text::default_multiline_document>())
        , cursor_state_(true)
    {
        document_->on_redraw.connect(
            [this](auto const &region)
            {
                this->on_document_changed(region);
            });

        document_->on_caret_position_changed.connect(
            [this]
            {
                this->on_caret_position_changed();
            });
    }

    //* =====================================================================
    /// \brief Retrieves the document
    //* =====================================================================
    std::shared_ptr<munin::text::document> get_document()
    {
        return document_;
    }

    //* =====================================================================
    /// \brief Gets the size of the document.
    //* =====================================================================
    terminalpp::extent get_size() const
    {
        return document_->get_size();
    }

    //* =====================================================================
    /// \brief Sets the size of the document.
    //* =====================================================================
    void set_size(terminalpp::extent const &size)
    {
        document_->set_size(size);
    }

    //* =====================================================================
    /// \brief Gets the preferred size of the document.
    //* =====================================================================
    terminalpp::extent get_preferred_size() const
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
    terminalpp::point get_cursor_position() const
    {
        return document_->get_caret_position();
    }

    //* =====================================================================
    /// \brief Sets the cursor position.
    //* =====================================================================
    void set_cursor_position(terminalpp::point const &position)
    {
        document_->set_caret_position(position);
    }

    //* =====================================================================
    /// \brief Draws the document onto the canvas.
    //* =====================================================================
    void draw(
        terminalpp::canvas_view &cvs
      , rectangle const &region)
    {
        for (odin::u32 row_index = region.origin.y;
             row_index < odin::u32(region.origin.y + region.size.height);
             ++row_index)
        {
            auto line = document_->get_line(row_index);
            auto column_index = region.origin.x;

            // Write the characters that are required by the document.
            for (;
                 column_index < odin::u32(region.origin.x + region.size.width)
              && column_index < odin::u32(line.size());
                 ++column_index)
            {
                cvs[column_index][row_index] = line[column_index];
            }

            // Pad the rest with blanks
            static terminalpp::element const default_element(' ');
            for (;
                column_index < odin::u32(region.size.width);
                ++column_index)
            {
                cvs[column_index][row_index] = default_element;
            }
        }
    }

    //* =====================================================================
    /// \brief Processes events.
    //* =====================================================================
    void do_event(boost::any const &event)
    {
        auto vk = boost::any_cast<terminalpp::virtual_key>(&event);
        
        if (vk)
        {
            do_vk_event(*vk);
        }

        auto report =
            boost::any_cast<terminalpp::ansi::mouse::report>(&event);

        if (report)
        {
            do_mouse_event(*report);
        }
    }

private :
    //* =====================================================================
    /// \brief Called when the underlying document changes.
    //* =====================================================================
    void on_document_changed(std::vector<rectangle> const &regions)
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
        self_.on_redraw({rectangle({}, self_.get_size())});
    }

    //* =====================================================================
    /// \brief Called by when the up arrow key has been pressed.
    //* =====================================================================
    void do_cursor_up_key_event(odin::u32 times)
    {
        auto position = document_->get_caret_position();

        if (times > odin::u32(position.y))
        {
            times = position.y;
        }

        document_->set_caret_position(
            terminalpp::point(position.x, position.y - times));
    }

    //* =====================================================================
    /// \brief Called when the down arrow key has been pressed.
    //* =====================================================================
    void do_cursor_down_key_event(odin::u32 times)
    {
        auto position = document_->get_caret_position();

        document_->set_caret_position(terminalpp::point(
            position.x
          , position.y + times));
    }

    //* =====================================================================
    /// \brief Called when the <- key has been pressed.
    //* =====================================================================
    void do_cursor_backward_key_event(odin::u32 times)
    {
        auto index = document_->get_caret_index();
        auto new_index = (index < times ? 0 : index - times);

        document_->set_caret_index(new_index);
    }

    //* =====================================================================
    /// \brief Called when the -> key has been pressed.
    //* =====================================================================
    void do_cursor_forward_key_event(odin::u32 times)
    {
        document_->set_caret_index(document_->get_caret_index() + times);
    }

    //* =====================================================================
    /// \brief Called when the BS key has been pressed.
    //* =====================================================================
    void do_bs_key_event()
    {
        auto caret_index = document_->get_caret_index();

        if (caret_index != 0)
        {
            document_->delete_text(
                std::make_pair(caret_index - 1, caret_index));
        }
    }
    
    //* =====================================================================
    /// \brief Called when the DEL key has been pressed.
    //* =====================================================================
    void do_del_key_event()
    {
        if (self_.is_enabled())
        {
            auto caret_index = document_->get_caret_index();
            document_->delete_text(
                std::make_pair(caret_index, caret_index + 1));
        }
    }

    //* =====================================================================
    /// \brief Called when the META+HOME key combination has been pressed.
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
    /// \brief Called when the HOME key has been pressed.
    //* =====================================================================
    void do_home_key_event()
    {
        // The HOME key goes to the first character of the current line.
        auto position = document_->get_caret_position();
        document_->set_caret_position(terminalpp::point(0, position.y));
    }

    //* =====================================================================
    /// \brief Called when the META+END key combination has been pressed.
    //* =====================================================================
    void do_meta_end_key_event()
    {
        // The META+END key combination will move the caret to the end of the
        // document.
        // Events called from this action should ensure that the view and
        // the cursor position are correctly placed.
        auto document_size = document_->get_size();
        document_->set_caret_index(
            document_size.width * document_size.height);
    }

    //* =====================================================================
    /// \brief Called when the END key has been pressed.
    //* =====================================================================
    void do_end_key_event()
    {
        // The END key goes to the last character of the current line.
        auto position = document_->get_caret_position();
        document_->set_caret_position(terminalpp::point(
            document_->get_size().width - 1
          , position.y));
    }

    //* =====================================================================
    /// \brief Called when a virtual key sequence is received
    //* =====================================================================
    void do_vk_event(terminalpp::virtual_key const &vk)
    {
        switch (vk.key)
        {
            case terminalpp::vk::cursor_left :
                do_cursor_backward_key_event(vk.repeat_count);
                break;
                
            case terminalpp::vk::cursor_right :
                do_cursor_forward_key_event(vk.repeat_count);
                break;
                
            case terminalpp::vk::cursor_up :
                do_cursor_up_key_event(vk.repeat_count);
                break;
                
            case terminalpp::vk::cursor_down :
                do_cursor_down_key_event(vk.repeat_count);
                break;
                
            case terminalpp::vk::home :
                if ((vk.modifiers & terminalpp::vk_modifier::meta) 
                  != terminalpp::vk_modifier::none)
                {
                    do_meta_home_key_event();
                }
                else
                {
                    do_home_key_event();
                }
                break;

            case terminalpp::vk::end :
                if ((vk.modifiers & terminalpp::vk_modifier::meta) 
                  != terminalpp::vk_modifier::none)
                {
                    do_meta_end_key_event();
                }
                else
                {
                    do_end_key_event();
                }
                break;
                
            case terminalpp::vk::bs :
                do_bs_key_event();
                break;

            case terminalpp::vk::del :
                do_del_key_event();
                break;
                
            case terminalpp::vk::f12 :
                if (self_.is_enabled())
                {
                    get_document()->delete_text(
                        std::make_pair(0, get_document()->get_text_size()));
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

                self_.set_cursor_position(terminalpp::point(
                    report.x_position_
                  , report.y_position_));
            }
        }
    }

    text_area                              &self_;
    std::shared_ptr<munin::text::document>  document_;
    bool                                    cursor_state_;
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
std::shared_ptr<munin::text::document> text_area::get_document()
{
    return pimpl_->get_document();
}

// ==========================================================================
// DO_GET_SIZE
// ==========================================================================
terminalpp::extent text_area::do_get_size() const
{
    return pimpl_->get_size();
}

// ==========================================================================
// DO_SET_SIZE
// ==========================================================================
void text_area::do_set_size(terminalpp::extent const &size)
{
    // The size is a property owned by the document, not this area.
    pimpl_->set_size(size);
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
terminalpp::extent text_area::do_get_preferred_size() const
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
terminalpp::point text_area::do_get_cursor_position() const
{
    return pimpl_->get_cursor_position();
}

// ==========================================================================
// DO_SET_CURSOR_POSITION
// ==========================================================================
void text_area::do_set_cursor_position(terminalpp::point const &position)
{
    if (position != get_cursor_position())
    {
        pimpl_->set_cursor_position(position);
    }        
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void text_area::do_draw(
    context         &ctx
  , rectangle const &region)
{
    pimpl_->draw(ctx.get_canvas(), region);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void text_area::do_event(boost::any const &event)
{
    pimpl_->do_event(event);
}

// ==========================================================================
// MAKE_TEXT_AREA
// ==========================================================================
std::shared_ptr<text_area> make_text_area()
{
    return std::make_shared<text_area>();

}

    
}


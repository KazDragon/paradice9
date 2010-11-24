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
#include "munin/basic_layout.hpp"
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

namespace detail {

class text_area_renderer 
    : public munin::basic_component<munin::ansi::element_type>
{
public :
    typedef munin::text::document<munin::ansi::element_type> document_type;
    
    //* =====================================================================
    /// \brief Constructor
    /// \param doc - the document that this renderer will be drawing
    //* =====================================================================
    text_area_renderer(
        shared_ptr<document_type> doc)
      : document_(doc)
      , cursor_state_(false)
      , document_base_(0)
    {
        document_->on_redraw.connect(
            bind(&text_area_renderer::on_document_changed, this, _1));
        
        document_->on_caret_position_changed.connect(
            bind(&text_area_renderer::on_caret_position_changed, this));
    }

    //* =====================================================================
    /// \brief Sets the cursor state.
    //* =====================================================================
    void set_cursor_state(bool state)
    {
        do_set_cursor_state(state);
    }

protected :    
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to get the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual extent do_get_preferred_size() const
    {
        return get_size();
    }

    //* =====================================================================
    /// \brief Called by draw().  Derived classes must override this function
    /// in order to draw onto the passed canvas.  A component must only draw 
    /// the part of itself specified by the region.
    ///
    /// \param cvs the canvas in which the component should draw itself.
    /// \param offset the position of the parent component (if there is one)
    ///        relative to the canvas.  That is, (0,0) to this component
    ///        is actually (offset.x, offset.y) in the canvas.
    /// \param region the region relative to this component's origin that
    /// should be drawn.
    //* =====================================================================
    virtual void do_draw(
        canvas<element_type> &cvs
      , point const          &offset
      , rectangle const      &region)
    {
        BOOST_AUTO(position, get_position());

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
                    munin::ansi::element_type(
                        ' '
                      , munin::ansi::attribute());
            }
        }
    }

    //* =====================================================================
    /// \brief Called by event().  Derived classes must override this 
    /// function in order to handle events in a custom manner.
    //* =====================================================================
    virtual void do_event(boost::any const &event)
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
    /// \brief Called by get_cursor_state().  Derived classes must override
    /// this function in order to return the cursor state in a custom manner.
    //* =====================================================================
    virtual bool do_get_cursor_state() const
    {
        return cursor_state_;
    }

    //* =====================================================================
    /// \brief Set the cursor state of this component.
    //* =====================================================================
    virtual void do_set_cursor_state(bool state)
    {
        cursor_state_ = state;
        on_cursor_state_changed(cursor_state_);
    }

    //* =====================================================================
    /// \brief Called by get_cursor_position().  Derived classes must
    /// override this function in order to return the cursor position in
    /// a custom manner.
    //* =====================================================================
    virtual point do_get_cursor_position() const
    {
        return cursor_position_;
    }
    
    //* =====================================================================
    /// \brief Called by set_size().  Derived classes must override this 
    /// function in order to set the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual void do_set_size(extent const &size)
    {
        // On top of setting the size of this component, we also want to set
        // the size of the document so that it can be rendered at the new
        // size.
        munin::basic_component<munin::ansi::element_type>::do_set_size(size);
        
        // TODO: Refactor this to set_size().
        document_->set_width(size.width);
        document_->set_height(size.height);
    }
    
private :
    //* =====================================================================
    /// \brief Called when the underlying document changes.
    //* =====================================================================
    void on_document_changed(vector<munin::rectangle> regions)
    {
        // TODO:
        render();
        /*
        // Search the regions, looking for regions that overlap with the
        // text that we are displaying.
        vector<munin::rectangle> overlapping_changes;
        
        // Determine the rectangle of the document that we are displaying.
        munin::rectangle display_rectangle(
            munin::point(document_base_, 0)
          , munin::extent(get_size().width, 1));
        
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
        */
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
            cursor_position_ = munin::point(new_position.x, document_base_);
            on_cursor_position_changed(cursor_position_);
        }
        else if (u32(new_position.y + 1) > document_base_ + get_size().height)
        {
            // The caret has gone off the bottom of our component.  Rebase
            // ourselves to that the caret will be at the bottom, and
            // repaint.
            document_base_ = (new_position.y + 1) - get_size().height;
            render();
            cursor_position_ = munin::point(
                new_position.x
              , new_position.y - document_base_);
            on_cursor_position_changed(cursor_position_);
        }
        else
        {
            // Simply set the cursor position.
            cursor_position_ = munin::point(
                new_position.x
              , new_position.y - document_base_);
            on_cursor_position_changed(cursor_position_);
        }
    }

    //* =====================================================================
    /// \brief Called when we want to render the document.
    //* =====================================================================
    void render()
    {
        // TODO: 
        /*
        document_view_.resize(get_size().width);
        fill(document_view_.begin()
           , document_view_.end()
           , munin::ansi::element_type(' ', munin::ansi::attribute()));
        
        // Find the segment of the document that we will display
        odin::runtime_array<document_type::character_type> line =
            document_->get_text_line(0);
        
        BOOST_AUTO(
            start_index
          , (min)(line.size(), document_base_));
        
        BOOST_AUTO(
            end_index
          , (min)(line.size(), document_base_ + document_view_.size()));
        
        copy(
            line.begin() + start_index
          , line.begin() + end_index
          , document_view_.begin());
        */
        
        munin::rectangle redraw_region(
            get_position()
          , get_size());
        vector<munin::rectangle> redraw_regions;
        redraw_regions.push_back(redraw_region);
        on_redraw(redraw_regions);
        
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
        if (ch == odin::ascii::BS || ch == odin::ascii::DEL)
        {
            document_->delete_text();
        }
        else if (isprint(ch))
        {
            munin::ansi::element_type data[] = {
                make_pair(ch, munin::ansi::attribute())
            };
        
            document_->insert_text(data);
        }
    }
    
    shared_ptr< munin::text::document<element_type> > document_;
    munin::point                                      cursor_position_;
    bool                                              cursor_state_;
    
    u32                                               document_base_;
    /*
    vector<munin::ansi::element_type>                 document_view_;
    */
};

class text_area_layout
    : public munin::basic_layout<munin::ansi::element_type>
{
public :
    enum hint_type
    {
        hint_type_frame
      , hint_type_renderer
    };

private :
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to retrieve the preferred size of the layout
    /// in a custom manner.
    //* =====================================================================
    virtual extent do_get_preferred_size() const
    {
        return extent(0, 0);
    }

    //* =====================================================================
    /// \brief Called by operator().  Derived classes must override this
    /// function in order to lay a container's components out in a custom
    /// manner.
    //* =====================================================================
    virtual void do_layout(
        boost::shared_ptr<container_type> const &cont)
    {
        // This layout expects only two subcomponents: a frame, and a central
        // component.  It lays them out in the expected format.  In addition,
        BOOST_AUTO(size, cont->get_size());

        for (u32 index = 0; index < get_number_of_components(); ++index)
        {
            BOOST_AUTO(comp, get_component(index));
            BOOST_AUTO(any_hint, get_hint(index));

            BOOST_AUTO(hint, any_cast<hint_type>(any_hint));

            if (hint == hint_type_frame)
            {
                comp->set_position(munin::point(0, 0));
                comp->set_size(size);
            }
            else if (hint == hint_type_renderer)
            {
                comp->set_position(munin::point(1, 1));
                comp->set_size(
                    munin::extent(size.width - 2, size.height - 2));
            }
        }
    }
};

}

struct text_area::impl
{
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
text_area::text_area()
  : munin::composite_component<element_type>(make_shared<basic_container>())
  , pimpl_(new impl)
{
    get_container()->set_layout(
        make_shared<detail::text_area_layout>());

    BOOST_AUTO(border, make_shared<frame>());
    get_container()->add_component(
        border
      , detail::text_area_layout::hint_type_frame);

    BOOST_AUTO(renderer, make_shared<detail::text_area_renderer>(
        make_shared<munin::ansi::text::default_multiline_document>()));

    get_container()->add_component(
        renderer
      , detail::text_area_layout::hint_type_renderer);

    renderer->set_cursor_state(true);
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
text_area::~text_area()
{
}

}}

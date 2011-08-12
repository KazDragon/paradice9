// ==========================================================================
// Hugin Wholist
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
#include "wholist.hpp"
#include "munin/algorithm.hpp"
#include "munin/canvas.hpp"
#include "munin/ansi/protocol.hpp"
#include "odin/ansi/protocol.hpp"
#include "odin/runtime_array.hpp"
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>
#include <vector>

using namespace munin;
using namespace munin::ansi;
using namespace odin;
using namespace boost;
using namespace std;

BOOST_STATIC_CONSTANT(u16, MIN_COLUMN_WIDTH        = 36);
BOOST_STATIC_CONSTANT(u16, NUMBER_OF_ROWS          = 3);
BOOST_STATIC_CONSTANT(u16, COLUMN_SEPERATOR_WIDTH  = 3);

namespace hugin {

// ==========================================================================
// WHOLIST::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct wholist::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(wholist &self)
        : self_(self)
        , name_index_(0)
        , current_selection_(0)
    {
    }
    
    // ======================================================================
    // DRAW
    // ======================================================================
    void draw(
        canvas          &cvs
      , rectangle const &region)
    {
        copy_region(region, view_, cvs);
    }
    
    // ======================================================================
    // SET_NAMES
    // ======================================================================
    void set_names(runtime_array<string> const &names)
    {
        names_ = names; 
        check_selected_index();
    }
    
    // ======================================================================
    // GET_NAMES
    // ======================================================================
    runtime_array<string> get_names() const
    {
        return names_;
    }
    
    // ======================================================================
    // GET_NUMBER_OF_COLUMNS
    // ======================================================================
    u32 get_number_of_columns() const
    {
        return columns_;
    }
    
    // ======================================================================
    // GET_SELECTED_INDEX
    // ======================================================================
    u32 get_selected_index() const
    {
        return current_selection_;
    }
    
    // ======================================================================
    // SET_SELECTED_INDEX
    // ======================================================================
    void set_selected_index(u32 index)
    {
        current_selection_ = index;
        check_selected_index();
    }
    
    // ======================================================================
    // RENDER
    // ======================================================================
    void render()
    {
        cache_constants();
        initialise_view();
        render_names();
        repaint();
    }
    
private :
    // ======================================================================
    // RENDER_NAMES
    // ======================================================================
    void render_names()
    {
        BOOST_AUTO(size, self_.get_size());

        // Find the begin and end indices of this page.
        u32 current_page_begin_index = u32(name_index_);
        u32 current_page_end_index   = u32(name_index_ + names_per_page_);

        // Loop through all the players in the current page.
        for (u32 current_cell_index = current_page_begin_index;
             current_cell_index < current_page_end_index;
             ++current_cell_index)
        {
            // Work out the current column and row.
            u32 column = current_cell_index % columns_;

            u32 row    = (current_cell_index - current_page_begin_index)
                       / columns_;

            // Because we are displaying vertically and not horizontally, 
            // the name being displayed here is a matrix rotation of the 
            // cell row/col.
            u32 const current_name_index =
                name_index_ + (column * rows_) + (row);

            if (current_name_index < names_.size())
            {
                attribute pen;
                pen.underlining_ = 
                    current_name_index == current_selection_
                 && self_.has_focus()
                    ? odin::ansi::graphics::UNDERLINING_UNDERLINED
                    : odin::ansi::graphics::UNDERLINING_NOT_UNDERLINED;
                
                // Trim down the name if it is too long.
                BOOST_AUTO(
                    current_name
                  , names_[current_name_index]);
                
                current_name = current_name.substr(
                    0, (min)(current_name.size(), size_t(MIN_COLUMN_WIDTH)));
                
                // Convert this to elements.
                BOOST_AUTO(
                    name_elements
                  , elements_from_string(current_name, pen));
                
                // Find the x coordinate of this cell.
                u32 cell_x_coordinate = 
                    (column * MIN_COLUMN_WIDTH)
                  + (column == 0 ? 0 : (column * COLUMN_SEPERATOR_WIDTH))
                  + (column == 0 ? 0 : padding_per_column_);
                
                // Find the y coordinate of this cell.
                u32 cell_y_coordinate = row;
                  
                // Copy these into the correct location.
                for (u32 index = 0; index < name_elements.size(); ++index)
                {
                    view_[cell_x_coordinate + index]
                         [cell_y_coordinate        ] = name_elements[index];
                }
            }
        }
    }
    
    // ======================================================================
    // INITIALISE_VIEW
    // ======================================================================
    void initialise_view()
    {
        BOOST_AUTO(size, self_.get_size());
        
        if (size.width <= 0 || size.height <= 0)
        {
            return;
        }
        
        // If necessary, we will have to totally recreate the view
        if (size != current_size_)
        {
            view_.set_size(size);
        }

        current_size_ = size;
        
        attribute pen;
        element_type const blank_element(' ', pen);
        
        // Blank out the view.
        for (s32 column = 0; column < size.width; ++column)
        {
            for (s32 row = 0; row < size.height; ++row)
            {
                view_[column][row] = blank_element;
            }
        }
    }

    // ======================================================================
    // REPAINT
    // ======================================================================
    void repaint()
    {
        vector<rectangle> regions;
        regions.push_back(rectangle(point(), self_.get_size()));
        self_.on_redraw(regions);
    }
    
    // ======================================================================
    // CHECK_SELECTED_INDEX
    // ======================================================================
    void check_selected_index()
    {
        if (current_selection_ != 0)
        {
            current_selection_ = 
                (min)(current_selection_, u32(names_.size() - 1));
        }

        // Scroll up if necessary.
        while (current_selection_ < name_index_)
        {
            if (name_index_ < names_per_page_)
            {
                name_index_ = 0;
            }
            else
            {
                name_index_ -= names_per_page_;
            }
        }
        
        // Scroll down if necessary.
        if (names_per_page_ != 0)
        {
            while (current_selection_ >= name_index_ + names_per_page_)
            {
                name_index_ += names_per_page_;
            }
        }
    }
    
    // ======================================================================
    // CACHE_CONSTANTS
    // ======================================================================
    void cache_constants()
    {
        BOOST_AUTO(size, self_.get_size());
        
        rows_    = size.height;
        columns_ = 0;
        
        if (size.width >= MIN_COLUMN_WIDTH)
        {
            do
            {
                ++columns_;
            } 
            while (((columns_ + 1) * MIN_COLUMN_WIDTH)
                 + (columns_ * COLUMN_SEPERATOR_WIDTH)
                < u32(size.width));
        }

        if (columns_ == 0)
        {
            padding_per_row_    = 0;
            padding_per_column_ = 0;
            names_per_page_     = 0;
            pages_              = 0;
        }
        else
        {
            // The padding necessary per row to make the columns flush 
            // with the edge.
            padding_per_row_ =
                (size.width - (columns_ * MIN_COLUMN_WIDTH))
              - ((columns_ - 1) * COLUMN_SEPERATOR_WIDTH);

            padding_per_column_ = padding_per_row_ / columns_;
            
            names_per_page_ = columns_ * rows_;
            
            pages_ = 
                (names_.size() / names_per_page_)
              + ((names_.size() % names_per_page_) != 0 ? 1 : 0);
        }
    }

    wholist                                      &self_;
    runtime_array<string>                         names_;
    canvas                                        view_;
    munin::extent                                 current_size_;
    u32                                           name_index_;
    u32                                           current_selection_;
    
    // A selection of cached constants to ease rendering
    u32                                           rows_;
    u32                                           columns_;
    u32                                           padding_per_row_;
    u32                                           padding_per_column_;
    u32                                           names_per_page_;
    u32                                           pages_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
wholist::wholist()
{
    pimpl_ = make_shared<impl>(ref(*this));
    on_focus_set.connect(bind(&impl::render, pimpl_.get()));
    on_focus_lost.connect(bind(&impl::render, pimpl_.get()));
}

// ==========================================================================
// SET_NAMES
// ==========================================================================
void wholist::set_names(runtime_array<string> const &names)
{
    pimpl_->set_names(names);
    pimpl_->render();
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
munin::extent wholist::do_get_preferred_size() const
{
    return munin::extent(get_size().width, NUMBER_OF_ROWS);
}

// ==========================================================================
// DO_SET_SIZE
// ==========================================================================
void wholist::do_set_size(munin::extent const &size)
{
    basic_component::do_set_size(size);
    pimpl_->render();
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void wholist::do_draw(
    canvas          &cvs
  , rectangle const &region)
{
    pimpl_->draw(cvs, region);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void wholist::do_event(any const &ev)
{
    BOOST_AUTO(sequence, any_cast<odin::ansi::control_sequence>(&ev));
    
    if (sequence != NULL)
    {
        if (sequence->initiator_ == odin::ansi::CONTROL_SEQUENCE_INTRODUCER
         && sequence->command_   == odin::ansi::CURSOR_UP)
        {
            u32 times = sequence->arguments_.empty()
                      ? 1
                      : atoi(sequence->arguments_.c_str());
                      
            BOOST_AUTO(selected_index, pimpl_->get_selected_index());
            
            pimpl_->set_selected_index(
                selected_index < times
              ? 0
              : selected_index - times);
            
            pimpl_->render();
        }

        if (sequence->initiator_ == odin::ansi::CONTROL_SEQUENCE_INTRODUCER
         && sequence->command_   == odin::ansi::CURSOR_DOWN)
        {
            u32 times = sequence->arguments_.empty()
                      ? 1
                      : atoi(sequence->arguments_.c_str());
                      
            BOOST_AUTO(selected_index, pimpl_->get_selected_index());
            
            pimpl_->set_selected_index(selected_index + times);
            
            pimpl_->render();            
        }
    }
    else
    {
        basic_component::do_event(ev);
    }
}

}


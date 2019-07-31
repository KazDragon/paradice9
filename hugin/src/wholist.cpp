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
#include "hugin/wholist.hpp"
#include "munin/algorithm.hpp"
#include "munin/context.hpp"
#include <terminalpp/canvas.hpp>
#include <terminalpp/encoder.hpp>
#include <terminalpp/virtual_key.hpp>
#include <vector>

BOOST_STATIC_CONSTANT(std::uint16_t, MIN_COLUMN_WIDTH        = 36);
BOOST_STATIC_CONSTANT(std::uint16_t, NUMBER_OF_ROWS          = 3);
BOOST_STATIC_CONSTANT(std::uint16_t, COLUMN_SEPERATOR_WIDTH  = 3);

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
        , view_({0, 0})
        , name_index_(0)
        , current_selection_(0)
    {
    }
    
    // ======================================================================
    // DRAW
    // ======================================================================
    void draw(
        munin::canvas_view &cvs,
        terminalpp::rectangle const  &region)
    {
        munin::copy_region(region, view_, cvs);
    }
    
    // ======================================================================
    // SET_NAMES
    // ======================================================================
    void set_names(std::vector<std::string> const &names)
    {
        names_ = names; 
        check_selected_index();
    }
    
    // ======================================================================
    // GET_NAMES
    // ======================================================================
    std::vector<std::string> get_names() const
    {
        return names_;
    }
    
    // ======================================================================
    // GET_NUMBER_OF_COLUMNS
    // ======================================================================
    std::uint32_t get_number_of_columns() const
    {
        return columns_;
    }
    
    // ======================================================================
    // GET_SELECTED_INDEX
    // ======================================================================
    std::uint32_t get_selected_index() const
    {
        return current_selection_;
    }
    
    // ======================================================================
    // SET_SELECTED_INDEX
    // ======================================================================
    void set_selected_index(std::uint32_t index)
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
        // Find the begin and end indices of this page.
        std::uint32_t current_page_begin_index = std::uint32_t(name_index_);
        std::uint32_t current_page_end_index   = std::uint32_t(name_index_ + names_per_page_);

        // Loop through all the players in the current page.
        for (std::uint32_t current_cell_index = current_page_begin_index;
             current_cell_index < current_page_end_index;
             ++current_cell_index)
        {
            // Work out the current column and row.
            std::uint32_t column = current_cell_index % columns_;

            std::uint32_t row    = (current_cell_index - current_page_begin_index)
                       / columns_;

            // Because we are displaying vertically and not horizontally, 
            // the name being displayed here is a matrix rotation of the 
            // cell row/col.
            std::uint32_t const current_name_index =
                name_index_ + (column * rows_) + (row);

            if (current_name_index < names_.size())
            {
                terminalpp::attribute pen;
                pen.underlining_ = 
                    current_name_index == current_selection_
                 && self_.has_focus()
                    ? terminalpp::ansi::graphics::underlining::underlined
                    : terminalpp::ansi::graphics::underlining::not_underlined;
                
                // Trim down the name if it is too long.
                auto current_name = names_[current_name_index];
                
                current_name = current_name.substr(
                    0, (std::min)(current_name.size(), size_t(MIN_COLUMN_WIDTH)));
                
                // Convert this to elements.
                auto name_elements = terminalpp::string(current_name, pen);
                
                // Find the x coordinate of this cell.
                std::uint32_t cell_x_coordinate = 
                    (column * MIN_COLUMN_WIDTH)
                  + (column == 0 ? 0 : (column * COLUMN_SEPERATOR_WIDTH))
                  + (column == 0 ? 0 : padding_per_column_);
                
                // Find the y coordinate of this cell.
                std::uint32_t cell_y_coordinate = row;
                  
                // Copy these into the correct location.
                for (std::uint32_t index = 0; index < name_elements.size(); ++index)
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
        auto size = self_.get_size();
        
        if (size.width <= 0 || size.height <= 0)
        {
            return;
        }
        
        // If necessary, we will have to totally recreate the view
        if (size != current_size_)
        {
            view_ = terminalpp::canvas(size);
        }

        current_size_ = size;
        
        terminalpp::attribute pen;
        terminalpp::element const blank_element(' ', pen);
        
        // Blank out the view.
        for (std::int32_t column = 0; column < size.width; ++column)
        {
            for (std::int32_t row = 0; row < size.height; ++row)
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
        std::vector<terminalpp::rectangle> regions;
        regions.push_back(terminalpp::rectangle({}, self_.get_size()));
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
                (std::min)(current_selection_, std::uint32_t(names_.size() - 1));
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
        auto size = self_.get_size();
        
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
                < std::uint32_t(size.width));
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
            
            if (names_per_page_ != 0)
            {
                pages_ = 
                    (names_.size() / names_per_page_)
                  + ((names_.size() % names_per_page_) != 0 ? 1 : 0);
            }
            else
            {
                pages_ = 0;
            }
        }
    }

    wholist                  &self_;
    std::vector<std::string>  names_;
    terminalpp::canvas        view_;
    terminalpp::extent        current_size_;
    std::uint32_t                 name_index_;
    std::uint32_t                 current_selection_;
    
    // A selection of cached constants to ease rendering
    std::uint32_t                 rows_;
    std::uint32_t                 columns_;
    std::uint32_t                 padding_per_row_;
    std::uint32_t                 padding_per_column_;
    std::uint32_t                 names_per_page_;
    std::uint32_t                 pages_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
wholist::wholist()
{
    pimpl_ = std::make_shared<impl>(ref(*this));
    on_focus_set.connect([this]{pimpl_->render();});
    on_focus_lost.connect([this]{pimpl_->render();});
}

// ==========================================================================
// SET_NAMES
// ==========================================================================
void wholist::set_names(std::vector<std::string> const &names)
{
    pimpl_->set_names(names);
    pimpl_->render();
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
terminalpp::extent wholist::do_get_preferred_size() const
{
    return terminalpp::extent(get_size().width, NUMBER_OF_ROWS);
}

// ==========================================================================
// DO_SET_SIZE
// ==========================================================================
void wholist::do_set_size(terminalpp::extent const &size)
{
    basic_component::do_set_size(size);
    pimpl_->render();
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void wholist::do_draw(
    munin::context         &ctx,
    terminalpp::rectangle const &region)
{
    pimpl_->draw(ctx.get_canvas(), region);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void wholist::do_event(boost::any const &ev)
{
    auto const *vk = boost::any_cast<terminalpp::virtual_key>(&ev);
    
    if (vk)
    {
        if (vk->key == terminalpp::vk::cursor_up)
        {
            auto times = vk->repeat_count;
            auto selected_index = pimpl_->get_selected_index();
            
            pimpl_->set_selected_index(
                selected_index < times
              ? 0
              : selected_index - times);
            
            pimpl_->render();
        }
        else if (vk->key == terminalpp::vk::cursor_down)
        {
            auto times = vk->repeat_count;
            auto selected_index = pimpl_->get_selected_index();
            
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


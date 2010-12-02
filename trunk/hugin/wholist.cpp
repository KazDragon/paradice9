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
#include "munin/canvas.hpp"
#include "munin/ansi/protocol.hpp"
#include "odin/ansi/protocol.hpp"
#include "odin/runtime_array.hpp"
#include <boost/bind.hpp>
#include <boost/typeof/typeof.hpp>
#include <vector>

using namespace munin;
using namespace munin::ansi;
using namespace odin;
using namespace boost;
using namespace std;

/*
typedef munin::component<munin::ansi::element_type>       component;
typedef munin::container<munin::ansi::element_type>       container;
typedef munin::card<munin::ansi::element_type>            card;
typedef munin::aligned_layout<munin::ansi::element_type>  aligned_layout;
typedef munin::compass_layout<munin::ansi::element_type>  compass_layout;
typedef munin::grid_layout<munin::ansi::element_type>     grid_layout;
typedef munin::ansi::basic_container                      basic_container;
typedef munin::ansi::edit                                 edit;
typedef munin::ansi::image                                image;
typedef munin::ansi::text_area                            text_area;
typedef munin::ansi::frame                                frame;
typedef munin::ansi::framed_component                     framed_component;
*/
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
    {
    }
    
    // ======================================================================
    // DRAW
    // ======================================================================
    void draw(
        canvas<munin::ansi::element_type> &cvs
      , point const                       &offset
      , rectangle const                   &region)
    {
        BOOST_AUTO(position, self_.get_position());
        
        munin::ansi::attribute pen;
        pen.foreground_colour = odin::ansi::graphics::COLOUR_RED;
        pen.intensity         = odin::ansi::graphics::INTENSITY_BOLD;
        
        for (s32 row = 0; row < region.size.height; ++row)
        {
            for (s32 column = 0; column < region.size.width; ++column)
            {
                BOOST_AUTO(x_coordinate, column + region.origin.x);
                BOOST_AUTO(y_coordinate, row    + region.origin.y);
                
                element_type element = 
                    x_coordinate >= s32(view_.size())
                 || y_coordinate >= s32(view_[0].size())
                     ? element_type(' ', attribute())
                     : view_[x_coordinate][y_coordinate];

                cvs[x_coordinate + position.x + offset.x]
                   [y_coordinate + position.y + offset.y] = element;
            }
        }
    }
    
    // ======================================================================
    // RENDER_BORDER
    // ======================================================================
    void render_border()
    {
        BOOST_AUTO(size, self_.get_size());

        BOOST_AUTO(focussed, self_.has_focus());
        attribute pen;
        pen.foreground_colour = focussed
            ? odin::ansi::graphics::COLOUR_CYAN
            : odin::ansi::graphics::COLOUR_DEFAULT;
        pen.intensity = focussed
            ? odin::ansi::graphics::INTENSITY_BOLD
            : odin::ansi::graphics::INTENSITY_NORMAL;
        pen.locale = odin::ansi::character_set::LOCALE_SCO;
        
        view_[0             ][0              ] = element_type(char(201), pen);
        view_[size.width - 1][0              ] = element_type(char(187), pen);
        view_[0             ][size.height - 1] = element_type(char(200), pen);
        view_[size.width - 1][size.height - 1] = element_type(char(188), pen);

        for (s32 row = 1; row < (size.height - 1); ++row)
        {
            view_[0             ][row] = element_type(char(186), pen);
            view_[size.width - 1][row] = element_type(char(186), pen);
        }
        
        for (s32 column = 1; column < (size.width - 1); ++column)
        {
            view_[column][0              ] = element_type(char(205), pen);
            view_[column][size.height - 1] = element_type(char(205), pen);
        }
        
        BOOST_AUTO(title, elements_from_string(" CURRENTLY PLAYING "));
        
        if (size.width >= s32(title.size() + 4))
        {
            for (s32 column = 2; column < s32(title.size() + 2); ++column)
            {
                view_[column][0] = title[column - 2];
            }
        }

        // TODO: some kind of visualisation of whether there is more to see.
        
        vector<rectangle> regions;
        regions.push_back(rectangle(point(), size));
        self_.on_redraw(regions);
    }
    
    // ======================================================================
    // RENDER
    // ======================================================================
    void render()
    {
        BOOST_AUTO(size, self_.get_size());
        
        // If necessary, we will have to totally recreate the view
        if (size != current_size_)
        {
            view_ = runtime_array< runtime_array<element_type> >(size.width);
        
            for (s32 column = 0; column < size.width; ++column)
            {
                view_[column] = runtime_array<element_type>(size.height);
            }
        }

        current_size_ = size;
        
        // Blank out the view.
        for (s32 column = 0; column < size.width; ++column)
        {
            for (s32 row = 0; row < size.height; ++row)
            {
                view_[column][row] = 
                    element_type(' ', munin::ansi::attribute());
            }
        }
        
        BOOST_STATIC_CONSTANT(u16, min_column_width           = 36);
        BOOST_STATIC_CONSTANT(u16, top_border_height          = 1);
        BOOST_STATIC_CONSTANT(u16, bottom_border_height       = 1);
        BOOST_STATIC_CONSTANT(u16, left_border_width          = 2);
        BOOST_STATIC_CONSTANT(u16, right_border_width         = 2);
        BOOST_STATIC_CONSTANT(u16, total_border_width         =
            left_border_width + right_border_width); 
        BOOST_STATIC_CONSTANT(u16, total_border_height        = 
            top_border_height + bottom_border_height); 
        BOOST_STATIC_CONSTANT(u16,    column_seperator_width  = 3);
        
        // If there is insufficient space to draw anything, repaint and
        // return.
        if (size.width < (min_column_width + total_border_width)
         || size.height < 2)
        {
            vector<rectangle> regions;
            regions.push_back(rectangle(point(), size));
            self_.on_redraw(regions);
            return;            
        }
        
/*
 +=== CURRENTLY PLAYING ======================================================+
 | <prefix> <    name1   > <  title  > || <prefix> <    name6   > <  title  > |
 | <prefix> <    name2   > <  title  > || <prefix> <    name7   > <  title  > |
 | <prefix> <    name3   > <  title  > || <prefix> <    name8   > <  title  > |
 | <prefix> <    name4   > <  title  > || <prefix> <    name9   > <  title  > |
 +====================================================================[1/2]===+

*/
        // We have enough space; draw some stuff.
        u16 const rows = u16(size.height - 2);
                   
        // Find the number of columns the client can currently support
        u16 columns = 0;
    
        do
        {
            ++columns;
        } 
        while ((total_border_width 
             + ((columns + 1) * min_column_width)
             + (columns * column_seperator_width))
            < size.width);
        
        // This is the number of names we can display per row.
        u16 const names_per_row = columns; 

        // The padding necessary per row to make the columns flush with the 
        // border.
        u16 const padding_per_row = u16(
            ((size.width - total_border_width) - (columns * min_column_width))
          - ((columns - 1) * column_seperator_width));
       
        u16 const padding_per_column = u16(padding_per_row / columns);
        
        // This is the number of names we can display per page.
        u16 const names_per_page = u16(rows * names_per_row);
        
        // This is the number of pages required to see all names.
        u16 const pages = u16(
            (names_.size() / names_per_page)
          + ((names_.size() % names_per_page) != 0 ? 1 : 0));

        // Make sure that name_index_ doesn't fall off the last page.
        name_index_ = (min)(name_index_, u32((pages - 1) * names_per_page));
        
        // Find the begin and end indices of this page.
        u16 current_page_begin_index = u16(name_index_);
        u16 current_page_end_index   = u16(name_index_ + names_per_page);

        // Find the number of columns we will be displaying.  For example,
        // if we can support three columns of 6, but there are only 7 
        // names to display, we only need display two columns.
        u16 names_on_page = names_.size() >= current_page_end_index
                          ? names_per_page
                          : names_.size() % names_per_page; 

        // Find the number of columns that will have names in this page.
        u16 columns_with_names = 
            (names_on_page + (rows - 1)) / rows;
        
        // Loop through all the players in the current page.
        for (u16 current_cell_index = current_page_begin_index;
             current_cell_index < current_page_end_index;
             ++current_cell_index)
        {
            // Work out the current column and row.
            u16 const column = current_cell_index % columns;

            u16 const row    = 
                (current_cell_index - current_page_begin_index)
              / columns;

            // Because we are displaying vertically and not horizontally, 
            // the name being displayed here is a matrix rotation of the 
            // cell row/col.
            u16 const current_name_index = u16(
                name_index_
              + (column * rows)
              + (row));

            if (current_name_index < names_.size())
            {
                // Trim down the name if it is too long.
                BOOST_AUTO(current_name, names_[current_name_index]);
                current_name = current_name.substr(
                    0, (min)(current_name.size(), size_t(min_column_width)));
                
                // Convert this to elements.
                BOOST_AUTO(name_elements, elements_from_string(current_name));
                
                // Find the x coordinate of this cell.
                u16 cell_x_coordinate = 
                    left_border_width
                  + (column * min_column_width)
                  + (column == 0 ? 0 : (column * column_seperator_width))
                  + (column == 0 ? 0 : padding_per_column);
                
                // Find the y coordinate of this cell.
                u16 cell_y_coordinate =
                    row
                  + top_border_height;
                  
                // Copy these into the correct location.
                for (u32 index = 0; index < name_elements.size(); ++index)
                {
                    view_[cell_x_coordinate + index]
                         [cell_y_coordinate        ] = name_elements[index];
                }
            }
        }

        render_border();

        /*
        
        // Pad the name out to the standard column width
        current_name += string(column_width - current_name.size(), ' ');
        
        // Add this text to the cell.
        cell_text += current_name;

        // Add padding if necessary
        // First, mandatory padding for each cell.
        string column_padding(padding_per_row / columns, ' ');
        
        // Next, any padding that only applies to some cells.
        column_padding += (padding_per_row % columns) > column ? " " : "";
        
        cell_text += column_padding;
        
        // Add the right border if necessary
        if (((current_cell_index + 1) % names_per_row) == 0)
        {
            cell_text += "|" + newline;
        }
        else
        {
            if (column + 1 < columns_with_names)
            {
                cell_text += " | ";
            }
            else
            {
                cell_text += "   ";
            }
        }
        
        // Finally, append this junk onto the main text.
        text += cell_text;
    }
    
    string page_descriptor = str(
        format("[%d/%d]") 
            % (current_page + 1) 
            % pages);
    
    text += "+="
          + string((screen_width - 6) - page_descriptor.size(), '=')
          + page_descriptor
          + "==+"
          + newline
          + restore_cursor_position;

    player->get_connection()->write(text);
    */
        /*
        // Discover number of columns
        BOOST_STATIC_CONSTANT(u32, left_border_width = 1);
        BOOST_STATIC_CONSTANT(u32, right_border_width = 1);
        BOOST_STATIC_CONSTANT(u32, top_border_width = 1);
        BOOST_STATIC_CONSTANT(u32, bottom_border_width = 1);
        BOOST_STATIC_CONSTANT(u32, seperator_size = 3);
        BOOST_STATIC_CONSTANT(u32, min_name_width = 37);
        
        u32 columns = 0;
        
        u32 width_remaining = u32(size.width);
        
        
        if (size.width < left_border_width 
        
        // Correct page_ if necessary
    
        // Draw Borders
        
        // Draw contents
        */
        // Repaint
        vector<rectangle> regions;
        regions.push_back(rectangle(point(), size));
        self_.on_redraw(regions);
    }
    
    wholist                                      &self_;
    runtime_array<string>                         names_;
    runtime_array< runtime_array<element_type> >  view_;
    munin::extent                                 current_size_;
    u32                                           name_index_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
wholist::wholist()
{
    pimpl_.reset(new impl(*this));
    on_focus_set.connect(bind(&impl::render_border, pimpl_.get()));
    on_focus_lost.connect(bind(&impl::render_border, pimpl_.get()));
}

// ==========================================================================
// SET_NAMES
// ==========================================================================
void wholist::set_names(runtime_array<string> const &names)
{
    pimpl_->names_ = names;
    pimpl_->render();
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
munin::extent wholist::do_get_preferred_size() const
{
    return munin::extent(get_size().width, 6);
}

// ==========================================================================
// DO_SET_SIZE
// ==========================================================================
void wholist::do_set_size(munin::extent const &size)
{
    munin::basic_component<munin::ansi::element_type>::do_set_size(size);
    pimpl_->render();
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void wholist::do_draw(
    munin::canvas<munin::ansi::element_type> &cvs
  , munin::point const                       &offset
  , munin::rectangle const                   &region)
{
    pimpl_->draw(cvs, offset, region);
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void wholist::do_event(any const &ev)
{
}

}

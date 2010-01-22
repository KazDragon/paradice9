// ==========================================================================
// Paradice Who
//
// Copyright (C) 2009 Matthew Chaplain, All Rights Reserved.
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
#include "who.hpp"
#include "client.hpp"
#include "connection.hpp"
#include "odin/types.hpp"
#include <boost/format.hpp>
#include <boost/utility.hpp>

using namespace std;
using namespace boost;
using namespace odin;

namespace paradice {

string get_player_address(shared_ptr<paradice::client> &client)
{
    string prefix = client->get_prefix();
    string name   = client->get_name();
    string title  = client->get_title();
    
    string address;
    
    if (!prefix.empty())
    {
        address += prefix + " ";
    }
    
    address += name;
    
    if (!title.empty())
    {
        address += " " + title;
    }
    
    return address;
}
    
void do_who(
    string const                 &/*text*/, 
    shared_ptr<paradice::client> &client)
{
    BOOST_STATIC_CONSTANT(string, save_cursor_position    = "\x1B[s");
    BOOST_STATIC_CONSTANT(string, restore_cursor_position = "\x1B[u");
    BOOST_STATIC_CONSTANT(string, cursor_position_home    = "\x1B[H");
    BOOST_STATIC_CONSTANT(string, erase_to_eol            = "\x1B[K");
    BOOST_STATIC_CONSTANT(
        string, newline = erase_to_eol + "\r\n");
    BOOST_STATIC_CONSTANT(u16,    column_width            = 36);
    BOOST_STATIC_CONSTANT(u16,    rows                    = 6);
    BOOST_STATIC_CONSTANT(u16,    left_border_width       = 2);
    BOOST_STATIC_CONSTANT(u16,    right_border_width      = 2);
    BOOST_STATIC_CONSTANT(
        u16, total_border_width = left_border_width + right_border_width); 
    BOOST_STATIC_CONSTANT(u16,    column_seperator_width  = 3);
    
    if (client->get_level() != paradice::client::level_in_game)
    {
        return;
    }

    pair<u16, u16> window_size = client->get_connection()->get_window_size();

    // We don't want to write in the final column of a row, because that will
    // cause a newline and may mess up on some clients.  Hence, we pretend it
    // is one smaller.
    u16 const screen_width  = window_size.first - 1;
    
    // Find the number of columns the client can currently support
    u16 columns = 0;

    if (screen_width > (total_border_width + column_width))
    {
        do
        {
            ++columns;
        } 
        while ((total_border_width 
             + ((columns + 1) * column_width)
             + (columns * column_seperator_width))
            <= screen_width);
    }
    
    // If the window's extends cannot support the who list, then stop right
    // here.
    if (columns == 0)
    {
        return;
    }
    
    // Work out the list of names we will have to print.
    vector<string> names;
    
    transform(
        paradice::clients.begin()
      , paradice::clients.end()
      , back_inserter(names)
      , &get_player_address);
    
    // Calculate the number of pages of Who List.
    // There are 6 rows available per page, with 1 name available for each
    // 38 characters of width, not including borders.
    // By the calculation above, there will be at least one name available per
    // row 

    // This is the number of names we can display per row.
    u16 const names_per_row = columns; 
    
    // The padding necessary per row to make the columns flush with the border.
    u16 const padding_per_row = 
        ((screen_width - total_border_width) 
       - ((columns - 1) * column_seperator_width)) % column_width;

    // This is the number of names we can display per page.
    u16 const names_per_page = rows * names_per_row;

    // This is the number of pages required to see all names.
    u16 const pages = u16(
        (names.size() / names_per_page)
      + ((names.size() % names_per_page) != 0 ? 1 : 0)); 
        
    // Create the top border of the group panel.
    string group_panel_top_border =
        "+==== CURRENTLY PLAYING ";
        
    group_panel_top_border 
        += string(
             (screen_width - group_panel_top_border.size())
           - right_border_width,
             '=')
         + "=+"
         + newline;
        
    string text =
        save_cursor_position
      + cursor_position_home
      + group_panel_top_border;
        
    u16 current_page             = client->get_who_page();
    u16 current_page_begin_index = current_page * names_per_page;
    u16 current_page_end_index   = (current_page + 1) * names_per_page;
    
/*
 +=== CURRENTLY PLAYING ======================================================+
 | <prefix> <    name1   > <  title  > || <prefix> <    name6   > <  title  > |
 | <prefix> <    name2   > <  title  > || <prefix> <    name7   > <  title  > |
 | <prefix> <    name3   > <  title  > || <prefix> <    name8   > <  title  > |
 | <prefix> <    name4   > <  title  > || <prefix> <    name9   > <  title  > |
 | <prefix> <    name5   > <  title  > || <prefix> <    name10  > <  title  > |
 +====================================================================[1/2]===+

   ---8---- ------14------ -----11----    ---8---- ------14------ -----11---- 
*/
    // Find the number of columns we will be displaying.  For example, if
    // we can support three columns of 6, but there are only 7 names to
    // display, we only need display two columns.
    u16 names_on_page = names.size() >= current_page_end_index
                      ? names_per_page
                      : names.size() % names_per_page; 
                      
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
            (current_cell_index - (current_page * names_per_page))
          / columns;
        
        string cell_text;

        // Add the left border if necessary.
        if ((current_cell_index % names_per_row) == 0)
        {
            cell_text += "| ";
        }
        
        // Because we are displaying vertically and not horizontally, the
        // name being displayed here is a matrix rotation of the cell row/col.
        u16 current_name_index = 
            (current_page * names_on_page)
          + (column * rows)
          + (row);
        
        // Get the appropriate name.
        string current_name =
            current_name_index >= names.size()
          ? ""
          : names[current_name_index];
          
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
            cell_text += " |" + newline;
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
          + "===+"
          + newline
          + restore_cursor_position;

    client->get_connection()->write(text);
}

}

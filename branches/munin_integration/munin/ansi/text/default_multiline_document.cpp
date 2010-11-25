// ==========================================================================
// Munin Default Multi Line Text Document.
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
#include "munin/ansi/text/default_multiline_document.hpp"
#include <algorithm>
#include <functional>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>

using namespace odin;
using namespace std;
using namespace boost;

namespace munin { namespace ansi { namespace text {

namespace {
    typedef default_multiline_document::character_type character_type;
}

struct default_multiline_document::impl
{
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    impl()
        : width_(78)
    {
        reindex(0);
    }
    
    //* =====================================================================
    /// \brief Reindexes the document from the given line number.
    //* =====================================================================
    void reindex(u32 line)
    {
        // Remove all indexed line values from the selected line onwards.
        line_indices_.erase(
            line_indices_.begin() + line
          , line_indices_.end());
        
        // Ensure that there is at least one line.
        if (line_indices_.empty())
        {
            line_indices_.push_back(0);
        }
        
        BOOST_AUTO(
            current_line_index
          , line_indices_[line_indices_.size() - 1]);
        
        // Starting from the index of the last line, count to the end of
        // the document, caching each start of each line.
        for (u32 index = current_line_index; index < text_.size(); ++index)
        {
            if (text_[index].first == '\n')
            {
                line_indices_.push_back(index + 1);
                current_line_index = index + 1;
            }
            else if (index != current_line_index
                 && ((index - current_line_index) % width_ == 0))
            {
                line_indices_.push_back(index);
                current_line_index = index;
            }
        }
    }
    
    //* =====================================================================
    /// \brief Retrieves the length of the specified line.
    //* =====================================================================
    u32 get_line_length(u32 line)
    {
        // First case: if there is no such line, the length is 0.
        if (line >= line_indices_.size())
        {
            return 0;
        }
        // Next case: if this is the last line, then the length is the
        // difference between the end of the document and the start of this
        // line.
        else if (line == line_indices_.size() - 1)
        {
            return text_.size() - line_indices_[line];
        }
        // Finally, if it is any other line, then the length is the 
        // difference between the start indices of this line and the next.
        else
        {
            return line_indices_[line + 1] - line_indices_[line];
        }
    }
    
    //* =====================================================================
    /// \brief Retrieves a position from a specified index.
    //* =====================================================================
    munin::point position_from_index(u32 index)
    {
        // Simple and common case: if the index is on the last line.
        BOOST_AUTO(last_line, line_indices_.size() - 1);
        BOOST_AUTO(last_line_index, line_indices_[last_line]);
        
        munin::point caret_position;
        
        if (index >= last_line_index)
        {
            // First, clamp it to the end of the text at most.
            if (index > text_.size())
            {
                index = text_.size();
            }
            
            // Now, set the caret position appropriately.
            caret_position = munin::point(index - last_line_index, last_line);
        }
        else
        {
            // Search through the line indices to find the y position of the 
            // required index.
            u32 current_line = 0;
            
            for (; current_line < line_indices_.size(); ++current_line)
            {
                BOOST_AUTO(line_index, line_indices_[current_line]);

                if (line_index > index)
                {
                    // The required index was the previous line.
                    --current_line;
                    break;
                }
            }
            
            // Set the caret position appropriately.
            caret_position = munin::point(
                index - line_indices_[current_line]
              , current_line);
        }
        
        // Finally, correct for off-the-end.
        if (u32(caret_position.x) == width_)
        {
            caret_position.x = 0;
            ++caret_position.y;
        }
        
        return caret_position;
    }
    
    // The collection of text that we are displaying.
    vector<character_type> text_;
    
    // The indices at which each text line starts.
    vector<u32>            line_indices_;
    
    // The width of the document
    u32                    width_;
    
    // The current position at which the caret resides in the document.
    munin::point           caret_position_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
default_multiline_document::default_multiline_document()
  : pimpl_(new impl)
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
default_multiline_document::~default_multiline_document()
{
}

// ==========================================================================
// DO_SET_WIDTH
// ==========================================================================
void default_multiline_document::do_set_width(u32 width)
{
    // Re-indexing and redrawing a document is potentially a lot of work.
    // Avoid it if we can by only reindexing and redrawing if the width has
    // actually changed.
    if (pimpl_->width_ != width)
    {
        BOOST_AUTO(caret_index, get_caret_index());

        pimpl_->width_ = width;
    
        // This will require that the entire document is reindexed to
        // discover all the new line ends.
        pimpl_->reindex(0);
        
        // The entire document may well have changed.  Redraw it all.    
        munin::rectangle region(
            munin::point(0, 0)
          , munin::extent(get_width(), get_height()));
        vector<munin::rectangle> regions;
        regions.push_back(region);
        on_redraw(regions);
        
        // Update the caret position based on the caret index before the
        // resize.
        set_caret_index(caret_index);
    }
}

// ==========================================================================
// DO_GET_WIDTH
// ==========================================================================
u32 default_multiline_document::do_get_width() const
{
    return pimpl_->width_;
}

// ==========================================================================
// DO_SET_HEIGHT
// ==========================================================================
void default_multiline_document::do_set_height(u32 height)
{
    // Height is ignored.  It is always the number of lines in the document.
    // However, since this may indicate a caret position change (due to the
    // document being shrunk vertically, for example), update the caret
    // position.
    set_caret_position(get_caret_position());
}

// ==========================================================================
// DO_GET_HEIGHT
// ==========================================================================
u32 default_multiline_document::do_get_height() const
{
    return u32(pimpl_->line_indices_.size());
}

// ==========================================================================
// DO_SET_CARET_POSITION
// ==========================================================================
void default_multiline_document::do_set_caret_position(munin::point const& pt)
{
    // The new position can go no lower than the end of the current document.
    pimpl_->caret_position_.y = 
        (min)(u32(pt.y), u32(pimpl_->line_indices_.size() - 1));
    
    // The new x-position can go no further than the space after the last
    // character on this line.
    BOOST_AUTO(
        line_length
      , pimpl_->get_line_length(pimpl_->caret_position_.y));
    
    pimpl_->caret_position_.x =
        (min)(u32(pt.x), u32(line_length));
}

// ==========================================================================
// DO_GET_CARET_POSITION
// ==========================================================================
munin::point default_multiline_document::do_get_caret_position() const
{
    return pimpl_->caret_position_;
}

// ==========================================================================
// DO_SET_CARET_INDEX
// ==========================================================================
void default_multiline_document::do_set_caret_index(u32 index)
{
    pimpl_->caret_position_ = pimpl_->position_from_index(index);
}

// ==========================================================================
// DO_GET_CARET_INDEX
// ==========================================================================
u32 default_multiline_document::do_get_caret_index() const
{
    BOOST_AUTO(caret_position, get_caret_position());
    
    return u32(caret_position.y) >= u32(pimpl_->line_indices_.size())
         ? u32(pimpl_->text_.size())
         : (min)(
               u32(pimpl_->line_indices_[caret_position.y] + caret_position.x)
             , u32(pimpl_->text_.size()));
}

// ==========================================================================
// DO_GET_TEXT_SIZE
// ==========================================================================
u32 default_multiline_document::do_get_text_size() const
{
    return pimpl_->text_.size();
}

// ==========================================================================
// DO_INSERT_TEXT
// ==========================================================================
void default_multiline_document::do_insert_text(
    runtime_array<character_type> const& text
  , optional<u32>                        index)
{
    // Get the current caret position and index.
    BOOST_AUTO(caret_position, get_caret_position());
    BOOST_AUTO(caret_index,    get_caret_index());
    BOOST_AUTO(insert_index,   caret_index);
    
    // The index at which we should insert is either the caret index if
    // the optional index passed in was uninitialised, or the specified
    // index if it was initialised.
    if (index.is_initialized())
    {
        insert_index = index.get();
        
        if (insert_index > pimpl_->text_.size())
        {
            insert_index = pimpl_->text_.size();
        }
    }
    
    BOOST_AUTO(insert_position, pimpl_->position_from_index(insert_index));
    
    // First, strip any '\r' or non-printable characters out of the
    // text.  We leave in '\n's because they serve as end-of-line markers.
    vector<character_type> stripped_text;
    stripped_text.reserve(text.size());
    
    BOOST_FOREACH(character_type ch, text)
    {
        if (isprint(ch.first) || ch.first == '\n')
        {
            stripped_text.push_back(ch);
        }
    }
    
    // Next, insert the text into the view at the current cursor index.
    pimpl_->text_.insert(
        pimpl_->text_.begin() + insert_index
      , stripped_text.begin()
      , stripped_text.end());
    
    // This will require a reindexing of the document from the current insert
    // row.
    pimpl_->reindex(insert_position.y);
    
    // If the caret index was or was to the right of the insert index, then
    // the index needs to be moved on by the number of characters that were 
    // added.
    if (caret_index >= insert_index)
    {
        set_caret_index(caret_index + stripped_text.size());
    }
    
    // Finally, since this could affect all regions from the insert row 
    // downwards, schedule those portions of the document for a redraw.    
    vector<munin::rectangle> regions;
    regions.push_back(munin::rectangle(
        munin::point(0, insert_position.y)
      , munin::extent(get_width(), get_height())));
    
    on_redraw(regions);
}

// ==========================================================================
// DO_DELETE_TEXT 
// ==========================================================================
void default_multiline_document::do_delete_text(pair<u32, u32> range)
{
    // Arrange the range in a start->end order.
    if (range.first > range.second)
    {
        swap(range.first, range.second);
    }
    
    // Discard nonexistent ranges.
    if (range.first >= u32(pimpl_->text_.size()))
    {
        return;
    }
    
    // Obtain the position of the start of the range.  We will need to reindex
    // from here later.
    BOOST_AUTO(range_start_position, pimpl_->position_from_index(range.first)); 
    
    // Obtain the caret index, in case that has to move later. 
    BOOST_AUTO(caret_index, get_caret_index());
    
    // Erase the text in question.
    pimpl_->text_.erase(
        pimpl_->text_.begin() + range.first
      , pimpl_->text_.begin() + range.second);
    
    // Reindex from the start of the deleted second.
    pimpl_->reindex(range_start_position.y);
    
    // Move the caret if necessary.
    if (caret_index > range.first && caret_index <= range.second)
    {
        set_caret_index(range.first);
    }
    else if (caret_index > range.second)
    {
        set_caret_index(caret_index - (range.second - range.first));
    }
    
    // Finally, notify that the document has changed.
    munin::rectangle region(
        munin::point(0, range_start_position.y)
      , munin::extent(
          pimpl_->width_
        , pimpl_->line_indices_.size() - range_start_position.y));
    vector<munin::rectangle> regions;
    regions.push_back(region);
    on_redraw(regions);
}

// ==========================================================================
// DO_GET_NUMBER_OF_LINES
// ==========================================================================
u32 default_multiline_document::do_get_number_of_lines() const
{
    return get_height();
}

// ==========================================================================
// DO_GET_TEXT_LINE
// ==========================================================================
runtime_array<character_type> 
    default_multiline_document::do_get_text_line(u32 index) const
{
    // Special case: if the text area is empty, return an empty array.
    if (pimpl_->text_.empty())
    {
        return runtime_array<character_type>();
    }

    if (index < pimpl_->line_indices_.size())
    {
        // The beginning point of the selected line is already cached into
        // the view.
        BOOST_AUTO(line_begin, pimpl_->line_indices_[index]);
        u32 line_length = 0;
        
        // Count forwards until we either move onto the next line or hit a \n.
        while (line_length < pimpl_->width_
           &&  line_begin + line_length < pimpl_->text_.size()
           &&  pimpl_->text_[line_begin + line_length].first != '\n')
        {
            ++line_length;
        }
        
        // Return the array of characters from the discovered line.
        return runtime_array<character_type>(
            &*pimpl_->text_.begin() + line_begin
          , line_length);
    }
    else
    {
        // There was no such line.  Return an empty one.
        return runtime_array<character_type>();
    }
}

}}}

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
#include "munin/text/default_multiline_document.hpp"
#include "terminalpp/string.hpp"
#include <algorithm>
#include <functional>
#include <vector>

namespace munin { namespace text {

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
    void reindex(odin::u32 line)
    {
        // Ignore this if the width is 0.
        if (width_ == 0)
        {
            return;
        }

        // Remove all indexed line values from the selected line onwards.
        line_indices_.erase(
            line_indices_.begin() + line
          , line_indices_.end());

        // Ensure that there is at least one line.
        if (line_indices_.empty())
        {
            line_indices_.push_back(0);
        }

        auto current_line_index = line_indices_[line_indices_.size() - 1];

        // Starting from the index of the last line, count to the end of
        // the document, caching each start of each line.
        for (odin::u32 index = current_line_index; index < text_.size(); ++index)
        {
            if (text_[index].glyph_.character_ == '\n')
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
    odin::u32 get_line_length(odin::u32 line)
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
    terminalpp::point position_from_index(odin::u32 index)
    {
        // Simple and common case: if the index is on the last line.
        auto last_line = line_indices_.size() - 1;
        auto last_line_index = line_indices_[last_line];

        terminalpp::point caret_position;

        if (index >= last_line_index)
        {
            // First, clamp it to the end of the text
            // (+1 for the invisible blank) at most.
            if (index > text_.size() + 1)
            {
                index = text_.size();
            }

            // Now, set the caret position appropriately.
            caret_position = terminalpp::point(index - last_line_index, last_line);
        }
        else
        {
            // Search through the line indices to find the y position of the
            // required index.
            odin::u32 current_line = 0;

            for (; current_line < line_indices_.size(); ++current_line)
            {
                auto line_index = line_indices_[current_line];

                if (line_index > index)
                {
                    // The required index was the previous line.
                    --current_line;
                    break;
                }
            }

            // Set the caret position appropriately.
            caret_position = terminalpp::point(
                index - line_indices_[current_line]
              , current_line);
        }

        // Finally, correct for off-the-end.
        if (odin::u32(caret_position.x) == width_)
        {
            caret_position.x = 0;
            ++caret_position.y;
        }

        return caret_position;
    }

    // The collection of text that we are displaying.
    std::vector<terminalpp::element> text_;

    // The indices at which each text line starts.
    std::vector<odin::u32>            line_indices_;

    // The width of the document
    odin::u32                         width_;

    // The current position at which the caret resides in the document.
    terminalpp::point                 caret_position_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
default_multiline_document::default_multiline_document()
  : pimpl_(std::make_shared<impl>())
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
default_multiline_document::~default_multiline_document()
{
}

// ==========================================================================
// DO_SET_SIZE
// ==========================================================================
void default_multiline_document::do_set_size(terminalpp::extent size)
{
    // Re-indexing and redrawing a document is potentially a lot of work.
    // Avoid it if we can by only reindexing and redrawing if the width has
    // actually changed.
    if (pimpl_->width_ != odin::u32(size.width))
    {
        auto caret_index = get_caret_index();

        pimpl_->width_ = size.width;

        // This will require that the entire document is reindexed to
        // discover all the new line ends.
        pimpl_->reindex(0);

        // The entire document may well have changed.  Redraw it all.
        on_redraw({munin::rectangle({}, get_size())});

        // Update the caret position based on the caret index before the
        // resize.
        set_caret_index(caret_index);
    }

    // Height is ignored.  It is always the number of lines in the document.
    // However, since this may indicate a caret position change (due to the
    // document being shrunk vertically, for example), update the caret
    // position.
    if (pimpl_->line_indices_.size() != odin::u32(size.height))
    {
        set_caret_position(get_caret_position());
    }
}

// ==========================================================================
// DO_GET_SIZE
// ==========================================================================
terminalpp::extent default_multiline_document::do_get_size() const
{
    return terminalpp::extent(pimpl_->width_, pimpl_->line_indices_.size());
}

// ==========================================================================
// DO_SET_CARET_POSITION
// ==========================================================================
void default_multiline_document::do_set_caret_position(
    terminalpp::point const& pt)
{
    // The new position can go no lower than the end of the current document.
    pimpl_->caret_position_.y =
        (std::min)(odin::u32(pt.y), odin::u32(pimpl_->line_indices_.size() - 1));

    // The new x-position can go no further than the space after the last
    // character on this line.
    auto line_length =
      pimpl_->get_line_length(pimpl_->caret_position_.y);

    if (line_length != 0)
    {
        pimpl_->caret_position_.x =
            (std::min)(odin::u32(pt.x), odin::u32(line_length));
    }
    else
    {
        pimpl_->caret_position_.x = 0;
    }
}

// ==========================================================================
// DO_GET_CARET_POSITION
// ==========================================================================
terminalpp::point default_multiline_document::do_get_caret_position() const
{
    return pimpl_->caret_position_;
}

// ==========================================================================
// DO_SET_CARET_INDEX
// ==========================================================================
void default_multiline_document::do_set_caret_index(odin::u32 index)
{
    pimpl_->caret_position_ = pimpl_->position_from_index(index);
}

// ==========================================================================
// DO_GET_CARET_INDEX
// ==========================================================================
odin::u32 default_multiline_document::do_get_caret_index() const
{
    auto caret_position = get_caret_position();

    return odin::u32(caret_position.y) >= odin::u32(pimpl_->line_indices_.size())
      ? odin::u32(pimpl_->text_.size())
      : (std::min)(
            odin::u32(pimpl_->line_indices_[caret_position.y] + caret_position.x),
            odin::u32(pimpl_->text_.size()));
}

// ==========================================================================
// DO_GET_TEXT_SIZE
// ==========================================================================
odin::u32 default_multiline_document::do_get_text_size() const
{
    return pimpl_->text_.size();
}

// ==========================================================================
// DO_INSERT_TEXT
// ==========================================================================
void default_multiline_document::do_insert_text(
    terminalpp::string const  &text,
    boost::optional<odin::u32> index)
{
    // Get the current caret position and index.
    auto caret_index = get_caret_index();
    auto insert_index = caret_index;

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

    auto insert_position = pimpl_->position_from_index(insert_index);

    // First, strip any non-printable characters out of the
    // text.  We leave in '\n's because they serve as end-of-line markers.
    std::vector<terminalpp::element> stripped_text;
    stripped_text.reserve(text.size());

    for (auto const &elem : text)
    {
        if (is_printable(elem.glyph_))
        {
            stripped_text.push_back(elem);
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
    on_redraw({munin::rectangle(
        terminalpp::point(0, insert_position.y), get_size())});
}

// ==========================================================================
// DO_SET_TEXT
// ==========================================================================
void default_multiline_document::do_set_text(terminalpp::string const &text)
{
    auto old_size = get_size();

    pimpl_->text_ = {text.begin(), text.end()};

    // Re-index from the start.
    pimpl_->reindex(0);

    // Re-set the caret position so that it snaps to the new text.
    set_caret_index(get_caret_index());

    // Schedule a redraw for the union of the old and new documents.
    auto new_size = get_size();

    on_redraw({munin::rectangle(
        terminalpp::point(0, 0)
      , terminalpp::extent(
          (std::max)(old_size.width, new_size.width),
          (std::max)(old_size.height, new_size.height)))});
}

// ==========================================================================
// DO_DELETE_TEXT
// ==========================================================================
void default_multiline_document::do_delete_text(std::pair<odin::u32, odin::u32> range)
{
    // Arrange the range in a start->end order.
    if (range.first > range.second)
    {
        using std::swap;
        swap(range.first, range.second);
    }

    // Discard nonexistent ranges.
    if (range.first >= odin::u32(pimpl_->text_.size()))
    {
        return;
    }

    // Obtain the position of the start of the range.  We will need to reindex
    // from here later.
    auto range_start_position = pimpl_->position_from_index(range.first);

    // Obtain the caret index, in case that has to move later.
    auto caret_index = get_caret_index();

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
    on_redraw({munin::rectangle(
        terminalpp::point(0, range_start_position.y)
      , terminalpp::extent(
          pimpl_->width_
        , pimpl_->line_indices_.size() - range_start_position.y))});
}

// ==========================================================================
// DO_GET_NUMBER_OF_LINES
// ==========================================================================
odin::u32 default_multiline_document::do_get_number_of_lines() const
{
    return odin::u32(pimpl_->line_indices_.size());
}

// ==========================================================================
// DO_GET_LINE
// ==========================================================================
terminalpp::string default_multiline_document::do_get_line(
    odin::u32 index) const
{
    // Special case: if the text area is empty, return an empty array.
    if (pimpl_->text_.empty())
    {
        return {};
    }

    if (index < pimpl_->line_indices_.size())
    {
        // The beginning point of the selected line is already cached into
        // the view.
        auto line_begin = pimpl_->line_indices_[index];
        odin::u32 line_length = 0;

        // Count forwards until we either move onto the next line or hit a \n.
        while (line_length < pimpl_->width_
           &&  line_begin + line_length < pimpl_->text_.size()
           &&  pimpl_->text_[line_begin + line_length].glyph_.character_ != '\n')
        {
            ++line_length;
        }

        // Return the array of characters from the discovered line.
        return terminalpp::string(
            pimpl_->text_.begin() + line_begin
          , pimpl_->text_.begin() + line_begin + line_length);
    }
    else
    {
        // There was no such line.  Return an empty one.
        return {};
    }
}

}}


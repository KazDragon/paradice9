// ==========================================================================
// Munin Default Single Line Text Document.
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
#include "munin/text/default_singleline_document.hpp"
#include "terminalpp/string.hpp"
#include <algorithm>
#include <functional>
#include <vector>

namespace munin { namespace text {

struct default_singleline_document::impl
{
    impl()
      : caret_index_(0)
    {
    }

    std::vector<terminalpp::element> text_;
    odin::u32                        caret_index_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
default_singleline_document::default_singleline_document()
  : pimpl_(std::make_shared<impl>())
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
default_singleline_document::~default_singleline_document()
{
}

// ==========================================================================
// DO_SET_SIZE
// ==========================================================================
void default_singleline_document::do_set_size(terminalpp::extent)
{
    // This is ignored.
}

// ==========================================================================
// DO_GET_SIZE
// ==========================================================================
terminalpp::extent default_singleline_document::do_get_size() const
{
    return terminalpp::extent(odin::u32(pimpl_->text_.size()), odin::u32(1));
}

// ==========================================================================
// DO_SET_CARET_POSITION
// ==========================================================================
void default_singleline_document::do_set_caret_position(
    terminalpp::point const& pt)
{
    pimpl_->caret_index_ = (std::min)(
        odin::u32(pimpl_->text_.size()), odin::u32(pt.x));
}

// ==========================================================================
// DO_GET_CARET_POSITION
// ==========================================================================
terminalpp::point default_singleline_document::do_get_caret_position() const
{
    return terminalpp::point(pimpl_->caret_index_, 0);
}

// ==========================================================================
// DO_SET_CARET_INDEX
// ==========================================================================
void default_singleline_document::do_set_caret_index(odin::u32 index)
{
    pimpl_->caret_index_ = (std::min)(
        odin::u32(pimpl_->text_.size()), index);
}

// ==========================================================================
// DO_GET_CARET_INDEX
// ==========================================================================
odin::u32 default_singleline_document::do_get_caret_index() const
{
    return pimpl_->caret_index_;
}

// ==========================================================================
// DO_GET_TEXT_SIZE
// ==========================================================================
odin::u32 default_singleline_document::do_get_text_size() const
{
    return pimpl_->text_.size();
}

// ==========================================================================
// DO_INSERT_TEXT
// ==========================================================================
void default_singleline_document::do_insert_text(
    terminalpp::string const  &text
  , boost::optional<odin::u32> index)
{
    auto old_index = pimpl_->caret_index_;

    // This is a single-line control, so we remove any \ns first.
    std::vector<terminalpp::element> stripped_text;
    stripped_text.reserve(text.size());

    for (auto const &elem : text)
    {
        if (elem.glyph_.character_ != '\n')
        {
            stripped_text.push_back(elem);
        }
    }

    pimpl_->text_.insert(
        pimpl_->text_.begin() + pimpl_->caret_index_
      , stripped_text.begin()
      , stripped_text.end());

    set_caret_index(get_caret_index() + stripped_text.size());

    on_redraw({rectangle(
        terminalpp::point(old_index, 0)
      , terminalpp::extent(odin::s32(text.size()), 0))});
}

// ==========================================================================
// DO_DELETE_TEXT
// ==========================================================================
void default_singleline_document::do_delete_text(std::pair<odin::u32, odin::u32> range)
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

    // Obtain the caret index, in case that has to move later.
    auto caret_index = get_caret_index();

    // Erase the text in question.
    pimpl_->text_.erase(
        pimpl_->text_.begin() + range.first
      , pimpl_->text_.begin() + range.second);

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
    on_redraw({rectangle(
        terminalpp::point(range.first, 0)
      , terminalpp::extent(pimpl_->text_.size() - range.first, 1))});
}

// ==========================================================================
// DO_SET_TEXT
// ==========================================================================
void default_singleline_document::do_set_text(terminalpp::string const &text)
{
    // We are going to need to redraw a square that is the max of the two
    // document sizes.
    auto old_size = pimpl_->text_.size();

    pimpl_->text_ = {text.begin(), text.end()};

    // If the caret index has gone off the end, this will cause it to shrink
    // back to where it's sensible.
    set_caret_index(get_caret_index());

    // Finally, notify that the document has changed.
    on_redraw({rectangle(
        terminalpp::point(0, 0)
      , terminalpp::extent((std::max)(old_size, text.size()), 1))});
}

// ==========================================================================
// DO_GET_NUMBER_OF_LINES
// ==========================================================================
odin::u32 default_singleline_document::do_get_number_of_lines() const
{
    return 1;
}

// ==========================================================================
// DO_GET_LINE
// ==========================================================================
terminalpp::string default_singleline_document::do_get_line(
    odin::u32 index) const
{
    return {pimpl_->text_.begin(), pimpl_->text_.end()};
}

}}


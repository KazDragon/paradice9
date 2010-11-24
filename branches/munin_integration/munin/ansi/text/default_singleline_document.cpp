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
#include "munin/ansi/text/default_singleline_document.hpp"
#include <algorithm>
#include <functional>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>

using namespace odin;
using namespace std;

namespace munin { namespace ansi { namespace text {

struct default_singleline_document::impl
{
    impl()
      : caret_index_(0)
    {
    }

    vector<default_singleline_document::character_type> text_;
    boost::optional<u32>                                selection_index_;
    u32                                                 caret_index_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
default_singleline_document::default_singleline_document()
  : pimpl_(new impl)
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
default_singleline_document::~default_singleline_document()
{
}

// ==========================================================================
// DO_SET_WIDTH
// ==========================================================================
void default_singleline_document::do_set_width(u32 width)
{
    // Width is ignored.  The width is the length of the document.
}

// ==========================================================================
// DO_GET_WIDTH
// ==========================================================================
u32 default_singleline_document::do_get_width() const
{
    return u32(pimpl_->text_.size());
}

// ==========================================================================
// DO_SET_HEIGHT
// ==========================================================================
void default_singleline_document::do_set_height(u32 height)
{
    // Height is ignored.  It is always 1.
}

// ==========================================================================
// DO_GET_HEIGHT
// ==========================================================================
u32 default_singleline_document::do_get_height() const
{
    return u32(1);
}

// ==========================================================================
// DO_SET_CARET_POSITION
// ==========================================================================
void default_singleline_document::do_set_caret_position(munin::point const& pt)
{
    pimpl_->caret_index_ = (min)(u32(pimpl_->text_.size()), u32(pt.x));
}

// ==========================================================================
// DO_GET_CARET_POSITION
// ==========================================================================
munin::point default_singleline_document::do_get_caret_position() const
{
    return munin::point(pimpl_->caret_index_, 0);
}

// ==========================================================================
// DO_SET_CARET_INDEX
// ==========================================================================
void default_singleline_document::do_set_caret_index(u32 index)
{
    pimpl_->caret_index_ = (min)(u32(pimpl_->text_.size()), index);
}

// ==========================================================================
// DO_GET_CARET_INDEX
// ==========================================================================
u32 default_singleline_document::do_get_caret_index() const
{
    return pimpl_->caret_index_;
}

// ==========================================================================
// DO_SELECT_TEXT 
// ==========================================================================
void default_singleline_document::do_select_text(
    odin::u32                  from
  , boost::optional<odin::u32> to)
{
    set_caret_index(from);
    
    if (to.is_initialized())
    {
        pimpl_->selection_index_ = (max)(u32(pimpl_->text_.size()), *to);
    }
    else
    {
        pimpl_->selection_index_ = u32(pimpl_->text_.size()); 
    }
}
    
// ==========================================================================
// DO_GET_SELECTED_TEXT_REGION 
// ==========================================================================
boost::optional< std::pair<odin::u32, odin::u32> >
    default_singleline_document::do_get_selected_text_region() const
{
    return boost::optional< std::pair<odin::u32, odin::u32> >();
}

// ==========================================================================
// DO_INSERT_TEXT
// ==========================================================================
void default_singleline_document::do_insert_text(
    runtime_array<character_type> const& text)
{
    BOOST_AUTO(old_index, pimpl_->caret_index_);
    
    // This is a single-line control, so we remove any \ns or \rs first.
    vector<character_type> stripped_text;
    stripped_text.reserve(text.size());

    BOOST_FOREACH(character_type ch, text)
    {
        if (ch.first != '\n' && ch.first != '\r')
        {
            stripped_text.push_back(ch);
        }
    }
    
    pimpl_->text_.insert(
        pimpl_->text_.begin() + pimpl_->caret_index_
      , stripped_text.begin()
      , stripped_text.end());

    set_caret_index(get_caret_index() + stripped_text.size());
    
    vector<munin::rectangle> regions;
    regions.push_back(munin::rectangle(
        munin::point(old_index, 0), munin::extent(s32(text.size()), 0)));
    on_redraw(regions);
}

// ==========================================================================
// DO_DELETE_TEXT 
// ==========================================================================
void default_singleline_document::do_delete_text()
{
    if (pimpl_->selection_index_)
    {
    }
    else if (pimpl_->caret_index_ != 0)
    {
        pimpl_->text_.erase(
            pimpl_->text_.begin() + (pimpl_->caret_index_ - 1)
          , pimpl_->text_.begin() + pimpl_->caret_index_);
        set_caret_index(pimpl_->caret_index_ - 1);
        
        vector<munin::rectangle> regions;
        regions.push_back(munin::rectangle(
            munin::point(pimpl_->caret_index_, 0)
          , munin::extent(pimpl_->text_.size() - pimpl_->caret_index_, 0)));
        on_redraw(regions);
    }
}

// ==========================================================================
// DO_GET_NUMBER_OF_LINES
// ==========================================================================
u32 default_singleline_document::do_get_number_of_lines() const
{
    return 1;
}

// ==========================================================================
// DO_GET_TEXT_LINE
// ==========================================================================
odin::runtime_array<default_singleline_document::character_type> 
    default_singleline_document::do_get_text_line(u32 index) const
{
    return odin::runtime_array<character_type>(
        &*pimpl_->text_.begin()
      , pimpl_->text_.size());
}

}}}

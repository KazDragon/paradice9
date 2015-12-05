// ==========================================================================
// Munin Text Document.
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
#include "munin/text/document.hpp"
#include "terminalpp/string.hpp"

namespace munin { namespace text {

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
document::~document()
{
}

// ==========================================================================
// SET_SIZE
// ==========================================================================
void document::set_size(terminalpp::extent size)
{
    do_set_size(size);
    on_caret_position_changed();
}

// ==========================================================================
// GET_SIZE
// ==========================================================================
terminalpp::extent document::get_size() const
{
    return do_get_size();
}

// ==========================================================================
// SET_CARET_POSITION
// ==========================================================================
void document::set_caret_position(terminalpp::point const& pt)
{
    do_set_caret_position(pt);
    on_caret_position_changed();
}

// ==========================================================================
// GET_CARET_POSITION
// ==========================================================================
terminalpp::point document::get_caret_position() const
{
    return do_get_caret_position();
}

// ==========================================================================
// SET_CARET_INDEX
// ==========================================================================
void document::set_caret_index(odin::u32 index)
{
    do_set_caret_index(index);
    on_caret_position_changed();
}

// ==========================================================================
// GET_CARET_INDEX
// ==========================================================================
odin::u32 document::get_caret_index() const
{
    return do_get_caret_index();
}

// ==========================================================================
// GET_TEXT_SIZE
// ==========================================================================
odin::u32 document::get_text_size() const
{
    return do_get_text_size();
}

// ==========================================================================
// INSERT_TEXT
// ==========================================================================
void document::insert_text(
    terminalpp::string const  &text
  , boost::optional<odin::u32> index)
{
    do_insert_text(text, index);
}

// ==========================================================================
// DELETE_TEXT
// ==========================================================================
void document::delete_text(std::pair<odin::u32, odin::u32> range)
{
    do_delete_text(range);
}

// ==========================================================================
// SET_TEXT
// ==========================================================================
void document::set_text(terminalpp::string const &text)
{
    do_set_text(text);
}

// ==========================================================================
// GET_NUMBER_OF_LINES
// ==========================================================================
odin::u32 document::get_number_of_lines() const
{
    return do_get_number_of_lines();
}

// ==========================================================================
// GET_LINE
// ==========================================================================
terminalpp::string document::get_line(odin::u32 index) const
{
    return do_get_line(index);
}

}}

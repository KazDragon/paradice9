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
#include "munin/ansi/text/default_singleline_document.hpp"

using namespace odin;

namespace munin { namespace ansi { namespace text {

// ==========================================================================
// DO_SET_WIDTH
// ==========================================================================
void default_singleline_document::do_set_width(u32 width)
{
}

// ==========================================================================
// DO_GET_WIDTH
// ==========================================================================
u32 default_singleline_document::do_get_width()
{
    return 0;
}

// ==========================================================================
// DO_SET_HEIGHT
// ==========================================================================
void default_singleline_document::do_set_height(u32 height)
{
}

// ==========================================================================
// DO_GET_HEIGHT
// ==========================================================================
u32 default_singleline_document::do_get_height()
{
    return 0;
}

// ==========================================================================
// DO_SET_CARET_POSITION
// ==========================================================================
void default_singleline_document::do_set_caret_position(munin::point const& pt)
{
}

// ==========================================================================
// DO_GET_CARET_POSITION
// ==========================================================================
munin::point default_singleline_document::do_get_caret_position()
{
    return munin::point();
}

// ==========================================================================
// DO_SET_CARET_INDEX
// ==========================================================================
void default_singleline_document::do_set_caret_index(u32 index)
{
}

// ==========================================================================
// DO_GET_CARET_INDEX
// ==========================================================================
u32 default_singleline_document::do_get_caret_index()
{
    return 0;
}

// ==========================================================================
// DO_INSERT_TEXT
// ==========================================================================
void default_singleline_document::do_insert_text(
    runtime_array<character_type> const& text)
{
}

}}}

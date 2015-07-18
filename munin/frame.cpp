// ==========================================================================
// Munin Frame.
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
#include "munin/frame.hpp"

namespace munin {

// ==========================================================================
// GET_TOP_BORDER_HEIGHT
// ==========================================================================
odin::s32 frame::get_top_border_height() const
{
    return do_get_top_border_height();
}

// ==========================================================================
// GET_BOTTOM_BORDER_HEIGHT
// ==========================================================================
odin::s32 frame::get_bottom_border_height() const
{
    return do_get_bottom_border_height();
}

// ==========================================================================
// GET_LEFT_BORDER_WIDTH
// ==========================================================================
odin::s32 frame::get_left_border_width() const
{
    return do_get_left_border_width();
}

// ==========================================================================
// GET_RIGHT_BORDER_WIDTH
// ==========================================================================
odin::s32 frame::get_right_border_width() const
{
    return do_get_right_border_width();
}

}

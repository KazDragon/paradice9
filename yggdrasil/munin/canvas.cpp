// ==========================================================================
// Munin Canvas.
//
// Copyright (C) 2014 Matthew Chaplain, All Rights Reserved.
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
#include "yggdrasil/munin/canvas.hpp"

namespace yggdrasil { namespace munin {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
canvas::canvas()
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
canvas::canvas(extent size)
  : size_(size)
{
    elements_.resize(size.width * size.height);
}

// ==========================================================================
// SET_SIZE
// ==========================================================================
void canvas::set_size(extent const& size)
{
    size_ = size;
    elements_.resize(size.width * size.height);
}

// ==========================================================================
// GET_SIZE
// ==========================================================================
extent canvas::get_size() const
{
    return size_;
}

// ==========================================================================
// OFFSET_BY
// ==========================================================================
void canvas::offset_by(const extent& offset)
{
    offset_ += offset;
}

// ==========================================================================
// GET_OFFSET
// ==========================================================================
extent canvas::get_offset() const
{
    return offset_;
}

// ==========================================================================
// OPERATOR[]
// ==========================================================================
detail::column_proxy canvas::operator[](yggdrasil::s32 index)
{
    return detail::column_proxy(elements_, offset_, size_.width, index);
}

namespace detail {
    
// ==========================================================================
// COLUMN_PROXY CONSTRUCTOR
// ==========================================================================
column_proxy::column_proxy(
    std::vector<element> &elements,
    extent offset,
    yggdrasil::s32 width,
    yggdrasil::s32 column)
  : elements_(elements),
    offset_(offset),
    width_(width),
    column_(column)
{

}

// ==========================================================================
// COLUMN_PROXY OPERATOR[]
// ==========================================================================
element& column_proxy::operator[](int index)
{
    return elements_[
        (width_ * (index + offset_.height)) // Y-coordinate
      + (column_ + offset_.width)];         // X-coordinate
}

}

}}

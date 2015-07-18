// ==========================================================================
// Munin Filled Box.
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
#include "yggdrasil/munin/filled_box.hpp"
#include "yggdrasil/munin/canvas.hpp"
#include "yggdrasil/munin/element.hpp"
#include "yggdrasil/munin/model.hpp"
#include "yggdrasil/munin/rectangle.hpp"

namespace yggdrasil { namespace munin {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
filled_box::filled_box()
  : filled_box(element(' '))
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
filled_box::filled_box(element brush)
  : model_(filled_box_model(brush))
{
}

// ==========================================================================
// DRAW
// ==========================================================================
void filled_box::draw(canvas& cvs, const rectangle& region) const
{
    element const &brush = model_.brush_;

    for (auto y = region.origin.y;
         y < region.origin.y + region.size.height;
         ++y)
    {
        for (auto x = region.origin.x;
             x < region.origin.x + region.size.width;
             ++x)
        {
            cvs[x][y] = brush;
        }
    }
}

// ==========================================================================
// GET_MODEL
// ==========================================================================
filled_box::filled_box_model &filled_box::get_model()
{
    return model_;
}

// ==========================================================================
// FILLED_BOX_MODEL CONSTRUCTOR
// ==========================================================================
filled_box::filled_box_model::filled_box_model(element brush)
  : brush_(brush)
{
}

// ==========================================================================
// FILLED_BOX_MODEL EVENT
// ==========================================================================
boost::any filled_box::filled_box_model::event(boost::any const &event)
{
    return nullptr;
}

// ==========================================================================
// FILLED_BOX_MODEL GET_SIZE
// ==========================================================================
extent const &filled_box::filled_box_model::get_size() const
{
    return size_;
}

// ==========================================================================
// FILLED_BOX_MODEL SET_SIZE
// ==========================================================================
void filled_box::filled_box_model::set_size(extent const &size)
{
    size_ = size;
}

// ==========================================================================
// FILLED_BOX_MODEL GET_PREFERRED_SIZE
// ==========================================================================
extent const &filled_box::filled_box_model::get_preferred_size() const
{
    static extent const preferred_size = {1, 1};
    return preferred_size;
}

}}

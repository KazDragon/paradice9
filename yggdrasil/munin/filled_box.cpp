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
#include "yggdrasil/munin/basic_model.hpp"
#include "yggdrasil/munin/canvas.hpp"
#include "yggdrasil/munin/element.hpp"
#include "yggdrasil/munin/ptree_model.hpp"
#include "yggdrasil/munin/rectangle.hpp"
#include "yggdrasil/munin/resource_manager.hpp"

namespace yggdrasil { namespace munin {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
filled_box::filled_box()
  : filled_box(get_resource_manager().get_default_properties("filled_box"))
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
filled_box::filled_box(boost::property_tree::ptree const &tree)
  : model_(create_model_from_ptree<basic_model>(tree))
{
}

// ==========================================================================
// DRAW
// ==========================================================================
void filled_box::draw(canvas& cvs, const rectangle& region) const
{
    auto brush = boost::any_cast<element>(model_.get_property("brush"));

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
model const &filled_box::get_model() const
{
    return model_;
}

// ==========================================================================
// GET_MODEL
// ==========================================================================
model &filled_box::get_model()
{
    return model_;
}

}}

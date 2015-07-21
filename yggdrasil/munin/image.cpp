// ==========================================================================
// Munin Image.
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
#include "yggdrasil/munin/image.hpp"
#include "yggdrasil/munin/basic_model.hpp"
#include "yggdrasil/munin/canvas.hpp"
#include "yggdrasil/munin/model.hpp"
#include "yggdrasil/munin/ptree_model.hpp"
#include "yggdrasil/munin/rectangle.hpp"
#include "yggdrasil/munin/resource_manager.hpp"
#include <boost/property_tree/ptree.hpp>
#include <utility>

namespace yggdrasil { namespace munin {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
image::image()
  : image(get_resource_manager().get_default_properties("image"))
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
image::image(boost::property_tree::ptree const &properties)
  : model_(create_model_from_ptree<basic_model>(properties))
{
}

// ==========================================================================
// DRAW
// ==========================================================================
void image::draw(canvas& cvs, const rectangle& region) const
{
    auto const &size = get_size(model_);
    auto background_brush = boost::any_cast<estring>(
        get_property(model_, "background_brush"));
    auto value = boost::any_cast<std::vector<estring>>(
        get_property(model_, "value"));
    
    for (auto row = region.origin.y;
         row < region.origin.y + region.size.height;
         ++row)
    {
        for (auto column = region.origin.x;
             column < region.origin.x + region.size.height;
             ++column)
        {
            if (row < value.size() && column < value[row].size())
            {
                cvs[column][row] = value[row][column];
            }
            else
            {
                cvs[column][row] = background_brush[0];
            }
        }
    }
}

// ==========================================================================
// GET_MODEL
// ==========================================================================
model& image::get_model()
{
    return model_;
}

}}
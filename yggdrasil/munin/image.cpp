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
#include "yggdrasil/munin/model.hpp"
#include "yggdrasil/munin/rectangle.hpp"
#include "yggdrasil/munin/canvas.hpp"
#include <utility>

namespace yggdrasil { namespace munin {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
image::image()
  : model_({})
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
image::image(std::vector<estring> im)
  : model_(std::move(im))
{
}

// ==========================================================================
// DRAW
// ==========================================================================
void image::draw(canvas& cvs, const rectangle& region) const
{
    auto const &size = model_.get_size();
    auto const &im   = model_.get_image();

    for (auto row = region.origin.y;
         row < region.origin.y + region.size.height;
         ++row)
    {
        for (auto column = region.origin.x;
             column < region.origin.x + region.size.height;
             ++column)
        {
            cvs[column][row] = im[row][column];
        }
    }
}

// ==========================================================================
// GET_MODEL
// ==========================================================================
image::image_model& image::get_model()
{
    return model_;
}

// ==========================================================================
// IMAGE_MODEL CONSTRUCTOR
// ==========================================================================
image::image_model::image_model(std::vector<estring> im)
  : image_(std::move(im))
{

}

// ==========================================================================
// IMAGE_MODEL EVENT
// ==========================================================================
boost::any image::image_model::event(const boost::any& event)
{
    return nullptr;
}

// ==========================================================================
// IMAGE_MODEL SET_SIZE
// ==========================================================================
void image::image_model::set_size(const extent& size)
{
    size_ = size;
}

// ==========================================================================
// IMAGE_MODEL GET_SIZE
// ==========================================================================
const extent& image::image_model::get_size() const
{
    return size_;
}

// ==========================================================================
// IMAGE_MODEL GET_PREFERRED_SIZE
// ==========================================================================
const extent& image::image_model::get_preferred_size() const
{
    static const extent preferred_size = {0, 0};
    return preferred_size;
}

// ==========================================================================
// IMAGE_MODEL GET_BACKGROUND_BRUSH
// ==========================================================================
const element& image::image_model::get_background_brush() const
{
    return background_brush_;
}

// ==========================================================================
// IMAGE_MODEL SET_BACKGROUND_BRUSH
// ==========================================================================
void image::image_model::set_background_brush(const element& brush)
{
    background_brush_ = brush;
}

// ==========================================================================
// IMAGE_MODEL GET_IMAGE
// ==========================================================================
std::vector<estring> const &image::image_model::get_image() const
{
    return image_;
}

// ==========================================================================
// IMAGE_MODEL SET_IMAGE
// ==========================================================================
void image::image_model::set_image(std::vector<estring> const &im)
{
    image_ = im;
}


}}
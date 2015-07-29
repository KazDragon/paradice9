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
#ifndef YGGDRASIL_MUNIN_IMAGE_HPP_
#define YGGDRASIL_MUNIN_IMAGE_HPP_

#include "yggdrasil/munin/estring.hpp"
#include "yggdrasil/munin/extent.hpp"
#include "yggdrasil/munin/model.hpp"
#include <boost/property_tree/ptree_fwd.hpp>

namespace yggdrasil { namespace munin {

class canvas;
class model;
class rectangle;

//* =========================================================================
/// \brief A graphical component that represents an image; a static x,y grid
/// of elements.
/// \par Properties
///   * value : array of estring - the image used
///   * background_brush : element - the element used to cover empty space
//* =========================================================================
class image
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    image();

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    image(boost::property_tree::ptree const &properties);

    //* =====================================================================
    /// \brief Draws the specified region of the component onto the canvas.
    //* =====================================================================
    void draw(canvas &cvs, rectangle const &region) const;

    //* =====================================================================
    /// \brief Returns the component's model.
    //* =====================================================================
    model &get_model();

private :
    model model_;
};

}}

#endif

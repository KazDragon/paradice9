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
#include <boost/any.hpp>
#include <vector>

namespace yggdrasil { namespace munin {

class canvas;
class model;
class rectangle;

//* =========================================================================
/// \brief A graphical component that represents an image; a static x,y grid
/// of elements.
//* =========================================================================
class image
{
public :
    //* =====================================================================
    /// \brief A model specialization for an image.
    //* =====================================================================
    struct image_model
    {
        //* =================================================================
        /// \brief Constructor
        //* =================================================================
        image_model(std::vector<estring> im);

        //* =================================================================
        /// \brief Sends an event to the model.
        //* =================================================================
        boost::any event(boost::any const &event);

        //* =================================================================
        /// \brief Sets the size of the component.
        //* =================================================================
        void set_size(extent const &size);

        //* =================================================================
        /// \brief Returns the size of the component.
        //* =================================================================
        extent const &get_size() const;

        //* =================================================================
        /// \brief Returns the preferred size of the component.
        //* =================================================================
        extent const &get_preferred_size() const;

        //* =================================================================
        /// \brief Returns the background brush size of the image; the
        /// brush to use if the image data is smaller than its actual
        /// size.
        //* =================================================================
        element const &get_background_brush() const;

        //* =================================================================
        /// \brief Sets the background brush of the image.
        //* =================================================================
        void set_background_brush(element const &brush);

        //* =================================================================
        /// \brief Returns an array of estring which represents the image
        /// line-by-line.
        //* =================================================================
        std::vector<estring> const &get_image() const;

        //* =================================================================
        /// \brief Sets the content of the image.
        //* =================================================================
        void set_image(std::vector<estring> const &im);

    private :
        std::vector<estring> image_;
        element background_brush_;
        extent size_;
    };

public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    image();

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    image(std::vector<estring> im);

    //* =====================================================================
    /// \brief Draws the specified region of the component onto the canvas.
    //* =====================================================================
    void draw(canvas &cvs, rectangle const &region) const;

    //* =====================================================================
    /// \brief Returns the component's model.
    //* =====================================================================
    image_model &get_model();

private :
    image_model model_;
};

}}

#endif

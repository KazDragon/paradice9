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
#ifndef YGGDRASIL_MUNIN_FILLED_BOX_HPP_
#define YGGDRASIL_MUNIN_FILLED_BOX_HPP_

#include "yggdrasil/munin/element.hpp"
#include "yggdrasil/munin/extent.hpp"
#include <boost/any.hpp>

namespace yggdrasil { namespace munin {

class canvas;
class model;
class rectangle;

//* =========================================================================
/// \brief A graphical component that is comprised of repeated copies of
/// a single element.
//* =========================================================================
class filled_box
{
public :
    //* =====================================================================
    /// \brief A model specialization for a filled box.
    //* =====================================================================
    struct filled_box_model
    {
        //* =================================================================
        /// \brief Constructor
        //* =================================================================
        filled_box_model(element brush);

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

        element brush_;
        extent size_;
    };

public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    filled_box();

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    filled_box(element brush);

    //* =====================================================================
    /// \brief Draws the specified region of the component onto the canvas.
    //* =====================================================================
    void draw(canvas &cvs, rectangle const &region) const;

    //* =====================================================================
    /// \brief Returns the component's model.
    //* =====================================================================
    filled_box_model &get_model();


private :
    filled_box_model model_;
};

}}

#endif
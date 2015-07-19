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
#include "yggdrasil/munin/basic_model.hpp"
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
    basic_model const &get_model() const;

    //* =====================================================================
    /// \brief Returns the component's model.
    //* =====================================================================
    basic_model &get_model();

private :
    basic_model model_;
};

}}

#endif
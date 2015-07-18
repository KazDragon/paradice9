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
#ifndef YGGDRASIL_MUNIN_CANVAS_HPP_
#define YGGDRASIL_MUNIN_CANVAS_HPP_

#include "yggdrasil/munin/element.hpp"
#include "yggdrasil/munin/extent.hpp"
#include <vector>

namespace yggdrasil { namespace munin {

namespace detail {
    class column_proxy;
}

//* =========================================================================
/// \brief A class that abstracts an (x,y) field of elements.
//* =========================================================================
class canvas
{
public :
    //* =====================================================================
    /// \brief Default Constructor
    //* =====================================================================
    canvas();

    //* =====================================================================
    /// \brief Constructs a canvas with the specified size.
    //* =====================================================================
    canvas(extent size);
    
    //* =====================================================================
    /// \brief Sets the size of the canvas.
    /// \par
    /// Note: the actual content of the canvas is unspecified after this
    /// operation.  A complete repaint is advised.
    //* =====================================================================
    void set_size(extent const &size);
    
    //* =====================================================================
    /// \brief Returns the current size of the canvas.
    //* =====================================================================
    extent get_size() const;
    
    //* =====================================================================
    /// \brief Offsets the origin of the canvas by an amount.
    //* =====================================================================
    void offset_by(extent const &offset);
    
    //* =====================================================================
    /// \brief Returns the current offset.
    //* =====================================================================
    extent get_offset() const;
    
    //* =====================================================================
    /// \brief Returns a reference to the column at the specified index.
    //* =====================================================================
    detail::column_proxy operator[](yggdrasil::s32 index);
    
private :
    std::vector<element> elements_;
    extent size_;
    extent offset_;
};

namespace detail {
    
class column_proxy
{
public :
    column_proxy(
        std::vector<element> &elements, 
        extent offset,
        yggdrasil::s32 width,
        yggdrasil::s32 column);
    
    element &operator[](yggdrasil::s32 index);
    
private :
    std::vector<element> &elements_;
    extent offset_;
    yggdrasil::s32 width_;
    yggdrasil::s32 column_;
};

}

}};

#endif

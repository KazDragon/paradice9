// ==========================================================================
// Munin Frame.
//
// Copyright (C) 2010 Matthew Chaplain, All Rights Reserved.
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
#ifndef MUNIN_FRAME_HPP_
#define MUNIN_FRAME_HPP_

#include "munin/basic_component.hpp"

namespace munin {

template <class ElementType>
class frame : public munin::basic_component<ElementType>
{
public :
    //* =====================================================================
    /// \brief Retrieves the height of the frame.
    //* =====================================================================
    odin::u32 get_frame_height() const
    {
        return do_get_frame_height();
    }

    //* =====================================================================
    /// \brief Retreives the width of the frame.
    //* =====================================================================
    odin::u32 get_frame_width() const
    {
        return do_get_frame_width();
    }

private :
    //* =====================================================================
    /// \brief Called by get_frame_width().  Derived classes must override 
    /// this function in order to get the width of the frame in a custom
    /// manner.
    //* =====================================================================
    virtual odin::u32 do_get_frame_width() const = 0;

    //* =====================================================================
    /// \brief Called by get_frame_height().  Derived classes must override 
    /// this function in order to get the height of the frame in a custom
    /// manner.
    //* =====================================================================
    virtual odin::u32 do_get_frame_height() const = 0;
};

}

#endif

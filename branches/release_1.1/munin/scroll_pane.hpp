// ==========================================================================
// Munin Scroll Pane
//
// Copyright (C) 2011 Matthew Chaplain, All Rights Reserved.
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
#ifndef MUNIN_SCROLL_PANE_HPP_
#define MUNIN_SCROLL_PANE_HPP_

#include "munin/composite_component.hpp"

namespace munin {

//* =========================================================================
/// \brief A container that constructs a scroll pane around an underlying
/// component.
//* =========================================================================
class scroll_pane
    : public composite_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    scroll_pane(
        boost::shared_ptr<component> underlying_component
      , bool                         top_border = true);

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~scroll_pane();

private :
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}

#endif


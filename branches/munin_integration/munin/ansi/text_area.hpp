// ==========================================================================
// Munin ANSI Text Area Component.
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
#ifndef MUNIN_ANSI_TEXT_AREA_HPP_
#define MUNIN_ANSI_TEXT_AREA_HPP_

#include "munin/composite_component.hpp"
#include "munin/ansi/ansi_types.hpp"
#include <boost/shared_ptr.hpp>

namespace munin { namespace ansi {

//* =========================================================================
/// \brief A class that models a multi-line text control with a frame
/// bordering it.
//* =========================================================================
class text_area 
  : public munin::composite_component<munin::ansi::element_type>
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    text_area();

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~text_area();

private :
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}}

#endif

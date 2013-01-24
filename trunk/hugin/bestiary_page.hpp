// ==========================================================================
// Hugin Bestiary Page
//
// Copyright (C) 2013 Matthew Chaplain, All Rights Reserved.
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
#ifndef HUGIN_BESTIARY_PAGE_HPP_
#define HUGIN_BESTIARY_PAGE_HPP_

#include "munin/composite_component.hpp"

namespace hugin {

//* =========================================================================
/// \brief The page for the bestiary tab in the encounter wizard.
//* =========================================================================
class bestiary_page : public munin::composite_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    bestiary_page();
    
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    ~bestiary_page();
    
private :
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}

#endif
// ==========================================================================
// Munin Graphics Context.
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
#ifndef MUNIN_ANSI_GRAPHICS_CONTEXT_HPP_
#define MUNIN_ANSI_GRAPHICS_CONTEXT_HPP_

#include "munin/graphics_context.hpp"
#include "munin/ansi/types.hpp"
#include "munin/types.hpp"
#include <boost/shared_ptr.hpp>

namespace munin { namespace ansi {

class graphics_context
    : public munin::graphics_context<munin::ansi::element_type>
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    graphics_context();
    
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~graphics_context();
    
    //* =====================================================================
    /// \brief Sets the size of the graphics context.
    //* =====================================================================
    void set_size(extent const &size);
    
    //* =====================================================================
    /// \brief Retrieves the size of the graphics context.
    //* =====================================================================
    extent get_size() const;
    
private :
    //* =====================================================================
    /// \brief Sets the value and attribute at the specified coordinates on
    /// the graphics context.
    //* =====================================================================
    virtual void set_value(
        odin::u32    column
      , odin::u32    row
      , element_type value); 

    //* =====================================================================
    /// \brief Retrieves the value and attribute at the specified coordinates
    /// on the graphics context.
    //* =====================================================================
    virtual element_type get_value(
        odin::u32 column
      , odin::u32 row) const;
    
    class impl;
    boost::shared_ptr<impl> pimpl_;
};

}}

#endif

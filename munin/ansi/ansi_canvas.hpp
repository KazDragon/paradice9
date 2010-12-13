// ==========================================================================
// Munin ANSI Canvas.
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
#ifndef MUNIN_ANSI_CANVAS_HPP_
#define MUNIN_ANSI_CANVAS_HPP_

#include "munin/canvas.hpp"
#include "munin/ansi/ansi_types.hpp"
#include "munin/types.hpp"
#include <boost/shared_ptr.hpp>

namespace munin { namespace ansi {

//* =========================================================================
/// \brief A canvas for ANSI components.
//* =========================================================================
class ansi_canvas
    : public munin::canvas<munin::ansi::element_type>
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    ansi_canvas();
    
    //* =====================================================================
    /// \brief Copy Constructor
    //* =====================================================================
    ansi_canvas(ansi_canvas const &cvs);
    
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~ansi_canvas();
    
    //* =====================================================================
    /// \brief Assigns this canvas to be a clone of another.
    //* =====================================================================
    ansi_canvas &operator=(ansi_canvas const &other);
    
    //* =====================================================================
    /// \brief Sets the size of the canvas.
    //* =====================================================================
    void set_size(extent const &size);
    
    //* =====================================================================
    /// \brief Retrieves the size of the canvas.
    //* =====================================================================
    extent get_size() const;
    
    //* =====================================================================
    /// \brief Returns true if this canvas is equal in content to the other.
    //* =====================================================================
    bool operator==(ansi_canvas const &other) const;
    
private :
    //* =====================================================================
    /// \brief Sets the value and attribute at the specified coordinates on
    /// the canvas.
    //* =====================================================================
    virtual void set_value(
        odin::s32    column
      , odin::s32    row
      , element_type value); 

    //* =====================================================================
    /// \brief Retrieves the value and attribute at the specified coordinates
    /// on the canvas.
    //* =====================================================================
    virtual element_type get_value(
        odin::s32 column
      , odin::s32 row) const;
    
    //* =====================================================================
    /// \brief Offsets the canvas.
    /// Offsets the canvas so that drawing operations take place at a new
    /// location.  For example, if a canvas is offset by (1, 2) and
    /// receives a set_value(3, 4, x), then this will be identical to a
    /// set_value(4, 6, x) of an unoffset canvas.  Offsets are cumulative.
    //* =====================================================================
    virtual void do_apply_offset(odin::s32 columns, odin::s32 rows);
    
    class impl;
    boost::shared_ptr<impl> pimpl_;
};

}}

#endif

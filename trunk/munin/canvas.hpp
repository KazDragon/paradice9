// ==========================================================================
// Munin Canvas.
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
#ifndef MUNIN_CANVAS_HPP_
#define MUNIN_CANVAS_HPP_

#include "munin/types.hpp"
#include "odin/types.hpp"
#include <boost/shared_ptr.hpp>
#include <utility>

namespace munin {
    
//* =========================================================================
/// \brief An object onto which components can draw themselves.
//* =========================================================================
class canvas
{
public :
    //* =====================================================================
    /// \brief A proxy into a row of elements on the canvas
    //* =====================================================================
    class row_proxy
    {
    public :
        // ==================================================================
        // CONSTRUCTOR
        // ==================================================================
        row_proxy(canvas &canvas, odin::s32 column, odin::s32 row);
        
        // ==================================================================
        // OPERATOR=
        // ==================================================================
        void operator=(element_type value);
        
        // ==================================================================
        // CONVERSION OPERATOR: ELEMENT TYPE
        // ==================================================================
        operator element_type();
        
    private :
        canvas    &canvas_;
        odin::s32  column_;
        odin::s32  row_;
    };
    
    //* =====================================================================
    /// \brief A proxy into a column of elements on the canvas
    //* =====================================================================
    class column_proxy
    {
    public :
        // ==================================================================
        // CONSTRUCTOR
        // ==================================================================
        column_proxy(canvas &canvas, odin::s32  column);
            
        // ==================================================================
        // OPERATOR[]
        // ==================================================================
        row_proxy operator[](odin::s32 row);
        
    private :
        canvas    &canvas_;
        odin::s32  column_;
    };
    
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    canvas();
    
    // ======================================================================
    // COPY CONSTRUCTOR
    // ======================================================================
    canvas(canvas const &other);
    
    // ======================================================================
    // OPERATOR=
    // ======================================================================
    canvas &operator=(canvas const &other);
    
    // ======================================================================
    // OPERATOR==
    // ======================================================================
    bool operator==(canvas const &other) const;
    
    // ======================================================================
    // DESTRUCTOR
    // ======================================================================
    ~canvas();
    
    // ======================================================================
    // OPERATOR[]
    // ======================================================================
    column_proxy operator[](odin::s32 column);
    
    // ======================================================================
    // SET_SIZE
    // ======================================================================
    void set_size(extent const &size);
    
    // ======================================================================
    // GET_SIZE
    // ======================================================================
    extent get_size() const;
    
    // ======================================================================
    // APPLY_OFFSET
    // ======================================================================
    void apply_offset(odin::s32 columns, odin::s32 rows);
    
private :
    class impl;
    boost::shared_ptr<impl> pimpl_;
};
    
}

#endif


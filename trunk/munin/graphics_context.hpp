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
#ifndef MUNIN_GRAPHICS_CONTEXT_HPP_
#define MUNIN_GRAPHICS_CONTEXT_HPP_

#include "odin/types.hpp"
#include <utility>

namespace munin {
    
// GRAPHICS_CONTEXT =========================================================
// ==========================================================================
template <class ElementType>
class graphics_context
{
public :
    typedef ElementType element_type;
    
    // ROW_PROXY ============================================================
    // ======================================================================
    class row_proxy
    {
        // COLUMN_PROXY =====================================================
        // ==================================================================
        class column_proxy
        {
        public :
            // ==============================================================
            // CONSTRUCTOR
            // ==============================================================
            column_proxy(
                graphics_context &ctx
              , odin::u32         row
              , odin::u32         column)
                : ctx_(ctx)
                , row_(row)
                , column_(column)
            {
            }
            
            // ==============================================================
            // OPERATOR=
            // ==============================================================
            void operator=(element_type value)
            {
                ctx_.set_value(row_, column_, value);
            }
            
            // ==============================================================
            // CONVERSION OPERATOR: ELEMENT TYPE
            // ==============================================================
            operator element_type()
            {
                return ctx_.get_value(row_, column_);
            }
            
        private :
            graphics_context &ctx_;
            odin::u32         row_;
            odin::u32         column_;
        };
        
    public :
        // ==================================================================
        // CONSTRUCTOR
        // ==================================================================
        row_proxy(
            graphics_context &ctx
          , odin::u32         row)
            : ctx_(ctx)
            , row_(row)
        {
        }
            
        // ==================================================================
        // OPERATOR[]
        // ==================================================================
        column_proxy operator[](odin::u32 column)
        {
            return column_proxy(ctx_, row_, column);
        }
        
    private :
        graphics_context &ctx_;
        odin::u32         row_;
    };
    
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~graphics_context()
    {
    }
    
    //* =====================================================================
    ///
    //* =====================================================================
    row_proxy operator[](odin::u32 row)
    {
        return row_proxy(*this, row);
    }
    
private :
    //* =====================================================================
    /// \brief Sets the value and attribute at the specified coordinates on
    /// the graphics context.
    //* =====================================================================
    virtual void set_value(
        odin::u32    row
      , odin::u32    column
      , element_type value) = 0; 

    //* =====================================================================
    /// \brief Retrieves the value and attribute at the specified coordinates
    /// on the graphics context.
    //* =====================================================================
    virtual element_type get_value(
        odin::u32 row
      , odin::u32 column) const = 0;
};
    
}

#endif

// ==========================================================================
// Munin Text Area.
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
#ifndef MUNIN_TEXT_AREA_HPP_
#define MUNIN_TEXT_AREA_HPP_

#include "munin/basic_component.hpp"
#include "munin/ansi/ansi_types.hpp"

namespace munin { 

//* =========================================================================
/// \brief An object that represents a text_area, or text_area.
//* =========================================================================
class text_area : public munin::basic_component<munin::ansi::element_type>  
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
    
    //* =====================================================================
    /// \brief Inserts the specified text into the text area at the cursor
    /// position.
    //* =====================================================================
    void insert_text(std::string const &text);
    
private :
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to get the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual munin::extent do_get_preferred_size() const;

    //* =====================================================================
    /// \brief Called by get_cursor_state().  Derived classes must override
    /// this function in order to return the cursor state in a custom manner.
    //* =====================================================================
    virtual bool do_get_cursor_state() const;
    
    //* =====================================================================
    /// \brief Called by get_cursor_position().  Derived classes must
    /// override this function in order to return the cursor position in
    /// a custom manner.
    //* =====================================================================
    virtual munin::point do_get_cursor_position() const;
    
    //* =====================================================================
    /// \brief Called by draw().  Derived classes must override this function
    /// in order to draw onto the passed canvas.  A component must only draw 
    /// the part of itself specified by the region.
    ///
    /// \param cvs the canvas in which the component should draw itself.
    /// \param offset the position of the parent component (if there is one)
    ///        relative to the canvas.  That is, (0,0) to this component
    ///        is actually (offset.x, offset.y) in the canvas.
    /// \param region the region relative to this component's origin that
    /// should be drawn.
    //* =====================================================================
    virtual void do_draw(
        munin::canvas<element_type> &cvs
      , munin::point const          &offset
      , munin::rectangle const      &region);

    //* =====================================================================
    /// \brief Called by event().  Derived classes must override this 
    /// function in order to handle events in a custom manner.
    //* =====================================================================
    virtual void do_event(boost::any const &event);
    
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}

#endif

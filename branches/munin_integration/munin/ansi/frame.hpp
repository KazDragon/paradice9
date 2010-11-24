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
#ifndef MUNIN_ANSI_FRAME_HPP_
#define MUNIN_ANSI_FRAME_HPP_

#include "munin/frame.hpp"
#include "munin/ansi/ansi_types.hpp"

namespace munin { namespace ansi {

//* =========================================================================
/// \brief An object that represents a frame, or frame.
//* =========================================================================
class frame : public munin::frame<munin::ansi::element_type>  
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    frame();
    
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~frame();
    
protected :
    //* =====================================================================
    /// \brief Called by get_frame_width().  Derived classes must override 
    /// this function in order to get the width of the frame in a custom
    /// manner.
    //* =====================================================================
    virtual odin::u32 do_get_frame_width() const;

    //* =====================================================================
    /// \brief Called by get_frame_height().  Derived classes must override 
    /// this function in order to get the height of the frame in a custom
    /// manner.
    //* =====================================================================
    virtual odin::u32 do_get_frame_height() const;

    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to get the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual munin::extent do_get_preferred_size() const;

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
    
private :    
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}}

#endif

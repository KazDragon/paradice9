// ==========================================================================
// Munin Named Frame.
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
#ifndef MUNIN_NAMED_FRAME_HPP_
#define MUNIN_NAMED_FRAME_HPP_

#include "munin/frame.hpp"
#include <string>

namespace munin {

//* =========================================================================
/// \brief An object that represents a frame, or frame.
//* =========================================================================
class named_frame : public frame
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    named_frame();
    
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~named_frame();
    
    //* =====================================================================
    /// \brief Sets the name displayed on the top border of the frame.
    //* =====================================================================
    void set_name(std::string const &name);
    
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
    virtual extent do_get_preferred_size() const;

    //* =====================================================================
    /// \brief Called by draw().  Derived classes must override this function
    /// in order to draw onto the passed canvas.  A component must only draw 
    /// the part of itself specified by the region.
    ///
    /// \param cvs the canvas in which the component should draw itself.
    /// \param region the region relative to this component's origin that
    /// should be drawn.
    //* =====================================================================
    virtual void do_draw(
        canvas          &cvs
      , rectangle const &region);
    
    //* =====================================================================
    /// \brief Called by set_attribute().  Derived classes must override this
    /// function in order to set an attribute in a custom manner.
    //* =====================================================================
    virtual void do_set_attribute(
        std::string const &name, boost::any const &attr);
    
private :    
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}

#endif

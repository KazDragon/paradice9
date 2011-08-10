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
#ifndef MUNIN_FRAME_HPP_
#define MUNIN_FRAME_HPP_

#include "munin/composite_component.hpp"
#include <boost/shared_ptr.hpp>

namespace munin {

//* =========================================================================
/// \brief A base class that takes a series of components and uses them to
/// construct a frame.
/// \par ATTRIBUTE_PEN
/// Set the ATTRIBUTE_PEN attribute to an value of type munin::attribute.
/// This will be passed down to subcomponents.  You can use this, for 
/// example, to show a border highlight when the interior component has
/// focus.
//* =========================================================================
class frame : public munin::composite_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    /// \par
    /// Takes 8 components.  The are arranged in the following manner:
    /// 1 2 3
    /// 4   5
    /// 6 7 8
    ///
    /// These form the frame outlines.
    ///
    /// Components 1, 2 and 3 are optional.  Leaving these NULL will create
    /// a frame without a top border.
    //* =====================================================================
    frame(
        boost::shared_ptr<component> top_left
      , boost::shared_ptr<component> top
      , boost::shared_ptr<component> top_right
      , boost::shared_ptr<component> left
      , boost::shared_ptr<component> right
      , boost::shared_ptr<component> bottom_left
      , boost::shared_ptr<component> bottom
      , boost::shared_ptr<component> bottom_right);

    //* =====================================================================
    /// \brief Returns the total border height of the top and bottom borders.
    /// For example, if each border has a height of 1, then this will return
    /// 2.
    //* =====================================================================
    odin::s32 get_border_height() const;
    
    //* =====================================================================
    /// \brief Returns the total border width of the left and right borders.
    /// For example, if each border has a width of 1, then this will return
    /// 2.
    //* =====================================================================
    odin::s32 get_border_width() const;

protected :
    //* =====================================================================
    /// \brief Called by get_border_height.  Derived classes must override 
    /// this function in order to retrieve the total border height in a
    /// custom manner.
    //* =====================================================================
    virtual odin::s32 do_get_border_height() const;

    //* =====================================================================
    /// \brief Called by get_border_width.  Derived classes must override 
    /// this function in order to retrieve the total border width in a
    /// custom manner.
    //* =====================================================================
    virtual odin::s32 do_get_border_width() const;

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

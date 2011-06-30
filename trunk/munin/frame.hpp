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

protected :
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

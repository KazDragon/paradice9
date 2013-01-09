// ==========================================================================
// Guibuilder Sized Block Component.
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
#ifndef GUIBUILDER_SIZED_BLOCK_HPP_
#define GUIBUILDER_SIZED_BLOCK_HPP_

#include "munin/basic_component.hpp"
#include <boost/shared_ptr.hpp>

namespace guibuilder {

//* =========================================================================
/// \brief A class that models a single-line text control with a frame
/// bordering it.
//* =========================================================================
class sized_block : public munin::basic_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    sized_block(
        munin::element_type const &element
      , munin::extent              size);

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~sized_block();

protected :
    //* =====================================================================
    /// \brief Called by set_size().  Derived classes must override this 
    /// function in order to set the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual void do_set_size(munin::extent const &size);

    //* =====================================================================
    /// \brief Called by get_size().  Derived classes must override this
    /// function in order to get the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual munin::extent do_get_size() const;

    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to get the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual munin::extent do_get_preferred_size() const;
    
    //* =====================================================================
    /// \brief Called by draw().  Derived classes must override this function
    /// in order to draw onto the passed context.  A component must only draw 
    /// the part of itself specified by the region.
    ///
    /// \param ctx the context in which the component should draw itself.
    /// \param region the region relative to this component's origin that
    /// should be drawn.
    //* =====================================================================
    virtual void do_draw(
        munin::context         &ctx
      , munin::rectangle const &region) = 0;
    
private :
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}

#endif


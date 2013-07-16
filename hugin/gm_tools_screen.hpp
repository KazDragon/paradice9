// ==========================================================================
// Hugin GM Tools Screen
//
// Copyright (C) 2013 Matthew Chaplain, All Rights Reserved.
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
#ifndef HUGIN_GM_TOOLS_SCREEN_HPP_
#define HUGIN_GM_TOOLS_SCREEN_HPP_

#include "munin/composite_component.hpp"
#include "paradice/beast.hpp"
#include <boost/shared_ptr.hpp>

namespace hugin {

//* =========================================================================
/// \brief The screen for the GM tools.
//* =========================================================================
class gm_tools_screen : public munin::composite_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    gm_tools_screen();
    
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    ~gm_tools_screen();
    
    //* =====================================================================
    /// \brief Sets the beasts to be used by the user interface
    //* =====================================================================
    void set_beasts(
        std::vector< boost::shared_ptr<paradice::beast> > const &beasts);

    //* =====================================================================
    /// \brief Returns the beasts used by the user interface
    //* =====================================================================
    std::vector< boost::shared_ptr<paradice::beast> > get_beasts() const;

    //* =====================================================================
    /// \fn on_back
    /// \brief Called when the 'back' button is pressed on the GM Tools
    /// screen.
    //* =====================================================================
    boost::signal<void ()> on_back;

protected :
    //* =====================================================================
    /// \brief Called by event().  Derived classes must override this 
    /// function in order to handle events in a custom manner.
    //* =====================================================================
    virtual void do_event(boost::any const &event);

private :
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}

#endif

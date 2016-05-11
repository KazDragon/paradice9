// ==========================================================================
// Hugin Delete Confirmation Dialog
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
#ifndef HUGIN_DELETE_CONFIRMATION_DIALOG_HPP_
#define HUGIN_DELETE_CONFIRMATION_DIALOG_HPP_

#include "munin/composite_component.hpp"

namespace terminalpp {
    class string;
}

namespace hugin {

//* =========================================================================
/// \brief A page for confirming deletions from the encounter wizards.
//* =========================================================================
class delete_confirmation_dialog : public munin::composite_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    delete_confirmation_dialog();
    
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    ~delete_confirmation_dialog();
    
    //* =====================================================================
    /// \brief Sets a description of what it is you want to be deleted
    //* =====================================================================
    void set_deletion_target_text(terminalpp::string const &text);

    //* =====================================================================
    /// \fn on_delete_confirmation
    /// \brief Connect to this signal to receive notifications about the
    /// "Yes" button being pressed.
    //* =====================================================================
    boost::signal<void ()> on_delete_confirmation;

    //* =====================================================================
    /// \fn on_delete_rejection
    /// \brief Connect to this signal to receive notifications about the
    /// "No" button being pressed.
    //* =====================================================================
    boost::signal<void ()> on_delete_rejection;

private :
    struct impl;
    std::shared_ptr<impl> pimpl_;
};

}

#endif

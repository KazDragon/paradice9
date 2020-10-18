// ==========================================================================
// Paradice Title Page
//
// Copyright (C) 2020 Matthew Chaplain, All Rights Reserved.
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
#ifndef PARADICE_UI_TITLE_PAGE_HPP_
#define PARADICE_UI_TITLE_PAGE_HPP_

#include "paradice/cryptography.hpp"
#include <munin/composite_component.hpp>
#include <munin/button.hpp>
#include <munin/image.hpp>
#include <boost/signals2/signal.hpp>

namespace paradice { namespace ui { 

class title_page
  : public munin::composite_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    title_page();

    //* =====================================================================
    /// \fn on_new_account
    /// Register a callback for when the "new" button is pressed.
    //* =====================================================================
    boost::signals2::signal<void ()> on_new_account;

    //* =====================================================================
    /// \fn on_account_login
    /// Register a callback for when the "login" button is pressed, with
    /// the account name and password attached.
    //* =====================================================================
    boost::signals2::signal<
        void (std::string const &, encrypted_string const &)
    > on_account_login;

private :
    std::shared_ptr<munin::button> new_button_;
    std::shared_ptr<munin::button> login_button_;
};

}}
#endif

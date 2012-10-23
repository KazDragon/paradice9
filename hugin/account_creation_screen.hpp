// ==========================================================================
// Hugin Account Creation Screen
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
#ifndef HUGIN_ACCOUNT_CREATION_SCREEN_HPP_
#define HUGIN_ACCOUNT_CREATION_SCREEN_HPP_

#include "munin/composite_component.hpp"
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

namespace hugin {

//* =========================================================================
/// \brief An abstraction of the primary user interface for the Paradice
/// application.
//* =========================================================================
class account_creation_screen
    : public munin::composite_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    account_creation_screen();
    
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    ~account_creation_screen();
    
    //* =====================================================================
    /// \brief Clears the screen of all data.
    //* =====================================================================
    void clear();
    
    //* =====================================================================
    /// \brief Set a function to be called when the user inputs the details
    /// for the creation of an account.
    //* =====================================================================
    void on_account_created(
        boost::function<
            void (std::string account_name
                , std::string password
                , std::string password_verify)> callback);
    
    //* =====================================================================
    /// \brief Set a function to be called when the user cancels the creation
    /// of an account.
    //* =====================================================================
    void on_account_creation_cancelled(boost::function<void ()> callback);
    
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

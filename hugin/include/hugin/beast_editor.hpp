// ==========================================================================
// Hugin Beast Editor
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
#ifndef HUGIN_BEAST_EDITOR_HPP_
#define HUGIN_BEAST_EDITOR_HPP_

#include "munin/composite_component.hpp"

namespace terminalpp {
    class string;
}

namespace hugin {

//* =========================================================================
/// \brief The page for editing beasts in the encounter wizard.
//* =========================================================================
class beast_editor : public munin::composite_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    beast_editor();
    
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    ~beast_editor();

    //* =====================================================================
    /// \brief Sets the name of the beast that the editor is working with.
    //* =====================================================================
    void set_beast_name(terminalpp::string const &name);

    //* =====================================================================
    /// \brief Retrieves the name of the beast that the editor is working 
    /// with.
    //* =====================================================================
    terminalpp::string get_beast_name() const;

    //* =====================================================================
    /// \brief Sets the description of the beast that the editor is working
    /// with.
    //* =====================================================================
    void set_beast_description(terminalpp::string const &description);

    //* =====================================================================
    /// \brief Retrieves the description of the beast that the editor is
    /// working with.
    //* =====================================================================
    terminalpp::string get_beast_description() const;

    //* =====================================================================
    /// \fn on_revert
    /// \brief Called when the revert button is pressed.
    //* =====================================================================
    boost::signal<void ()> on_revert;

    //* =====================================================================
    /// \fn on_save
    /// \brief Called when the save button is pressed.
    //* =====================================================================
    boost::signal<void ()> on_save;

private :
    struct impl;
    std::shared_ptr<impl> pimpl_;
};

}

#endif

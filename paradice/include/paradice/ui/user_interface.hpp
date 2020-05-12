// ==========================================================================
// Paradice User Interface
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
#ifndef PARADICE_UI_USER_INTERFACE_HPP_
#define PARADICE_UI_USER_INTERFACE_HPP_

#include "paradice/export.hpp"
#include <munin/composite_component.hpp>
#include <memory>

namespace paradice { namespace ui {

//* =========================================================================
/// \brief An abstraction of the primary user interface for the Paradice
/// application.
//* =========================================================================
class PARADICE_EXPORT user_interface
  : public munin::composite_component
{
public:
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    user_interface();

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    ~user_interface() override;

protected:
    //* =====================================================================
    /// \brief Called by event().  Derived classes must override this 
    /// function in order to handle events in a custom manner.
    //* =====================================================================
    void do_event(boost::any const &event) override;

private:
    struct impl;
    std::unique_ptr<impl> pimpl_;
};

}}

#endif

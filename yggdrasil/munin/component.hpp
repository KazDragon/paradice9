// ==========================================================================
// Munin Component.
//
// Copyright (C) 2014 Matthew Chaplain, All Rights Reserved.
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
#ifndef YGGDRASIL_MUNIN_COMPONENT_HPP_
#define YGGDRASIL_MUNIN_COMPONENT_HPP_

#include "yggdrasil/munin/model.hpp"
#include "yggdrasil/kvasir/make_unique.hpp"
#include "yggdrasil/kvasir/thunk.hpp"
#include <memory>
#include <utility>

namespace yggdrasil { namespace munin {

class canvas;
class rectangle;

MEMBER_THUNK(draw);
MEMBER_THUNK(get_model);

//* =========================================================================
/// \brief A conceptual UI component.
/// \par
/// A component represents a structure that can be represented on the screen.
/// Classes that model the "component" concept have a small set of functions:
/// they know how to draw themselves on the screen, and they can have their
/// model queried for other basic functions such as size-setting and event-
/// passing.
//* =========================================================================
class component
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    template <class Component>
    component(Component &&mdl)
      : self_(yggdrasil::kvasir::make_unique<impl<Component>>(
          std::forward<Component>(mdl)))
    {
    }

    SELF_THUNK_C(draw);
    SELF_THUNK  (get_model);

private :
    //* =====================================================================
    /// \brief The concept that component will implement.
    //* =====================================================================
    struct concept
    {
        //* =================================================================
        /// \brief Destructor
        //* =================================================================
        virtual ~concept(){}

        //* =================================================================
        /// \brief Returns the model for the component.
        //* =================================================================
        virtual model get_model_() = 0;

        //* =================================================================
        /// \brief Draws the specified region of the component onto their
        /// canvas.
        //* =================================================================
        virtual void draw_(canvas &cvs, rectangle const &region) const = 0;
    };

    //* =====================================================================
    /// \brief The wrapper of the component concept, templated around a class
    /// that will actually provide the implementation.
    //* =====================================================================
    template <class Component>
    struct impl : concept
    {
        //* =================================================================
        /// \brief Constructor
        //* =================================================================
        impl(Component &&mdl)
          : model_(std::forward<Component>(mdl))
        {
        }

        //* =================================================================
        /// \brief Returns the model for the component.
        //* =================================================================
        virtual model get_model_() override
        {
            return model_.get_model();
        }

        //* =================================================================
        /// \brief Draws the specified region of the component onto their
        /// canvas.
        //* =================================================================
        virtual void draw_(canvas &cvs, rectangle const &region) const override
        {
            model_.draw(cvs, region);
        }

        Component model_;
    };

    std::unique_ptr<concept> self_;
};

}}

#endif

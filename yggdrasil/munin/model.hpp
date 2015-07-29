// ==========================================================================
// Munin Model.
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
#ifndef YGGDRASIL_MUNIN_MODEL_HPP_
#define YGGDRASIL_MUNIN_MODEL_HPP_

#include "yggdrasil/munin/extent.hpp"
#include "yggdrasil/kvasir/make_unique.hpp"
#include "yggdrasil/kvasir/thunk.hpp"
#include <boost/any.hpp>
#include <memory>
#include <utility>

namespace yggdrasil { namespace munin {

MEMBER_THUNK(event);
MEMBER_THUNK(set_size);
MEMBER_THUNK(get_property);
MEMBER_THUNK(set_property);
MEMBER_THUNK(get_signal);

//* =========================================================================
/// \brief A conceptual UI "model".
/// \par
/// A model represents the data required in order to represent the component
/// on the screen.  Classes that model the "component" concept have a small
/// set of properties that are common to all components:
///   * They have a mutable name.
///   * They have a mutable size.
///   * They know what size they would prefer to be.
///   * They can accept events.
///   * They have a mutable set of named properties.
///   * They have a set of named signals.
/// Classes themselves can further expand on the model, so long as their
/// models fulfil the model concept.
//* =========================================================================
class model
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    template <class Model>
    model(Model &&mdl)
      : self_(std::make_unique<impl<Model>>(std::forward<Model>(mdl)))
    {
    }

    SELF_THUNK  (event);
    SELF_THUNK_C(get_size);
    SELF_THUNK  (set_size);
    SELF_THUNK_C(get_preferred_size);
    SELF_THUNK  (set_property);
    SELF_THUNK_C(get_property);
    SELF_THUNK_C(get_signal);

private :
    //* =====================================================================
    /// \brief The concept that model will implement.
    //* =====================================================================
    struct concept
    {
        //* =================================================================
        /// \brief Destructor
        //* =================================================================
        virtual ~concept(){}

        //* =================================================================
        /// \brief Send an event of any type to the component, and possibly
        /// get a response.
        //* =================================================================
        virtual boost::any event_(boost::any const &ev) = 0;

        //* =================================================================
        /// \brief Set the size of the component.
        //* =================================================================
        virtual void set_size_(extent const &size) = 0;

        //* =================================================================
        /// \brief Return the size of the component.
        //* =================================================================
        virtual extent get_size_() const = 0;

        //* =================================================================
        /// \brief Returns the size that the component would prefer to have.
        //* =================================================================
        virtual extent get_preferred_size_() const = 0;

        //* =================================================================
        /// \brief Sets a property to have a specific value.
        //* =================================================================
        virtual void set_property_(
            std::string const &name, boost::any const &value) = 0;

        //* =================================================================
        /// \brief Returns a property with a specific name.
        //* =================================================================
        virtual boost::any get_property_(std::string const &name) const = 0;

        //* =================================================================
        /// \brief Returns a signal with a specific name.
        //* =================================================================
        virtual boost::any get_signal_(std::string const &name) const = 0;
    };

    //* =====================================================================
    /// \brief The wrapper of the model concept, templated around a class
    /// that will actually provide the implementation.
    //* =====================================================================
    template <class Model>
    struct impl : concept
    {
        //* =================================================================
        /// \brief Constructor
        //* =================================================================
        impl(Model &&mdl)
          : model_(std::forward<Model>(mdl))
        {
        }

        //* =================================================================
        /// \brief Send an event of any type to the component, and possibly
        /// get a response.
        //* =================================================================
        virtual boost::any event_(boost::any const &ev) override
        {
            return model_.event(ev);
        }

        //* =================================================================
        /// \brief Set the size of the component.
        //* =================================================================
        virtual void set_size_(extent const &size) override
        {
            model_.set_size(size);
        }

        //* =================================================================
        /// \brief Return the size of the component.
        //* =================================================================
        virtual extent get_size_() const override
        {
            return model_.get_size();
        }

        //* =================================================================
        /// \brief Returns the size that the component would prefer to have.
        //* =================================================================
        virtual extent get_preferred_size_() const override
        {
            return model_.get_preferred_size();
        }

        //* =================================================================
        /// \brief Sets a property to have a specific value.
        //* =================================================================
        virtual void set_property_(
            std::string const &name, boost::any const &value) override
        {
            model_.set_property(name, value);
        }

        //* =================================================================
        /// \brief Returns a property with a specific name.
        //* =================================================================
        virtual boost::any get_property_(
            std::string const &name) const override
        {
            return model_.get_property(name);
        }

        //* =================================================================
        /// \brief Returns a signal with a specific name.
        //* =================================================================
        virtual boost::any get_signal_(
            std::string const &name) const override
        {
            return model_.get_signal(name);
        }

        Model model_;
    };

    std::unique_ptr<concept> self_;
};

}}

#endif

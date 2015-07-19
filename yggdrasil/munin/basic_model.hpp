// ==========================================================================
// Munin Basic Model.
//
// Copyright (C) 2015 Matthew Chaplain, All Rights Reserved.
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
#ifndef YGGDRASIL_MUNIN_BASIC_MODEL_HPP_
#define YGGDRASIL_MUNIN_BASIC_MODEL_HPP_

#include "yggdrasil/munin/extent.hpp"
#include <boost/any.hpp>
#include <unordered_map>
#include <string>

namespace yggdrasil { namespace munin {
    
//* =========================================================================
/// \brief An implemenation of the Model concept
/// \see yggdrasil::munin::model
//* =========================================================================
class basic_model
{
public :
    //* =====================================================================
    /// \brief Default constructor
    //* =====================================================================
    basic_model() = default;

    //* =====================================================================
    /// \brief Copy Constructor
    /// This is deleted, since copying the model would imply copying signals,
    /// which would mean that components could receive updates from objects
    /// that it has not registered with.  And that would be weird.
    //* =====================================================================
    basic_model(basic_model const &other) = delete;

    //* =====================================================================
    /// \brief Move Constructor
    //* =====================================================================
    basic_model(basic_model &&other) = default;

    //* =====================================================================
    /// \brief Copy Assignment Operator
    /// See Copy Constructor above for deletion rationale.
    //* =====================================================================
    basic_model &operator=(basic_model const &other) = delete;

    //* =====================================================================
    /// \brief Move Assignment Operator
    //* =====================================================================
    basic_model &operator=(basic_model &&other)= default;
    
    //* =====================================================================
    /// \brief Send an event of any type to the component, and possibly get
    /// a response.
    //* =====================================================================
    boost::any event(boost::any const &ev);

    //* =====================================================================
    /// \brief Set the size of the component.
    //* =====================================================================
    void set_size(extent const &size);

    //* =====================================================================
    /// \brief Return the size of the component.
    //* =====================================================================
    extent get_size() const;

    //* =====================================================================
    /// \brief Returns the size that the component would prefer to have.
    //* =====================================================================
    extent get_preferred_size() const;

    //* =====================================================================
    /// \brief Sets a property to have a specific value.
    //* =====================================================================
    boost::any get_property(std::string const &name) const;

    //* =====================================================================
    /// \brief Returns a property with a specific name.
    //* =====================================================================
    void set_property(std::string const &name, boost::any const &value);

    //* =====================================================================
    /// \brief Returns a signal with a specific name.
    //* =====================================================================
    boost::any get_signal(std::string const &name) const;
    
private :
    std::unordered_map<std::string, boost::any> properties_;
    std::unordered_map<std::string, boost::any> signals_;
    extent size_;
};

}}

#endif

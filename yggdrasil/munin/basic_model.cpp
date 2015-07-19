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
#include "yggdrasil/munin/basic_model.hpp"
namespace yggdrasil { namespace munin {
    
// ==========================================================================
// EVENT
// ==========================================================================
boost::any basic_model::event(boost::any const &ev)
{
    return {};
}

// ==========================================================================
// SET_SIZE
// ==========================================================================
void basic_model::set_size(extent const &size)
{
    size_ = size;
}

// ==========================================================================
// GET_SIZE
// ==========================================================================
extent basic_model::get_size() const
{
    return size_;
}

// ==========================================================================
// GET_PREFERRED_SIZE
// ==========================================================================
extent basic_model::get_preferred_size() const
{
    return {};
}

// ==========================================================================
// GET_PROPERTY
// ==========================================================================
boost::any basic_model::get_property(std::string const &name) const
{
    auto it = properties_.find(name);
    return it == properties_.end() ? boost::any{} : it->second;
}

// ==========================================================================
// SET_PROPERTY
// ==========================================================================
void basic_model::set_property(
    std::string const &name, boost::any const &value)
{
    properties_[name] = value;
}

// ==========================================================================
// GET_SIGNAL
// ==========================================================================
boost::any basic_model::get_signal(std::string const &name) const
{
    auto it = signals_.find(name);
    return it == signals_.end() ? boost::any{} : it->second;
}
    
}}
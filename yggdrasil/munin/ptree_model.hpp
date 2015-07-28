// ==========================================================================
// Munin Property Tree Model.
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
#ifndef YGGDRASIL_MUNIN_PTREE_MODEL_HPP_
#define YGGDRASIL_MUNIN_PTREE_MODEL_HPP_

#include "yggdrasil/munin/model.hpp"
#include <boost/property_tree/ptree_fwd.hpp>

namespace yggdrasil { namespace munin {

namespace detail {

void populate_model_from_ptree(
    ::yggdrasil::munin::model &model,
    ::boost::property_tree::ptree const &tree);
}

template <class ConcreteModel>
ConcreteModel create_model_from_ptree(::boost::property_tree::ptree const& tree)
{
    auto concrete_model = ConcreteModel{};
    auto model = ::yggdrasil::munin::model{concrete_model};

    ::yggdrasil::munin::detail::populate_model_from_ptree(model, tree);

    return concrete_model;
}

}}

#endif

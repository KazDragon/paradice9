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
#include "yggdrasil/munin/estring.hpp"
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <vector>

namespace yggdrasil { namespace munin {

template <class ConcreteModel>
ConcreteModel create_model_from_ptree(boost::property_tree::ptree const& tree)
{
    auto concrete_model = ConcreteModel{};
    auto model = yggdrasil::munin::model{concrete_model};
    
    auto const &properties = tree.get_child_optional("properties");
    
    // It's perfectly valid for there to be no properties.  It means that the
    // component in question doesn't have any model values.
    if (properties) 
    {
        for (auto const &entry : *properties)
        {
            auto const &property = entry.second;
            auto const &name = property.get<std::string>("name");

            auto const &default_value_property = property.get_child("default");
            
            // If the "default" property has children of its own, then it is
            // an array.  Otherwise, it is a textual value.
            if (default_value_property.begin() == default_value_property.end())
            {
                set_property(
                    model, 
                    name, 
                    estring(default_value_property.get_value<std::string>()));
            }
            else
            {
                std::vector<estring> value;
                
                for (auto const &child : default_value_property)
                {
                    value.push_back(estring(child.second.get_value<std::string>()));
                }
                
                set_property(model, name, value);
            }
        }
    }

    return concrete_model;
}

}}

#endif

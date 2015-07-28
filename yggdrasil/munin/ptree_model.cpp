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
#include "yggdrasil/munin/ptree_model.hpp"
#include "yggdrasil/munin/estring.hpp"
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <vector>

namespace yggdrasil { namespace munin { namespace detail {

void populate_typed_property(
    yggdrasil::munin::model &model,
    boost::property_tree::ptree const &tree,
    std::string const &name,
    std::string const &type)
{
    auto const &default_value_property = tree.get_child("default");

    if (type == "char")
    {
        set_property(
            model,
            name,
            char(default_value_property.get_value<std::string>()[0]));
    }
    else if (type == "element")
    {
        set_property(
            model,
            name,
            estring(default_value_property.get_value<std::string>())[0]);
    }
    else if (type == "string")
    {
        set_property(
            model,
            name,
            default_value_property.get_value<std::string>());
    }
    else if (type == "estring")
    {
        set_property(
            model,
            name,
            estring(default_value_property.get_value<std::string>()));
    }
    else if (type == "array of char")
    {
        std::vector<char> value;
        
        for (auto const &child : default_value_property)
        {
            value.push_back(child.second.get_value<std::string>()[0]);
        }
        
        set_property(model, name, value);
    }
    else if (type == "array of element")
    {
        std::vector<element> value;
        
        for (auto const &child : default_value_property)
        {
            value.push_back(
                estring(child.second.get_value<std::string>())[0]);
        }
        
        set_property(model, name, value);
    }
    else if (type == "array of string")
    {
        std::vector<std::string> value;
        
        for (auto const &child : default_value_property)
        {
            value.push_back(child.second.get_value<std::string>());
        }
        
        set_property(model, name, value);
    }
    else if (type == "array of estring")
    {
        std::vector<estring> value;

        for (auto const &child : default_value_property)
        {
            value.push_back(estring(child.second.get_value<std::string>()));
        }

        set_property(model, name, value);
    }
}

void populate_untyped_property(
    yggdrasil::munin::model &model,
    boost::property_tree::ptree const &tree,
    std::string name)
{
    auto const &default_value_property = tree.get_child("default");

    // If the "default" property has children of its own, then it is
    // an array of estring.  Otherwise, it is an estring.
    if (default_value_property.begin() == default_value_property.end())
    {
        populate_typed_property(model, tree, name, "estring");
    }
    else
    {
        populate_typed_property(model, tree, name, "array of estring");
    }
}

void populate_model_from_ptree(
    yggdrasil::munin::model &model,
    boost::property_tree::ptree const &tree)
{
    auto const &properties = tree.get_child_optional("properties");

    // It's perfectly valid for there to be no properties.  It means that the
    // component in question doesn't have any model values.
    if (properties)
    {
        for (auto const &entry : *properties)
        {
            auto const &property = entry.second;
            auto const &name = property.get<std::string>("name");
            auto const &type = property.get_optional<std::string>("type");

            if (type)
            {
                populate_typed_property(model, property, name, *type);
            }
            else
            {
                populate_untyped_property(model, property, name);
            }
        }
    }
}

}}}


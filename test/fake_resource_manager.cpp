#include "fake_resource_manager.hpp"

static std::unique_ptr<yggdrasil::munin::resource_manager> resource_manager;

void fake_resource_manager::add_default_properties(
    std::string const &name, 
    boost::property_tree::ptree const &tree)
{
    default_properties_[name] = tree;
}

boost::property_tree::ptree const &fake_resource_manager::get_default_properties(
    std::string const &name) const
{
    return default_properties_.find(name)->second;
}

void set_resource_manager(
    std::unique_ptr<yggdrasil::munin::resource_manager> mgr)
{
    resource_manager = std::move(mgr);
}

namespace yggdrasil { namespace munin {
    
resource_manager const &get_resource_manager()
{
    return *::resource_manager;
}

}}

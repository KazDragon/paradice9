#include "yggdrasil/munin/resource_manager.hpp"
#include <boost/property_tree/ptree.hpp>
#include <map>
#include <memory>

class fake_resource_manager : public yggdrasil::munin::resource_manager
{
public :
    void add_default_properties(
        std::string const &name,
        boost::property_tree::ptree const &tree);
    
    //* =====================================================================
    /// \brief Retrieves a property tree that represents the default property
    /// values for a component with the given name.
    //* =====================================================================
    boost::property_tree::ptree const &get_default_properties(
        std::string const &name) const override;
private :
    std::map<std::string, boost::property_tree::ptree> default_properties_;
};

void set_resource_manager(std::unique_ptr<yggdrasil::munin::resource_manager> mgr);
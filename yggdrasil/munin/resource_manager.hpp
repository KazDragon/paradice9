// ==========================================================================
// Munin Resource Manager.
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
#ifndef YGGDRASIL_MUNIN_RESOURCE_MANAGER_HPP_
#define YGGDRASIL_MUNIN_RESOURCE_MANAGER_HPP_

#include <boost/property_tree/ptree_fwd.hpp>
#include <string>

namespace yggdrasil { namespace munin {

//* =========================================================================
/// \brief A resource manager is a store of assets used to make Munin
/// components.  In particular, the property trees that can be used to
/// provide the models for fundamental components, and for composite
/// components.
//* =========================================================================
class resource_manager
{
public :
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~resource_manager() = default;
    
    //* =====================================================================
    /// \brief Retrieves a property tree that represents the default property
    /// values for a component with the given name.
    //* =====================================================================
    virtual boost::property_tree::ptree const &get_default_properties(
        std::string const &name) const = 0;

protected :
    resource_manager() = default;
};

//* =========================================================================
/// \brief Retrieves the application's resource manager.
/// \par NOTE
/// This is not provided by Munin.  An application using the Munin library
/// must provide its own resource manager, and must populate the property
/// trees itself.
//* =========================================================================
extern resource_manager const &get_resource_manager();

}}

#endif

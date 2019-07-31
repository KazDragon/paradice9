// ==========================================================================
// Munin Container.
//
// Copyright (C) 2010 Matthew Chaplain, All Rights Reserved.
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
#ifndef MUNIN_CONTAINER_HPP_
#define MUNIN_CONTAINER_HPP_

#include "munin/export.hpp"
#include "munin/component.hpp"
#include <boost/optional.hpp>
#include <vector>

namespace munin {

class layout;

BOOST_STATIC_CONSTANT(
    std::uint32_t, HIGHEST_LAYER = (std::numeric_limits<std::uint32_t>::max)());
BOOST_STATIC_CONSTANT(
    std::uint32_t, LOWEST_LAYER = (std::numeric_limits<std::uint32_t>::min)());
BOOST_STATIC_CONSTANT(
    std::uint32_t, DEFAULT_LAYER = (std::numeric_limits<std::uint32_t>::max)() / 2);

//* =========================================================================
/// \brief A graphical element capable of containing and arranging other
/// subcomponents.
//* =========================================================================
class MUNIN_EXPORT container
    : public component
    , public std::enable_shared_from_this<container>
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    container();

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~container();

    //* =====================================================================
    /// \brief Retrieves the number of components that this container
    /// contains.
    //* =====================================================================
    std::uint32_t get_number_of_components() const;

    //* =====================================================================
    /// \brief Adds a component to the container.
    /// \param component The component to add to the container
    /// \param layout_hint A hint to be passed to the container's current
    ///        layout.
    //* =====================================================================
    void add_component(
        std::shared_ptr<component> const &comp
      , boost::any                 const &layout_hint = boost::any()
      , std::uint32_t                         layer = DEFAULT_LAYER);

    //* =====================================================================
    /// \brief Removes a component from the container.
    //* =====================================================================
    void remove_component(std::shared_ptr<component> const &component);

    //* =====================================================================
    /// \brief Retrieves a component from the container.
    //* =====================================================================
    std::shared_ptr<component> get_component(std::uint32_t index) const;

    //* =====================================================================
    /// \brief Retrieves a component's hint from the container.
    //* =====================================================================
    boost::any get_component_hint(std::uint32_t index) const;

    //* =====================================================================
    /// \brief Retrieves a component's layer from the container.
    //* =====================================================================
    std::uint32_t get_component_layer(std::uint32_t index) const;

    //* =====================================================================
    /// \brief Sets the container's current layout for a given layer
    //* =====================================================================
    void set_layout(
        std::unique_ptr<munin::layout> lyt
      , std::uint32_t                      layer = DEFAULT_LAYER);

    //* =====================================================================
    /// \brief Retrieves the current layout from the container for a given
    /// layer.
    //* =====================================================================
    boost::optional<munin::layout &> get_layout(
        std::uint32_t layer = DEFAULT_LAYER) const;

    //* =====================================================================
    /// \brief Returns an array of layers that currently have layouts
    //* =====================================================================
    std::vector<std::uint32_t> get_layout_layers() const;

protected :
    //* =====================================================================
    /// \brief Called by get_number_of_components().  Derived classes must
    /// override this function in order to retrieve the number of components
    /// in this container in a custom manner.
    //* =====================================================================
    virtual std::uint32_t do_get_number_of_components() const = 0;

    //* =====================================================================
    /// \brief Called by add_component().  Derived classes must override
    /// this function in order to add a component to the container in a
    /// custom manner.
    //* =====================================================================
    virtual void do_add_component(
        std::shared_ptr<component> const &comp
      , boost::any                 const &hint
      , std::uint32_t                         layer) = 0;

    //* =====================================================================
    /// \brief Called by remove_component().  Derived classes must override
    /// this function in order to add a component to the container in a
    /// custom manner.
    //* =====================================================================
    virtual void do_remove_component(
        std::shared_ptr<component> const &comp) = 0;

    //* =====================================================================
    /// \brief Called by get_component().  Derived classes must override this
    /// function in order to retrieve a component in a custom manner.
    //* =====================================================================
    virtual std::shared_ptr<component> do_get_component(
        std::uint32_t index) const = 0;

    //* =====================================================================
    /// \brief Called by get_component_hint().  Derived classes must
    /// override this function in order to retrieve a component hint in a
    /// custom manner.
    //* =====================================================================
    virtual boost::any do_get_component_hint(std::uint32_t index) const = 0;

    //* =====================================================================
    /// \brief Called by get_component_layer().  Derived classes must
    /// override this function in order to retrieve a component layer in a
    /// custom manner.
    //* =====================================================================
    virtual std::uint32_t do_get_component_layer(std::uint32_t index) const = 0;

    //* =====================================================================
    /// \brief Called by set_layout.  Derived classes must override this
    /// function in order to set a layout in a custom manner.
    //* =====================================================================
    virtual void do_set_layout(
        std::unique_ptr<munin::layout> lyt
      , std::uint32_t                      layer) = 0;

    //* =====================================================================
    /// \brief Called by get_layout.  Derived classes must override this
    /// function in order to get the container's layout in a custom manner.
    //* =====================================================================
    virtual boost::optional<munin::layout &> do_get_layout(
        std::uint32_t layer) const = 0;

    //* =====================================================================
    /// \brief Called by get_layout_layers.  Derived classes must override
    /// this function in order to get the container's layout in a custom
    /// manner.
    //* =====================================================================
    virtual std::vector<std::uint32_t> do_get_layout_layers() const = 0;

    //* =====================================================================
    /// \brief Called by draw().  Derived classes must override this function
    /// in order to draw onto the passed context.  A component must only draw
    /// the part of itself specified by the region.
    ///
    /// \param ctx the context in which the component should draw itself.
    /// \param region the region relative to this component's origin that
    /// should be drawn.
    //* =====================================================================
    virtual void do_draw(
        context                     &ctx
      , terminalpp::rectangle const &region);

private :
    struct impl;
    std::shared_ptr<impl> pimpl_;
};

}

#endif

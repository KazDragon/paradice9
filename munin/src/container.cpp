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
#include "munin/container.hpp"
#include "munin/algorithm.hpp"
#include "munin/context.hpp"
#include "munin/layout.hpp"
#include <terminalpp/canvas_view.hpp>
#include <boost/scope_exit.hpp>
#include <vector>

namespace munin {

namespace {
    typedef std::pair<
        std::shared_ptr<component>
      , std::vector<boost::signals::connection>
    > component_connections_type;
}

// ==========================================================================
// CONTAINER::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct container::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(container &self)
        : self_(self)
    {
    }

    // ======================================================================
    // SUBCOMPONENT_REDRAW_HANDLER
    // ======================================================================
    void subcomponent_redraw_handler(
        std::weak_ptr<component> weak_subcomponent,
        std::vector<rectangle>   regions)
    {
        auto subcomponent = weak_subcomponent.lock();

        if (subcomponent != NULL)
        {
            // Each region is bound to the origin of the component in question.
            // It must be rebound to the origin of the container.  We do this
            // by offsetting the regions' origins by the origin of the
            // subcomponent within this container.
            auto origin = subcomponent->get_position();

            for (auto &rect : regions)
            {
                rect.origin.x += origin.x;
                rect.origin.y += origin.y;
            }

            // This new information must be passed up the component heirarchy.
            self_.on_redraw(regions);
        }
    }

    // ======================================================================
    // SUBCOMPONENT_CURSOR_POSITION_CHANGE_HANDLER
    // ======================================================================
    void subcomponent_cursor_position_change_handler(
        std::weak_ptr<component> weak_subcomponent
      , terminalpp::point        position)
    {
        auto subcomponent = weak_subcomponent.lock();

        if (subcomponent != NULL && subcomponent->has_focus())
        {
            self_.on_cursor_position_changed(self_.get_position() + position);
        }
    }

    // ======================================================================
    // ENSURE_COMPONENTS_SORTED
    // ======================================================================
    void ensure_components_sorted()
    {
        if (dirty_)
        {
            components_.clear();
            std::vector<odin::u32> layers;

            auto number_of_components = self_.get_number_of_components();

            for (odin::u32 index = 0; index < number_of_components; ++index)
            {
                auto comp       = self_.get_component(index);
                auto comp_layer = self_.get_component_layer(index);

                auto component_insert_position = components_.begin();
                auto layer_insert_position =     layers.begin();

                while (component_insert_position != components_.end()
                    && layer_insert_position != layers.end()
                    && comp_layer >= *layer_insert_position)
                {
                    ++component_insert_position;
                    ++layer_insert_position;
                }

                components_.insert(component_insert_position, comp);
                layers.insert(layer_insert_position, comp_layer);
            }

            dirty_ = false;
        }
    }

    container                               &self_;
    bool                                     dirty_ = true;
    std::vector<std::shared_ptr<component>>  components_;
    std::vector<component_connections_type>  component_connections_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
container::container()
{
    pimpl_ = std::make_shared<impl>(ref(*this));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
container::~container()
{
    for(auto &con : pimpl_->component_connections_)
    {
        for (auto &cnx : con.second)
        {
            cnx.disconnect();
        }
    }
}

// ==========================================================================
// GET_NUMBER_OF_COMPONENTS
// ==========================================================================
odin::u32 container::get_number_of_components() const
{
    return do_get_number_of_components();
}

// ==========================================================================
// ADD_COMPONENT
// ==========================================================================
void container::add_component(
    std::shared_ptr<component> const &comp
  , boost::any                 const &layout_hint
  , odin::u32                         layer)
{
    do_add_component(comp, layout_hint, layer);
    pimpl_->dirty_ = true;

    component_connections_type component_connections;
    component_connections.first = comp;

    // Subscribe to the component's redraw event.
    component_connections.second.push_back(
        comp->on_redraw.connect(
            std::bind(
                &container::impl::subcomponent_redraw_handler
              , pimpl_
              , std::weak_ptr<component>(comp)
              , std::placeholders::_1)));

    component_connections.second.push_back(
        comp->on_cursor_position_changed.connect(
            std::bind(
                &container::impl::subcomponent_cursor_position_change_handler
              , pimpl_
              , std::weak_ptr<component>(comp)
              , std::placeholders::_1)));

    component_connections.second.push_back(
        comp->on_layout_change.connect([this]{on_layout_change();}));

    pimpl_->component_connections_.push_back(component_connections);
    on_layout_change();

    // A redraw of the container is required.
    on_redraw({ rectangle({}, get_size()) });
}

// ==========================================================================
// REMOVE_COMPONENT
// ==========================================================================
void container::remove_component(std::shared_ptr<component> const &comp)
{
    pimpl_->dirty_ = true;

    // Disconnect any signals for the component.
    for (auto cur = std::begin(pimpl_->component_connections_);
         cur != std::end(pimpl_->component_connections_);
        )
    {
        if (cur->first == comp)
        {
            for (auto &cnx : cur->second)
            {
                cnx.disconnect();
            }

            cur = pimpl_->component_connections_.erase(cur);
        }
        else
        {
            ++cur;
        }
    }

    do_remove_component(comp);

    // Now that the subcomponent has been removed, it becomes necessary
    // to re-lay the container out.
    on_layout_change();
}

// ==========================================================================
// GET_COMPONENT
// ==========================================================================
std::shared_ptr<component> container::get_component(odin::u32 index) const
{
    return do_get_component(index);
}

// ==========================================================================
// GET_COMPONENT_HINT
// ==========================================================================
boost::any container::get_component_hint(odin::u32 index) const
{
    return do_get_component_hint(index);
}

// ==========================================================================
// GET_COMPONENT_LAYER
// ==========================================================================
odin::u32 container::get_component_layer(odin::u32 index) const
{
    return do_get_component_layer(index);
}

// ==========================================================================
// SET_LAYOUT
// ==========================================================================
void container::set_layout(
    std::unique_ptr<munin::layout> lyt
  , odin::u32                      layer /*= DEFAULT_LAYER*/)
{
    do_set_layout(std::move(lyt), layer);
}

// ==========================================================================
// GET_LAYOUT
// ==========================================================================
boost::optional<munin::layout &> container::get_layout(odin::u32 layer) const
{
    return do_get_layout(layer);
}

// ==========================================================================
// GET_LAYOUT_LAYERS
// ==========================================================================
std::vector<odin::u32> container::get_layout_layers() const
{
    return do_get_layout_layers();
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void container::do_draw(
    context         &ctx
  , rectangle const &region)
{
    auto &cvs = ctx.get_canvas();

    // First, we obtain a list of components sorted by layer from lowest
    // to highest.
    pimpl_->ensure_components_sorted();

    for (auto const &current_component : pimpl_->components_)
    {
        // The area we want to draw is the intersection of the region
        // passed in above and the region of space that the component
        // occupies.
        rectangle component_region(
            current_component->get_position()
          , current_component->get_size());

        auto draw_region = intersection(region, component_region);

        if (draw_region.is_initialized())
        {
            // The draw region is currently relative to this container's
            // origin.  It should be relative to the child's origin.
            draw_region->origin -= component_region.origin;

            // The canvas must have an offset applied to it so that the
            // inner component can pretend that it is being drawn with its
            // container being at position (0,0).
            auto const position = current_component->get_position();
            cvs.offset_by({position.x, position.y});

            // Ensure that the offset is unapplied before exit of this
            // function.
            BOOST_SCOPE_EXIT( (&cvs)(&position) )
            {
                cvs.offset_by({-position.x, -position.y});
            } BOOST_SCOPE_EXIT_END

            current_component->draw(ctx, draw_region.get());
        }
    }
}

}


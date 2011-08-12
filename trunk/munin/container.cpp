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
#include "munin/canvas.hpp"
#include "munin/layout.hpp"
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/scope_exit.hpp>
#include <boost/weak_ptr.hpp>
#include <vector>

using namespace odin;
using namespace boost;
using namespace boost::assign;
using namespace std;

namespace munin {

namespace {
    typedef std::pair<
        boost::shared_ptr<component>
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
        weak_ptr<component> weak_subcomponent
      , vector<rectangle>   regions)
    {
        BOOST_AUTO(subcomponent, weak_subcomponent.lock());

        if (subcomponent != NULL)
        {
            // Each region is bound to the origin of the component in question.
            // It must be rebound to the origin of the container.  We do this
            // by offsetting the regions' origins by the origin of the
            // subcomponent within this container.
            BOOST_AUTO(origin, subcomponent->get_position());
            
            BOOST_FOREACH(rectangle &rect, regions)
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
        weak_ptr<component> weak_subcomponent
      , point               position)
    {
        BOOST_AUTO(subcomponent, weak_subcomponent.lock());
            
        if (subcomponent != NULL && subcomponent->has_focus())
        {
            self_.on_cursor_position_changed(self_.get_position() + position);
        }
    }
    
    // ======================================================================
    // GET_COMPONENTS_SORTED
    // ======================================================================
    vector< shared_ptr<component> > get_components_sorted() const
    {
        vector< shared_ptr<component> > components;
        vector< u32                   > layers;
        
        BOOST_AUTO(number_of_components, self_.get_number_of_components());
        
        for (u32 index = 0; index < number_of_components; ++index)
        {
            BOOST_AUTO(comp,       self_.get_component(index));
            BOOST_AUTO(comp_layer, self_.get_component_layer(index));
            
            BOOST_AUTO(component_insert_position, components.begin());
            BOOST_AUTO(layer_insert_position,     layers.begin());
            
            while (component_insert_position != components.end()
                && layer_insert_position != layers.end()
                && comp_layer >= *layer_insert_position)
            {
                ++component_insert_position;
                ++layer_insert_position;
            }
            
            components.insert(component_insert_position, comp);
            layers.insert(layer_insert_position, comp_layer);
        }
        
        return components;
    }
    
    container                               &self_;
    std::vector<component_connections_type>  component_connections_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
container::container()
{
    pimpl_ = make_shared<impl>(ref(*this));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
container::~container()
{
    BOOST_FOREACH(
        component_connections_type &con
      , pimpl_->component_connections_)
    {
        BOOST_FOREACH(boost::signals::connection &cnx, con.second)
        {
            cnx.disconnect();
        }
    }
}

// ==========================================================================
// GET_NUMBER_OF_COMPONENTS
// ==========================================================================
u32 container::get_number_of_components() const
{
    return do_get_number_of_components();
}

// ==========================================================================
// ADD_COMPONENT
// ==========================================================================
void container::add_component(
    shared_ptr<component> const &comp
  , any                   const &layout_hint
  , u32                          layer)
{
    do_add_component(comp, layout_hint, layer);

    component_connections_type component_connections;
    component_connections.first = comp;
    
    // Subscribe to the component's redraw event.
    component_connections.second.push_back(
        comp->on_redraw.connect(
            boost::bind(
                &container::impl::subcomponent_redraw_handler
              , pimpl_
              , boost::weak_ptr<component>(comp)
              , _1)));
    
    component_connections.second.push_back(
        comp->on_cursor_position_changed.connect(
            boost::bind(
                &container::impl::subcomponent_cursor_position_change_handler
              , pimpl_
              , boost::weak_ptr<component>(comp)
              , _1)));
    
    component_connections.second.push_back(
        comp->on_layout_change.connect(
            boost::bind(boost::ref(on_layout_change))));
    
    pimpl_->component_connections_.push_back(component_connections);
    on_layout_change();

    // A redraw of the container is required.
    on_redraw(list_of(rectangle(point(), get_size())));
}

// ==========================================================================
// REMOVE_COMPONENT
// ==========================================================================
void container::remove_component(shared_ptr<component> const &comp)
{
    // Disconnect any signals for the component.
    for (vector<component_connections_type>::iterator cur =
             pimpl_->component_connections_.begin();
         cur != pimpl_->component_connections_.end();
         )
    {
        if (cur->first == comp)
        {
            BOOST_FOREACH(boost::signals::connection& cnx, cur->second)
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
shared_ptr<component> container::get_component(u32 index) const
{
    return do_get_component(index);
}

// ==========================================================================
// GET_COMPONENT_HINT
// ==========================================================================
any container::get_component_hint(u32 index) const
{
    return do_get_component_hint(index);
}

// ==========================================================================
// GET_COMPONENT_LAYER
// ==========================================================================
u32 container::get_component_layer(u32 index) const
{
    return do_get_component_layer(index);
}

// ==========================================================================
// SET_LAYOUT
// ==========================================================================
void container::set_layout(
    shared_ptr<munin::layout> const &lyt
  , u32                              layer /*= DEFAULT_LAYER*/)
{
    do_set_layout(lyt, layer);
}

// ==========================================================================
// GET_LAYOUT
// ==========================================================================
shared_ptr<layout> container::get_layout(u32 layer) const
{
    return do_get_layout(layer);
}

// ==========================================================================
// GET_LAYOUT_LAYERS
// ==========================================================================
runtime_array<u32> container::get_layout_layers() const
{
    return do_get_layout_layers();
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void container::do_draw(
    canvas          &cvs
  , rectangle const &region)
{
    // First, we obtain a list of components sorted by layer from lowest
    // to highest.
    BOOST_AUTO(components, pimpl_->get_components_sorted());

    BOOST_FOREACH(shared_ptr<component> current_component, components)
    {
        // The area we want to draw is the intersection of the region
        // passed in above and the region of space that the component
        // occupies.
        rectangle component_region(
            current_component->get_position()
          , current_component->get_size());
        
        BOOST_AUTO(draw_region, intersection(region, component_region));
        
        if (draw_region.is_initialized())
        {
            // The draw region is currently relative to this container's
            // origin.  It should be relative to the child's origin.
            draw_region->origin -= component_region.origin;

            // The canvas must have an offset applied to it so that the
            // inner component can pretend that it is being drawn with its
            // container being at position (0,0).
            point const position = current_component->get_position();//this->get_position();
            cvs.apply_offset(position.x, position.y);

            // Ensure that the offset is unapplied before exit of this
            // function.
            BOOST_SCOPE_EXIT( (&cvs)(&position) )
            {
                cvs.apply_offset(-position.x, -position.y);
            } BOOST_SCOPE_EXIT_END

            current_component->draw(cvs, draw_region.get());
        }
    }
}

}


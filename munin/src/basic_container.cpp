// ==========================================================================
// Munin Basic Container.
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
#include "munin/basic_container.hpp"
#include "munin/layout.hpp"
#include <terminalpp/ansi/mouse.hpp>
#include <boost/scope_exit.hpp>

namespace munin {

namespace {
    typedef std::map<odin::u32, std::unique_ptr<layout>> layered_layout_map;
}

// ==========================================================================
// BASIC_CONTAINER::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct basic_container::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(basic_container &self)
        : self_(self)
        , has_focus_(false)
        , cursor_state_(false)
        , enabled_(true)
    {
    }

    // ======================================================================
    // SUBCOMPONENT_FOCUS_SET_HANDLER
    // ======================================================================
    void subcomponent_focus_set_handler(
        std::weak_ptr<component> const &subcomponent)
    {
        auto focussed_component = subcomponent.lock();

        if (focussed_component != NULL)
        {
            // Iterate through all our subcomponents and ensure that this is
            // the only subcomponent to have focus.  Then, if we do not have
            // focus, set our focus.
            for (auto const &current_component : components_)
            {
                if (current_component != focussed_component
                 && current_component->has_focus())
                {
                    current_component->lose_focus();
                }
            }

            if (!has_focus_)
            {
                has_focus_ = true;
                self_.on_focus_set();
            }

            focus_changed();
        }
    }

    // ======================================================================
    // SUBCOMPONENT_FOCUS_LOST_HANDLER
    // ======================================================================
    void subcomponent_focus_lost_handler(
        std::weak_ptr<component> const &subcomponent)
    {
        auto unfocussed_component = subcomponent.lock();

        if (unfocussed_component != NULL)
        {
            // Iterate through all our subcomponents to see if we have any
            // subcomponents focused (for example, setting the focus of a
            // new subcomponent may cause the focus for the reporting
            // component to be lost).  If we do not, then unfocus this
            // component.
            bool component_is_focused = false;

            for (auto const &current_component : components_)
            {
                if (current_component->has_focus())
                {
                    component_is_focused = true;
                    break;
                }
            }

            if (!component_is_focused)
            {
                has_focus_ = false;
                self_.on_focus_lost();
            }

            focus_changed();
        }
    }

    // ======================================================================
    // FOCUS_CHANGED
    // ======================================================================
    void focus_changed()
    {
        if (has_focus_)
        {
            // Find the currently focussed subcomponent.
            for (auto const &current_component : components_)
            {
                if (current_component->has_focus())
                {
                    // Update for the new cursor state if necessary.
                    bool new_cursor_state =
                        current_component->get_cursor_state();

                    if (new_cursor_state != cursor_state_)
                    {
                        cursor_state_ = new_cursor_state;
                        self_.on_cursor_state_changed(cursor_state_);
                    }

                    // If the cursor state is enabled, then update for a new
                    // cursor position.
                    if (cursor_state_)
                    {
                        terminalpp::point cursor_position =
                            current_component->get_position()
                          + current_component->get_cursor_position();

                        self_.on_cursor_position_changed(cursor_position);
                    }
                }
            }
        }
    }

    // ======================================================================
    // SUBCOMPONENT_CURSOR_STATE_CHANGE_HANDLER
    // ======================================================================
    void subcomponent_cursor_state_change_handler(
        std::weak_ptr<component> const &weak_subcomponent
      , bool                            state)
    {
        auto subcomponent = weak_subcomponent.lock();

        if (subcomponent
         && state != cursor_state_
         && subcomponent->has_focus())
        {
            cursor_state_ = state;
            self_.on_cursor_state_changed(cursor_state_);
        }
    }

    // ======================================================================
    // SUBCOMPONENT_CURSOR_POSITION_CHANGE_HANDLER
    // ======================================================================
    void subcomponent_cursor_position_change_handler(
        std::weak_ptr<component> const &weak_subcomponent
      , terminalpp::point               position)
    {
        auto subcomponent = weak_subcomponent.lock();

        if (subcomponent && subcomponent->has_focus())
        {
            self_.on_cursor_position_changed(
                subcomponent->get_position() + position);
        }
    }

    // ======================================================================
    // SUBCOMPONENT_POSITION_CHANGE_HANDLER
    // ======================================================================
    void subcomponent_position_change_handler(
        std::weak_ptr<component> const &weak_subcomponent
      , terminalpp::point               changed_from
      , terminalpp::point               changed_to)
    {
        auto subcomponent = weak_subcomponent.lock();

        if (subcomponent)
        {
            auto const subcomponent_size = subcomponent->get_size();

            std::vector<rectangle> regions =
            {
                rectangle(changed_from, subcomponent_size),
                rectangle(changed_to, subcomponent_size)
            };

            self_.on_redraw(
            {
                { changed_from, subcomponent_size },
                { changed_to, subcomponent_size },
            });
        }
    }

    // ======================================================================
    // SUBCOMPONENT_PREFERRED_SIZE_CHANGE_HANDLER
    // ======================================================================
    void subcomponent_preferred_size_changed_handler(
        std::weak_ptr<component> const &weak_subcomponent)
    {
        auto subcomponent = weak_subcomponent.lock();

        if (subcomponent)
        {
            self_.on_preferred_size_changed();
        }
    }

    // ======================================================================
    // FOCUS_NEXT_HAS_FOCUS
    // ======================================================================
    void focus_next_has_focus()
    {
        // We have focus.  Therefore, find the subcomponent that also has
        // focus and tell it to move to the next focus.  After that, if it
        // has no focus, we move the focus to the next subcomponent that
        // can be focused.  If there is no such object, then we lose our
        // focus.
        odin::u32 number_of_components = odin::u32(components_.size());

        for (odin::u32 index = 0; index < number_of_components; ++index)
        {
            auto current_component = components_[index];

            if (current_component->has_focus())
            {
                current_component->focus_next();

                if (!current_component->has_focus())
                {
                    bool focused = false;

                    // Find the index of the next component that can have focus.
                    for (odin::u32 next_index = index + 1;
                         next_index < number_of_components && !focused;
                         ++next_index)
                    {
                        auto next_component = components_[next_index];

                        if (next_component->can_focus())
                        {
                            next_component->focus_next();
                            focused = true;
                        }
                    }

                    if (!focused)
                    {
                        has_focus_ = false;
                        self_.on_focus_lost();
                    }
                }

                break;
            }
        }
    }

    // ======================================================================
    // FOCUS_NEXT_NO_FOCUS
    // ======================================================================
    void focus_next_no_focus()
    {
        // We do not currently have any focus, therefore we command the first
        // subcomponent to focus its next component.
        for (auto const &current_component : components_)
        {
            if (current_component->can_focus())
            {
                current_component->focus_next();
                break;
            }
        }
    }

    // ======================================================================
    // FOCUS_PREVIOUS_HAS_FOCUS
    // ======================================================================
    void focus_previous_has_focus()
    {
        // We do have focus.  Find the currently focused component and tell
        // it to move its focus to its previous subcomponent.  If, after
        // that, it no longer has focus, then we find the component prior
        // to that that can have focus and tell it to set its previous focus.
        // If there is no component to focus, then just lose our focus.
        auto number_of_components = odin::u32(components_.size());

        for (odin::u32 index = 0; index < number_of_components; ++index)
        {
            auto current_component = components_[index];

            if (current_component->has_focus())
            {
                current_component->focus_previous();

                if (!current_component->has_focus())
                {
                    bool focused = false;

                    // Seek for the first subcomponent prior to the unfocused
                    // one that can have a focus, and focus it.
                    for (auto last_index = index;
                         last_index > 0 && !focused;
                         --last_index)
                    {
                        auto last_component = components_[last_index - 1];

                        if (last_component->can_focus())
                        {
                            // If the component is a container, this will focus
                            // the last component in that container.
                            last_component->focus_previous();
                            focused = true;
                        }
                    }

                    if (!focused)
                    {
                        // There are no subcomponents to focus.
                        has_focus_ = false;
                        self_.on_focus_lost();
                    }
                }

                break;
            }
        }
    }

    // ======================================================================
    // FOCUS_PREVIOUS_NO_FOCUS
    // ======================================================================
    void focus_previous_no_focus()
    {
        // Find the last component that could have focus and focus its
        // last element.
        auto number_of_components = odin::u32(components_.size());

        for (auto index = number_of_components; index > 0; --index)
        {
            auto last_component = components_[index - 1];

            if (last_component->can_focus())
            {
                last_component->focus_previous();
                break;
            }
        }
    }

    basic_container                                     &self_;
    std::weak_ptr<component>                             parent_;
    std::vector<std::shared_ptr<component>>              components_;
    std::vector<boost::any>                              component_hints_;
    std::vector<odin::u32>                               component_layers_;
    std::vector<std::vector<boost::signals::connection>> component_connections_;
    layered_layout_map                                   layouts_;
    rectangle                                            bounds_;
    bool                                                 has_focus_;
    bool                                                 cursor_state_;
    bool                                                 enabled_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
basic_container::basic_container()
{
    pimpl_ = std::make_shared<impl>(std::ref(*this));
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
basic_container::~basic_container()
{
    for (auto &connections : pimpl_->component_connections_)
    {
        for (auto &connection : connections)
        {
            connection.disconnect();
        }
    }
}

// ==========================================================================
// DO_SET_POSITION
// ==========================================================================
void basic_container::do_set_position(terminalpp::point const &position)
{
    pimpl_->bounds_.origin = position;
}

// ==========================================================================
// DO_GET_POSITION
// ==========================================================================
terminalpp::point basic_container::do_get_position() const
{
    return pimpl_->bounds_.origin;
}

// ==========================================================================
// DO_SET_SIZE
// ==========================================================================
void basic_container::do_set_size(terminalpp::extent const &size)
{
    pimpl_->bounds_.size = size;
    on_layout_change();
}

// ==========================================================================
// DO_GET_SIZE
// ==========================================================================
terminalpp::extent basic_container::do_get_size() const
{
    return pimpl_->bounds_.size;
}

// ==========================================================================
// DO_SET_PARENT
// ==========================================================================
void basic_container::do_set_parent(std::shared_ptr<component> const &parent)
{
    pimpl_->parent_ = parent;
}

// ==========================================================================
// DO_GET_PARENT
// ==========================================================================
std::shared_ptr<component> basic_container::do_get_parent() const
{
    return pimpl_->parent_.lock();
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
terminalpp::extent basic_container::do_get_preferred_size() const
{
    // If there are any layouts, then find the union of their preferred
    // sizes.  Otherwise, our current size is just fine.
    if (pimpl_->layouts_.empty())
    {
        return get_size();
    }

    terminalpp::extent preferred_size(0, 0);

    // Sort the components/hints into layers
    typedef std::map<odin::u32, std::vector<std::shared_ptr<component>>> clmap;
    typedef std::map<odin::u32, std::vector<boost::any>>                 chmap;

    clmap component_layers_map;
    chmap component_hints_map;

    // Iterate through the components, sorting them by layer
    for (odin::u32 index = 0; index < pimpl_->components_.size(); ++index)
    {
        auto comp =  pimpl_->components_[index];
        auto hint =  pimpl_->component_hints_[index];
        auto layer = pimpl_->component_layers_[index];

        component_layers_map[layer].push_back(comp);
        component_hints_map[layer].push_back(hint);
    }

    // Iterate through the layers, layout out each.
    for (auto &component_layer_pair : component_layers_map)
    {
        auto layer =      component_layer_pair.first;
        auto components = component_layer_pair.second;
        auto hints =      component_hints_map[layer];

        auto lyt = get_layout(layer);

        if (!lyt || components.size() == 0)
        {
            // Either there is no layout for this layer, or there are no
            // components in this layer.  Hence no point in laying it out.
            // Continue with the next layer.
            continue;
        }

        auto lp_preferred_size = lyt->get_preferred_size(components, hints);

        preferred_size.width =
            (std::max)(preferred_size.width, lp_preferred_size.width);
        preferred_size.height =
            (std::max)(preferred_size.height, lp_preferred_size.height);
    }

    return preferred_size;
}

// ==========================================================================
// DO_GET_NUMBER_OF_COMPONENTS
// ==========================================================================
odin::u32 basic_container::do_get_number_of_components() const
{
    return odin::u32(pimpl_->components_.size());
}

// ==========================================================================
// DO_ADD_COMPONENT
// ==========================================================================
void basic_container::do_add_component(
    std::shared_ptr<component> const &comp
  , boost::any                 const &hint
  , odin::u32                         layer)
{
    // Store the component and the layer in which it is to be drawn.
    pimpl_->components_.push_back(comp);
    pimpl_->component_hints_.push_back(hint);
    pimpl_->component_layers_.push_back(layer);

    std::vector<boost::signals::connection> component_connections;

    // Register for callbacks for when the new subcomponent either gains
    // or loses focus.  We can make sure our own focus is correct based
    // on this information.
    component_connections.push_back(comp->on_focus_set.connect(
        std::bind(
            &basic_container::impl::subcomponent_focus_set_handler
          , pimpl_
          , std::weak_ptr<component>(comp))));

    component_connections.push_back(comp->on_focus_lost.connect(
        std::bind(
            &basic_container::impl::subcomponent_focus_lost_handler
          , pimpl_
          , std::weak_ptr<component>(comp))));

    // Register for callbacks for when the subcomponent's cursor state
    // or position changes.
    component_connections.push_back(comp->on_cursor_state_changed.connect(
        std::bind(
            &basic_container::impl::subcomponent_cursor_state_change_handler
          , pimpl_
          , std::weak_ptr<component>(comp)
          , std::placeholders::_1)));

    component_connections.push_back(comp->on_cursor_position_changed.connect(
        std::bind(
            &basic_container::impl::subcomponent_cursor_position_change_handler
          , pimpl_
          , std::weak_ptr<component>(comp)
          , std::placeholders::_1)));

    // Register for callbacks for when the subcomponent's position
    // changes.
    component_connections.push_back(comp->on_position_changed.connect(
        std::bind(
            &basic_container::impl::subcomponent_position_change_handler
          , pimpl_
          , std::weak_ptr<component>(comp)
          , std::placeholders::_1
          , std::placeholders::_2)));

    // Register for callbacks for when the subcomponent's preferred size
    // changes.
    component_connections.push_back(comp->on_preferred_size_changed.connect(
        std::bind(
            &basic_container::impl::subcomponent_preferred_size_changed_handler
           , pimpl_
           , std::weak_ptr<component>(comp))));

    pimpl_->component_connections_.push_back(component_connections);

    comp->set_parent(shared_from_this());
}

// ==========================================================================
// DO_REMOVE_COMPONENT
// ==========================================================================
void basic_container::do_remove_component(
    std::shared_ptr<component> const &comp)
{
    // When we remove a component, we must also remove the associated hint,
    // layer, and signal connections (which must also be disconnected).
    auto current_component = pimpl_->components_.begin();
    auto current_hint =      pimpl_->component_hints_.begin();
    auto current_layer =     pimpl_->component_layers_.begin();
    auto current_signals =   pimpl_->component_connections_.begin();

    while (current_component != pimpl_->components_.end()
        && current_hint      != pimpl_->component_hints_.end()
        && current_layer     != pimpl_->component_layers_.end()
        && current_signals   != pimpl_->component_connections_.end())
    {
        if (*current_component == comp)
        {
            current_component  = pimpl_->components_.erase(current_component);
            current_hint       = pimpl_->component_hints_.erase(current_hint);
            current_layer      = pimpl_->component_layers_.erase(current_layer);

            for (auto &sig : *current_signals)
            {
                sig.disconnect();
            }

            current_signals = pimpl_->component_connections_.erase(current_signals);
        }
        else
        {
            ++current_component;
            ++current_hint;
            ++current_layer;
            ++current_signals;
        }
    }

    comp->set_parent({});
}

// ==========================================================================
// DO_GET_COMPONENT
// ==========================================================================
std::shared_ptr<component> basic_container::do_get_component(odin::u32 index) const
{
    return pimpl_->components_[index];
}

// ==========================================================================
// DO_GET_COMPONENT_HINT
// ==========================================================================
boost::any basic_container::do_get_component_hint(odin::u32 index) const
{
    return pimpl_->component_hints_[index];
}

// ==========================================================================
// DO_GET_COMPONENT_LAYER
// ==========================================================================
odin::u32 basic_container::do_get_component_layer(odin::u32 index) const
{
    return pimpl_->component_layers_[index];
}

// ==========================================================================
// DO_SET_LAYOUT
// ==========================================================================
void basic_container::do_set_layout(
    std::unique_ptr<munin::layout> lyt
  , odin::u32                      layer)
{
    pimpl_->layouts_[layer] = std::move(lyt);
}

// ==========================================================================
// DO_GET_LAYOUT
// ==========================================================================
boost::optional<layout &> basic_container::do_get_layout(odin::u32 layer) const
{
    auto result = pimpl_->layouts_.find(layer);

    return result == pimpl_->layouts_.end()
                   ? boost::optional<munin::layout &>()
                   : boost::optional<munin::layout &>(*result->second.get());
}

// ==========================================================================
// DO_GET_LAYOUT_LAYERS
// ==========================================================================
std::vector<odin::u32> basic_container::do_get_layout_layers() const
{
    std::vector<odin::u32> layers(pimpl_->layouts_.size());

    odin::u32 index = 0;
    for (auto const &lp : pimpl_->layouts_)
    {
        layers[index++] = lp.first;
    }

    return layers;
}

// ==========================================================================
// DO_HAS_FOCUS
// ==========================================================================
bool basic_container::do_has_focus() const
{
    return pimpl_->has_focus_;
}

// ==========================================================================
// DO_SET_CAN_FOCUS
// ==========================================================================
void basic_container::do_set_can_focus(bool focus)
{
    // As this is controlled by contained components, this is
    // completely ignored.
}

// ==========================================================================
// DO_CAN_FOCUS
// ==========================================================================
bool basic_container::do_can_focus() const
{
    for (auto const &comp : pimpl_->components_)
    {
        if (comp->can_focus())
        {
            return true;
        }
    }

    return false;
}

// ==========================================================================
// DO_SET_FOCUS
// ==========================================================================
void basic_container::do_set_focus()
{
    if (can_focus())
    {
        // Find the first component that can be focussed and focus it.
        for (auto const &current_component : pimpl_->components_)
        {
            if (current_component->can_focus())
            {
                current_component->set_focus();
                break;
            }
        }

        pimpl_->has_focus_ = true;
        on_focus_set();
    }
}

// ==========================================================================
// DO_LOSE_FOCUS
// ==========================================================================
void basic_container::do_lose_focus()
{
    for (auto const &current_component : pimpl_->components_)
    {
        if (current_component->has_focus())
        {
            current_component->lose_focus();
        }
    }

    pimpl_->has_focus_ = false;
}

// ==========================================================================
// DO_FOCUS_NEXT
// ==========================================================================
void basic_container::do_focus_next()
{
    if (pimpl_->has_focus_)
    {
        pimpl_->focus_next_has_focus();
    }
    else
    {
        pimpl_->focus_next_no_focus();
    }
}

// ==========================================================================
// DO_FOCUS_PREVIOUS
// ==========================================================================
void basic_container::do_focus_previous()
{
    if (pimpl_->has_focus_)
    {
        pimpl_->focus_previous_has_focus();
    }
    else
    {
        pimpl_->focus_previous_no_focus();
    }
}

// ==========================================================================
// DO_GET_FOCUSSED_COMPONENT
// ==========================================================================
std::shared_ptr<component> basic_container::do_get_focussed_component()
{
    if (has_focus())
    {
        for (auto const &current_component : pimpl_->components_)
        {
            if (current_component->has_focus())
            {
                return current_component->get_focussed_component();
            }
        }
    }

    return {};
}

// ==========================================================================
// DO_ENABLE
// ==========================================================================
void basic_container::do_enable()
{
    pimpl_->enabled_ = true;
}

// ==========================================================================
// DO_DISABLE
// ==========================================================================
void basic_container::do_disable()
{
    pimpl_->enabled_ = false;
}

// ==========================================================================
// DO_IS_ENABLED
// ==========================================================================
bool basic_container::do_is_enabled() const
{
    return pimpl_->enabled_;
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void basic_container::do_event(boost::any const &event)
{
    // We split the events into two types.  Mouse events are passed to
    // whichever component is under the mouse click.  All other events are
    // passed to the focussed component.
    auto report = boost::any_cast<terminalpp::ansi::mouse::report>(&event);
    
    if (report != NULL)
    {
        for (auto const &current_component : pimpl_->components_)
        {
            auto position = current_component->get_position();
            auto size =     current_component->get_size();

            // Check to see if the reported position is within the component's
            // bounds.
            if (report->x_position_ >= position.x
             && report->x_position_  < position.x + size.width
             && report->y_position_ >= position.y
             && report->y_position_  < position.y + size.height)
            {
                // Copy the mouse's report and adjust it so that the
                // subcomponent's position is taken into account.
                terminalpp::ansi::mouse::report subreport;
                subreport.button_     = report->button_;
                subreport.x_position_ = odin::u8(report->x_position_ - position.x);
                subreport.y_position_ = odin::u8(report->y_position_ - position.y);

                // Forward the event onto the component, then look no further.
                current_component->event(subreport);
                break;
            }
        }
    }
    else
    {
        for (auto const &current_component : pimpl_->components_)
        {
            if (current_component->has_focus())
            {
                current_component->event(event);
                break;
            }
        }
    }
}

// ==========================================================================
// DO_GET_CURSOR_STATE
// ==========================================================================
bool basic_container::do_get_cursor_state() const
{
    return pimpl_->cursor_state_;
}

// ==========================================================================
// DO_GET_CURSOR_POSITION
// ==========================================================================
terminalpp::point basic_container::do_get_cursor_position() const
{
    // If we have no focus, then return the default position.
    if (pimpl_->has_focus_ && pimpl_->cursor_state_)
    {
        // Find the subcomponent that has focus and get its cursor
        // position.  This must then be offset by the subcomponent's
        // position within our container.
        for (auto const &current_component : pimpl_->components_)
        {
            if (current_component->has_focus())
            {
                return current_component->get_position()
                     + current_component->get_cursor_position();
            }
        }
    }

    // Either we do not have focus, or the currently focussed subcomponent
    // does not have a cursor.  Return the default position.
    return {};
}

// ==========================================================================
// DO_SET_CURSOR_POSITIONG
// ==========================================================================
void basic_container::do_set_cursor_position(terminalpp::point const &position)
{
    // If we have no focus, then ignore this.
    if (pimpl_->has_focus_ && pimpl_->cursor_state_)
    {
        // Find the subcomponent that has focus and set its cursor
        // position.  This must then be offset by the subcomponent's
        // position within our container.
        for (auto const &current_component : pimpl_->components_)
        {
            if (current_component->has_focus())
            {
                current_component->set_cursor_position(
                    position - current_component->get_position());
            }
        }
    }
}

// ==========================================================================
// DO_SET_ATTRIBUTE
// ==========================================================================
void basic_container::do_set_attribute(
    std::string const &name
  , boost::any  const &attr)
{
    // There's no reason that a container should have any attributes set,
    // so instead this is passed on to all contained components.
    for (auto const &comp : pimpl_->components_)
    {
        comp->set_attribute(name, attr);
    }
}

// ==========================================================================
// DO_LAYOUT
// ==========================================================================
void basic_container::do_layout()
{
    // Sort the components/hints into layers
    typedef std::map<odin::u32, std::vector<std::shared_ptr<component>>> clmap;
    typedef std::map<odin::u32, std::vector<boost::any>>                 chmap;

    clmap component_layers_map;
    chmap component_hints_map;

    // Iterate through the components, sorting them by layer
    for (odin::u32 index = 0; index < pimpl_->components_.size(); ++index)
    {
        auto comp =  pimpl_->components_[index];
        auto hint =  pimpl_->component_hints_[index];
        auto layer = pimpl_->component_layers_[index];

        component_layers_map[layer].push_back(comp);
        component_hints_map[layer].push_back(hint);
    }

    auto size = get_size();

    // Iterate through the layers, layout out each.
    for (auto const &component_layer_pair : component_layers_map)
    {
        auto layer =      component_layer_pair.first;
        auto components = component_layer_pair.second;
        auto hints =      component_hints_map[layer];

        auto lyt = get_layout(layer);

        if (!lyt || components.size() == 0)
        {
            // Either there is no layout for this layer, or there are no
            // components in this layer.  Hence no point in laying it out.
            // Continue with the next layer.
            continue;
        }

        (*lyt)(components, hints, size);
    }

    // Now that all the sizes are correct for this container, iterate through
    // each subcomponent, and lay them out in turn.
    for (auto const &comp : pimpl_->components_)
    {
        comp->layout();
    }
}

}


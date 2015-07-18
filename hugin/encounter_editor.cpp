// ==========================================================================
// Hugin encounter Editor
//
// Copyright (C) 2013 Matthew Chaplain, All Rights Reserved.
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
#include "hugin/encounter_editor.hpp"
#include <munin/algorithm.hpp>
#include <munin/ansi/protocol.hpp>
#include <munin/basic_container.hpp>
#include <munin/button.hpp>
#include <munin/compass_layout.hpp>
#include <munin/edit.hpp>
#include <munin/filled_box.hpp>
#include <munin/framed_component.hpp>
#include <munin/grid_layout.hpp>
#include <munin/horizontal_squeeze_layout.hpp>
#include <munin/list.hpp>
#include <munin/scroll_pane.hpp>
#include <munin/solid_frame.hpp>
#include <munin/text_area.hpp>
#include <paradice/beast.hpp>

namespace hugin {

namespace {
    BOOST_STATIC_CONSTANT(odin::u32, BALANCED_LAYOUT_PREFERRED = 0);
    BOOST_STATIC_CONSTANT(odin::u32, BALANCED_LAYOUT_SHARED = 1);
}

// ==========================================================================
// \brief A class that implements a balanced horizontal layout, with two 
// kinds of component: one where width is preferred, and one where width is
// shared equally across all shared types.
// ==========================================================================
class balanced_layout : public munin::layout
{
private :
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to retrieve the preferred size of the layout
    /// in a custom manner.
    //* =====================================================================
    virtual munin::extent do_get_preferred_size(
        std::vector<std::shared_ptr<munin::component>> const &components,
        std::vector<boost::any>                        const &hints) const
    {
        munin::extent preferred_size(0, 0);

        for (auto const &comp : components)
        {
            auto size = comp->get_size();
            preferred_size.width += size.width;
            preferred_size.height = 
                (std::max)(size.height, preferred_size.height);
        }

        return preferred_size;
    }
    
    //* =====================================================================
    /// \brief Called by operator().  Derived classes must override this
    /// function in order to lay a container's components out in a custom
    /// manner.
    //* =====================================================================
    virtual void do_layout(
        std::vector<std::shared_ptr<munin::component>> const &components,
        std::vector<boost::any>                        const &hints,
        munin::extent                                         size)
    {
        odin::u32 shared_components = 0;
        auto shared_width = size.width;

        // First, iterate the components and factor out any "preferred size"
        // components so we can see what must be shared.
        for (size_t index = 0; index < components.size(); ++index)
        {
            odin::u32 const *psize_hint = boost::any_cast<odin::u32>(&hints[index]);
            odin::u32 size_hint = psize_hint 
              ? *psize_hint 
              : BALANCED_LAYOUT_PREFERRED;

            if (size_hint == BALANCED_LAYOUT_PREFERRED)
            {
                auto preferred_width =
                    components[index]->get_preferred_size().width;

                shared_width -= (std::min)(shared_width, preferred_width);
            }
            else
            {
                ++shared_components;
            }
        }

        // Now obtain the minimum width for the shared components.
        odin::u32 shared_component_width =
            shared_components == 0
          ? 0
          : shared_width / shared_components;

        // There may be some left over units.  They can be shared across the
        // components
          odin::u32 shared_component_extras =
            shared_components == 0
          ? 0
          : shared_width % shared_components;

        // Now lay out each component
          odin::u32 current_x = 0;

        for (size_t index = 0; index < components.size(); ++index)
        {
            auto comp = components[index];

            odin::u32 const *psize_hint = boost::any_cast<odin::u32>(&hints[index]);
            odin::u32 size_hint = psize_hint 
              ? *psize_hint 
              : BALANCED_LAYOUT_PREFERRED;

            if (size_hint == BALANCED_LAYOUT_PREFERRED)
            {
                auto width = comp->get_preferred_size().width;
                comp->set_position(munin::point(current_x, 0));
                comp->set_size(munin::extent(width, size.height));

                current_x += width;
            }
            else // size_hint == BALANCED_LAYOUT_SHARED
            {
                odin::u32 extras = 0;

                if (shared_component_extras != 0)
                {
                    ++extras;
                    --shared_component_extras;
                }

                auto width = shared_component_width + extras;
                comp->set_position(munin::point(current_x, 0));
                comp->set_size(munin::extent(width, size.height));

                current_x += width;
            }
        }
    }
};

// ==========================================================================
// ENCOUNTER_EDITOR::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct encounter_editor::impl
{
    std::shared_ptr<munin::edit>             encounter_name_field_;
    std::shared_ptr<munin::list>             encounter_bestiary_list_;
    std::shared_ptr<munin::list>             encounter_beasts_list_;

    std::shared_ptr<munin::text_area>        beast_description_area_;
    
    std::shared_ptr<munin::basic_container>  split_container_;
    std::shared_ptr<munin::basic_container>  lower_container_;

    std::shared_ptr<munin::basic_container>  buttons_container_;
    std::shared_ptr<munin::basic_container>  bestiary_button_container_;
    std::shared_ptr<munin::basic_container>  beast_button_container_;
    
    bool                                     beasts_active_;

    std::shared_ptr<munin::button>           insert_button_;
    std::shared_ptr<munin::button>           delete_button_;

    std::shared_ptr<munin::button>           edit_button_;

    std::shared_ptr<munin::button>           save_button_;
    std::shared_ptr<munin::button>           revert_button_;

    std::shared_ptr<munin::button>           up_button_;
    std::shared_ptr<munin::button>           down_button_;

    std::vector<std::shared_ptr<paradice::beast>>  bestiary_;
    std::vector<std::shared_ptr<paradice::beast>>  beasts_;

    void update_bestiary_list()
    {
        std::vector<std::vector<munin::element_type>> beast_names;

        for (auto const &current_beast : bestiary_)
        {
            beast_names.push_back(munin::string_to_elements(
                current_beast->get_name()));
        }

        encounter_bestiary_list_->set_items(beast_names);
        encounter_bestiary_list_->set_item_index(-1);
    }

    void update_beasts_list()
    {
        std::vector<std::vector<munin::element_type>> beast_names;

        for (auto const &beast : beasts_)
        {
            beast_names.push_back(munin::string_to_elements(
                beast->get_name()));
        }

        encounter_beasts_list_->set_items(beast_names);
        encounter_beasts_list_->set_item_index(-1);
    }

    void on_bestiary_item_changed()
    {
        // Only one item of the bestiary and beasts lists can be highlighted
        // at once.  Therefore, if this has been changed to a valid value,
        // de-select the beasts list's item.
        if (encounter_bestiary_list_->get_item_index() != -1)
        {
            encounter_beasts_list_->set_item_index(-1);
        }
    }

    void on_beasts_item_changed()
    {
        // Only one item of the bestiary and beasts lists can be highlighted
        // at once.  Therefore, if this has been changed to a valid value,
        // de-select the bestiary list's item.
        auto index = encounter_beasts_list_->get_item_index();

        if (index == -1)
        {
            // There is no longer any selected beast.  Remove the description
            // text area if it ever was there.  Also switch the buttons around.
            if (beasts_active_)
            {
                split_container_->remove_component(lower_container_);
                buttons_container_->remove_component(beast_button_container_);
                buttons_container_->add_component(bestiary_button_container_);
                beasts_active_ = false;
            }
        }
        else
        {
            // Update the text of the encounter area to be that of the newly
            // selected beast.
            auto doc = beast_description_area_->get_document();
            doc->delete_text({0, doc->get_text_size()});
            doc->insert_text(
                munin::string_to_elements(beasts_[index]->get_description()));
            doc->set_caret_index(0);

            // Also show the text area if we aren't already, and switch the
            // buttons around.
            if (!beasts_active_)
            {
                split_container_->add_component(lower_container_);
                buttons_container_->remove_component(
                    bestiary_button_container_);
                buttons_container_->add_component(beast_button_container_);
                beasts_active_ = true;
            }

            encounter_bestiary_list_->set_item_index(-1);
        }
    }

    void on_insert()
    {
        // Check to see if a beast is selected in the bestiary list on the 
        // left.
        auto selected_beast_index =
            encounter_bestiary_list_->get_item_index();

        // If it is, then clone a new beast, and drop it in the beast list
        // on the right.
        if (selected_beast_index != -1)
        {
            auto const &selected_beast = bestiary_[selected_beast_index];

            auto new_beast = std::make_shared<paradice::beast>();
            new_beast->set_name(selected_beast->get_name());
            new_beast->set_description(selected_beast->get_description());

            beasts_.push_back(new_beast);
            update_beasts_list();
        }
    }

    void on_delete()
    {
        // Check to see if a beast is selected in the beast list on the right.
        auto selected_beast_index =
            encounter_beasts_list_->get_item_index();

        // If it is, then delete the beast from the list on the right.
        if (selected_beast_index != -1)
        {
            beasts_.erase(beasts_.begin() + selected_beast_index);
            update_beasts_list();
            encounter_beasts_list_->set_item_index(selected_beast_index);
        }
    }

    void on_edit()
    {
    }

    void on_up()
    {
    }

    void on_down()
    {
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
encounter_editor::encounter_editor()
    : pimpl_(std::make_shared<impl>())
{
    // Initialise all the viewable components and connect the events.
    pimpl_->encounter_name_field_ = std::make_shared<munin::edit>();

    pimpl_->encounter_bestiary_list_ = std::make_shared<munin::list>();
    pimpl_->encounter_bestiary_list_->on_item_changed.connect(
        [this](auto idx){pimpl_->on_bestiary_item_changed();});

    pimpl_->encounter_beasts_list_ = std::make_shared<munin::list>();
    pimpl_->encounter_beasts_list_->on_item_changed.connect(
        [this](auto idx){pimpl_->on_beasts_item_changed();});

    pimpl_->beast_description_area_ = std::make_shared<munin::text_area>();

    pimpl_->insert_button_ = std::make_shared<munin::button>(munin::string_to_elements(" + "));
    pimpl_->insert_button_->on_click.connect([this]{pimpl_->on_insert();});

    pimpl_->delete_button_ = std::make_shared<munin::button>(munin::string_to_elements(" - "));
    pimpl_->delete_button_->on_click.connect([this]{pimpl_->on_delete();});

    pimpl_->edit_button_ = std::make_shared<munin::button>(munin::string_to_elements("Edit"));
    pimpl_->edit_button_->on_click.connect([this]{pimpl_->on_edit();});

    pimpl_->up_button_ = std::make_shared<munin::button>(munin::string_to_elements(" ^ "));
    pimpl_->up_button_->on_click.connect([this]{pimpl_->on_up();});

    pimpl_->down_button_ = std::make_shared<munin::button>(munin::string_to_elements(" v "));
    pimpl_->down_button_->on_click.connect([this]{pimpl_->on_down();});

    pimpl_->save_button_ = std::make_shared<munin::button>(munin::string_to_elements(" Save "));
    pimpl_->revert_button_ = std::make_shared<munin::button>(munin::string_to_elements(" Revert "));

    pimpl_->save_button_->on_click.connect([this]{on_save();});
    pimpl_->revert_button_->on_click.connect([this]{on_revert();});

    // Initialise the dynamic containers
    pimpl_->split_container_           = std::make_shared<munin::basic_container>();
    pimpl_->lower_container_           = std::make_shared<munin::basic_container>();
    pimpl_->buttons_container_         = std::make_shared<munin::basic_container>();
    pimpl_->bestiary_button_container_ = std::make_shared<munin::basic_container>();
    pimpl_->beast_button_container_    = std::make_shared<munin::basic_container>();
    pimpl_->beasts_active_ = false;

    // Now lay out the containers.
    pimpl_->bestiary_button_container_->set_layout(
        std::make_shared<munin::compass_layout>());
    pimpl_->bestiary_button_container_->add_component(
        pimpl_->insert_button_,
        munin::COMPASS_LAYOUT_NORTH);
    pimpl_->bestiary_button_container_->add_component(
        std::make_shared<munin::filled_box>(munin::element_type(' ')),
        munin::COMPASS_LAYOUT_CENTRE);

    auto inner_beast_button_container = std::make_shared<munin::basic_container>();
    inner_beast_button_container->set_layout(std::make_shared<munin::compass_layout>());
    inner_beast_button_container->add_component(
        pimpl_->down_button_,
        munin::COMPASS_LAYOUT_NORTH);
    inner_beast_button_container->add_component(
        std::make_shared<munin::filled_box>(munin::element_type(' ')),
        munin::COMPASS_LAYOUT_CENTRE);

    pimpl_->beast_button_container_->set_layout(std::make_shared<munin::compass_layout>());
    pimpl_->beast_button_container_->add_component(
        pimpl_->up_button_,
        munin::COMPASS_LAYOUT_NORTH);
    pimpl_->beast_button_container_->add_component(
        inner_beast_button_container,
        munin::COMPASS_LAYOUT_CENTRE);
    pimpl_->beast_button_container_->add_component(
        pimpl_->delete_button_
        ,munin:: COMPASS_LAYOUT_SOUTH);

    pimpl_->buttons_container_->set_layout(std::make_shared<munin::grid_layout>(1, 1));
    pimpl_->buttons_container_->add_component(
        pimpl_->bestiary_button_container_);

    auto outer_container = std::make_shared<munin::basic_container>();
    outer_container->set_layout(std::make_shared<balanced_layout>());
    outer_container->add_component(
        std::make_shared<munin::scroll_pane>(pimpl_->encounter_bestiary_list_)
      , BALANCED_LAYOUT_SHARED);
    outer_container->add_component(
        pimpl_->buttons_container_
      , BALANCED_LAYOUT_PREFERRED);
    outer_container->add_component(
        std::make_shared<munin::scroll_pane>(pimpl_->encounter_beasts_list_)
      , BALANCED_LAYOUT_SHARED);

    auto upper_container = std::make_shared<munin::basic_container>();
    upper_container->set_layout(std::make_shared<munin::compass_layout>());
    upper_container->add_component(
        std::make_shared<munin::framed_component>(
            std::make_shared<munin::solid_frame>()
          , pimpl_->encounter_name_field_),
        munin::COMPASS_LAYOUT_NORTH);
    upper_container->add_component(
        outer_container,
        munin::COMPASS_LAYOUT_CENTRE);

    pimpl_->split_container_->set_layout(
        std::make_shared<munin::horizontal_squeeze_layout>());
    pimpl_->split_container_->add_component(upper_container);

    auto edit_button_container = std::make_shared<munin::basic_container>();
    edit_button_container->set_layout(std::make_shared<munin::compass_layout>());
    edit_button_container->add_component(
        std::make_shared<munin::filled_box>(munin::element_type(' ')),
        munin::COMPASS_LAYOUT_CENTRE);
    edit_button_container->add_component(
        pimpl_->edit_button_,
        munin::COMPASS_LAYOUT_EAST);

    pimpl_->beasts_active_ = false;

    pimpl_->lower_container_->set_layout(std::make_shared<munin::compass_layout>());
    pimpl_->lower_container_->add_component(
        std::make_shared<munin::scroll_pane>(pimpl_->beast_description_area_),
        munin::COMPASS_LAYOUT_CENTRE);
    pimpl_->lower_container_->add_component(
        edit_button_container,
        munin::COMPASS_LAYOUT_SOUTH);

    auto lower_buttons_container = std::make_shared<munin::basic_container>();
    lower_buttons_container->set_layout(std::make_shared<munin::compass_layout>());
    lower_buttons_container->add_component(
        pimpl_->save_button_, munin::COMPASS_LAYOUT_WEST);
    lower_buttons_container->add_component(
        std::make_shared<munin::filled_box>(munin::element_type(' ')), 
        munin::COMPASS_LAYOUT_CENTRE);
    lower_buttons_container->add_component(
        pimpl_->revert_button_, munin::COMPASS_LAYOUT_EAST);

    auto content = get_container();
    content->set_layout(std::make_shared<munin::compass_layout>());
    content->add_component(
        pimpl_->split_container_, munin::COMPASS_LAYOUT_CENTRE);
    content->add_component(
        lower_buttons_container, munin::COMPASS_LAYOUT_SOUTH);
}
    
// ==========================================================================
// DESTRUCTOR
// ==========================================================================
encounter_editor::~encounter_editor()
{
}

// ==========================================================================
// SET_BESTIARY
// ==========================================================================
void encounter_editor::set_bestiary(
    std::vector<std::shared_ptr<paradice::beast>> const &beasts)
{
    pimpl_->bestiary_ = beasts;
    pimpl_->update_bestiary_list();
}

// ==========================================================================
// SET_ENCOUNTER_BEASTS
// ==========================================================================
void encounter_editor::set_encounter_beasts(
    std::vector<std::shared_ptr<paradice::beast>> const &beasts)
{
    pimpl_->beasts_ = beasts;
    pimpl_->update_beasts_list();
}

// ==========================================================================
// GET_ENCOUNTER_BEASTS
// ==========================================================================
std::vector<std::shared_ptr<paradice::beast>> 
    encounter_editor::get_encounter_beasts() const
{
    return pimpl_->beasts_;
}

// ==========================================================================
// SET_ENCOUNTER_NAME
// ==========================================================================
void encounter_editor::set_encounter_name(std::string const &name)
{
    auto doc = pimpl_->encounter_name_field_->get_document();
    doc->delete_text({0, doc->get_text_size()});
    doc->insert_text(munin::string_to_elements(name));
}

// ==========================================================================
// GET_ENCOUNTER_NAME
// ==========================================================================
std::string encounter_editor::get_encounter_name() const
{
    auto doc = pimpl_->encounter_name_field_->get_document();
    auto name = doc->get_line(0);
    
    return munin::ansi::string_from_elements(name);
}

}

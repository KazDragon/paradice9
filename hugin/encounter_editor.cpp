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
#include <munin/basic_container.hpp>
#include <munin/button.hpp>
#include <munin/compass_layout.hpp>
#include <munin/edit.hpp>
#include <munin/filled_box.hpp>
#include <munin/framed_component.hpp>
#include <munin/list.hpp>
#include <munin/scroll_pane.hpp>
#include <munin/solid_frame.hpp>
#include <munin/text_area.hpp>
#include <munin/vertical_squeeze_layout.hpp>
#include <paradice/beast.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

using namespace paradice;
using namespace odin;
using namespace munin;
using namespace boost;
using namespace std;

namespace hugin {

// ==========================================================================
// \brief A class that implements a balanced horizontal layout, with two 
// kinds of component: one where width is preferred, and one where width is
// shared equally across all shared types.
// ==========================================================================
class balanced_layout : public layout
{
public :
    BOOST_STATIC_CONSTANT(u32, PREFERRED = 0);
    BOOST_STATIC_CONSTANT(u32, SHARED = 1);

private :
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to retrieve the preferred size of the layout
    /// in a custom manner.
    //* =====================================================================
    virtual munin::extent do_get_preferred_size(
        vector< shared_ptr<component> > const &components
      , vector< any >                   const &hints) const
    {
        munin::extent preferred_size(0, 0);

        BOOST_FOREACH(shared_ptr<component> const &comp, components)
        {
            BOOST_AUTO(size, comp->get_size());
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
        vector< shared_ptr<component> > const &components
      , vector< any >                   const &hints
      , munin::extent                          size)
    {
        u32 shared_components = 0;
        BOOST_AUTO(shared_width, size.width);

        // First, iterate the components and factor out any "preferred size"
        // components so we can see what must be shared.
        for (size_t index = 0; index < components.size(); ++index)
        {
            u32 const *psize_hint = any_cast<u32>(&hints[index]);
            u32 size_hint = psize_hint ? *psize_hint : PREFERRED;

            if (size_hint == PREFERRED)
            {
                BOOST_AUTO(
                    preferred_width
                  , components[index]->get_preferred_size().width);

                shared_width -= (std::min)(shared_width, preferred_width);
            }
            else
            {
                ++shared_components;
            }
        }

        // Now obtain the minimum width for the shared components.
        u32 shared_component_width =
            shared_components == 0
          ? 0
          : shared_width / shared_components;

        // There may be some left over units.  They can be shared across the
        // components
        u32 shared_component_extras =
            shared_components == 0
          ? 0
          : shared_width % shared_components;

        // Now lay out each component
        u32 current_x = 0;

        for (size_t index = 0; index < components.size(); ++index)
        {
            BOOST_AUTO(comp, components[index]);

            u32 const *psize_hint = any_cast<u32>(&hints[index]);
            u32 size_hint = psize_hint ? *psize_hint : PREFERRED;

            if (size_hint == PREFERRED)
            {
                BOOST_AUTO(width, comp->get_preferred_size().width);
                comp->set_position(point(current_x, 0));
                comp->set_size(munin::extent(width, size.height));

                current_x += width;
            }
            else // size_hint == SHARED
            {
                u32 extras = 0;

                if (shared_component_extras != 0)
                {
                    ++extras;
                    --shared_component_extras;
                }

                BOOST_AUTO(width, shared_component_width + extras);
                comp->set_position(point(current_x, 0));
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
    shared_ptr<edit>             encounter_name_field_;
    shared_ptr<munin::list>      encounter_bestiary_list_;
    shared_ptr<munin::list>      encounter_beasts_list_;

    shared_ptr<edit>             beast_name_field_;
    shared_ptr<text_area>        beast_description_area_;
    
    shared_ptr<basic_container>  split_container_;
    shared_ptr<basic_container>  lower_container_;

    shared_ptr<button>           insert_button_;
    shared_ptr<button>           delete_button_;

    shared_ptr<button>           save_button_;
    shared_ptr<button>           revert_button_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
encounter_editor::encounter_editor()
    : pimpl_(make_shared<impl>())
{
    // Initialise all the viewable components
    pimpl_->encounter_name_field_ = make_shared<edit>();
    pimpl_->encounter_bestiary_list_ = make_shared<munin::list>();
    pimpl_->encounter_beasts_list_ = make_shared<munin::list>();

    pimpl_->beast_name_field_ = make_shared<edit>();
    pimpl_->beast_description_area_ = make_shared<text_area>();

    pimpl_->insert_button_ = make_shared<button>(string_to_elements(" + "));
    pimpl_->delete_button_ = make_shared<button>(string_to_elements(" - "));

    pimpl_->save_button_ = make_shared<button>(string_to_elements(" Save "));
    pimpl_->revert_button_ = make_shared<button>(string_to_elements(" Revert "));

    BOOST_AUTO(manipulators_container, make_shared<basic_container>());
    manipulators_container->set_layout(make_shared<compass_layout>());
    manipulators_container->add_component(
        pimpl_->insert_button_
      , COMPASS_LAYOUT_NORTH);
    manipulators_container->add_component(
        make_shared<filled_box>(element_type(' '))
      , COMPASS_LAYOUT_CENTRE);
    manipulators_container->add_component(
        pimpl_->delete_button_
      , COMPASS_LAYOUT_SOUTH);

    BOOST_AUTO(outer_container, make_shared<basic_container>());
    outer_container->set_layout(make_shared<balanced_layout>());
    outer_container->add_component(
        make_shared<scroll_pane>(pimpl_->encounter_bestiary_list_)
      , balanced_layout::SHARED);
    outer_container->add_component(
        manipulators_container
      , balanced_layout::PREFERRED);
    outer_container->add_component(
        make_shared<scroll_pane>(pimpl_->encounter_beasts_list_)
      , balanced_layout::SHARED);

    BOOST_AUTO(upper_container, make_shared<basic_container>());
    upper_container->set_layout(make_shared<compass_layout>());
    upper_container->add_component(
        make_shared<framed_component>(
            make_shared<solid_frame>()
          , pimpl_->encounter_name_field_)
      , COMPASS_LAYOUT_NORTH);
    upper_container->add_component(
        outer_container
      , COMPASS_LAYOUT_CENTRE);

    pimpl_->split_container_ = make_shared<basic_container>();
    pimpl_->split_container_->set_layout(
        make_shared<vertical_squeeze_layout>());
    pimpl_->split_container_->add_component(upper_container);

    pimpl_->lower_container_ = make_shared<basic_container>();
    pimpl_->lower_container_->set_layout(make_shared<compass_layout>());
    pimpl_->lower_container_->add_component(
        make_shared<framed_component>(
            make_shared<solid_frame>()
          , pimpl_->beast_name_field_)
      , COMPASS_LAYOUT_NORTH);
    pimpl_->lower_container_->add_component(
        make_shared<scroll_pane>(pimpl_->beast_description_area_)
      , COMPASS_LAYOUT_CENTRE);

    BOOST_AUTO(buttons_container, make_shared<basic_container>());
    buttons_container->set_layout(make_shared<compass_layout>());
    buttons_container->add_component(
        pimpl_->save_button_, COMPASS_LAYOUT_WEST);
    buttons_container->add_component(
        make_shared<filled_box>(element_type(' ')), COMPASS_LAYOUT_CENTRE);
    buttons_container->add_component(
        pimpl_->revert_button_, COMPASS_LAYOUT_EAST);

    BOOST_AUTO(content, get_container());
    content->set_layout(make_shared<compass_layout>());
    content->add_component(
        pimpl_->split_container_, COMPASS_LAYOUT_CENTRE);
    content->add_component(
        buttons_container, COMPASS_LAYOUT_SOUTH);
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
void encounter_editor::set_bestiary(vector< shared_ptr<beast> > beasts)
{
    vector< vector<element_type> > beast_names;

    BOOST_FOREACH(shared_ptr<const beast> const &current_beast, beasts)
    {
        beast_names.push_back(string_to_elements(current_beast->get_name()));
    }

    pimpl_->encounter_bestiary_list_->set_items(beast_names);
    pimpl_->encounter_bestiary_list_->set_item_index(-1);
}

// ==========================================================================
// SET_ENCOUNTER_BEASTS
// ==========================================================================
void encounter_editor::set_encounter_beasts(vector< shared_ptr<beast> > beasts)
{
}

// ==========================================================================
// GET_ENCOUNTER_BEASTS
// ==========================================================================
vector< shared_ptr<beast> > encounter_editor::get_encounter_beasts() const
{
    return vector< shared_ptr<beast> >();
}

// ==========================================================================
// SET_ENCOUNTER_NAME
// ==========================================================================
void encounter_editor::set_encounter_name(string const &name)
{
    /*
    BOOST_AUTO(doc, pimpl_->name_field_->get_document());
    doc->delete_text(make_pair(0, doc->get_text_size()));
    doc->insert_text(string_to_elements(name));
    */
}

// ==========================================================================
// GET_ENCOUNTER_NAME
// ==========================================================================
string encounter_editor::get_encounter_name() const
{
    /*
    BOOST_AUTO(doc, pimpl_->name_field_->get_document());
    BOOST_AUTO(name, doc->get_line(0));
    
    return string_from_elements(name);
    */
    return "";
}

}

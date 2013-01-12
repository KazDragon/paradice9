// ==========================================================================
// Munin Tabbed Frame.
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
#include "munin/tabbed_frame.hpp"
#include "munin/ansi/protocol.hpp"
#include "munin/basic_container.hpp"
#include "munin/canvas.hpp"
#include "munin/compass_layout.hpp"
#include "munin/context.hpp"
#include "munin/filled_box.hpp"
#include "munin/grid_layout.hpp"
#include "munin/image.hpp"
#include "munin/vertical_strip_layout.hpp"
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>

using namespace munin::ansi;
using namespace odin;
using namespace boost;
using namespace std;

namespace munin {

namespace {

// ==========================================================================
// TABBED_FRAME_HEADER_RIVET
// ==========================================================================
class tabbed_frame_header_rivet : public composite_component
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    tabbed_frame_header_rivet(
        element_type const &top_element
      , element_type const &middle_element
      , element_type const &bottom_element)
      : top_element_(make_shared<filled_box>(top_element))
      , middle_element_(make_shared<filled_box>(middle_element))
      , bottom_element_(make_shared<filled_box>(bottom_element))
    {
        BOOST_AUTO(content, get_container());
        content->set_layout(make_shared<grid_layout>(3, 1));
        content->add_component(top_element_);
        content->add_component(middle_element_);
        content->add_component(bottom_element_);
    };

    // ======================================================================
    // SET_TOP_ELEMENT
    // ======================================================================
    void set_top_element(element_type const &top_element)
    {
        top_element_->set_fill(top_element);
    }

    // ======================================================================
    // SET_MIDDLE_ELEMENT
    // ======================================================================
    void set_middle_element(element_type const &middle_element)
    {
        middle_element_->set_fill(middle_element);
    }

    // ======================================================================
    // SET_BOTTOM_ELEMENT
    // ======================================================================
    void set_bottom_element(element_type const &bottom_element)
    {
        bottom_element_->set_fill(bottom_element);
    }

private :
    shared_ptr<filled_box> top_element_;
    shared_ptr<filled_box> middle_element_;
    shared_ptr<filled_box> bottom_element_;
};

// ==========================================================================
// TABBED_FRAME_HEADER_LABEL
// ==========================================================================
class tabbed_frame_header_label : public composite_component
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    tabbed_frame_header_label(
        element_type         const &top_element
      , vector<element_type> const &label
      , element_type         const &bottom_element)
    {
        top_box_ = make_shared<filled_box>(top_element);
        label_ = make_shared<image>(label);
        bottom_box_ = make_shared<filled_box>(bottom_element);

        BOOST_AUTO(content, get_container());
        content->set_layout(make_shared<compass_layout>());
        content->add_component(top_box_, COMPASS_LAYOUT_NORTH);
        content->add_component(label_, COMPASS_LAYOUT_CENTRE);
        content->add_component(bottom_box_, COMPASS_LAYOUT_SOUTH);
    }

    // ======================================================================
    // SET_TOP_ELEMENT
    // ======================================================================
    void set_top_element(element_type const &element)
    {
        top_box_->set_fill(element);
    }

    // ======================================================================
    // SET_LABEL
    // ======================================================================
    void set_label(vector<element_type> const &text)
    {
        label_->set_image(text);
    }

    // ======================================================================
    // SET_BOTTOM_ELEMENT
    // ======================================================================
    void set_bottom_element(element_type const &element)
    {
        bottom_box_->set_fill(element);
    }

private :
    boost::shared_ptr<filled_box> top_box_;
    boost::shared_ptr<image>      label_;
    boost::shared_ptr<filled_box> bottom_box_;
};

// ==========================================================================
// TABBED_FRAME_HEADER
// ==========================================================================
class tabbed_frame_header : public composite_component
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    tabbed_frame_header()
      : selected_(0)
    {
        get_container()->set_layout(make_shared<grid_layout>(1, 1));
        assemble();
    }

    // ======================================================================
    // INSERT_TAB
    // ======================================================================
    void insert_tab(string const &text, optional<u32> index = optional<u32>())
    {
        // If index is not initialised, then place the new tab at the end
        // of the current list of tabs.  Otherwise, place it at the index
        // specified.
        BOOST_AUTO(
            actual_index
          , index.is_initialized() ? index.get() : tabs_.size());

        tabs_.insert(tabs_.begin() + actual_index, text);
        
        // Remove any existing content and rebuild.
        if (content_)
        {
            get_container()->remove_component(content_);
        }

        assemble();
    }

private :
    // ======================================================================
    // ASSEMBLE
    // ======================================================================
    void assemble()
    {
        // Special case: if there are no tabs, then assemble a default empty
        // tab.
        if (tabs_.empty())
        {
            assemble_default();
            return;
        }

        // Left and right rivets are always present, along with filler.
        BOOST_AUTO(
            left_rivet
          , make_shared<tabbed_frame_header_rivet>(
                element_type(double_lined_top_left_corner)
              , element_type(double_lined_vertical_beam)
              , element_type(double_lined_left_tee)));
               
        BOOST_AUTO(
            right_rivet
          , make_shared<tabbed_frame_header_rivet>(
                element_type(' ')
              , element_type(' ')
              , element_type(double_lined_top_right_corner)));

        BOOST_AUTO(filler, make_shared<basic_container>());
        filler->set_layout(make_shared<compass_layout>());
        filler->add_component(
            make_shared<filled_box>(element_type(' '))
          , COMPASS_LAYOUT_NORTH);
        filler->add_component(
            make_shared<filled_box>(element_type(' '))
          , COMPASS_LAYOUT_CENTRE);
        filler->add_component(
            make_shared<filled_box>(element_type(double_lined_horizontal_beam))
          , COMPASS_LAYOUT_SOUTH);

        // To create the middle section, use a vertical strip layout and
        // alternate label/rivet until complete.
        BOOST_AUTO(tab_section, make_shared<basic_container>());
        tab_section->set_layout(make_shared<vertical_strip_layout>());
        tab_section->add_component(left_rivet);

        for (vector<string>::size_type index = 0;
             index < tabs_.size();
             ++index)
        {
            tab_section->add_component(
                make_shared<tabbed_frame_header_label>(
                    element_type(double_lined_horizontal_beam)
                  , elements_from_string(tabs_[index])
                  , element_type(double_lined_horizontal_beam)));

            // Special case: if this is the last rivet, or if this tab is
            // selected, then it is always a top right corner.
            if (index == tabs_.size() - 1
             || index == selected_)
            {
                tab_section->add_component(
                    make_shared<tabbed_frame_header_rivet>(
                        element_type(double_lined_top_right_corner)
                      , element_type(double_lined_vertical_beam)
                      , element_type(double_lined_bottom_tee)));
            }
            // Special case: if the next tab is selected, then it is always a
            // top left corner.
            else if (selected_ == index + 1)
            {
                tab_section->add_component(
                    make_shared<tabbed_frame_header_rivet>(
                        element_type(double_lined_top_left_corner)
                      , element_type(double_lined_vertical_beam)
                      , element_type(double_lined_bottom_tee)));
            }
            // Otherwise, this is a tee.
            else
            {
                tab_section->add_component(
                    make_shared<tabbed_frame_header_rivet>(
                        element_type(double_lined_top_tee)
                      , element_type(double_lined_vertical_beam)
                      , element_type(double_lined_bottom_tee)));
            }
        }

        content_ = make_shared<basic_container>();
        content_->set_layout(make_shared<compass_layout>());
        content_->add_component(tab_section, COMPASS_LAYOUT_WEST);
        content_->add_component(filler, COMPASS_LAYOUT_CENTRE);
        content_->add_component(right_rivet, COMPASS_LAYOUT_EAST);
        get_container()->add_component(content_);
    }

    // ======================================================================
    // ASSEMBLE_DEFAULT
    // ======================================================================
    void assemble_default()
    {
        BOOST_AUTO(
            left_rivet
          , make_shared<tabbed_frame_header_rivet>(
                element_type(double_lined_top_left_corner)
              , element_type(double_lined_vertical_beam)
              , element_type(double_lined_left_tee)));

        BOOST_AUTO(
            label
          , make_shared<tabbed_frame_header_label>(
                element_type(double_lined_horizontal_beam)
              , munin::ansi::elements_from_string("")
              , element_type(double_lined_horizontal_beam)));

        BOOST_AUTO(
            right_rivet
          , make_shared<tabbed_frame_header_rivet>(
                element_type(double_lined_top_right_corner)
              , element_type(double_lined_vertical_beam)
              , element_type(double_lined_bottom_tee)));

        BOOST_AUTO(filler, make_shared<basic_container>());
        filler->set_layout(make_shared<compass_layout>());
        filler->add_component(
            make_shared<filled_box>(element_type(' '))
          , COMPASS_LAYOUT_NORTH);
        filler->add_component(
            make_shared<filled_box>(element_type(' '))
          , COMPASS_LAYOUT_CENTRE);
        filler->add_component(
            make_shared<filled_box>(element_type(double_lined_horizontal_beam))
          , COMPASS_LAYOUT_SOUTH);

        BOOST_AUTO(
            rightmost
          , make_shared<tabbed_frame_header_rivet>(
                element_type(' ')
              , element_type(' ')
              , element_type(double_lined_top_right_corner)));

        BOOST_AUTO(tabs, make_shared<basic_container>());
        tabs->set_layout(make_shared<vertical_strip_layout>());
        tabs->add_component(left_rivet);
        tabs->add_component(label);
        tabs->add_component(right_rivet);

        content_ = make_shared<basic_container>();
        content_->set_layout(make_shared<compass_layout>());
        content_->add_component(tabs, COMPASS_LAYOUT_WEST);
        content_->add_component(filler, COMPASS_LAYOUT_CENTRE);
        content_->add_component(right_rivet, COMPASS_LAYOUT_EAST);
        get_container()->add_component(content_);
    }

private :
    vector<string>              tabs_;
    shared_ptr<basic_container> content_;
    u32                         selected_;
};

}

// ==========================================================================
// TABBED_FRAME::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct tabbed_frame::impl
{
    shared_ptr<tabbed_frame_header> header_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
tabbed_frame::tabbed_frame()
    : pimpl_(make_shared<impl>())
{
    // North - tabs
    // TODO
    pimpl_->header_ = make_shared<tabbed_frame_header>();

    BOOST_AUTO(north, make_shared<basic_container>());
    north->set_layout(make_shared<grid_layout>(1, 1));
    north->add_component(pimpl_->header_);

    // West Container
    BOOST_AUTO(west, make_shared<basic_container>());
    west->set_layout(make_shared<grid_layout>(1, 1));
    west->add_component(
        make_shared<filled_box>(element_type(double_lined_vertical_beam)));

    // East Container
    BOOST_AUTO(east, make_shared<basic_container>());
    east->set_layout(make_shared<grid_layout>(1, 1));
    east->add_component(
        make_shared<filled_box>(element_type(double_lined_vertical_beam)));

    // South Container
    BOOST_AUTO(south, make_shared<basic_container>());
    south->set_layout(make_shared<compass_layout>());
    south->add_component(
        make_shared<filled_box>(element_type(double_lined_bottom_left_corner))
      , COMPASS_LAYOUT_WEST);
    south->add_component(
        make_shared<filled_box>(element_type(double_lined_horizontal_beam))
      , COMPASS_LAYOUT_CENTRE);
    south->add_component(
        make_shared<filled_box>(element_type(double_lined_bottom_right_corner))
      , COMPASS_LAYOUT_EAST);

    // All together.
    BOOST_AUTO(content, get_container());
    content->set_layout(make_shared<compass_layout>());
    content->add_component(north, COMPASS_LAYOUT_NORTH);
    content->add_component(south, COMPASS_LAYOUT_SOUTH);
    content->add_component(west, COMPASS_LAYOUT_WEST);
    content->add_component(east, COMPASS_LAYOUT_EAST);

    pimpl_->header_->insert_tab("My first tab");
    pimpl_->header_->insert_tab("My second tab");
}

// ==========================================================================
// DO_GET_TOP_BORDER_HEIGHT
// ==========================================================================
s32 tabbed_frame::do_get_top_border_height() const
{
    return 3;
}

// ==========================================================================
// DO_GET_BOTTOM_BORDER_HEIGHT
// ==========================================================================
s32 tabbed_frame::do_get_bottom_border_height() const
{
    return 1;
}

// ==========================================================================
// DO_GET_LEFT_BORDER_WIDTH
// ==========================================================================
s32 tabbed_frame::do_get_left_border_width() const
{
    return 1;
}

// ==========================================================================
// DO_GET_RIGHT_BORDER_WIDTH
// ==========================================================================
s32 tabbed_frame::do_get_right_border_width() const
{
    return 1;
}

}

// ==========================================================================
// Munin Framed Component.
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
#include "munin/framed_component.hpp"
#include "munin/algorithm.hpp"
#include "munin/basic_container.hpp"
#include "munin/frame.hpp"
#include "munin/layout.hpp"
#include <terminalpp/attribute.hpp>
#include <terminalpp/extent.hpp>

namespace munin {

namespace {

//* =========================================================================
/// \brief A specialised layout for frames that place the interior component
/// inside the frame's borders.
//* =========================================================================
class framed_component_layout
    : public layout
{
public :
    enum hint_type
    {
        hint_border
      , hint_interior
    };

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    framed_component_layout()
        : border_top_height_(0)
        , border_bottom_height_(0)
        , border_left_width_(0)
        , border_right_width_(0)
    {
    }

private :
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to retrieve the preferred size of the layout
    /// in a custom manner.
    //* =====================================================================
    virtual terminalpp::extent do_get_preferred_size(
        std::vector<std::shared_ptr<component>> const &components,
        std::vector<boost::any>                 const &hints) const
    {
        terminalpp::extent preferred_interior_size;
        terminalpp::extent preferred_border_size;

        for (odin::u32 index = 0; index < components.size(); ++index)
        {
            auto comp     = components[index];
            auto any_hint = hints[index];
            auto hint     = boost::any_cast<hint_type>(any_hint);

            if (hint == hint_interior)
            {
                preferred_interior_size = comp->get_preferred_size();
            }
            else if (hint == hint_border)
            {
                auto border = std::dynamic_pointer_cast<frame>(comp);

                border_top_height_    = border->get_top_border_height();
                border_bottom_height_ = border->get_bottom_border_height();
                border_left_width_    = border->get_left_border_width();
                border_right_width_   = border->get_right_border_width();

                preferred_border_size = terminalpp::extent(
                    border_left_width_ + border_right_width_
                  , border_top_height_ + border_bottom_height_);
            }
        }

        return terminalpp::extent(
            preferred_interior_size.width  + preferred_border_size.width
          , preferred_interior_size.height + preferred_border_size.height);
    }

    //* =====================================================================
    /// \brief Called by operator().  Derived classes must override this
    /// function in order to lay a container's components out in a custom
    /// manner.
    //* =====================================================================
    virtual void do_layout(
        std::vector<std::shared_ptr<component>> const &components,
        std::vector<boost::any>                 const &hints,
        terminalpp::extent                             size)
    {
        // This layout expects only two subcomponents: a frame, and a central
        // component.  It lays them out in the expected format.
        for (odin::u32 index = 0; index < components.size(); ++index)
        {
            auto comp     = components[index];
            auto any_hint = hints[index];
            auto hint     = boost::any_cast<hint_type>(any_hint);

            if (hint == hint_border)
            {
                comp->set_position({});
                comp->set_size(size);

                auto border = std::dynamic_pointer_cast<frame>(comp);

                border_top_height_    = border->get_top_border_height();
                border_bottom_height_ = border->get_bottom_border_height();
                border_left_width_    = border->get_left_border_width();
                border_right_width_   = border->get_right_border_width();
            }
            else if (hint == hint_interior)
            {
                comp->set_position(terminalpp::point(
                    border_left_width_
                  , border_top_height_));

                terminalpp::extent new_size;

                // Have care that the new size we would set does not become
                // less than zero due to taking the borders into account.
                new_size.width =
                    (size.width >= (border_left_width_ + border_right_width_))
                  ? (size.width - border_left_width_) - border_right_width_
                  : 0;

                new_size.height =
                    (size.height >= (border_top_height_ + border_bottom_height_))
                  ? (size.height - border_top_height_) - border_bottom_height_
                  : 0;

                comp->set_size(new_size);
            }
        }
    }

    mutable odin::s32 border_top_height_;
    mutable odin::s32 border_bottom_height_;
    mutable odin::s32 border_left_width_;
    mutable odin::s32 border_right_width_;
};

}

// ==========================================================================
// FRAMED_COMPONENT::IMPLEMENTATION_STRUCTURE
// ==========================================================================
struct framed_component::impl
{
    void update_pen()
    {
        border_->set_attribute(
            ATTRIBUTE_PEN
          , interior_->has_focus()
                ? focussed_pen_
                : unfocussed_pen_);
    }

    std::shared_ptr<frame>     border_;
    std::shared_ptr<component> interior_;

    terminalpp::attribute focussed_pen_;
    terminalpp::attribute unfocussed_pen_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
framed_component::framed_component(
    std::shared_ptr<frame>     const &border,
    std::shared_ptr<component> const &interior)
  : pimpl_(std::make_shared<impl>())
{
    pimpl_->border_   = border;
    pimpl_->interior_ = interior;

    auto container = get_container();

    container->set_layout(
        std::unique_ptr<framed_component_layout>(new framed_component_layout()));
    container->add_component(
        border, framed_component_layout::hint_border);
    container->add_component(
        interior, framed_component_layout::hint_interior);

    interior->on_focus_set.connect([this]{pimpl_->update_pen();});
    interior->on_focus_lost.connect([this]{pimpl_->update_pen();});

    pimpl_->focussed_pen_.foreground_colour_ = terminalpp::high_colour(1, 4, 5);
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
framed_component::~framed_component()
{
}

// ==========================================================================
// DO_SET_ATTRIBUTE
// ==========================================================================
void framed_component::do_set_attribute(std::string const &name, boost::any const &attr)
{
    if (name == FOCUSSED_BORDER_PEN)
    {
        terminalpp::attribute const *pen = 
            boost::any_cast<terminalpp::attribute>(&attr);

        if (pen != nullptr)
        {
            pimpl_->focussed_pen_ = *pen;
            pimpl_->update_pen();
        }
    }
    else if (name == UNFOCUSSED_BORDER_PEN)
    {
        terminalpp::attribute const *pen = 
            boost::any_cast<terminalpp::attribute>(&attr);

        if (pen != nullptr)
        {
            pimpl_->unfocussed_pen_ = *pen;
            pimpl_->update_pen();
        }
    }
}

// ==========================================================================
// DO_EVENT
// ==========================================================================
void framed_component::do_event(boost::any const &event)
{
    bool handled = false;

    auto report =
        boost::any_cast<terminalpp::ansi::mouse::report>(&event);

    if (report)
    {
        // If this intersects the interior, then we send the event onto
        // that.  Otherwise, it's clicking the frame, so we set focus
        // instead.
        if (intersection(
            rectangle(pimpl_->interior_->get_position()
                    , pimpl_->interior_->get_size())
          , rectangle(terminalpp::point(report->x_position_, report->y_position_)
                    , terminalpp::extent{1, 1})))
        {
            auto position = pimpl_->interior_->get_position();
            terminalpp::ansi::mouse::report subreport;
            subreport.button_     = report->button_;
            subreport.x_position_ = terminalpp::u8(report->x_position_ - position.x);
            subreport.y_position_ = terminalpp::u8(report->y_position_ - position.y);

            pimpl_->interior_->event(subreport);
            handled = true;
        }
        else
        {
            auto position = pimpl_->border_->get_position();
            terminalpp::ansi::mouse::report subreport;
            subreport.button_     = report->button_;
            subreport.x_position_ = terminalpp::u8(report->x_position_ - position.x);
            subreport.y_position_ = terminalpp::u8(report->y_position_ - position.y);

            pimpl_->border_->event(subreport);

            set_focus();
            handled = true;
        }
    }

    if (!handled)
    {
        composite_component::do_event(event);
    }
}

// ==========================================================================
// MAKE_FRAMED_COMPONENT
// ==========================================================================
std::shared_ptr<component> make_framed_component(
    std::shared_ptr<frame>     const &border,
    std::shared_ptr<component> const &interior)
{
    return std::make_shared<framed_component>(border, interior);
}

}

// ==========================================================================
// Munin Filled Box Component.
//
// Copyright (C) 2011 Matthew Chaplain, All Rights Reserved.
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
#include "munin/filled_box.hpp"
#include "munin/context.hpp"
#include "odin/core.hpp"
#include <terminalpp/canvas_view.hpp>

namespace munin {

// ==========================================================================
// FILLED_BOX::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct filled_box::impl
{
    terminalpp::element element_;
    terminalpp::extent  preferred_size_;
    bool                locked_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
filled_box::filled_box(terminalpp::element const &element)
    : pimpl_(std::make_shared<impl>())
{
    pimpl_->element_        = element;
    pimpl_->preferred_size_ = terminalpp::extent(1, 1);
    pimpl_->locked_         = false;
    set_can_focus(false);
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
filled_box::~filled_box()
{
}

// ==========================================================================
// SET_PREFERRED_SIZE
// ==========================================================================
void filled_box::set_preferred_size(terminalpp::extent preferred_size)
{
    pimpl_->preferred_size_ = preferred_size;
}

// ==========================================================================
// SET_FILL
// ==========================================================================
void filled_box::set_fill(terminalpp::element const &element)
{
    pimpl_->element_ = element;
    on_redraw({rectangle({}, get_size())});
}

// ==========================================================================
// GET_FILL
// ==========================================================================
terminalpp::element filled_box::get_fill() const
{
    return pimpl_->element_;
}

// ==========================================================================
// DO_GET_PREFERRED_SIZE
// ==========================================================================
terminalpp::extent filled_box::do_get_preferred_size() const
{
    return pimpl_->preferred_size_;
}

// ==========================================================================
// DO_SET_ATTRIBUTE
// ==========================================================================
void filled_box::do_set_attribute(std::string const &name, boost::any const &attr)
{
    bool attribute_changed = false;

    if (name == ATTRIBUTE_LOCK)
    {
        auto lock = boost::any_cast<bool>(&attr);

        if (lock != nullptr)
        {
            pimpl_->locked_ = *lock;
        }
    }

    if (pimpl_->locked_)
    {
        return;
    }

    if (name == ATTRIBUTE_GLYPH)
    {
        auto gly = boost::any_cast<terminalpp::glyph>(&attr);

        if (gly != nullptr)
        {
            pimpl_->element_.glyph_ = *gly;
            attribute_changed = true;
        }
    }

    if (name == ATTRIBUTE_PEN)
    {
        auto pen = boost::any_cast<terminalpp::attribute>(&attr);

        if (pen != nullptr)
        {
            pimpl_->element_.attribute_ = *pen;
            attribute_changed = true;
        }
    }

    if (attribute_changed)
    {
        on_redraw({rectangle({}, get_size())});
    }
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void filled_box::do_draw(
    context         &ctx
  , rectangle const &region)
{
    auto &cvs = ctx.get_canvas();

    for (odin::u32 row = region.origin.y;
         row < odin::u32(region.origin.y + region.size.height);
         ++row)
    {
        for (odin::u32 column = region.origin.x;
             column < odin::u32(region.origin.x + region.size.width);
             ++column)
        {
            cvs[column][row] = pimpl_->element_;
        }
    }
}

// ==========================================================================
// MAKE_FILLED_BOX
// ==========================================================================
std::shared_ptr<filled_box> make_fill(terminalpp::element const &fill)
{
    return std::make_shared<filled_box>(fill);
}

}


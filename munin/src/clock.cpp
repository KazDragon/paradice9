// ==========================================================================
// Munin Clock Component.
//
// Copyright (C) 2012 Matthew Chaplain, All Rights Reserved.
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
#include "munin/clock.hpp"
#include "munin/compass_layout.hpp"
#include "munin/context.hpp"
#include "munin/container.hpp"
#include "munin/image.hpp"
#include "munin/grid_layout.hpp"
#include <terminalpp/string.hpp>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/format.hpp>
#include <memory>

namespace munin {

// ==========================================================================
// CLOCK::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct clock::impl : public std::enable_shared_from_this<impl>
{
    std::shared_ptr<boost::asio::deadline_timer> timer_;
    std::shared_ptr<image>                       image_;

    void update_time()
    {
        auto now = boost::posix_time::second_clock::local_time();
        auto time = now.time_of_day();
        long hours   = time.hours();
        long minutes = time.minutes();

        image_->set_image(terminalpp::string(boost::str(
            boost::format("%02d:%02d") % hours % minutes)));

        schedule_next_update();
    }

private :
    void schedule_next_update()
    {
        auto now = boost::posix_time::second_clock::local_time();
        long seconds_until_next_minute =
            60 - (now.time_of_day().seconds() % 60);

        timer_->expires_from_now(boost::posix_time::seconds(seconds_until_next_minute));
        timer_->async_wait(
            [wp=std::weak_ptr<impl>(shared_from_this())]
            (auto ec)
            {
                impl::schedule_next_update_thunk(wp, ec);
            });
    }

    static void schedule_next_update_thunk(
        std::weak_ptr<impl> weak_this
      , boost::system::error_code const &ec)
    {
        if (!ec)
        {
            std::shared_ptr<impl> strong_this(weak_this.lock());

            if (strong_this)
            {
                strong_this->update_time();
            }
        }
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
clock::clock()
    : pimpl_(std::make_shared<impl>())
{
    pimpl_->image_ = make_image("00:00");

    get_container()->set_layout(make_compass_layout());
    get_container()->add_component(pimpl_->image_, COMPASS_LAYOUT_SOUTH);
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
clock::~clock()
{
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void clock::do_draw(
    context         &ctx
  , rectangle const &region)
{
    // If draw has never been called before, then it is time to initialise
    // the timer.
    if (!pimpl_->timer_)
    {
        pimpl_->timer_ = std::make_shared<boost::asio::deadline_timer>(
            std::ref(ctx.get_strand().get_io_service()));
        pimpl_->update_time();
    }

    composite_component::do_draw(ctx, region);
}

// ==========================================================================
// MAKE_CLOCK
// ==========================================================================
std::shared_ptr<component> make_clock()
{
    return std::make_shared<clock>();
}

}

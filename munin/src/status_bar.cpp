// ==========================================================================
// Munin Status Bar Component.
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
#include "munin/status_bar.hpp"
#include "munin/compass_layout.hpp"
#include "munin/container.hpp"
#include "munin/context.hpp"
#include "munin/image.hpp"
#include "munin/grid_layout.hpp"
#include "terminalpp/string.hpp"
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/format.hpp>

namespace munin {

namespace {
    BOOST_STATIC_CONSTANT(std::uint32_t, DELAY_BEFORE_MARQUEE   = 3);
    BOOST_STATIC_CONSTANT(std::uint32_t, MARQUEE_SPEED_MS       = 15);
    BOOST_STATIC_CONSTANT(std::uint32_t, CHARACTERS_PER_MARQUEE = 2);
}

// ==========================================================================
// STATUS_BAR::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct status_bar::impl : public std::enable_shared_from_this<impl>
{
    std::shared_ptr<boost::asio::deadline_timer> timer_;
    std::shared_ptr<image>                       image_;
    terminalpp::string                           message_;
    std::int32_t                                    tick_;

    void start_marquee()
    {
        tick_ = 0;
        timer_->expires_from_now(boost::posix_time::seconds(DELAY_BEFORE_MARQUEE));
        timer_->async_wait(
            [wp=std::weak_ptr<impl>(shared_from_this())]
            (auto const &)
            {
                auto pthis = wp.lock();

                if (pthis)
                {
                    pthis->marquee_progress();
                }
            });
    }

private :
    void marquee_progress()
    {
        if (message_.size() < CHARACTERS_PER_MARQUEE)
        {
            image_->set_image("");
        }
        else
        {
            message_ = terminalpp::string{message_.begin() + CHARACTERS_PER_MARQUEE, message_.end()};
            image_->set_image(message_);

            timer_->expires_from_now(boost::posix_time::milliseconds(MARQUEE_SPEED_MS));
            timer_->async_wait(
                [wp=std::weak_ptr<impl>(shared_from_this())]
                (auto const &)
                {
                    auto pthis = wp.lock();

                    if (pthis)
                    {
                        pthis->marquee_progress();
                    }
                });

        }
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
status_bar::status_bar()
  : pimpl_(std::make_shared<impl>())
{
    pimpl_->image_ = make_image(pimpl_->message_);
    pimpl_->tick_  = 0;

    get_container()->set_layout(make_compass_layout());
    get_container()->add_component(pimpl_->image_, COMPASS_LAYOUT_SOUTH);
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
status_bar::~status_bar()
{
}

// ==========================================================================
// SET_MESSAGE
// ==========================================================================
void status_bar::set_message(terminalpp::string const &message)
{
    pimpl_->message_ = message;
    pimpl_->tick_    = -1;
    pimpl_->image_->set_image(pimpl_->message_);
}

// ==========================================================================
// DO_DRAW
// ==========================================================================
void status_bar::do_draw(
    context                     &ctx,
    terminalpp::rectangle const &region)
{
    // If draw has never been called before, then it is time to initialise
    // the timer.
    if (!pimpl_->timer_)
    {
        pimpl_->timer_ = std::move(std::make_shared<boost::asio::deadline_timer>(
            std::ref(ctx.get_strand().get_io_service())));
    }

    // If a message has been written since the last time this was redrawn,
    // then we must begin a timer to ensure that it marquees (if necessary)
    // and then vanishes.
    if (pimpl_->tick_ == -1)
    {
        pimpl_->start_marquee();
    }

    composite_component::do_draw(ctx, region);
}

}


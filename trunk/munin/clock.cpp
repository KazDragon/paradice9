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
#include "munin/ansi/protocol.hpp"
#include "munin/compass_layout.hpp"
#include "munin/context.hpp"
#include "munin/image.hpp"
#include "munin/grid_layout.hpp"
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::assign;
using namespace boost::posix_time;
using namespace munin::ansi;

namespace munin {

// ==========================================================================
// CLOCK::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct clock::impl : public enable_shared_from_this<impl>
{
    shared_ptr<deadline_timer> timer_;
    shared_ptr<image>          image_;

    void update_time()
    {
        ptime now(second_clock::local_time());
        time_duration time(now.time_of_day());
        long hours   = time.hours();
        long minutes = time.minutes();
        long seconds = time.seconds();

        vector< vector<element_type> > image_data;
        image_data.push_back(elements_from_string(str(
            format("%02d:%02d") % hours % minutes)));
        image_->set_image(image_data);

        schedule_next_update();
    }

private :
    void schedule_next_update()
    {
        ptime now(second_clock::local_time());
        long seconds_until_next_minute = 
            60 - (now.time_of_day().seconds() % 60);

        timer_->expires_from_now(seconds(seconds_until_next_minute));
        timer_->async_wait(bind(
            &impl::schedule_next_update_thunk
          , weak_ptr<impl>(shared_from_this())
          , boost::asio::placeholders::error));
    }

    static void schedule_next_update_thunk(
        weak_ptr<impl> weak_this
      , boost::system::error_code const &ec)
    {
        if (ec)
        {
            shared_ptr<impl> strong_this(weak_this.lock());

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
    : pimpl_(make_shared<impl>())
{
    pimpl_->image_ = make_shared<image>(elements_from_string("00:00"));

    get_container()->set_layout(make_shared<compass_layout>());
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
        pimpl_->timer_ = make_shared<deadline_timer>(
            ref(ctx.get_strand().get_io_service()));
        pimpl_->update_time();
    }

    composite_component::do_draw(ctx, region);
}

}


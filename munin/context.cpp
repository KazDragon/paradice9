// ==========================================================================
// Munin Context.
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
#include "munin/context.hpp"

namespace munin {

// ==========================================================================
// CONTEXT::IMPLEMENTATION STRUCTURE
// ==========================================================================
class context::impl
{
public :
    impl(canvas &cvs, boost::asio::strand &st)
        : canvas_(cvs)
        , strand_(st)
    {
    }

    canvas              &canvas_;
    boost::asio::strand &strand_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
context::context(canvas &cvs, boost::asio::strand &st)
  : pimpl_(std::make_shared<impl>(std::ref(cvs), std::ref(st)))
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
context::~context()
{
}

// ==========================================================================
// GET_CANVAS
// ==========================================================================
canvas &context::get_canvas()
{
    return pimpl_->canvas_;
}

// ==========================================================================
// GET_STRAND
// ==========================================================================
boost::asio::strand &context::get_strand()
{
    return pimpl_->strand_;
}

}

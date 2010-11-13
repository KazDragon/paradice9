// ==========================================================================
// Munin ANSI Edit Component.
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
#include "munin/ansi/edit.hpp"
#include "munin/ansi/basic_container.hpp"
#include "munin/ansi/frame.hpp"
#include <boost/make_shared.hpp>

using namespace boost;

namespace munin { namespace ansi {

namespace detail {

enum subcomponent
{
    subcomponent_frame
  , subcomponent_renderer
};

}

struct edit::impl
{
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
edit::edit()
  : munin::composite_component<element_type>(make_shared<basic_container>())
  , pimpl_(new impl)
{
    // TODO:
    /*
    get_container()->set_layout(
        shared_ptr<detail::edit_layout>(new detail::edit_layout));
        */
    get_container()->add_component(
        make_shared<frame>(), detail::subcomponent_frame);

    // TODO:
    /*
    get_container()->add_component(
        shared_ptr<edit_renderer>
      , detail::subcomponent_renderer);
      */
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
edit::~edit()
{
}

}}
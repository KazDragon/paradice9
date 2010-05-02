// ==========================================================================
// Odin Telnet Input Visitor
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
#include "odin/telnet/input_visitor.hpp"
#include "odin/telnet/command_router.hpp"
#include "odin/telnet/negotiation_router.hpp"
#include "odin/telnet/subnegotiation_router.hpp"
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace odin { namespace telnet {

// ==============================================================================
// INPUT_VISITOR IMPLEMENTATION STRUCTURE
// ==============================================================================
struct input_visitor::impl
{
    shared_ptr<command_router>        command_router_;
    shared_ptr<negotiation_router>    negotiation_router_;
    shared_ptr<subnegotiation_router> subnegotiation_router_;
    function<void (string)>           text_handler_;
};

// ==============================================================================
// CONSTRUCTOR
// ==============================================================================
input_visitor::input_visitor(
    shared_ptr<command_router> const        &command_router
  , shared_ptr<negotiation_router> const    &negotiation_router
  , shared_ptr<subnegotiation_router> const &subnegotiation_router
  , function<void (string)> const           &text_handler)
  : pimpl_(new impl)
{
    pimpl_->command_router_        = command_router;
    pimpl_->negotiation_router_    = negotiation_router;
    pimpl_->subnegotiation_router_ = subnegotiation_router;
    pimpl_->text_handler_          = text_handler;
}
    
// ==============================================================================
// DESTRUCTOR
// ==============================================================================
input_visitor::~input_visitor()
{
}

// ==============================================================================
// OPERATOR()(COMMAND_TYPE)
// ==============================================================================
void input_visitor::operator()(command_type const &command)
{
    (*pimpl_->command_router_)(command);
}

// ==============================================================================
// OPERATOR()(NEGOTIATION_TYPE)
// ==============================================================================
void input_visitor::operator()(negotiation_type const &negotiation)
{
    (*pimpl_->negotiation_router_)(negotiation);
}

// ==============================================================================
// OPERATOR()(SUBNEGOTIATION_TYPE)
// ==============================================================================
void input_visitor::operator()(subnegotiation_type const &subnegotiation)
{
    (*pimpl_->subnegotiation_router_)(subnegotiation);
}

// ==============================================================================
// OPERATOR()(STRING)
// ==============================================================================
void input_visitor::operator()(string const &text)
{
    pimpl_->text_handler_(text);
}

// ==============================================================================
// APPLY_INPUT_RANGE
// ==============================================================================
void apply_input_range(
    input_visitor                                       &visitor
  , odin::runtime_array<stream::input_value_type> const &values)
{
    BOOST_FOREACH(odin::telnet::stream::input_value_type value, values)
    {
        apply_visitor(visitor, value);
    }
}

}}

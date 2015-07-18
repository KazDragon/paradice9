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

namespace odin { namespace telnet {

// ==============================================================================
// INPUT_VISITOR IMPLEMENTATION STRUCTURE
// ==============================================================================
struct input_visitor::impl
{
    impl(
        std::shared_ptr<command_router>           command_router,
        std::shared_ptr<negotiation_router>       negotiation_router,
        std::shared_ptr<subnegotiation_router>    subnegotiation_router,
        std::function<void (std::string const &)> text_handler)
      : command_router_(std::move(command_router)),
        negotiation_router_(std::move(negotiation_router)),
        subnegotiation_router_(std::move(subnegotiation_router)),
        text_handler_(std::move(text_handler))
    {
    }
    
    std::shared_ptr<command_router>           command_router_;
    std::shared_ptr<negotiation_router>       negotiation_router_;
    std::shared_ptr<subnegotiation_router>    subnegotiation_router_;
    std::function<void (std::string const &)> text_handler_;
};

// ==============================================================================
// CONSTRUCTOR
// ==============================================================================
input_visitor::input_visitor(
	std::shared_ptr<command_router>           command_router,
    std::shared_ptr<negotiation_router>       negotiation_router,
    std::shared_ptr<subnegotiation_router>    subnegotiation_router,
    std::function<void (std::string const &)> text_handler)
  : pimpl_(std::make_shared<impl>(
        command_router, 
        negotiation_router, 
        subnegotiation_router, 
        text_handler))
{
}
    
// ==============================================================================
// DESTRUCTOR
// ==============================================================================
input_visitor::~input_visitor()
{
}

// ==============================================================================
// OPERATOR()(COMMAND)
// ==============================================================================
void input_visitor::operator()(command const &cmd)
{
    (*pimpl_->command_router_)(cmd);
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
void input_visitor::operator()(std::string const &text)
{
    pimpl_->text_handler_(text);
}

// ==============================================================================
// APPLY_INPUT_RANGE
// ==============================================================================
void apply_input_range(
    input_visitor                    &visitor
  , stream::input_storage_type const &values)
{
	for (auto const &value : values)
    {
        apply_visitor(visitor, value);
    }
}

}}

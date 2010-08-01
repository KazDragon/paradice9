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
#ifndef ODIN_TELNET_INPUT_VISITOR_HPP_
#define ODIN_TELNET_INPUT_VISITOR_HPP_

#include "odin/telnet/protocol.hpp"
#include "odin/telnet/stream.hpp"
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>
#include <string>

namespace odin { namespace telnet {

class command_router;
class negotiation_router;
class subnegotiation_router;

class input_visitor
    : public boost::static_visitor<>
{
public :
    input_visitor(
        boost::shared_ptr<command_router> const        &command_router
      , boost::shared_ptr<negotiation_router> const    &negotiation_router
      , boost::shared_ptr<subnegotiation_router> const &subnegotiation_router
      , boost::function<void (std::string)> const      &text_handler);
    
    ~input_visitor();
    
    void operator()(command_type const &command);
    
    void operator()(negotiation_type const &negotiation);
    
    void operator()(subnegotiation_type const &subnegotiation);
    
    void operator()(std::string const &text);
    
private :
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

// A helper function that applies the visitor to a range of values.
void apply_input_range(
    input_visitor                                       &visitor
  , odin::runtime_array<stream::input_value_type> const &values);

}}

#endif

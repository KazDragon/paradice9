// ==========================================================================
// Paradice Command
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
#ifndef PARADICE_COMMAND_HPP_
#define PARADICE_COMMAND_HPP_

#include "paradice/client.hpp"
#include <boost/shared_ptr.hpp>
#include <string>

//* =========================================================================
/// \brief Declare a Paradice command.
//* =========================================================================
#define PARADICE_COMMAND_DECL(cmd) \
    void do_##cmd ( \
        std::string const         &arguments \
      , boost::shared_ptr<client> &player)

//* =========================================================================
/// \brief Define and implement a Paradice command.
//* =========================================================================
#define PARADICE_COMMAND_IMPL(cmd) \
    void do_##cmd ( \
        std::string const         &arguments \
      , boost::shared_ptr<client> &player)
    
//* =========================================================================
/// \brief Invoke a Paradice command
//* =========================================================================
#define INVOKE_PARADICE_COMMAND(cmd, args, player) \
    do_##cmd ( (args), (player) )

#endif

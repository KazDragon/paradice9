// ==========================================================================
// Paradice Client
//
// Copyright (C) 2009 Matthew Chaplain, All Rights Reserved.
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
#include "client.hpp"
#include "connection.hpp"
#include "munin/ansi/protocol.hpp"
#include <boost/foreach.hpp>
#include <deque>
#include <string>
#include <vector>

using namespace odin;
using namespace boost;
using namespace std;

namespace paradice {

vector< shared_ptr<client> > clients;

// ==========================================================================
// CLIENT IMPLEMENTATION STRUCTURE
// ==========================================================================
struct client::impl
{

    shared_ptr<connection> connection_;
    client::level          level_;
    client::command_mode   command_mode_;

    u16                    who_page_;
    string                 prefix_;
    string                 name_;
    string                 title_;

    string                 last_command_;
    deque<string>          backtrace_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client()
    : pimpl_(new impl)
{
    pimpl_->level_        = level_intro_screen;
    pimpl_->command_mode_ = command_mode_mud;
    pimpl_->who_page_     = 0;
}
    
// ==========================================================================
// DESTRUCTOR
// ==========================================================================
client::~client()
{
}

// ==========================================================================
// SET_CONNECTION
// ==========================================================================
void client::set_connection(shared_ptr<connection> const &new_connection)
{
    pimpl_->connection_ = new_connection;
}

// ==========================================================================
// GET_CONNECTION
// ==========================================================================
shared_ptr<connection> client::get_connection()
{
    return pimpl_->connection_;
}

// ==========================================================================
// GET_LEVEL
// ==========================================================================
client::level client::get_level() const
{
    return pimpl_->level_;
}

// ==========================================================================
// SET_LEVEL
// ==========================================================================
void client::set_level(level new_level)
{
    pimpl_->level_ = new_level;
}

// ==========================================================================
// SET_NAME
// ==========================================================================
void client::set_name(string const &name)
{
    pimpl_->name_ = name;

    /*
    get_connection()->write(
        munin::ansi::set_window_title(
            pimpl_->name_ + " - Paradice"));
    */
}

// ==========================================================================
// GET_NAME
// ==========================================================================
string client::get_name() const
{
    return pimpl_->name_;
}

// ==========================================================================
// SET_TITLE
// ==========================================================================
void client::set_title(string const &title)
{
    pimpl_->title_ = title;
}

// ==========================================================================
// GET_TITLE
// ==========================================================================
string client::get_title() const
{
    return pimpl_->title_;
}

// ==========================================================================
// SET_PREFIX
// ==========================================================================
void client::set_prefix(string const &prefix)
{
    pimpl_->prefix_ = prefix;
}

// ==========================================================================
// GET_PREFIX
// ==========================================================================
string client::get_prefix() const
{
    return pimpl_->prefix_;
}

// ==========================================================================
// SET_WHO_PAGE
// ==========================================================================
void client::set_who_page(u16 page)
{
    pimpl_->who_page_ = page;
}

// ==========================================================================
// GET_WHO_PAGE
// ==========================================================================
u16 client::get_who_page() const
{
    return pimpl_->who_page_;
}

// ==========================================================================
// SET_LAST_COMMAND
// ==========================================================================
void client::set_last_command(string const &cmd)
{
    pimpl_->last_command_ = cmd;
}

// ==========================================================================
// GET_LAST_COMMAND
// ==========================================================================
string client::get_last_command() const
{
    return pimpl_->last_command_;
}

// ==========================================================================
// SET_COMMAND_MODE
// ==========================================================================
void client::set_command_mode(command_mode mode)
{
    pimpl_->command_mode_ = mode;
}

// ==========================================================================
// GET_COMMAND_MODE
// ==========================================================================
client::command_mode client::get_command_mode() const
{
    return pimpl_->command_mode_;
}

// ==========================================================================
// ADD_BACKTRACE
// ==========================================================================
void client::add_backtrace(string const &text)
{
    pimpl_->backtrace_.push_back(text);

    if (pimpl_->backtrace_.size() > 40)
    {
        pimpl_->backtrace_.pop_front();
    }
}

// ==========================================================================
// GET_BACKTRACE
// ==========================================================================
string client::get_backtrace() const
{
    string text;

    BOOST_FOREACH(string trace, pimpl_->backtrace_)
    {
        text += trace + "\r\n";
    }

    return text;
}

}

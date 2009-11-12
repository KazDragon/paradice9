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
#include <boost/foreach.hpp>
#include <deque>
#include <string>
#include <vector>

using namespace boost;
using namespace std;

namespace paradice {

vector< shared_ptr<client> > clients;
    
struct client::impl
{
    shared_ptr<socket>     socket_;
    shared_ptr<connection> connection_;
    client::level          level_;
    client::command_mode   command_mode_;

    string                 name_;
    string                 title_;

    string                 last_command_;
    deque<string>          backtrace_;
};

client::client()
    : pimpl_(new impl)
{
    pimpl_->level_        = level_intro_screen;
    pimpl_->command_mode_ = command_mode_mud;
}
    
client::~client()
{
}

void client::set_socket(shared_ptr<socket> const &new_socket)
{
    pimpl_->socket_ = new_socket;
}

void client::set_connection(shared_ptr<connection> const &new_connection)
{
    pimpl_->connection_ = new_connection;
}

shared_ptr<socket> client::get_socket()
{
    return pimpl_->socket_;
}

shared_ptr<connection> client::get_connection()
{
    return pimpl_->connection_;
}

client::level client::get_level() const
{
    return pimpl_->level_;
}

void client::set_level(level new_level)
{
    pimpl_->level_ = new_level;
}

void client::set_name(string const &name)
{
    pimpl_->name_ = name;
}

string client::get_name() const
{
    return pimpl_->name_;
}

void client::set_title(string const &title)
{
    pimpl_->title_ = title;
}

string client::get_title() const
{
    return pimpl_->title_;
}

void client::set_last_command(string const &cmd)
{
    pimpl_->last_command_ = cmd;
}

string client::get_last_command() const
{
    return pimpl_->last_command_;
}

void client::set_command_mode(command_mode mode)
{
    pimpl_->command_mode_ = mode;
}

client::command_mode client::get_command_mode() const
{
    return pimpl_->command_mode_;
}

void client::add_backtrace(string const &text)
{
    pimpl_->backtrace_.push_back(text);

    if (pimpl_->backtrace_.size() > 10)
    {
        pimpl_->backtrace_.pop_front();
    }
}

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

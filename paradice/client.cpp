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
#include "munin/window.hpp"
#include "hugin/user_interface.hpp"
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>
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
    shared_ptr<account>               account_;
    shared_ptr<character>             character_;

    shared_ptr<connection>            connection_;
    shared_ptr<hugin::user_interface> user_interface_;

    string                            last_command_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client()
    : pimpl_(new impl)
{
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
// SET_USER_INTERFACE
// ==========================================================================
void client::set_user_interface(
    shared_ptr<hugin::user_interface> user_interface)
{
    pimpl_->user_interface_ = user_interface;
}

// ==========================================================================
// GET_USER_INTERFACE
// ==========================================================================
shared_ptr<hugin::user_interface> client::get_user_interface()
{
    return pimpl_->user_interface_;
}

// ==========================================================================
// SET_WINDOW_TITLE
// ==========================================================================
void client::set_window_title(std::string const &title)
{
    BOOST_AUTO(connection, get_connection());
    
    if (connection != NULL)
    {
        BOOST_AUTO(window, connection->get_window());
        
        if (window != NULL)
        {
            window->set_title(title);
        }
    }
}

// ==========================================================================
// SET_ACCOUNT
// ==========================================================================
void client::set_account(shared_ptr<account> acc)
{
    pimpl_->account_ = acc;
}

// ==========================================================================
// GET_ACCOUNT
// ==========================================================================
shared_ptr<account> client::get_account() const
{
    return pimpl_->account_;
}

// ==========================================================================
// SET_CHARACTER
// ==========================================================================
void client::set_character(shared_ptr<character> ch)
{
    pimpl_->character_ = ch;
}

// ==========================================================================
// GET_CHARACTER
// ==========================================================================
shared_ptr<character> client::get_character() const
{
    return pimpl_->character_;
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

}

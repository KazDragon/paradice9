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
#ifndef PARADICE_CLIENT_HPP_
#define PARADICE_CLIENT_HPP_

#include "odin/types.hpp"
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace hugin {
    class user_interface;
}

namespace paradice {
    class account;
    class character;
    class connection;
    class context;
}

namespace munin {
    class window;
}

namespace boost { namespace asio {
    class io_service;
}}

namespace paradice {

class client
    : public boost::enable_shared_from_this<client>
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    client(
        boost::asio::io_service    &io_service
      , boost::shared_ptr<context>  ctx);

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    ~client();

    //* =====================================================================
    /// \brief Sets the connection on which this client operates.
    //* =====================================================================
    void set_connection(boost::shared_ptr<connection> const &new_connection);
    
    //* =====================================================================
    /// \brief Gets the user interface for the client.
    //* =====================================================================
    boost::shared_ptr<hugin::user_interface> get_user_interface();
    
    //* =====================================================================
    /// \brief Gets the window for the client.
    //* =====================================================================
    boost::shared_ptr<munin::window> get_window();

    //* =====================================================================
    /// \brief Sets the title of the client's window
    //* =====================================================================
    void set_window_title(std::string const &title);

    //* =====================================================================
    /// \brief Sets the size of the client's window
    //* =====================================================================
    void set_window_size(odin::u16 width, odin::u16 height);

    //* =====================================================================
    /// \brief Sets the account that the client is currently using.
    //* =====================================================================
    void set_account(boost::shared_ptr<account> acc);
    
    //* =====================================================================
    /// \brief Retrieves the account that the client is currently using.
    //* =====================================================================
    boost::shared_ptr<account> get_account() const;
    
    //* =====================================================================
    /// \brief Sets the character that the client is currently using.
    //* =====================================================================
    void set_character(boost::shared_ptr<character> ch);
    
    //* =====================================================================
    /// \brief Retreives the character that the client is currently using.
    //* =====================================================================
    boost::shared_ptr<character> get_character() const;

    //* =====================================================================
    /// \brief Disconnects the client from the server.
    //* =====================================================================
    void disconnect();
    
    //* =====================================================================
    /// \brief Sets up a callback for if the client's connection dies.
    //* =====================================================================
    void on_connection_death(boost::function<void ()> callback);
    
private :
    class impl;
    boost::shared_ptr<impl> pimpl_;
};

}

#endif

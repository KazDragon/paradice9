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

#include "paradice/export.hpp"
#include <functional>
#include <memory>
#include <vector>

/*
namespace hugin {
    class user_interface;
}
*/

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
    class io_context;
}}

namespace paradice {

class PARADICE_EXPORT client
    : public std::enable_shared_from_this<client>
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    client(
        boost::asio::io_context  &io_context
      , std::shared_ptr<context>  ctx);

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    ~client();

    //* =====================================================================
    /// \brief Sets the connection on which this client operates.
    //* =====================================================================
    void set_connection(std::shared_ptr<connection> const &new_connection);

    //* =====================================================================
    /// \brief Gets the user interface for the client.
    //* =====================================================================
    //std::shared_ptr<hugin::user_interface> get_user_interface();

    //* =====================================================================
    /// \brief Gets the window for the client.
    //* =====================================================================
    std::shared_ptr<munin::window> get_window();

    //* =====================================================================
    /// \brief Sets the title of the client's window
    //* =====================================================================
    void set_window_title(std::string const &title);

    //* =====================================================================
    /// \brief Sets the size of the client's window
    //* =====================================================================
    void set_window_size(std::uint16_t width, std::uint16_t height);

    //* =====================================================================
    /// \brief Sets the account that the client is currently using.
    //* =====================================================================
    void set_account(std::shared_ptr<account> const &acc);

    //* =====================================================================
    /// \brief Retrieves the account that the client is currently using.
    //* =====================================================================
    std::shared_ptr<account> get_account() const;

    //* =====================================================================
    /// \brief Sets the character that the client is currently using.
    //* =====================================================================
    void set_character(std::shared_ptr<character> const &ch);

    //* =====================================================================
    /// \brief Retreives the character that the client is currently using.
    //* =====================================================================
    std::shared_ptr<character> get_character() const;

    //* =====================================================================
    /// \brief Disconnects the client from the server.
    //* =====================================================================
    void disconnect();

    //* =====================================================================
    /// \brief Sets up a callback for if the client's connection dies.
    //* =====================================================================
    void on_connection_death(std::function<void ()> const &callback);

private :
    class impl;
    std::shared_ptr<impl> pimpl_;
};

}

#endif

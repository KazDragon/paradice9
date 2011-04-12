// ==========================================================================
// Paradice Connection
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
#ifndef PARADICE_CONNECTION_HPP_
#define PARADICE_CONNECTION_HPP_

#include "odin/types.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <string>
#include <utility>

namespace boost { namespace asio {
    class io_service;
}}

namespace munin {
    class window;
}

namespace odin { namespace net {
    class socket;
}}

namespace paradice {

//* =========================================================================
/// \brief An connection to a socket that abstracts away details about the
/// protocols used.
//* =========================================================================
class connection
{
public :
    //* =====================================================================
    /// \brief Create a connection object that uses the passed socket as
    /// a communications point, and calls the passed function whenever data
    /// is received.
    //* =====================================================================
    connection(boost::shared_ptr<odin::net::socket> socket);
    
    //* =====================================================================
    /// \brief Destructor.
    //* =====================================================================
    ~connection();
    
    //* =====================================================================
    /// \brief Returns the window that this connection maintains.
    //* =====================================================================
    boost::shared_ptr<munin::window> get_window();

    //* =====================================================================
    /// \brief Set a function to be called when the window size changes.
    //* =====================================================================
    void on_window_size_changed(
        boost::function<void (odin::u16, odin::u16)> callback);

    //* =====================================================================
    /// \brief Set a function to be called when text is received from the
    /// connection.
    //* =====================================================================
    void on_text(
        boost::function<void (std::string)> callback);

    //* =====================================================================
    /// \brief Set a function to be called when a mouse report is received
    /// from the connection.
    //* =====================================================================
    void on_mouse_report(
        boost::function<void (odin::ansi::mouse_report)> callback);
    
    //* =====================================================================
    /// \brief Set a function to be called when an ANSI control sequence
    /// is received from the client.
    //* =====================================================================
    void on_control_sequence(
        boost::function<void (odin::ansi::control_sequence)> callback);

    //* =====================================================================
    /// \brief Disconnects the socket.
    //* =====================================================================
    void disconnect();

    //* =====================================================================
    /// \brief Reconnects a new socket to this connection.
    //* =====================================================================
    void reconnect(boost::shared_ptr<odin::net::socket> socket);
    
    //* =====================================================================
    /// \brief Asynchronously retrieves the terminal type of the connection.
    //* =====================================================================
    void async_get_terminal_type(
        boost::function<void (std::string)> callback);
    
private :
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}

#endif


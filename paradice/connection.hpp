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

#include "paradice/export.hpp"
#include "odin/core.hpp"
#include "odin/ansi/protocol.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace odin { namespace net {
    class socket;
}}

namespace paradice {

//* =========================================================================
/// \brief An connection to a socket that abstracts away details about the
/// protocols used.
//* =========================================================================
class PARADICE_EXPORT connection
{
public :
    //* =====================================================================
    /// \brief Create a connection object that uses the passed socket as
    /// a communications point, and calls the passed function whenever data
    /// is received.
    //* =====================================================================
    connection(std::shared_ptr<odin::net::socket> socket);

    //* =====================================================================
    /// \brief Destructor.
    //* =====================================================================
    ~connection();

    //* =====================================================================
    /// \brief Starts reading from the other end of the connection.  Until
    /// this is called, no reads will take place.
    //* =====================================================================
    void start();

    //* =====================================================================
    /// \brief Writes raw data to the connection.
    //* =====================================================================
    void write(std::vector<odin::u8> const &data);

    //* =====================================================================
    /// \brief Set a function to be called when the window size changes.
    //* =====================================================================
    void on_window_size_changed(
        std::function<void (odin::u16, odin::u16)> const &callback);

    //* =====================================================================
    /// \brief Set a function to be called when text is received from the
    /// connection.
    //* =====================================================================
    void on_text(
        std::function<void (char)> const &callback);

    //* =====================================================================
    /// \brief Set a function to be called when a mouse report is received
    /// from the connection.
    //* =====================================================================
    void on_mouse_report(
        std::function<void (odin::ansi::mouse_report const &)> const &callback);

    //* =====================================================================
    /// \brief Set a function to be called when an ANSI control sequence
    /// is received from the client.
    //* =====================================================================
    void on_control_sequence(
        std::function<void (odin::ansi::control_sequence const &)> const &callback);

    //* =====================================================================
    /// \brief Set up a callback to be called when the underlying socket
    /// dies.
    //* =====================================================================
    void on_socket_death(std::function<void ()> const &callback);

    //* =====================================================================
    /// \brief Disconnects the socket.
    //* =====================================================================
    void disconnect();

    //* =====================================================================
    /// \brief Reconnects a new socket to this connection.
    //* =====================================================================
    void reconnect(std::shared_ptr<odin::net::socket> const &socket);

    //* =====================================================================
    /// \brief Asynchronously retrieves the terminal type of the connection.
    //* =====================================================================
    void async_get_terminal_type(
        std::function<void (std::string const &)> const &callback);

private :
    struct impl;
    std::shared_ptr<impl> pimpl_;
};

}

#endif


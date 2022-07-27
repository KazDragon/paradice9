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

#include "paradice/core.hpp"
#include <functional>
#include <memory>
#include <utility>

namespace paradice {

//* =========================================================================
/// \brief An connection to a communication channel (e.g. a TCP socket) that 
/// abstracts away details about the protocols used.
//* =========================================================================
class PARADICE_EXPORT connection
{
public :
    //* =====================================================================
    /// \brief Create a connection object that communicates with the passed-
    /// in communications channel.
    //* =====================================================================
    template <class Channel>
    explicit connection(Channel &&ep)
      : connection(
            std::unique_ptr<channel_concept>(
                std::make_unique<channel_model<Channel>>(
                    std::forward<Channel>(ep))))
    {
    }

    //* =====================================================================
    /// \brief Move constructor
    //* =====================================================================
    connection(connection &&other) noexcept;

    //* =====================================================================
    /// \brief Destructor.
    //* =====================================================================
    ~connection();

    //* =====================================================================
    /// \brief Move assignment
    //* =====================================================================
    connection &operator=(connection &&other) noexcept;

    //* =====================================================================
    /// \brief Closes the connection.
    //* =====================================================================
    void close();

    //* =====================================================================
    /// \brief Returns whether the channel of the connection is still
    ///        alive.
    //* =====================================================================
    bool is_alive() const;

    //* =====================================================================
    /// \brief Asynchronously reads from the connection.
    ///
    /// A single read may yield zero or more callbacks to the data 
    /// continuation.  This is because parts or all of the data may be
    /// consumed by Telnet handling.  Therefore, a second continuation is
    /// provided to show that the requested read has been completed and a
    /// new read request may be issued.
    //* =====================================================================
    void async_read(std::function<void (bytes)> const &data_continuation);

    //* =====================================================================
    /// \brief Writes to the connection.
    //* =====================================================================
    void write(bytes data);

    //* =====================================================================
    /// \brief Requests terminal type of the connection, calling the
    ///        supplied continuation with the results.
    //* =====================================================================
    void async_get_terminal_type(
        std::function<void (std::string const &)> const &continuation);

    //* =====================================================================
    /// \brief Set a function to be called when the window size changes.
    //* =====================================================================
    void on_window_size_changed(
        std::function<void (std::uint16_t, std::uint16_t)> const &continuation);

private :
    struct channel_concept
    {
        virtual ~channel_concept() = default;
        virtual void async_read(std::function<void (bytes)> const &callback) = 0;
        virtual void write(bytes data) = 0;
        virtual bool is_alive() const = 0;
        virtual void close() = 0;
    };

    template <typename Channel>
    struct channel_model final : channel_concept
    {
        channel_model(Channel &&ep)
          : channel_(std::forward<Channel>(ep))
        {
        }

        void async_read(std::function<void (bytes)> const &callback) override
        {
            channel_.async_read(callback);
        }

        void write(bytes data) override
        {
            channel_.write(data);
        }

        bool is_alive() const override
        {
            return channel_.is_alive();
        }

        void close() override
        {
            channel_.close();
        }

        Channel channel_;
    };

    // ======================================================================
    // Internal Constructor
    // ======================================================================
    connection(std::unique_ptr<channel_concept> ep);

    struct impl;
    friend struct impl;
    std::unique_ptr<impl> pimpl_;
};

}

#endif


// ==========================================================================
// Munin ANSI Window.
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
#ifndef MUNIN_ANSI_WINDOW_HPP_
#define MUNIN_ANSI_WINDOW_HPP_

#include "munin/export.hpp"
#include <terminalpp/extent.hpp>
#include <terminalpp/terminal.hpp>
#include <boost/any.hpp>
#include <boost/asio/strand.hpp>
#include <boost/signal.hpp>
#include <string>

namespace munin {

class container;

//* =========================================================================
/// \brief An object that represents a top-level window.
//* =========================================================================
class MUNIN_EXPORT window
{
public :
    //* =====================================================================
    /// \brief Constructor
    /// \param strand A boost::asio::strand in which all asynchronous calls
    /// will be run.
    //* =====================================================================
    window(boost::asio::strand &strand, terminalpp::terminal::behaviour const &behaviour);

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    ~window();

    //* =====================================================================
    /// \brief Gets the size of the window.
    //* =====================================================================
    terminalpp::extent get_size() const;

    //* =====================================================================
    /// \brief Sets the size of the window.  Following this, the contents
    /// of the window are re-laid out.
    //* =====================================================================
    void set_size(terminalpp::extent size);

    //* =====================================================================
    /// \brief Sets the title of the window.
    //* =====================================================================
    void set_title(std::string const &title);

    //* =====================================================================
    /// \brief Enables mouse tracking for the window.
    //* =====================================================================
    void enable_mouse_tracking();

    //* =====================================================================
    /// \brief Retrieve the top level container in the window.  This
    /// contains all the components that are displayed in this window.
    //* =====================================================================
    std::shared_ptr<container> get_content();

    //* =====================================================================
    /// \brief Sends data that has been received from the client.  This is
    /// expected to be in ANSI format, and will be converted to events that
    /// are passed down into the focussed component.
    //* =====================================================================
    void data(std::string const &data);

    //* =====================================================================
    /// \fn on_repaint
    /// \param paint_data A sequence of characters that contain the ANSI-
    /// compliant bytes necessary to repaint the window.
    /// \brief Connect to this signal in order to receive notification about
    /// when the window has repainted and the data for how to repaint it.
    //* =====================================================================
    boost::signal
    <
        void (std::string const &paint_data)
    > on_repaint;

private :
    class impl;
    std::shared_ptr<impl> pimpl_;
};

}

#endif

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

#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>
#include <string>

namespace boost { namespace asio {
    class io_service;
}}

namespace munin {

class container;

//* =========================================================================
/// \brief An object that represents a top-level window.
//* =========================================================================
class window
{
public :
    //* =====================================================================
    /// \brief Constructor
    /// \param io_service A boost::asio::io_service object that is used for
    /// coalescing multiple paint requests.  It is assumed that its run()
    /// method is invoked elsewhere.
    //* =====================================================================
    window(boost::asio::io_service &io_service);
    
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    ~window();

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
    boost::shared_ptr<container> get_content();
    
    //* =====================================================================
    /// \brief Send an event to the window.  This will be passed down to the
    /// focused component, who should interpret it according to its
    /// specifications.
    //* =====================================================================
    void event(boost::any const &event);
    
    //* =====================================================================
    /// \fn on_repaint
    /// \param paint_data A sequence of characters that contain the ANSI-
    /// compliant bytes necessary to repaint the window.
    /// \brief Connect to this signal in order to receive notification about
    /// when the window has repainted and the data for how to repaint it.
    //* =====================================================================
    boost::signal
    <
        void (std::string paint_data)
    > on_repaint;
    
private :
    class impl;
    boost::shared_ptr<impl> pimpl_;
};

}

#endif

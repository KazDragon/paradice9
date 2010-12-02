// ==========================================================================
// GuiBuilder Client
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
#ifndef GUIBUILDER_CLIENT_HPP_
#define GUIBUILDER_CLIENT_HPP_

#include "odin/io/datastream.hpp"
#include "odin/ansi/protocol.hpp"
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

namespace boost { namespace asio {
    class io_service;
}}

namespace munin { namespace ansi {
    class window;
}}

namespace guibuilder {

typedef odin::io::datastream<odin::u8, odin::u8> stream;

class client
{
public :
    client(
        boost::shared_ptr<stream>  connection
      , boost::asio::io_service   &io_service);

    ~client();

    boost::shared_ptr<munin::ansi::window> get_window();

    void on_window_size_changed(
        boost::function<void (odin::u16 width, odin::u16 height)> callback);

    void on_text(boost::function<void (std::string text)> callback);

    void on_control_sequence(
        boost::function<void (odin::ansi::control_sequence)> callback);
    
private :
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}

#endif

// ==========================================================================
// Hugin User Interface
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
#ifndef GUIBUILDER_UI_HPP_
#define GUIBUILDER_UI_HPP_

#include "munin/composite_component.hpp"
#include "munin/ansi/ansi_types.hpp"
#include <odin/runtime_array.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

namespace hugin {

BOOST_STATIC_CONSTANT(std::string, FACE_INTRO = "Intro");
BOOST_STATIC_CONSTANT(std::string, FACE_MAIN  = "Main");

class user_interface
    : public munin::composite_component<munin::ansi::element_type>
{
public :
    user_interface();
    
    void on_username_entered(
        boost::function<void (std::string)> callback);

    void on_input_entered(
        boost::function<void (std::string)> callback);
    
    void select_face(std::string const &face_name);
    
    void add_output_text(
        odin::runtime_array<munin::ansi::element_type> const &text);
    
    void set_statusbar_text(
        odin::runtime_array<munin::ansi::element_type> const &text);
    
    void update_wholist(
        odin::runtime_array<std::string> const &names);
    
protected :
    virtual void do_event(boost::any const &event);

private :
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}

#endif

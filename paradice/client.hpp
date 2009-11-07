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

#include <boost/shared_ptr.hpp>
#include <vector>

namespace paradice {

class socket;
class connection;

class client
{
public :
    enum level
    {
        level_intro_screen
      , level_in_game
    };
    
    client();
    ~client();

    void set_socket(boost::shared_ptr<socket> const &new_socket);
    void set_connection(boost::shared_ptr<connection> const &new_connection);
    
    boost::shared_ptr<socket>     get_socket();
    boost::shared_ptr<connection> get_connection();
    
    level get_level() const;
    void set_level(level new_level);
    
    void set_name(std::string const &name);
    std::string get_name() const;
    
    void set_title(std::string const &title);
    std::string get_title() const;

    void set_last_command(std::string const &cmd);
    std::string get_last_command() const;

    void add_backtrace(std::string const &text);
    std::string get_backtrace() const;

private :
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

extern std::vector< boost::shared_ptr<client> > clients;

}

#endif

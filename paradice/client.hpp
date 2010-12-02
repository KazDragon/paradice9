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
#include <boost/shared_ptr.hpp>
#include <vector>

namespace hugin {
    class user_interface;
}

namespace paradice {
    class socket;
    class connection;
}

namespace paradice {

class client
{
public :
    //* =====================================================================
    /// \brief Defines the style of command that the client wishes to use.
    //* =====================================================================
    enum command_mode
    {
        command_mode_mud
      , command_mode_mmo
    };
    
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    client();

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    ~client();

    //* =====================================================================
    /// \brief Sets the connection on which this client operates.
    //* =====================================================================
    void set_connection(boost::shared_ptr<connection> const &new_connection);
    
    //* =====================================================================
    /// \brief Retrieves the connection on which this client operates.
    //* =====================================================================
    boost::shared_ptr<connection> get_connection();
    
    //* =====================================================================
    /// \brief Sets the user interface for the client.
    //* =====================================================================
    void set_user_interface(
        boost::shared_ptr<hugin::user_interface> user_interface);
    
    //* =====================================================================
    /// \brief Gets the user interface for the client.
    //* =====================================================================
    boost::shared_ptr<hugin::user_interface> get_user_interface();
    
    //* =====================================================================
    /// \brief Sets the name that the client goes by.
    //* =====================================================================
    void set_name(std::string const &name);

    //* =====================================================================
    /// \brief Gets the name that the client goes by.
    //* =====================================================================
    std::string get_name() const;
    
    //* =====================================================================
    /// \brief Sets the post-name text that the client uses as a title.
    //* =====================================================================
    void set_title(std::string const &title);

    //* =====================================================================
    /// \brief Gets the post-name text that the client uses as a title.
    //* =====================================================================
    std::string get_title() const;
    
    //* =====================================================================
    /// \brief Sets the pre-name text that the client uses as a prefix.
    //* =====================================================================
    void set_prefix(std::string const &prefix);

    //* =====================================================================
    /// \brief Gets the pre-name text that the client uses as a prefix.
    //* =====================================================================
    std::string get_prefix() const;

    //* =====================================================================
    /// \brief Sets the command that the client last used.
    //* =====================================================================
    void set_last_command(std::string const &cmd);

    //* =====================================================================
    /// \brief Gets the command that the client last used.
    //* =====================================================================
    std::string get_last_command() const;

    //* =====================================================================
    /// \brief Sets the style of command that the client wishes to use.
    //* =====================================================================
    void set_command_mode(command_mode mode);

    //* =====================================================================
    /// \brief Returns the style of command that the client is using.
    //* =====================================================================
    command_mode get_command_mode() const;

private :
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}

#endif

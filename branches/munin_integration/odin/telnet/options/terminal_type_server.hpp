// ==========================================================================
// Odin Telnet Terminal Type Server Option
//
// Copyright (C) 2003 Matthew Chaplain, All Rights Reserved.
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
#ifndef ODIN_TELNET_OPTIONS_STATUS_SERVER_HPP_
#define ODIN_TELNET_OPTIONS_STATUS_SERVER_HPP_

#include "odin/telnet/server_option.hpp"
#include <string>

namespace odin { namespace telnet { namespace options {

class terminal_type_server : public odin::telnet::server_option
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    public :
        terminal_type_server(boost::shared_ptr<odin::telnet::stream> stream);

    // ======================================================================
    // DESTRUCTOR
    // ======================================================================
    public :
        virtual ~terminal_type_server();
        
    // ======================================================================
    // OPTION PUBLIC INTERFACE
    // ======================================================================
    public :
        // ==================================================================
        // OPTION::ON_SUBNEGOTIATION
        //  FUNCTION : Called when a subnegotiation for this option is 
        //             received.
        // ==================================================================
        virtual void on_subnegotiation(
            subnegotiation_type const &subnegotiation);
    
        // ==================================================================
        // OPTION::GET_NEGOTIATION_TYPE
        //  FUNCTION : Returns the negotiation type of this option (for 
        //             example, odin::telnet::NAWS, or odin::telnet::RTCE).
        // ==================================================================
        virtual negotiation_type get_negotiation_type() const;
        
    // ======================================================================
    // PUBLIC MEMBER FUNCTIONS
    // ======================================================================
    public :
        // ==================================================================
        // ADD_TERMINAL_TYPE
        //  FUNCTION : Adds a type of terminal to the option.
        // ==================================================================
        void add_terminal_type(std::string const &type);
        
        // ==================================================================
        // GET_CURRENT_TERMINAL_TYPE
        //  FUNCTION : Gets the current type of terminal to the option.
        //             According to the RFC, this is the type of terminal
        //             that has most recently been queried for by the
        //             server.
        // ==================================================================
        std::string get_current_terminal_type() const;
    
    // ======================================================================
    // PRIVATE IMPLEMENTATION
    // ======================================================================
    public :
        struct impl;
        impl *pimpl_;
};

}}}

#endif

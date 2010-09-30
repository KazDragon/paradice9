// ==========================================================================
// Odin Telnet Terminal Type Client Option
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
#ifndef ODIN_TELNET_OPTIONS_STATUS_CLIENT_HPP_
#define ODIN_TELNET_OPTIONS_STATUS_CLIENT_HPP_

#include "odin/telnet/client_option.hpp"
#include <boost/function.hpp>
#include <string>

namespace odin { namespace telnet { namespace options {

// TERMINAL_TYPE_CLIENT =====================================================
//  FUNCTION : Implements the client-option side of the telnet TERMINAL_TYPE
//             option as specified in RFC 1091.
//  USAGE    : See odin::telnet:client_option.
// ==========================================================================
class terminal_type_client : public odin::telnet::client_option
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    public :
        terminal_type_client(boost::shared_ptr<odin::telnet::stream> stream);

    // ======================================================================
    // DESTRUCTOR
    // ======================================================================
    public :
        virtual ~terminal_type_client();
        
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
        // ON_TERMINAL_TYPE_ADDED
        //  FUNCTION : Sets up a callback for when a terminal type is added
        //             by the server.
        // ==================================================================
        void on_terminal_type_added(boost::function<void ()> fn);
        
        // ==================================================================
        // ON_TERMINAL_TYPES_COMPLETE
        //  FUNCTION : Sets up a callback for when the server has finished
        //             listing its terminal types.
        // ==================================================================
        void on_terminal_types_complete(boost::function<void ()> fn);

        // ==================================================================
        // ON_TERMINAL_TYPE_SET
        //  FUNCTION : Sets up a callback for when, after a call to
        //             set_current_terminal_type(), the terminal type has
        //             been set to the specified type.
        // ==================================================================
        void on_terminal_type_set(boost::function<void ()> fn);

        // ==================================================================
        // DETECT_TERMINAL_TYPES
        //  FUNCTION : Begins negotiations with the server to obtain the
        //             supported terminal types.  The callback set up in
        //             on_terminal_type_added() is called for each type
        //             found, and the callback set up in 
        //             on_terminal_types_complete is called once the entire
        //             set has been received.
        // ==================================================================
        void detect_terminal_types();
        
        // ==================================================================
        // GET_NUMBER_OF_TERMINAL_TYPES
        //  FUNCTION : Obtain the number of detected terminal types.
        // ==================================================================
        unsigned int get_number_of_terminal_types() const;

        // ==================================================================
        // GET_TERMINAL_TYPE
        //  FUNCTION : Gets the nth terminal type from the list of detecetd
        //             terminal types.
        // ==================================================================
        std::string get_terminal_type(unsigned int index);
        
        // ==================================================================
        // SET_CURRENT_TERMINAL_TYPE
        //  FUNCTION : Sets the client's terminal type to the specified type.
        // ==================================================================
        void set_current_terminal_type(std::string type);
        
        // ==================================================================
        // GET_CURRENT_TERMINAL_TYPE
        //  FUNCTION : Gets the current type of terminal to the option.
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

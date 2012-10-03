// ==========================================================================
// Odin Telnet Negotiate About Window Size (NAWS) Server Option
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
#ifndef ODIN_TELNET_OPTIONS_NAWS_HPP_
#define ODIN_TELNET_OPTIONS_NAWS_HPP_

#include "odin/telnet/server_option.hpp"

namespace odin { namespace telnet { namespace options {

class naws_server : public odin::telnet::server_option
{
    
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    public :
        naws_server(boost::shared_ptr<odin::telnet::stream> stream);
        
    // ======================================================================
    // DESTRUCTOR
    // ======================================================================
    public :
        virtual ~naws_server();

    // ======================================================================
    // OPTION PUBLIC INTERFACE
    // ======================================================================
    public :
        // ==================================================================
        // OPTION::GET_NEGOTIATION_TYPE
        //  FUNCTION : Returns the negotiation type of this option (for 
        //             example, odin::telnet::NAWS, or odin::telnet::RTCE).
        // ==================================================================
        virtual odin::telnet::negotiation_type get_negotiation_type() const;
        
    // ======================================================================
    // PUBLIC MEMBER FUNCTIONS
    // ======================================================================
    public :
        // ==================================================================
        // SET_SIZE
        //  FUNCTION : Announces to the client that the size of the server's
        //             output window has changed.
        // ==================================================================
        void set_size(unsigned short width, unsigned short height);  
        
    // ======================================================================
    // PRIVATE IMPLEMENTATION
    // ======================================================================
    private :
        struct impl;
        impl *pimpl_;
};
            
}}}

#endif


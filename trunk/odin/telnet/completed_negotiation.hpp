// ==========================================================================
// Odin Completed Telnet Negotiation.
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
#ifndef ODIN_TELNET_NEGOTIATION_HPP_
#define ODIN_TELNET_NEGOTIATION_HPP_

#include "odin/telnet/protocol.hpp"

namespace odin { namespace telnet {
        
// INITIATOR =================================================================
//  FUNCTION : Distinguishes the initiator of a telnet connection
// ===========================================================================
enum initiator
{
    local
  , remote
};

// COMPLETED_NEGOTIATION =====================================================
//  FUNCTION : Models a WILL/WONT/DO/DONT client-server negotiation over the
//             Telnet Protocol
//
//  USAGE    : Create an instance, passing the initiator of the negotiation,
//             and the negotiation itself.  Fill in the other half of the
//             negotiation when it arrives/is sent.
//
//             Attempting to use an option not starting WILL, WONT, DO or
//             DONT will result in a bad_telnet_negotiation exception.
//             
//             Attempting to overwrite a part of the negotiation that has
//             already been negotiated will also result in that exception.
//
//             telnet_negotiation has value semantics.
// ===========================================================================
class completed_negotiation
{
    // =======================================================================
    // CONSTRUCTORS
    // =======================================================================
    public :
        // May throw std::bad_alloc
        // type must be local or remote.
        // local_request and remote_request must be WILL, WONT, DO or DONT
        // Anything else is undefined.
        // Strongly exception safe.
        completed_negotiation(
            initiator           initiated_by
          , negotiation_type    type
          , negotiation_request local_request
          , negotiation_request remote_request);

        // May throw std::bad_alloc
        // Strongly exception safe.                         
        completed_negotiation(completed_negotiation const &other);
    
    // =======================================================================
    // OPERATORS
    // =======================================================================
    public :
        // May throw std::bad_alloc
        // Strongly exception safe.
        completed_negotiation &operator=(completed_negotiation const &other);

    // =======================================================================
    // PUBLIC MEMBER FUNCTIONS
    // =======================================================================
    public :
        // Returns the initiator of the negotiation
        // Never throws.
        initiator get_initiator() const;
        
        // Returns the 
        // Never throws.
        negotiation_type get_type() const;
        
        // Returns WILL, WONT, DO, DONT.
        // Never throws.
        int get_local_request() const;
        int get_remote_request() const;

    // =======================================================================
    // DESTRUCTOR
    // =======================================================================
    public :
        ~completed_negotiation();

    // =======================================================================
    // PRIVATE IMPLEMENTATION
    // =======================================================================
    private :
        struct impl;
        impl *pimpl_;
};

}}

#endif



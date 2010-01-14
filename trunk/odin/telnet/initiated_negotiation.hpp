// ==========================================================================
// Odin Initiated Telnet Negotiation.
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
#ifndef ODIN_TELNET_INITIATED_NEGOTIATION_HPP_
#define ODIN_TELNET_INITIATED_NEGOTIATION_HPP_

#include "odin/telnet/completed_negotiation.hpp"

namespace odin { namespace telnet {
        
// INITIATED_NEGOTIATION =====================================================
//  FUNCTION : Models a telnet negotiation which has been initiated.
// ===========================================================================
class initiated_negotiation
{
    // =======================================================================
    // CONSTRUCTORS
    // =======================================================================
    public :
        // initiated_by must be either local or remote.
        // request must be WILL/WONT/DO or DONT.
        // Anything else is undefined behaviour.
        // May throw std::bad_alloc.
        // Strongly exception safe.
        initiated_negotiation(
            initiator                initiated_by
          , option_id_type           type
          , negotiation_request_type request);
          
        
        // May throw std::bad_alloc.
        // Strongly exception safe.
        initiated_negotiation(initiated_negotiation const &);
    
    // =======================================================================
    // OPERATORS
    // =======================================================================
    public :
        // May throw std::bad_alloc.
        // Strongly exception safe.
        initiated_negotiation &operator=(initiated_negotiation const &);
        
        // Nothrow guarantee.
        bool operator==(initiated_negotiation const &);
    
    // =======================================================================
    // PUBLIC MEMBER FUNCTIONS
    // =======================================================================
    public :
        // Returns the initiator of the negotiation.
        // Nothrow guarantee.
        initiator get_initiator() const;
        
        // Returns the WILL/WONT/DO/DONT request type.
        // Nothrow guarantee.
        negotiation_request_type get_request() const;
        
        // Returns the type of the request (ECHO, STATUS, etc.)
        // Nothrow guarantee.
        option_id_type get_option_id() const;
        
    // =======================================================================
    // PRIVATE IMPLEMENTATION
    // =======================================================================
    private :
        struct impl;
        impl *pimpl_;
};

}}

#endif

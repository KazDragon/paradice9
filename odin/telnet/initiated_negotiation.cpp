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
#include "odin/telnet/initiated_negotiation.hpp"
#include <memory>

namespace odin { namespace telnet {
        
// INITIATED_NEGOTIATION::IMPLEMENTATION STRUCTURE ===========================
// ===========================================================================
struct initiated_negotiation::impl
{
    initiator                initiated_by_;
    option_id_type           option_id_;
    negotiation_request_type request_;
};
        
// ===========================================================================
// INITIATED_NEGOTIATION::CONSTRUCTOR
// ===========================================================================
initiated_negotiation::initiated_negotiation(
    initiator                initiated_by
  , option_id_type           option_id
  , negotiation_request_type request)
{
    std::auto_ptr<impl> pimpl(new impl);
    pimpl->initiated_by_ = initiated_by;
    pimpl->option_id_    = option_id;
    pimpl->request_      = request;
    pimpl_ = pimpl.release();
}
         
// ===========================================================================
// INITIATED_NEGOTIATION::COPY CONSTRUCTOR
// ===========================================================================
initiated_negotiation::initiated_negotiation(initiated_negotiation const &other)
{
    std::auto_ptr<impl> pimpl(new impl);
    *pimpl = *other.pimpl_;
    pimpl_ = pimpl.release();
}
    
// ===========================================================================
// INITIATED_NEGOTIATION::OPERATOR=
// ===========================================================================
initiated_negotiation &initiated_negotiation::operator=(
    initiated_negotiation const &other)
{
    *pimpl_ = *other.pimpl_;
    return *this;
}    
        
// ===========================================================================
// INITIATED_NEGOTIATION::OPERATOR==
// ===========================================================================
bool initiated_negotiation::operator==(initiated_negotiation const &other)
{
    return pimpl_->initiated_by_ == other.pimpl_->initiated_by_
        && pimpl_->option_id_    == other.pimpl_->option_id_
        && pimpl_->request_      == other.pimpl_->request_;
}
    
// ===========================================================================
// INITIATED_NEGOTIATION::GET_INITIATOR
// ===========================================================================
initiator initiated_negotiation::get_initiator() const
{
    return pimpl_->initiated_by_;
}
        
// ===========================================================================
// INITIATED_NEGOTIATION::GET_REQUEST
// ===========================================================================
negotiation_request_type initiated_negotiation::get_request() const
{
    return pimpl_->request_;
}
        
// ===========================================================================
// INITIATED_NEGOTIATION::GET_TYPE
// ===========================================================================
option_id_type initiated_negotiation::get_option_id() const
{
    return pimpl_->option_id_;
}
          
}}


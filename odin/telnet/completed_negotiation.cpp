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
#include "completed_negotiation.hpp"
#include "initiated_negotiation.hpp"
#include <cassert>
#include <memory>

namespace odin { namespace telnet {
        
// ===========================================================================
// COMPLETED_NEGOTIATION::IMPLEMENTATION STRUCTURE
// ===========================================================================
struct completed_negotiation::impl
{
    initiator           initiated_by_;
    negotiation_type    type_;
    negotiation_request local_request_;
    negotiation_request remote_request_;
};
        
// ===========================================================================
// COMPLETED_NEGOTIATION::CONSTRUCTOR
// ===========================================================================
completed_negotiation::completed_negotiation(
    initiator           initiated_by
  , negotiation_type    type
  , negotiation_request local_request
  , negotiation_request remote_request)
{
    assert(initiated_by == local || initiated_by == remote);
    assert(local_request == WILL || local_request == WONT
        || local_request == DO   || local_request == DONT);
    assert(remote_request == WILL || remote_request == WONT
        || remote_request == DO   || remote_request == DONT);
    
    std::auto_ptr<impl> pimpl(new impl);
    
    pimpl->initiated_by_     = initiated_by;
    pimpl->type_             = type;
    pimpl->local_request_    = local_request;
    pimpl->remote_request_   = remote_request;
    
    pimpl_ = pimpl.release();
}

// ===========================================================================
// COMPLETED_NEGOTIATION::CONSTRUCTOR
// ===========================================================================
completed_negotiation::completed_negotiation(
    initiated_negotiation const &negotiation
  , negotiation_request request)
{
    std::auto_ptr<impl> pimpl(new impl);

    pimpl->initiated_by_  = negotiation.get_initiator();
    pimpl->type_          = negotiation.get_type();
    
    if (pimpl->initiated_by_ == local)
    {
        pimpl->local_request_  = negotiation.get_request();
        pimpl->remote_request_ = request;
    }
    else
    {
        pimpl->local_request_  = request;
        pimpl->remote_request_ = negotiation.get_request();
    }

    pimpl_ = pimpl.release();
}

// ===========================================================================
// COMPLETED_NEGOTIATION::COPY CONSTRUCTOR
// ===========================================================================
completed_negotiation::completed_negotiation(completed_negotiation const &other)
    : pimpl_(new impl)
{
    std::auto_ptr<impl> pimpl(new impl);
    *pimpl = *other.pimpl_;
    pimpl_ = pimpl.release();
}

// ===========================================================================
// COMPLETED_NEGOTIATION::DESTRUCTOR
// ===========================================================================
completed_negotiation::~completed_negotiation()
{
    delete pimpl_;
}

// ===========================================================================
// COMPLETED_NEGOTIATION::OPERATOR=
// ===========================================================================
completed_negotiation &completed_negotiation::operator=(
    completed_negotiation const &other)
{
    *pimpl_ = *other.pimpl_;
    return *this;
}
        
// ===========================================================================
// COMPLETED_NEGOTIATION::GET_INITIATOR
// ===========================================================================
initiator completed_negotiation::get_initiator() const
{
    return pimpl_->initiated_by_;
}
    
// ===========================================================================
// COMPLETED_NEGOTIATION::GET_TYPE
// ===========================================================================
negotiation_type completed_negotiation::get_type() const
{
    return pimpl_->type_;
}
    
// ===========================================================================
// COMPLETED_NEGOTIATION::GET_LOCAL_REQUEST
// ===========================================================================
int completed_negotiation::get_local_request() const
{
    return pimpl_->local_request_;
}

// ===========================================================================
// COMPLETED_NEGOTIATION::GET_REMOTE_NEGOTIATION
// ===========================================================================
int completed_negotiation::get_remote_request() const
{
    return pimpl_->remote_request_;
}

// ===========================================================================
// COMPLETED_NEGOTIATION::OPERATOR==
// ===========================================================================
bool operator==(
    completed_negotiation const &lhs
  , completed_negotiation const &rhs)
{
    return lhs.get_initiator()      == rhs.get_initiator()
        && lhs.get_type()           == rhs.get_type()
        && lhs.get_local_request()  == rhs.get_local_request()
        && lhs.get_remote_request() == rhs.get_remote_request();
}

}}


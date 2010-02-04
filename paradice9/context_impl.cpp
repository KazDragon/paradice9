// ==========================================================================
// Paradice Context Implementation
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
#include "context_impl.hpp"
#include <algorithm>
#include <vector>

using namespace std;
using namespace boost;
using namespace odin;

// ==========================================================================
// CONTEXT_IMPL IMPLEMENTATION STRUCTURE
// ==========================================================================
struct context_impl::impl
{
    vector< shared_ptr<paradice::client> > clients_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
context_impl::context_impl()
    : pimpl_(new impl)
{
}
    
// ==========================================================================
// DESTRUCTOR
// ==========================================================================
context_impl::~context_impl()
{
}
    
// ==========================================================================
// GET_CLIENTS
// ==========================================================================
runtime_array< shared_ptr<paradice::client> > context_impl::get_clients()
{
    runtime_array< shared_ptr<paradice::client> > result(
        pimpl_->clients_.size());
    
    copy(pimpl_->clients_.begin(), pimpl_->clients_.end(), result.begin());
    
    return result;
}

// ==========================================================================
// ADD_CLIENT
// ==========================================================================
void context_impl::add_client(shared_ptr<paradice::client> const &cli)
{
    pimpl_->clients_.push_back(cli);
}

// ==========================================================================
// REMOVE_CLIENT
// ==========================================================================
void context_impl::remove_client(shared_ptr<paradice::client> const &cli)
{
    pimpl_->clients_.erase(
        std::remove(
            pimpl_->clients_.begin()
          , pimpl_->clients_.end()
          , cli)
      , pimpl_->clients_.end());
}

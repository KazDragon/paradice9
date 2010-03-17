// ==========================================================================
// Paradice Context
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
#ifndef PARADICE_CONTEXT_HPP_
#define PARADICE_CONTEXT_HPP_

#include "odin/runtime_array.hpp"
#include <boost/shared_ptr.hpp>

namespace paradice {

class client;

//* =========================================================================
/// \brief Describes the interface for a context in which a Paradice server
/// can run.
//* =========================================================================
class context
{
public :
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~context() {}
    
    //* =====================================================================
    /// \brief Retrieves a list of clients currently connected to Paradice.
    //* =====================================================================
    virtual odin::runtime_array< boost::shared_ptr<client> > get_clients() = 0;

    //* =====================================================================
    /// \brief Adds a client to the list of clients currently connected
    /// to Paradice.
    //* =====================================================================
    virtual void add_client(boost::shared_ptr<client> const &cli) = 0;

    //* =====================================================================
    /// \brief Removes a client from the list of clients currently
    /// connected to Paradice.
    //* =====================================================================
    virtual void remove_client(boost::shared_ptr<client> const &cli) = 0;
};

}

#endif

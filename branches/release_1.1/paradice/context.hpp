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

#include <boost/shared_ptr.hpp>
#include <vector>

namespace paradice {

class account;
class character;
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
    virtual std::vector< boost::shared_ptr<client> > get_clients() = 0;
    
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

    //* =====================================================================
    /// \brief For all clients, updates their lists of names.
    //* =====================================================================
    virtual void update_names() = 0;

    //* =====================================================================
    /// \brief Returns how a character appears to others, including prefix
    /// and suffix.
    //* =====================================================================
    virtual std::string get_moniker(boost::shared_ptr<character> &ch) = 0;
    
    //* =====================================================================
    /// \brief Loads an account from a specific account name and returns it.
    /// Returns an empty shared_ptr<> if there was no account with that name
    /// found.
    //* =====================================================================
    virtual boost::shared_ptr<account> load_account(
        std::string const &name) = 0;

    //* =====================================================================
    /// \brief Saves an account.
    //* =====================================================================
    virtual void save_account(boost::shared_ptr<account> acct) = 0;

    //* =====================================================================
    /// \brief Loads a character that is identified by the passed name and
    /// returns it.  Returns an empty shared_ptr<> if there was no character
    /// with that name found.
    //* =====================================================================
    virtual boost::shared_ptr<character> load_character(
        std::string const &name) = 0;

    //* =====================================================================
    /// \brief Saves a character.
    //* =====================================================================
    virtual void save_character(boost::shared_ptr<character> ch) = 0;

    //* =====================================================================
    /// \brief Enacts a server shutdown.
    //* =====================================================================
    virtual void shutdown() = 0;
};

}

#endif

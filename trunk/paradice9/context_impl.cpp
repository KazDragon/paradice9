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
#include "paradice/account.hpp"
#include "paradice/character.hpp"
#include "paradice/client.hpp"
#include "hugin/user_interface.hpp"
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/typeof/typeof.hpp>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace boost;
using namespace boost::archive;
using namespace boost::filesystem;
using namespace odin;

// ==========================================================================
// GET_ACCOUNTS_PATH
// ==========================================================================
static path get_accounts_path()
{
    BOOST_AUTO(cwd, current_path());
    BOOST_AUTO(accounts_path, cwd / "accounts");
    
    if (!exists(accounts_path))
    {
        create_directory(accounts_path);
    }
    
    return accounts_path;
}

// ==========================================================================
// GET_CHARACTERS_PATH
// ==========================================================================
static path get_characters_path()
{
    BOOST_AUTO(cwd, current_path());
    BOOST_AUTO(characters_path, cwd / "characters");
    
    if (!exists(characters_path))
    {
        create_directory(characters_path);
    }
    
    return characters_path;
}

// ==========================================================================
// GET_CHARACTER_ADDRESS
// ==========================================================================
static string get_character_address(shared_ptr<paradice::character> &ch)
{
    string prefix = ch->get_prefix();
    string name   = ch->get_name();
    string title  = ch->get_suffix();
    
    string address;
    
    if (!prefix.empty())
    {
        address += prefix + " ";
    }
    
    address += name;
    
    if (!title.empty())
    {
        address += " " + title;
    }
    
    return address;
}
    
// ==========================================================================
// CONTEXT_IMPL IMPLEMENTATION STRUCTURE
// ==========================================================================
struct context_impl::impl
{
    impl(
        asio::io_service                   &io_service
      , shared_ptr<odin::net::server>       server
      , shared_ptr<asio::io_service::work>  work)
      : strand_(io_service)
      , server_(server)
      , work_(work)
    {
    }
    
    // ======================================================================
    // ADD_CLIENT
    // ======================================================================
    void add_client(shared_ptr<paradice::client> const &cli)
    {
        clients_.push_back(cli);
    }

    // ======================================================================
    // REMOVE_CLIENT
    // ======================================================================
    void remove_client(shared_ptr<paradice::client> const &cli)
    {
        clients_.erase(
            std::remove(
                clients_.begin()
              , clients_.end()
              , cli)
          , clients_.end());
    }

    // ======================================================================
    // UPDATE_NAMES
    // ======================================================================
    void update_names()
    {
        vector<string> names;
        
        BOOST_FOREACH(shared_ptr<paradice::client> cur_client, clients_)
        {
            BOOST_AUTO(character, cur_client->get_character());
            
            if (character != NULL)
            {
                BOOST_AUTO(name, get_character_address(character));
                
                if (!name.empty())
                {
                    names.push_back(name);
                }
            }
        }
        
        BOOST_FOREACH(shared_ptr<paradice::client> cur_client, clients_)
        {
            BOOST_AUTO(user_interface, cur_client->get_user_interface());
            
            if (user_interface != NULL)
            {
                user_interface->update_wholist(names);
            }
        }
    }

    // ======================================================================
    // LOAD_ACCOUNT
    // ======================================================================
    void load_account(string const &name, shared_ptr<paradice::account> &acct)
    {
        BOOST_AUTO(account_path, get_accounts_path() / name);
        
        if (exists(account_path))
        {
            ifstream in(account_path.string().c_str());
            xml_iarchive ia(in);
            
            acct = make_shared<paradice::account>();
            ia >> boost::serialization::make_nvp("account", *acct);
        }
    }

    // ======================================================================
    // SAVE_ACCOUNT
    // ======================================================================
    void save_account(shared_ptr<paradice::account> acct)
    {
        BOOST_AUTO(
            account_path
          , get_accounts_path() / acct->get_name());
        
        ofstream out(account_path.string().c_str());
        xml_oarchive oa(out);
        oa << boost::serialization::make_nvp("account", *acct);
    }

    // ======================================================================
    // LOAD_CHARACTER
    // ======================================================================
    void load_character(
        string const                    &name
      , shared_ptr<paradice::character> &ch)
    {
        BOOST_AUTO(
            character_path
          , get_characters_path() / name);
        
        if (exists(character_path))
        {
            ifstream in(character_path.string().c_str());
            xml_iarchive ia(in);
            
            ch = make_shared<paradice::character>();
            ia >> boost::serialization::make_nvp("character", *ch);
        }
    }

    // ======================================================================
    // SAVE_CHARACTER
    // ======================================================================
    void save_character(shared_ptr<paradice::character> &ch)
    {
        BOOST_AUTO(
            character_path
          , get_characters_path() / ch->get_name());
        
        ofstream out(character_path.string().c_str());
        xml_oarchive oa(out);
        oa << boost::serialization::make_nvp("character", *ch);
    }

    asio::strand                           strand_;
    shared_ptr<odin::net::server>          server_;
    shared_ptr<asio::io_service::work>     work_;
    vector< shared_ptr<paradice::client> > clients_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
context_impl::context_impl(
    asio::io_service                   &io_service
  , shared_ptr<odin::net::server>       server
  , shared_ptr<asio::io_service::work>  work)
    : pimpl_(new impl(io_service, server, work))
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
vector< shared_ptr<paradice::client> > context_impl::get_clients()
{
    return pimpl_->clients_;
}

// ==========================================================================
// ADD_CLIENT
// ==========================================================================
void context_impl::add_client(shared_ptr<paradice::client> const &cli)
{
    pimpl_->strand_.dispatch(bind(&impl::add_client, pimpl_, ref(cli)));
}

// ==========================================================================
// REMOVE_CLIENT
// ==========================================================================
void context_impl::remove_client(shared_ptr<paradice::client> const &cli)
{
    pimpl_->strand_.dispatch(bind(&impl::remove_client, pimpl_, ref(cli)));
}

// ==========================================================================
// UPDATE_NAMES
// ==========================================================================
void context_impl::update_names()
{
    pimpl_->strand_.post(bind(&impl::update_names, pimpl_));
}

// ==========================================================================
// GET_MONIKER
// ==========================================================================
string context_impl::get_moniker(shared_ptr<paradice::character> &ch)
{
    string prefix = ch->get_prefix();
    string name   = ch->get_name();
    string title  = ch->get_suffix();
    
    string address;
    
    if (!prefix.empty())
    {
        address += prefix + " ";
    }
    
    address += name;
    
    if (!title.empty())
    {
        address += " " + title;
    }
    
    return address;
}
 
// ==========================================================================
// LOAD_ACCOUNT
// ==========================================================================
shared_ptr<paradice::account> context_impl::load_account(string const &name)
{
    shared_ptr<paradice::account> acct;
    pimpl_->strand_.dispatch(bind(
        &impl::load_account, pimpl_, name, ref(acct)));

    return acct;
}

// ==========================================================================
// SAVE_ACCOUNT
// ==========================================================================
void context_impl::save_account(shared_ptr<paradice::account> acct)
{
    pimpl_->strand_.dispatch(bind(&impl::save_account, pimpl_, acct));
}

// ==========================================================================
// LOAD_CHARACTER
// ==========================================================================
shared_ptr<paradice::character> context_impl::load_character(
    string const &name)
{
    shared_ptr<paradice::character> ch;
    pimpl_->strand_.dispatch(bind(
        &impl::load_character, pimpl_, name, ref(ch)));
    return ch;
}

// ==========================================================================
// SAVE_CHARACTER
// ==========================================================================
void context_impl::save_character(shared_ptr<paradice::character> ch)
{
    pimpl_->strand_.dispatch(bind(&impl::save_character, pimpl_, ch));
}

// ==========================================================================
// SHUTDOWN
// ==========================================================================
void context_impl::shutdown()
{
    pimpl_->work_.reset();
    pimpl_->server_->shutdown();
}

